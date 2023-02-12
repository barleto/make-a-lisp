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
        auto oldList = (MALListType*)ast;
        auto newList = new MALListType();
        for (auto p = oldList->values.begin(); p != oldList->values.end(); p++) {
            newList->values.push_back(EVAL(*p, env));
        }
        return newList;
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
    assert(args.size() == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == MALType::Types::Float);
    assert(b->type() == MALType::Types::Float);

    float result = ((MALNumberType*)a)->value + ((MALNumberType*)b)->value;
    return new MALNumberType(result);
}

MALType* sub(std::vector<MALType*> args) {
    assert(args.size() == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == MALType::Types::Float);
    assert(b->type() == MALType::Types::Float);

    float result = ((MALNumberType*)a)->value - ((MALNumberType*)b)->value;
    return new MALNumberType(result);
}

MALType* mult(std::vector<MALType*> args) {
    assert(args.size() == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == MALType::Types::Float);
    assert(b->type() == MALType::Types::Float);

    float result = ((MALNumberType*)a)->value * ((MALNumberType*)b)->value;
    return new MALNumberType(result);
}

MALType* divs(std::vector<MALType*> args) {
    assert(args.size() == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == MALType::Types::Float);
    assert(b->type() == MALType::Types::Float);

    float result = ((MALNumberType*)a)->value / ((MALNumberType*)b)->value;
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