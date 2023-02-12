#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <assert.h>

#include "linenoise.h"
#include "Reader.h"
#include "Type.h"
#include "Env.h"

MALType* READ(std::string input) { 
    return read_str(input);
}

MALType* EVAL(MALType* ast, Env& env);

MALType* eval_ast(MALType* ast, Env& env) {
    switch (ast->type()) {
    case MALType::Types::Symbol: {
        auto symbol = (MALSymbolType*)ast;
        return env.get(symbol);
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

MALType* handleSpecialForms(MALListType* astList, Env& env, MALSymbolType* lookupSymbol) {
    if (lookupSymbol->name == "def!") {
        if (astList->values.size() < 3) {
            throw std::runtime_error("ERROR: 'def!' need at least two params. Params found: '" + astList->to_string() + "'");
        }
        if (astList->values[1]->type() != MALType::Types::Symbol) {
            throw std::runtime_error("ERROR: 'def!' first param must be a symbol.");
        }
        auto symbol = (MALSymbolType*)astList->values[1];
        auto evaledValue = EVAL(astList->values[2], env);
        env.set(symbol, evaledValue);
        return evaledValue;
    } else if (lookupSymbol->name == "let*") {
        if (astList->values.size() < 3) {
            throw std::runtime_error("ERROR: 'let*' need at least two params. Params found: '" + astList->to_string() + "'");
        }
        auto newEnv(env);
        if (astList->values[1]->type() != MALType::Types::List) {
            throw std::runtime_error("ERROR: 'let*' first param must be a list.");
        }
        auto bindingList = (MALListType*)astList->values[1];
        if (bindingList->values.size() % 2 != 0) {
            throw std::runtime_error("ERROR: Mismatched number of elements in binding list: '"+ bindingList->to_string() + "'.");
        }
        for (int i = 0; i < bindingList->values.size(); i += 2) {
            if (bindingList->values[i]->type() != MALType::Types::Symbol) {
                throw std::runtime_error("ERROR: First element in a binding pair must be a symbol. Found '" + bindingList->values[i]->to_string() + "' instead.");
            }
            auto symbol = (MALSymbolType*)bindingList->values[i];
            auto evaledValue = EVAL(bindingList->values[i + 1], newEnv);
            newEnv.set(symbol, evaledValue);
        }
        return EVAL(astList->values[2], newEnv);
    }
    return nullptr;
}

MALType* EVAL(MALType* ast, Env& env) {
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
    auto symbol = (MALSymbolType*)evalList->values[0];
    if (symbol->type() != MALType::Types::Function) {
        return ast;
    }

    evalList->values.erase(evalList->values.begin());
    auto malType = env.get(symbol);
    if (malType->type() != MALType::Types::Function) {
        throw std::runtime_error("EVAL error: Symbol '" + symbol->name + "' is not a function.");
    }
    auto funcType = (MALFuncType*)malType;
    return funcType->fn(evalList->values);
    
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
    env.set(new MALSymbolType(op), new MALFuncType(op, add));
    op = "-";
    env.set(new MALSymbolType(op), new MALFuncType(op, sub));
    op = "*";
    env.set(new MALSymbolType(op), new MALFuncType(op, mult));
    op = "/";
    env.set(new MALSymbolType(op), new MALFuncType(op, divs));


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