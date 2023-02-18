#pragma once
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


MALTypePtr read_str(std::string& input);
void tokenize(std::string& input, Reader& reader);
MALTypePtr read_form(Reader& reader);
MALTypePtr read_list(Reader& reader);
MALTypePtr read_vector(Reader& reader);
MALTypePtr read_map(Reader& reader);
MALTypePtr read_deref_shortcut(Reader& reader);
MALTypePtr read_atom(Reader& reader);
std::string pr_str(MALTypePtr malType, bool print_readably);
