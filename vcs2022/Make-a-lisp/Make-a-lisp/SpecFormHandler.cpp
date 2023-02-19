#include "SpecFormHandler.h"
#include "assert.h"

std::shared_ptr<HandleSpecialFormResult> handleLetStar(MALListTypePtr astList, EnvPtr env)
{
    checkArgsIsAtLeast("let*", astList, 3, astList->values.size());
    EnvPtr newEnv(new Env(env));
    if (!astList->values[1]->isSequence()) {
        throw std::runtime_error("ERROR: 'let*' binding list must be of type list or vector.");
    }
    auto bindingList = std::dynamic_pointer_cast<MALSequenceType>(astList->values[1]);
    if (bindingList->size() % 2 != 0) {
        throw std::runtime_error("ERROR: Mismatched number of elements in binding list: '" + bindingList->to_string(true) + "'.");
    }
    for (int i = 0; i < bindingList->size(); i += 2) {
        if (bindingList->getAt(i)->type() != MALType::Types::Symbol) {
            throw std::runtime_error("ERROR: First element in a binding pair must be a symbol. Found '" + bindingList->getAt(i)->to_string(true) + "' instead.");
        }
        auto symbol = std::dynamic_pointer_cast<MALSymbolType>(bindingList->getAt(i));
        auto evaledValue = EVAL(bindingList->getAt(i + 1), newEnv);
        newEnv->set(symbol, evaledValue);
    }
    auto sfr = new HandleSpecialFormResult{true, newEnv, astList->values[2] };
    std::shared_ptr<HandleSpecialFormResult> result(sfr);
    return result;
}

std::shared_ptr<HandleSpecialFormResult> handleDefBang(MALListTypePtr astList, EnvPtr env)
{
    checkArgsIsAtLeast("def!", astList, 2, astList->values.size());
    if (astList->values[1]->type() != MALType::Types::Symbol) {
        throw std::runtime_error("ERROR: 'def!' first param must be a symbol.");
    }
    auto symbol = std::dynamic_pointer_cast<MALSymbolType>(astList->values[1]);
    auto evaledValue = EVAL(astList->values[2], env);
    env->set(symbol, evaledValue);

    auto sfr = new HandleSpecialFormResult{ false, env, evaledValue };
    std::shared_ptr<HandleSpecialFormResult> result(sfr);
    return result;
}

std::shared_ptr<HandleSpecialFormResult> handleDo(MALListTypePtr astList, EnvPtr env)
{
    if (astList->size() <= 1) {
        auto sfr = new HandleSpecialFormResult{ false, env, MALNilType::Nil };
        std::shared_ptr<HandleSpecialFormResult> result(sfr);
        return result;
    }
    //Evaluate all the elements of the list using eval and return the final evaluated element.
    for (int i = 1; i < astList->values.size() - 1; i++) {
        EVAL(astList->values[i], env);
    }
    MALTypePtr lastValue(astList->values.size() > 0 ? astList->values[astList->values.size() - 1] : MALNilType::Nil);

    auto sfr = new HandleSpecialFormResult{ true, env, lastValue };
    std::shared_ptr<HandleSpecialFormResult> result(sfr);
    return result;
}

std::shared_ptr<HandleSpecialFormResult> handleIf(MALListTypePtr astList, EnvPtr env)
{
    /*Evaluate the first parameter (second element). If the result (condition) is anything other than nil or false,
    then evaluate the second parameter (third element of the list) and return the result. Otherwise, evaluate the third
    parameter (fourth element) and return the result. If condition is false and there is no third parameter,
    then just return nil.*/
    checkArgsIsAtLeast("if", astList, 3, astList->values.size());
    auto conditionResult = EVAL(astList->values[1], env);
    auto conditionResultType = conditionResult->type();
    auto isConditionResultNil = conditionResultType == MALType::Types::Nil;
    auto isConditionResultFalse = conditionResultType == MALType::Types::Bool && !(std::dynamic_pointer_cast<MALBoolType>(conditionResult)->value);
    auto isConditionResultTrue = !isConditionResultNil && !isConditionResultFalse;
    MALTypePtr astToEval;
    if (isConditionResultTrue) {
        astToEval = astList->values[2];
    }
    else if (astList->values.size() <= 3) { //if false bu there's nof alse branch, retunr nil
        astToEval = MALNilType::Nil;
    } else{ //continue eval on false branch
        astToEval = astList->values[3];
    }
    auto sfr = new HandleSpecialFormResult{ true, env, astToEval };
    std::shared_ptr<HandleSpecialFormResult> result(sfr);
    return result;
}


