#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <assert.h>

#include "linenoise.h"
#include "Reader.h"
#include "Type.h"

MALType* READ(std::string input) { 
    return read_str(input);
}

MALType* EVAL(MALType* ast, Env& env);

MALType* eval_ast(MALType* ast, Env& env) {
    switch (ast->type()) {
    case MALType::Types::Symbol: {
        auto symbol = (MALSymbolType*)ast;
        auto got = env.find(symbol);
        if (got == env.end()) {
            throw std::runtime_error("EVAL error: Symbol '" + symbol->name + "' was not found.");
        }
        return ast;
    }
    case MALType::Types::List: {
        auto list = (MALListType*)ast;
        auto size = list->values.size();
        for (int i = 0; i < size; i++) {
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

MALType* EVAL(MALType* ast, Env& env) {
    if (ast->type() == MALType::Types::List) {
        auto list = (MALListType*)ast;
        if (list->values.size() == 0) {
            return ast;
        }
        else {
            auto evalList = (MALListType*)eval_ast(list, env);
            auto symbol = (MALSymbolType*)evalList->values[0];
            if (symbol->type() != MALType::Types::Symbol) {
                return ast;
            }
            evalList->values.erase(evalList->values.begin());
            auto got = env.find(symbol);
            if (got == env.end()) {
                throw std::runtime_error("EVAL error: Symbol '" + symbol->name + "' was not found.");
            }
            return got->second(evalList->values);
        }
    }
    else {
        return eval_ast(ast, env);
    }
}

std::string PRINT(std::string input) { 
    return input;
}

std::string rep(std::string input, Env& env) {
    try {
        auto ast = READ(input);
        auto result = EVAL(ast, env);
        return PRINT(pr_str(result));
    }
    catch (std::runtime_error& e) {
        return e.what();
    }
}

MALFunc wrapLambda(MALFunc lambda) {
    return MALFunc(lambda);
}

MALType* add(std::vector<MALType*> args) {
    float result = 0;
    for (auto p = args.begin(); p != args.end(); p++) {
        result += ((MALNumberType*)*p)->value;
    }
    return new MALNumberType(result);
}

MALType* sub(std::vector<MALType*> args) {
    float result = 0;
    for (auto p = args.begin(); p != args.end(); p++) {
        result -= ((MALNumberType*)*p)->value;
    }
    return new MALNumberType(result);
}

MALType* mult(std::vector<MALType*> args) {
    float result = 1;
    for (auto p = args.begin(); p != args.end(); p++) {
        result *= ((MALNumberType*)*p)->value;
    }
    return new MALNumberType(result);
}

MALType* divs(std::vector<MALType*> args) {
    float result = args.size() > 0? ((MALNumberType*)args[0])->value : 0;
    for (auto p = args.begin(); p != args.end(); p++) {
        result /= ((MALNumberType*)*p)->value;
    }
    return new MALNumberType(result);
}

int main() {
    const auto history_path = "history.txt";
    linenoise::LoadHistory(history_path);
    std::string input;

    Env env;
    std::string op;
    op = "+";
    env[new MALSymbolType(op)] = (MALFunc)add;
    op = "-";
    env[new MALSymbolType(op)] = (MALFunc)sub;
    op = "*";
    env[new MALSymbolType(op)] = (MALFunc)mult;
    op = "/";
    env[new MALSymbolType(op)] = (MALFunc)divs;

    for (;;) {
        auto quit = linenoise::Readline("user> ", input);
        if (quit)
            break;
        std::cout << rep(input, env) << std::endl;
        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);

    return 0;
}