#include "reader.hpp"

Reader::Reader(std::vector<Token> tokens) : tokens(tokens)
{
}

Token *Reader::next()
{
    auto t = &(this->tokens[this->tokenIndex]);
    this->tokenIndex++;
    return t;
}

Token* Reader::peek()
{
    Type p;
    return &(this->tokens[this->tokenIndex]);
}

void read_str(std::string &input)
{
    auto tokens = tokenize(input);
    auto reader = Reader(tokens);
    read_form(reader);
}

std::vector<Token> tokenize(std::string &input)
{
    //token regexp [\s,]*(~@|[\[\]{}()'`~^@]|"(?:\\.|[^\\"])*"?|;.*|[^\s\[\]{}('"`,;)]*)
    return std::vector<Token>();
}

void read_form(Reader& reader)
{
    auto token = reader.peek();
    switch ((*token)[0])
    {
    case '(':
        /* code */
        //read_list(reader);
        break;
    default:
        //read_atom(reader);
        break;
    }
}
