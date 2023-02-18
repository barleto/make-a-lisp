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

EnvPtr replEnv(new Env());

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
    if (env == nullptr) {
        env = replEnv;
    }
    EnvPtr currentEnv = env;
    MALTypePtr currentAst = ast;
    for (;;) {
        if (currentAst->type() != MALType::Types::List || currentAst->asList()->isVector) {
            return eval_ast(currentAst, currentEnv);
        }
        auto astAsList = std::dynamic_pointer_cast<MALListType>(currentAst);
        if (astAsList->values.size() == 0) {
            return currentAst;
        }
        if (astAsList->values[0]->type() == MALType::Types::Symbol) {
            std::shared_ptr<HandleSpecialFormResult> result = handleSpecialForms(astAsList, currentEnv);
            if (result->tco) {
                currentEnv = result->env;
                currentAst = result->ast;
                continue;
            }
            if (result->ast != nullptr) {
                return result->ast;
            }
        }

        auto evalList = std::dynamic_pointer_cast<MALListType>(eval_ast(astAsList, currentEnv));
        if (evalList->values[0]->type() != MALType::Types::Function) {
            throw std::runtime_error("Error: function not found with name '" + astAsList->values[0]->to_string(true) + "' in '" + astAsList->to_string(true) + "'");
        }
        auto callable = std::dynamic_pointer_cast<MALCallableType>(evalList->values[0]);
        if (callable->type() != MALType::Types::Function) {
            return currentAst;
        }

        //make args as a copy of the original MALTypes list
        auto args = evalList->values;
        args.erase(args.begin());
        if (callable->isBuiltin()) {
            auto builtinFunc = std::dynamic_pointer_cast<MALBuiltinFuncType>(evalList->values[0]);
            return builtinFunc->fn(args, currentEnv);
        }
        else {
            auto func = std::dynamic_pointer_cast<MALFuncType>(evalList->values[0]);
            EnvPtr newEnv(new Env(func->env, func->bindingsList, args));
            currentEnv = newEnv;
            currentAst = func->funcBody;
            continue;
        }
    }
}

void PRINT(MALTypePtr result) {
    std::cout << "\033[32m=> " << pr_str(result, true) << "\033[0m" << std::endl;
}
MALTypePtr readEval(std::string input, EnvPtr env) {
    auto ast = READ(input);
    return EVAL(ast, env);
}

void rep(std::string input, EnvPtr env) {
    try {
        if (input.size() > 0 && input[0] != ';') {
            auto result = readEval(input, env);
            PRINT(result);
        }
    }
    catch (std::runtime_error& e) {
        std::cout << "\033[31m" << e.what() << "\033[0m" << std::endl;
    }
}


void installBuiltInOps(EnvPtr env)
{
    addBuiltInOperationsToEnv(env);
    readEval("(def! not (fn* (a) (if a false true)))", env);
    readEval("(def! load-file (fn* (filename) \
        (eval (read-string (str \"(do \" (slurp filename) \"\nnil)\")))))", env);
}

void installBuiltInSymbols(EnvPtr env, int argc, char* argv[])
{
    MALListTypePtr astList(new MALListType());
    env->set(std::shared_ptr<MALSymbolType>(new MALSymbolType("*ARGV*")), astList);
    for (int i = 0; i < argc; i++) {
        astList->values.push_back(std::shared_ptr<MALStringType>(new MALStringType(std::string(argv[i]))));
    }
}


void REPL(std::string& input, const char* const& history_path)
{
    for (;;) {
        auto quit = linenoise::Readline("user> ", input);
        if (quit) {
            break;
        }

        linenoise::AddHistory(input.c_str());
        linenoise::SaveHistory(history_path);
        rep(input, replEnv);
    }
}


int main(int argc, char* argv[]) {
    const auto history_path = "history.txt";
    linenoise::LoadHistory(history_path);
    std::string input;

    installBuiltInOps(replEnv);
    installBuiltInSymbols(replEnv, argc, argv);

    if (argc >= 2) {
        std::string fileName(argv[1]);
        readEval("(load-file \"" + fileName + "\")", replEnv);
        return 0;
    }

    REPL(input, history_path);

    return 0;
}