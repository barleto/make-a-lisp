#include "Type.h"

std::string MALListType::to_string()
{
    std::string result = "";
    auto size = this->values.size();
    for (int i = 0; i < size; i++) {
        result += this->values[i]->to_string();
        if (i != size - 1) {
            result += " ";
        }
    }
    return "(" + result+ ")";
}

std::string MALIntType::to_string()
{
    return std::to_string(this->value);
}

std::string MALSymbolType::to_string()
{
    return this->name;
}

std::string MALStringType::to_string()
{
    return this->value;
}

std::string MALNilType::to_string()
{
    return "nil";
}

std::string MALBoolType::to_string()
{
    return this->value? "true" : "false";
}

std::string MalKeywordType::to_string()
{
    return ":" + this->value;
}

std::string MALVectorType::to_string()
{
    std::string result = "";
    auto size = this->values.size();
    for (int i = 0; i < size; i++) {
        result += this->values[i]->to_string();
        if (i != size - 1) {
            result += " ";
        }
    }
    return "[" + result + "]";
}

std::string MALHashMapType::to_string()
{
    std::string result = "";
    auto size = this->values.size();
    int i = 0;
    for (auto p = this->values.begin(); p != this->values.end(); p++, i++) {
        auto pair = *p;
        result += pair.first->to_string() + " ";
        result += pair.second->to_string();
        if (i != size - 1) {
            result += " ";
        }
    }
    return "{" + result + "}";
}

const bool MALAtomType::isCompound()
{
    return false;
}

const bool MALCompoundType::isCompound()
{
    return true;
}