std::shared_ptr<HandleSpecialFormResult> handleClosure(MALListTypePtr astList, EnvPtr env)
{
    /*Return a new function closure. The body of that closure does the following:
    - Create a new environment using env (closed over from outer scope) as the outer parameter,
    the first parameter (second list element of ast from the outer scope) as the binds parameter,
    and the parameters to the closure as the exprs parameter.
    - Call EVAL on the second parameter (third list element of ast from outer scope), using the new environment.
    Use the result as the return value of the closure.
    */
    checkArgsIs("fn*", astList, 3, astList->values.size());
    if (!astList->values[1]->isSequence()) {
        throw std::runtime_error("Error: First parameter of 'fn*' must be a sequence (e.g. list or vector). Found: " + astList->values[1]->to_string(true));
    }
    auto bindingsList = astList->values[1]->asSequence();
    for (auto i = 0; i < bindingsList->size(); i++) {
        auto element = bindingsList->getAt(i);
        if (element->type() != MALType::Types::Symbol) {
            throw std::runtime_error("ERROR: All elements of the binding list of 'fn*' must be symbols. Found: " + element->to_string(true));
        }
    }

    auto funcBody = astList->values[2]; //also called the "ast"
    auto func = std::shared_ptr<MALFuncType>(new MALFuncType(
        astList->to_string(true),
        env,
        bindingsList,
        funcBody
    ));

    auto sfr = new HandleSpecialFormResult{ false, env, func };
    std::shared_ptr<HandleSpecialFormResult> result(sfr);
    return result;
}

std::shared_ptr<HandleSpecialFormResult> handleQuote(MALListTypePtr astList, EnvPtr env) {
    checkArgsNumber("quote", 1, astList->size() - 1);
    auto sfr = new HandleSpecialFormResult{ false, env, astList->values[1]};
    std::shared_ptr<HandleSpecialFormResult> result(sfr);
    return result;
}

MALTypePtr quasiquote(MALTypePtr ast, bool ignoreUnquote = false) {
    auto astAsList = ast->asList();
    std::shared_ptr<MALSymbolType> argAsSymbol(nullptr);
    std::shared_ptr<MALListType> argAsList(nullptr);
    if (ast->type() == MALType::Types::List)
    {
        if (astAsList->size() > 0 && astAsList->values[0]->tryAsSymbol(argAsSymbol) && argAsSymbol->name == "unquote" && !ignoreUnquote) {
            return astAsList->values[1];
        }
        else {
            MALListTypePtr result(new MALListType());
            for (int i = astAsList->size() - 1; i >= 0; i--) {
                if (astAsList->values[i]->tryAsList(argAsList) && argAsList->size() > 0 && argAsList->values[0]->tryAsSymbol(argAsSymbol) && argAsSymbol->name == "splice-unquote") {
                    auto oldResult = result;
                    result = MALListTypePtr(new MALListType());
                    result->values.push_back(MALSymbolTypePtr(new MALSymbolType("concat")));
                    result->values.push_back(argAsList->values[1]);
                    result->values.push_back(oldResult);
                }
                else {
                    auto oldResult = result;
                    result = MALListTypePtr(new MALListType());
                    result->values.push_back(MALSymbolTypePtr(new MALSymbolType("cons")));
                    result->values.push_back(quasiquote(astAsList->values[i]));
                    result->values.push_back(oldResult);
                }
            }
            return result;
        }
    }
    else if (ast->type() == MALType::Types::Vector) {
        MALListTypePtr result(new MALListType());
        result->values.push_back(MALSymbolTypePtr(new MALSymbolType("vec")));
        MALListTypePtr vector2List(new MALListType());
        auto vector = ast->asSequence();
        for (int i = 0; i < vector->size(); i++) {
            vector2List->values.push_back(vector->getAt(i));
        }
        result->values.push_back(quasiquote(vector2List, true));
        return result;
    }
    else if (ast->type() == MALType::Types::HashMap || ast->type() == MALType::Types::Symbol) {
        MALListTypePtr result(new MALListType());
        result->values.push_back(MALSymbolTypePtr(new MALSymbolType("quote")));
        result->values.push_back(ast);
        return result;
    }
    else {
        return ast;
    }
}

std::shared_ptr<HandleSpecialFormResult> handleQuasiquoteExpand(MALListTypePtr astList, EnvPtr env) {
    checkArgsNumber("quasiquoteexpand", 1, astList->size() - 1);
    auto sfr = new HandleSpecialFormResult{ false, env, quasiquote(astList->values[1]) };
    std::shared_ptr<HandleSpecialFormResult> result(sfr);
    return result;
}

std::shared_ptr<HandleSpecialFormResult> handleQuasiquote(MALListTypePtr astList, EnvPtr env) {
    checkArgsNumber("quasiquote", 1, astList->size() - 1);
    auto sfr = new HandleSpecialFormResult{ true, env, quasiquote(astList->values[1]) };
    std::shared_ptr<HandleSpecialFormResult> result(sfr);
    return result;
}

