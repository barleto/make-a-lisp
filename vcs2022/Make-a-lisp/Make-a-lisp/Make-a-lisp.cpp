#include <iostream>
#include <string>

#include "linenoise.h"
#include "Reader.h"
#include "Type.h"

MALType* READ(std::string input) { 
    return read_str(input);
}

std::string EVAL(MALType* ast) {
    return pr_str(ast);
}

std::string PRINT(std::string input) { 
    return input;
}

std::string rep(std::string input) {
    try {
        auto ast = READ(input);
        auto result = EVAL(ast);
        return PRINT(result);
    }
    catch (std::runtime_error& e) {
        return e.what();
    }
}

int main() {
    const auto history_path = "history.txt";
    linenoise::LoadHistory(history_path);

    std::string input;
    for (;;) {
        auto quit = linenoise::Readline("user> ", input);
        if (quit)
            break;
        std::cout << rep(input) << std::endl;
        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);

    return 0;
}