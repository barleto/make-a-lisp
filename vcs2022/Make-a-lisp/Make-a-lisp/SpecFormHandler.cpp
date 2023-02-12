#include "SpecFormHandler.h"
#include "assert.h"

MALType* handleLetStart(MALListType* astList, Env* env)
{
    checkArgsIsAtLeast("let*", astList, 3, astList->values.size());
    Env* newEnv = new Env(env);
    if (!astList->values[1]->isIteratable()) {
        throw std::runtime_error("ERROR: 'let*' binding list must be of type list or vector.");
    }
    auto bindingList = (MALIteratableContainerType*)astList->values[1];
    if (bindingList->size() % 2 != 0) {
        throw std::runtime_error("ERROR: Mismatched number of elements in binding list: '" + bindingList->to_string() + "'.");
    }
    for (int i = 0; i < bindingList->size(); i += 2) {
        if (bindingList->getAt(i)->type() != MALType::Types::Symbol) {
            throw std::runtime_error("ERROR: First element in a binding pair must be a symbol. Found '" + bindingList->getAt(i)->to_string() + "' instead.");
        }
        auto symbol = (MALSymbolType*)bindingList->getAt(i);
        auto evaledValue = EVAL(bindingList->getAt(i + 1), newEnv);
        newEnv->set(symbol, evaledValue);
    }
    return EVAL(astList->values[2], newEnv);
}

MALType* handleDefBang(MALListType* astList, Env* env)
{
    checkArgsIsAtLeast("def!", astList, 2, astList->values.size());
    if (astList->values[1]->type() != MALType::Types::Symbol) {
        throw std::runtime_error("ERROR: 'def!' first param must be a symbol.");
    }
    auto symbol = (MALSymbolType*)astList->values[1];
    auto evaledValue = EVAL(astList->values[2], env);
    env->set(symbol, evaledValue);
    return evaledValue;
}

MALType* handleDo(MALListType* astList, Env* env)
{
    //Evaluate all the elements of the list using eval and return the final evaluated element.
    astList->values.erase(astList->values.begin());
    for (int i = 0; i < astList->values.size(); i++) {
        astList->values[i] = EVAL(astList->values[i], env);
    }
    return astList->values[astList->values.size() - 1];
}

MALType* handleIf(MALListType* astList, Env* env)
{
    /*Evaluate the first parameter (second element). If the result (condition) is anything other than nil or false,
    then evaluate the second parameter (third element of the list) and return the result. Otherwise, evaluate the third
    parameter (fourth element) and return the result. If condition is false and there is no third parameter,
    then just return nil.*/
    checkArgsIsAtLeast("if", astList, 3, astList->values.size());
    auto result = EVAL(astList->values[1], env);
    auto conditionResultType = result->type();
    auto isTypeNil = conditionResultType == MALType::Types::Nil;
    auto isTypeFalse = conditionResultType == MALType::Types::Bool && !((MALBoolType*)result)->value;
    if (!isTypeNil && !isTypeFalse) {
        return EVAL(astList->values[2], env);
    }
    if (astList->values.size() <= 3) {
        return new MALNilType();
    }
    return EVAL(astList->values[3], env);
}

MALType* handleSpecialForms(MALListType* astList, Env* env, MALSymbolType* lookupSymbol) {
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
        /*Return a new function closure. The body of that closure does the following:
        - Create a new environment using env (closed over from outer scope) as the outer parameter, 
            the first parameter (second list element of ast from the outer scope) as the binds parameter, 
            and the parameters to the closure as the exprs parameter.
        - Call EVAL on the second parameter (third list element of ast from outer scope), using the new environment. 
            Use the result as the return value of the closure.
        */
        checkArgsIs("if", astList, 3, astList->values.size());
        if (astList->values[1]->type() != MALType::Types::List) {
            throw std::runtime_error("Error: First parameter of 'fn*' mus be a list. Found: " + astList->values[1]->to_string());
        }
        auto bindingsList = (MALListType*)astList->values[1];
        for (auto p = bindingsList->values.begin(); p != bindingsList->values.end(); p++) {
            auto element = *p;
            if (element->type() != MALType::Types::Symbol) {
                throw std::runtime_error("ERROR: All elements of the binding list of 'fn*' must be symbols. Found: " + element->to_string());
            }
        }
        auto fnBody = astList->values[2];
        return new MALFuncType(astList->to_string(), (MALFunctor)[env, bindingsList, fnBody](std::vector<MALType*> args) -> MALType* {
            auto newEnv = new Env(env, bindingsList, args);
            auto result = EVAL(fnBody->deepCopy() , newEnv);
            //delete newEnv;
            return result;
        });
    }
    return nullptr;
}
