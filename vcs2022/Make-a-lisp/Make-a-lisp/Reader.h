#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>

#include "Type.h"

using Token = std::string;

class Reader
{
    int tokenIndex = 0;
public:
    std::vector<Token> tokens;
    Token next();
    Token peek();
    bool isEOF();
};


MALType* read_str(std::string& input);
void tokenize(std::string& input, Reader& reader);
MALType* read_form(Reader& reader);
MALType* read_list(Reader& reader);
MALType* read_atom(Reader& reader);
std::string pr_str(MALType* root);
