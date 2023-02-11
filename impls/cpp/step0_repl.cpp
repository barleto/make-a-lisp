// Your First C++ Program

#include <iostream>
#include <string>

std::string READ(std::string input)
{
    return input;
}

std::string EVAL(std::string input)
{
    return input;
}

std::string PRINT(std::string input)
{
    return input;
}

std::string rep(std::string input)
{
    auto ast = READ(input);
    auto result = EVAL(ast);
    return PRINT(result);
}

int main()
{

    const auto history_path = "history.txt";

    std::string input;
    for (;;)
    {
        if (!std::getline(std::cin, input))
        {
            break;
        }
        std::cout << rep(input) << std::endl;
    }

    return 0;
}
