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

MALTypePtr read_str(std::string& input)
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

MALTypePtr read_form(Reader& reader)
{
    if (reader.tokens.size() <= 0) {
        return std::shared_ptr<MALNilType>(new MALNilType());
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
    case '@':
        return read_deref_shortcut(reader);
        break;
    default:
        return read_atom(reader);
        break;
    }
}

MALTypePtr read_list(Reader& reader)
{
    auto malList = std::shared_ptr<MALListType>(new MALListType());
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

MALTypePtr read_vector(Reader& reader)
{
    //auto malVector = std::shared_ptr<MALVectorType>(new MALVectorType());
    auto malVector = std::shared_ptr<MALListType>(new MALListType());
    malVector->isVector = true;
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

std::shared_ptr<MALHashMapType> vectorToMalMap(std::vector<MALTypePtr>& hashMapInitializer) {
    if (hashMapInitializer.size() % 2 != 0) {
        throw std::runtime_error("Error parsing: Odd number of keys. Mssing last value.");
    }

    auto malMap = std::shared_ptr<MALHashMapType>(new MALHashMapType());
    for (int i = 0; i < hashMapInitializer.size(); i += 2) {
        if (hashMapInitializer[i]->isContainer()) {
            throw std::runtime_error("Error parsing HashMap: Keys can't be of conpound type.");
        }
        auto key = hashMapInitializer[i];
        auto value = hashMapInitializer[i+1];
        malMap->values.erase(key);
        malMap->values.insert(std::pair<MALTypePtr, MALTypePtr>(key, value));
    }
    return malMap;
}

MALTypePtr read_map(Reader& reader)
{
    auto malVector = std::shared_ptr<MALVectorType>(new MALVectorType());
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

MALTypePtr read_deref_shortcut(Reader& reader)
{
    auto malVector = std::shared_ptr<MALVectorType>(new MALVectorType());
    reader.next();
    std::string token = reader.next();
    MALListTypePtr ast(new MALListType());
    ast->values.push_back(MALSymbolTypePtr(new MALSymbolType("deref")));
    ast->values.push_back(MALSymbolTypePtr(new MALSymbolType(token)));
    return ast;
}

MALTypePtr read_atom(Reader& reader)
{
    auto token = reader.next();

    std::regex intRegex("^[-]?[0-9]+(.[0-9]+)?$", std::regex_constants::ECMAScript);
    if (std::regex_search(token, intRegex)) {
        return std::shared_ptr<MALNumberType>(new MALNumberType(stod(token)));
    }
    else if (token[0] == '"') {

        return std::shared_ptr<MALStringType>(new MALStringType(token.substr(1, token.size() - 2)));
    }
    else if (token[0] == ':') {
        return std::shared_ptr<MalKeywordType>(new MalKeywordType(token.substr(1)));
    }
    else if (token == "true") {
        return std::shared_ptr<MALBoolType>(new MALBoolType(true));
    }
    else if (token == "false") {
        return std::shared_ptr<MALBoolType>(new MALBoolType(false));
    }
    else if (token == "nil") {
        return std::shared_ptr<MALNilType>(new MALNilType());
    }
    else {
        return std::shared_ptr<MALSymbolType>(new MALSymbolType(token));
    }
    return nullptr;
}

std::string pr_str(MALTypePtr malType)
{
    return malType->to_string();
}
