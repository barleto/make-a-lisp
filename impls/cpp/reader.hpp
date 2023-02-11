#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>

#include "type.hpp"

using Token = std::string;

class Reader
{
    int tokenIndex = 0;
    std::vector<Token> tokens;
    void read_from();
public:
    Reader(std::vector<Token> tokens);
    Token* next();
    Token* peek();
};


void read_str(std::string& input);
std::vector<Token> tokenize(std::string& input);
void read_form(Reader &reader);
