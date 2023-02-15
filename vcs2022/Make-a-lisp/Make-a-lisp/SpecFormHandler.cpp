#include "SpecFormHandler.h"
#include "assert.h"

std::shared_ptr<SpecFormResult> handleLetStart(MALListTypePtr astList, EnvPtr env)
{
    checkArgsIsAtLeast("let*", astList, 3, astList->values.size());
    EnvPtr newEnv(new Env(env));
    if (!astList->values[1]->isIteratable()) {
        throw std::runtime_error("ERROR: 'let*' binding list must be of type list or vector.");
    }
    auto bindingList = std::dynamic_pointer_cast<MALIteratableContainerType>(astList->values[1]);
    if (bindingList->size() % 2 != 0) {
        throw std::runtime_error("ERROR: Mismatched number of elements in binding list: '" + bindingList->to_string() + "'.");
    }
    for (int i = 0; i < bindingList->size(); i += 2) {
        if (bindingList->getAt(i)->type() != MALType::Types::Symbol) {
            throw std::runtime_error("ERROR: First element in a binding pair must be a symbol. Found '" + bindingList->getAt(i)->to_string() + "' instead.");
        }
        auto symbol = std::dynamic_pointer_cast<MALSymbolType>(bindingList->getAt(i));
        auto evaledValue = EVAL(bindingList->getAt(i + 1), newEnv);
        newEnv->set(symbol, evaledValue);
    }
    auto sfr = new SpecFormResult{ env, EVAL(astList->values[2], newEnv) };
    std::shared_ptr<SpecFormResult> result(sfr);
    return result;
}

std::shared_ptr<SpecFormResult> handleDefBang(MALListTypePtr astList, EnvPtr env)
{
    checkArgsIsAtLeast("def!", astList, 2, astList->values.size());
    if (astList->values[1]->type() != MALType::Types::Symbol) {
        throw std::runtime_error("ERROR: 'def!' first param must be a symbol.");
    }
    auto symbol = std::dynamic_pointer_cast<MALSymbolType>(astList->values[1]);
    auto evaledValue = EVAL(astList->values[2], env);
    env->set(symbol, evaledValue);

    auto sfr = new SpecFormResult{ env, evaledValue };
    std::shared_ptr<SpecFormResult> result(sfr);
    return result;
}

std::shared_ptr<SpecFormResult> handleDo(MALListTypePtr astList, EnvPtr env)
{
    //Evaluate all the elements of the list using eval and return the final evaluated element.
    MALTypePtr lastValue(new MALNilType());
    for (int i = 1; i < astList->values.size(); i++) {
        lastValue = EVAL(astList->values[i], env);
    }

    auto sfr = new SpecFormResult{ env, lastValue };
    std::shared_ptr<SpecFormResult> result(sfr);
    return result;
}

std::shared_ptr<SpecFormResult> handleIf(MALListTypePtr astList, EnvPtr env)
{
    /*Evaluate the first parameter (second element). If the result (condition) is anything other than nil or false,
    then evaluate the second parameter (third element of the list) and return the result. Otherwise, evaluate the third
    parameter (fourth element) and return the result. If condition is false and there is no third parameter,
    then just return nil.*/
    checkArgsIsAtLeast("if", astList, 3, astList->values.size());
    auto conditionResult = EVAL(astList->values[1], env);
    auto conditionResultType = conditionResult->type();
    auto isTypeNil = conditionResultType == MALType::Types::Nil;
    auto isTypeFalse = conditionResultType == MALType::Types::Bool && !(std::dynamic_pointer_cast<MALBoolType>(conditionResult)->value);
    if (!isTypeNil && !isTypeFalse) {
        auto sfr = new SpecFormResult{ env, EVAL(astList->values[2], env) };
        std::shared_ptr<SpecFormResult> result(sfr);
        return result;
    }
    if (astList->values.size() <= 3) {
        auto sfr = new SpecFormResult{ env, MALTypePtr(new MALNilType()) };
        std::shared_ptr<SpecFormResult> result(sfr);
        return result;
    }
    auto sfr = new SpecFormResult{ env, EVAL(astList->values[3], env) };
    std::shared_ptr<SpecFormResult> result(sfr);
    return result;
}


std::shared_ptr<SpecFormResult> handleClosure(MALListTypePtr astList, EnvPtr env)
{
    /*Return a new function closure. The body of that closure does the following:
    - Create a new environment using env (closed over from outer scope) as the outer parameter,
    the first parameter (second list element of ast from the outer scope) as the binds parameter,
    and the parameters to the closure as the exprs parameter.
    - Call EVAL on the second parameter (third list element of ast from outer scope), using the new environment.
    Use the result as the return value of the closure.
    */
    checkArgsIs("fn*", astList, 3, astList->values.size());
    if (astList->values[1]->type() != MALType::Types::List) {
        throw std::runtime_error("Error: First parameter of 'fn*' mus be a list. Found: " + astList->values[1]->to_string());
    }
    auto bindingsList = std::dynamic_pointer_cast<MALListType>(astList->values[1]);
    for (auto p = bindingsList->values.begin(); p != bindingsList->values.end(); p++) {
        auto element = *p;
        if (element->type() != MALType::Types::Symbol) {
            throw std::runtime_error("ERROR: All elements of the binding list of 'fn*' must be symbols. Found: " + element->to_string());
        }
    }

    auto funcBody = astList->values[2];
    auto lambda = (MALFunctor)[env, bindingsList, funcBody](std::vector<MALTypePtr> args) -> MALTypePtr {
        EnvPtr newEnv(new Env(env, bindingsList, args));
        auto result = EVAL(funcBody, newEnv);
        //delete newEnv;
        return result;
    };

    auto sfr = new SpecFormResult{ env, std::shared_ptr<MALFuncType>(new MALFuncType(astList->to_string(), lambda)) };
    std::shared_ptr<SpecFormResult> result(sfr);
    return result;
}

std::shared_ptr<SpecFormResult> handleSpecialForms(MALListTypePtr astList, EnvPtr env, MALSymbolTypePtr lookupSymbol) {
    if (lookupSymbol->name == "def!") {
        return handleDefBang(astList, env);
    }
    else if (lookupSymbol->name == "let*") {
        return handleLetStart(astList, env);
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
    auto sfr = new SpecFormResult{ env, nullptr };
    std::shared_ptr<SpecFormResult> result(sfr);
    return result;
}
