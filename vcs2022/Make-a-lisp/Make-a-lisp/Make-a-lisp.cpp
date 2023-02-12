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

MALType* READ(std::string input) { 
    return read_str(input);
}

MALType* EVAL(MALType* ast, Env* env);

MALType* eval_ast(MALType* ast, Env* env) {
    switch (ast->type()) {
    case MALType::Types::Symbol: {
        auto symbol = (MALSymbolType*)ast;
        return env->get(symbol);
    }
    case MALType::Types::List: {
        auto list = (MALListType*)ast;
        for (int i = 0; i < list->values.size(); i++) {
            list->values[i] = (EVAL(list->values[i], env));
        }
        return list;
        break;
    }
    case MALType::Types::Vector: {
        auto vector = (MALVectorType*)ast;
        auto size = vector->values.size();
        for (int i = 0; i < size; i++) {
            vector->values[i] = (EVAL(vector->values[i], env));
        }
        return vector;
        break;
    }
    case MALType::Types::HashMap: {
        auto map = (MALHashMapType*)ast;
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

MALType* EVAL(MALType* ast, Env* env) {
    if (ast->type() != MALType::Types::List) {
        return eval_ast(ast, env);
    }
    auto list = (MALListType*)ast;
    if (list->values.size() == 0) {
        return ast;
    }

    if (list->values[0]->type() == MALType::Types::Symbol) {
        MALType* result = handleSpecialForms(list, env, (MALSymbolType*)list->values[0]);
        if (result != nullptr) {
            return result;
        }
    }

    auto evalList = (MALListType*)eval_ast(list, env);
    auto func = (MALFuncType*)evalList->values[0];
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

std::string rep(std::string input, Env* env) {
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

    auto env = new Env();
    addBuiltInOperationsToEnv(env);

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