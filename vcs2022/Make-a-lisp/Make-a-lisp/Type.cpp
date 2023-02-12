#include "Type.h"

std::string MALType::typeToString(Types t)
{
    switch (t) {
    case Types::List: {
        return "List";
    }
    case Types::Float: {
        return "Number";
    }
    case Types::Symbol: {
        return "Symbol";
    }
    case Types::String: {
        return "String";
    }
    case Types::Bool: {
        return "Bool";
    }
    case Types::Nil: {
        return "Nil";
    }
    case Types::Keyword: {
        return "Keyword";
    }
    case Types::Vector: {
        return "Vector";
    }
    case Types::HashMap: {
        return "HashMap";
    }
    case Types::Function: {
        return "Function";
    }
    default:
        return "Unknown";
    }
}

MALType* MALListType::deepCopy() {
    auto result = new MALListType();
    for (auto p = this->values.begin(); p != this->values.end(); p++) {
        auto malType = *p;
        result->values.push_back(malType->deepCopy());
    }
    return result;
}

bool MALListType::isEqualTo(MALType* other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = (MALListType*)other;
    if (castOther->size() != this->size()) {
        return false;
    }
    for (int i = 0; i < this->size(); i++) {
        if (!(this->values[i]->isEqualTo(castOther->values[i]))) {
            return false;
        }
    }
    return true;
}

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
    return "(" + result + ")";
}

MALType* MALNumberType::deepCopy() {
    return new MALNumberType(this->value);
}

bool MALNumberType::isEqualTo(MALType* other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = (MALNumberType*)other;
    return castOther->value == this->value;
}

std::string MALNumberType::to_string()
{
    return std::to_string(this->value);
}

MALType* MALSymbolType::deepCopy() {
    return new MALSymbolType(this->name);
}

bool MALSymbolType::isEqualTo(MALType* other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = (MALSymbolType*)other;
    return castOther->name == this->name;
}

std::string MALSymbolType::to_string()
{
    return this->name;
}

MALType* MALStringType::deepCopy() {
    return new MALStringType(this->value);
}

bool MALStringType::isEqualTo(MALType* other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = (MALStringType*)other;
    return castOther->value == this->value;
}

std::string MALStringType::to_string()
{
    return this->value;
}

MALType* MALNilType::deepCopy() {
    return new MALNilType();
}

bool MALNilType::isEqualTo(MALType* other)
{
    return other->type() == this->type();
}

std::string MALNilType::to_string()
{
    return "nil";
}

MALType* MALBoolType::deepCopy() {
    return new MALBoolType(this->value);
}

bool MALBoolType::isEqualTo(MALType* other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = (MALBoolType*)other;
    return castOther->value == this->value;
}

std::string MALBoolType::to_string()
{
    return this->value ? "true" : "false";
}

MALType* MalKeywordType::deepCopy() {
    return new MalKeywordType(this->value);
}

bool MalKeywordType::isEqualTo(MALType* other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = (MalKeywordType*)other;
    return castOther->value == this->value;
}

std::string MalKeywordType::to_string()
{
    return ":" + this->value;
}

MALType* MALVectorType::deepCopy() {
    auto result = new MALVectorType();
    for (auto p = this->values.begin(); p != this->values.end(); p++) {
        auto malType = *p;
        result->values.push_back(malType->deepCopy());
    }
    return result;
}

bool MALVectorType::isEqualTo(MALType* other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = (MALVectorType*)other;
    if (castOther->size() != this->size()) {
        return false;
    }
    for (int i = 0; i < this->size(); i++) {
        if (!(this->values[i]->isEqualTo(castOther->values[i]))) {
            return false;
        }
    }
    return true;
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

MALType* MALHashMapType::deepCopy() {
    auto result = new MALHashMapType();
    for (auto p = this->values.begin(); p != this->values.end(); p++) {
        auto malType = *p;
        result->values.insert(std::pair<MALType*, MALType*>(p->first->deepCopy(), p->second->deepCopy()));
    }
    return result;
}

bool MALHashMapType::isEqualTo(MALType* other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = (MALHashMapType*)other;
    if (castOther->size() != this->size()) {
        return false;
    }
    auto r = castOther->values.begin();
    for (auto p = this->values.begin(); p != this->values.end(); p++, r++) {
        auto otherSymbol = (MALSymbolType*)r->first;
        auto got = this->values.find(otherSymbol);
        //check if both have the same keys
        if (got == this->values.end()) {
            return false;
        }
        //check if values are different
        if (!(p->second->isEqualTo(r->second))) {
            return false;
        }
    }
    return true;
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

MALType* MALFuncType::deepCopy() {
    return new MALFuncType(this->name, this->fn);
}

bool MALFuncType::isEqualTo(MALType* other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = (MALFuncType*)other;
    return other == this;
}

std::string MALFuncType::to_string()
{
    return "<function:" + this->name + ">";
}
