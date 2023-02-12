#include "Reader.h"
#include <stdexcept>

Token Reader::next()
{
    if (this->isEOF()) {
        throw std::runtime_error("ERROR: Trying to next access token out of bounds.");
    }
    auto t = this->tokens[this->tokenIndex];
    this->tokenIndex++;
    return t;
}

Token Reader::peek()
{
    if (this->isEOF()) {
        throw std::runtime_error("ERROR: Trying to peek access token out of bounds.");
    }
    return this->tokens[this->tokenIndex];
}

bool Reader::isEOF()
{
    return this->tokenIndex >= this->tokens.size();
}

MALType* read_str(std::string& input)
{
    auto reader = Reader();
    tokenize(input, reader);
    return read_form(reader);
}

void tokenize(std::string& input, Reader& reader)
{
    //token regexp [\s,\n]*(~@|[\[\]{}()'`~^@]|"(?:\\.|[^\\"])*"?|;.*|[^\s\[\]{}('"`,;)]*)
    std::regex word_regex("[\\s,]*(~@|[\\[\\]{}()'`~^@]|\"(?:\\\\.|[^\\\\\"])*\"?|;.*|[^\\s\\[\\]{}('\"`,;)]*)");
    auto words_begin = std::sregex_iterator(input.begin(), input.end(), word_regex);
    auto words_end = std::sregex_iterator();
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string match_str = match[1];
        if (match_str.length() > 0 && match_str[0] != ';') {
            reader.tokens.push_back(match_str);
        }
    }
}

MALType* read_form(Reader& reader)
{
    if (reader.tokens.size() <= 0) {
        return new MALNilType();
    }
    auto token = reader.peek();
    switch (token[0])
    {
    case '(':
        return read_list(reader);
        break;
    case '[':
        return read_vector(reader);
        break;
    case '{':
        return read_map(reader);
        break;
    default:
        return read_atom(reader);
        break;
    }
}

MALType* read_list(Reader& reader)
{
    auto malList = new MALListType();
    reader.next();
    for (;;) {
        Token token;
        try {
            token = reader.peek();
        }
        catch (std::runtime_error&) {
            throw std::runtime_error("ERROR: Unmatched '('");
        }
        switch (token[0])
        {
        case ')':
            reader.next();
            return malList;
            break;
        default:
            malList->values.push_back(read_form(reader));
            break;
        }
    }
    throw std::runtime_error("Error parsing: no matching ')' found until the end of input.");
    return nullptr;
}

MALType* read_vector(Reader& reader)
{
    auto malVector = new MALVectorType();
    reader.next();
    for (;;) {
        Token token;
        try {
            token = reader.peek();
        }
        catch (std::runtime_error&) {
            throw std::runtime_error("ERROR: Unmatched '['");
        }
        switch (token[0])
        {
        case ']':
            reader.next();
            return malVector;
            break;
        default:
            malVector->values.push_back(read_form(reader));
            break;
        }
    }
    throw std::runtime_error("Error parsing: no matching ']' found until the end of input.");
    return nullptr;
}

MALHashMapType* vectorToMalMap(std::vector<MALType*>& vector) {
    if (vector.size() % 2 != 0) {
        throw std::runtime_error("Error parsing: Odd number of keys. Mssing last value.");
    }

    auto malMap = new MALHashMapType();
    for (int i = 0; i < vector.size(); i += 2) {
        if (vector[i]->isCompound()) {
            throw std::runtime_error("Error parsing HashMap: Keys can't be of conpound type.");
        }
        auto key = vector[i];
        auto value = vector[i+1];
        malMap->values.insert(std::pair<MALType*, MALType*>(key, value));
    }
    return malMap;
}

MALType* read_map(Reader& reader)
{
    auto malVector = new MALVectorType();
    reader.next();
    for (;;) {
        Token token;
        try {
            token = reader.peek();
        }
        catch (std::runtime_error&) {
            throw std::runtime_error("ERROR: Unmatched '{'");
        }
        switch (token[0])
        {
        case '}':
            reader.next();
            return vectorToMalMap(malVector->values);
            break;
        default:
            malVector->values.push_back(read_form(reader));
            break;
        }
    }
    throw std::runtime_error("Error parsing: no matching ']' found until the end of input.");
    return nullptr;
}

MALType* read_atom(Reader& reader)
{
    auto token = reader.next();

    std::regex intRegex("^[0-9]+$", std::regex_constants::ECMAScript);
    if (std::regex_search(token, intRegex)) {
        return new MALNumberType(stoi(token));
    }
    else if (token[0] == '"') {
        return new MALStringType(token);
    }
    else if (token[0] == ':') {
        return new MalKeywordType(token.substr(1));
    }
    else if (token == "true") {
        return new MALBoolType(true);
    }
    else if (token == "false") {
        return new MALBoolType(false);
    }
    else if (token == "nil") {
        return new MALNilType();
    }
    else {
        return new MALSymbolType(token);
    }
    return nullptr;
}

std::string pr_str(MALType* malType)
{
    return malType->to_string();
}