bool isMacroCall(MALTypePtr ast, EnvPtr env) {
    MALListTypePtr astAsList;
    MALSymbolTypePtr astAsSymbol;
    std::shared_ptr<MALCallableType> astAsCallable;
    if (ast->tryAsList(astAsList) && astAsList->size() > 0 && astAsList->getAt(0)->tryAsSymbol(astAsSymbol)) {
        auto envValue = env->find(astAsSymbol);
        return envValue != nullptr && envValue->tryAsCallable(astAsCallable) && astAsCallable->is_macro;
    }
    return false;
}

MALTypePtr macroexpand(MALTypePtr ast, EnvPtr env) {
    MALListTypePtr astAsList;
    MALSymbolTypePtr astAsSymbol;
    std::shared_ptr<MALCallableType> astAsCallable;
    while (isMacroCall(ast, env)) {
        if (!ast->tryAsList(astAsList)) {
            throw std::runtime_error("ERROR: Macroexpand: ast is not a list.");
        }
        if (!astAsList->getAt(0)->tryAsSymbol(astAsSymbol)) {
            throw std::runtime_error("ERROR: Macroexpand: first element needs to be a Symbol.");
        }
        auto envValue = env->get(astAsSymbol);
        if (!envValue->tryAsCallable(astAsCallable)) {
            throw std::runtime_error("ERROR: Macroexpand: Value of key '" + astAsSymbol->to_string(true) + "' it's not a function bu it must be.");
        }
        MALListTypePtr args(new MALListType());
        for (int i = 1; i < astAsList->size(); i++) {
            args->push_back(astAsList->getAt(i));
        }
        auto func = std::dynamic_pointer_cast<MALFuncType>(astAsCallable);
        EnvPtr newEnv(new Env(func->env, func->bindingsList, args->values));
        ast = EVAL(func->funcBody, newEnv);
    }
    return ast;
}

std::shared_ptr<HandleSpecialFormResult> handleDefMacro(MALListTypePtr astList, EnvPtr env) {
    /* This is very similar to the def! form, but before the evaluated value (mal function) 
    is set in the environment, the is_macro attribute should be set to true.*/
    checkArgsIs("defmacro!", astList, 2, astList->values.size() - 1);
    if (astList->values[1]->type() != MALType::Types::Symbol) {
        throw std::runtime_error("ERROR: 'defmacro!' first param must be a symbol.");
    }
    auto symbol = std::dynamic_pointer_cast<MALSymbolType>(astList->values[1]);
    auto evaledValue = EVAL(astList->values[2], env);
    env->set(symbol, evaledValue);
    evaledValue->is_macro = true;
    auto sfr = new HandleSpecialFormResult{ false, env, evaledValue };
    std::shared_ptr<HandleSpecialFormResult> result(sfr);
    return result;

}

std::shared_ptr<HandleSpecialFormResult> handleMacroexpand(MALListTypePtr astList, EnvPtr env) {
    checkArgsNumber("macroexpand", 1, astList->size() - 1);
    auto sfr = new HandleSpecialFormResult{ false, env, macroexpand(astList->values[1], env) };
    std::shared_ptr<HandleSpecialFormResult> result(sfr);
    return result;
}

std::shared_ptr<HandleSpecialFormResult> handleSpecialForms(MALListTypePtr astList, EnvPtr env) {
    auto macroedAstList = macroexpand(astList, env);
    if (!macroedAstList->tryAsList(astList) || astList->size() <= 0) {
        auto sfr = new HandleSpecialFormResult{ false, env, eval_ast(macroedAstList, env) };
        std::shared_ptr<HandleSpecialFormResult> result(sfr);
        return result;
    }
    auto lookupSymbol = std::dynamic_pointer_cast<MALSymbolType>(astList->values[0]);
    if (lookupSymbol->name == "def!") {
        return handleDefBang(astList, env);
    }
    else if (lookupSymbol->name == "let*") {
        return handleLetStar(astList, env);
    }
    else if (lookupSymbol->name == "do") {
        return handleDo(astList, env);
    }
    else if (lookupSymbol->name == "if") {
        return handleIf(astList, env);
    }
    else if (lookupSymbol->name == "fn*") {
        return handleClosure(astList, env);
    }
    else if (lookupSymbol->name == "quote") {
        return handleQuote(astList, env);
    }
    else if (lookupSymbol->name == "quasiquote") {
        return handleQuasiquote(astList, env);
    }
    else if (lookupSymbol->name == "quasiquoteexpand") {
        return handleQuasiquoteExpand(astList, env);
    }
    else if (lookupSymbol->name == "defmacro!") {
        return handleDefMacro(astList, env);
    }
    else if (lookupSymbol->name == "macroexpand") {
        return handleMacroexpand(astList, env);
    }
    auto sfr = new HandleSpecialFormResult{ false, env, nullptr };
    std::shared_ptr<HandleSpecialFormResult> result(sfr);
    return result;
}
