#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <assert.h>

#include "linenoise.h"
#include "Reader.h"
#include "Type.h"
#include "Env.h"
#include "SpecFormHandler.h"
#include "core.h"

MALTypePtr READ(std::string input) { 
    return read_str(input);
}

MALTypePtr EVAL(MALTypePtr ast, EnvPtr env);

MALTypePtr eval_ast(MALTypePtr ast, EnvPtr env) {
    switch (ast->type()) {
    case MALType::Types::Symbol: {
        auto symbol = std::dynamic_pointer_cast<MALSymbolType>(ast);
        return env->get(symbol);
    }
    case MALType::Types::List: {
        auto list = std::dynamic_pointer_cast<MALListType>(ast);
        std::shared_ptr<MALListType> newList(new MALListType());
        for (int i = 0; i < list->values.size(); i++) {
            newList->values.push_back(EVAL(list->values[i], env));
        }
        return newList;
        break;
    }
    case MALType::Types::Vector: {
        auto vector = std::dynamic_pointer_cast<MALVectorType>(ast);
        std::shared_ptr<MALVectorType> newList(new MALVectorType());
        for (int i = 0; i < vector->values.size(); i++) {
            newList->values.push_back(EVAL(vector->values[i], env));
        }
        return newList;
        break;
    }
    case MALType::Types::HashMap: {
        auto map = std::dynamic_pointer_cast<MALHashMapType>(ast);
        for (auto p = map->values.begin(); p != map->values.end(); p++) {
            map->values[p->first] = EVAL(p->second, env);
        }
        return map;
        break;
    }
    default:
        return ast;
        break;
    }
}

MALTypePtr EVAL(MALTypePtr ast, EnvPtr env) {
    if (ast->type() != MALType::Types::List) {
        return eval_ast(ast, env);
    }
    auto list = std::dynamic_pointer_cast<MALListType>(ast);
    if (list->values.size() == 0) {
        return ast;
    }

    if (list->values[0]->type() == MALType::Types::Symbol) {
        MALTypePtr result = handleSpecialForms(list, env, std::dynamic_pointer_cast<MALSymbolType>(list->values[0]));
        if (result != nullptr) {
            return result;
        }
    }

    auto evalList = std::dynamic_pointer_cast<MALListType>(eval_ast(list, env));
    auto func = std::dynamic_pointer_cast<MALFuncType>(evalList->values[0]);
    if (func->type() != MALType::Types::Function) {
        return ast;
    }

    auto args = evalList->values;
    args.erase(args.begin());
    return func->fn(args);
}

std::string PRINT(std::string input) { 
    return input;
}

std::string rep(std::string input, EnvPtr env) {
    try {
        auto ast = READ(input);
        auto result = EVAL(ast, env);
        return PRINT(pr_str(result));
    }
    catch (std::runtime_error& e) {
        return e.what();
    }
}

int main() {
    const auto history_path = "history.txt";
    linenoise::LoadHistory(history_path);
    std::string input;

    EnvPtr env(new Env());
    addBuiltInOperationsToEnv(env);
    rep("(def! not (fn* (a) (if a false true)))", env);
    for (;;) {
        auto quit = linenoise::Readline("user> ", input);
        if (quit)
            break;
        linenoise::AddHistory(input.c_str());
        linenoise::SaveHistory(history_path);
        std::cout << rep(input, env) << std::endl;
    }


    return 0;
}