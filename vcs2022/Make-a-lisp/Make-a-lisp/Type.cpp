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

MALTypePtr MALListType::deepCopy() {
    auto result = std::shared_ptr<MALListType>(new MALListType());
    for (auto p = this->values.begin(); p != this->values.end(); p++) {
        auto malType = *p;
        result->values.push_back(malType->deepCopy());
    }
    return result;
}

bool MALListType::isEqualTo(MALTypePtr other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = std::dynamic_pointer_cast<MALListType>(other);
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

MALTypePtr MALNumberType::deepCopy() {
    return std::shared_ptr<MALNumberType>(new MALNumberType(this->value));
}

bool MALNumberType::isEqualTo(MALTypePtr other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = std::dynamic_pointer_cast<MALNumberType>(other);
    return castOther->value == this->value;
}

std::string MALNumberType::to_string()
{
    return std::to_string(this->value);
}

MALTypePtr MALSymbolType::deepCopy() {
    return std::shared_ptr<MALSymbolType>(new MALSymbolType(this->name));
}

bool MALSymbolType::isEqualTo(MALTypePtr other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = std::dynamic_pointer_cast<MALSymbolType>(other);
    return castOther->name == this->name;
}

std::string MALSymbolType::to_string()
{
    return this->name;
}

MALTypePtr MALStringType::deepCopy() {
    return std::shared_ptr<MALStringType>(new MALStringType(this->value));
}

bool MALStringType::isEqualTo(MALTypePtr other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = std::dynamic_pointer_cast<MALStringType>(other);
    return castOther->value == this->value;
}

std::string MALStringType::to_string()
{
    return this->value;
}

MALTypePtr MALNilType::deepCopy() {
    return std::shared_ptr<MALNilType>(new MALNilType());
}

bool MALNilType::isEqualTo(MALTypePtr other)
{
    return other->type() == this->type();
}

std::string MALNilType::to_string()
{
    return "nil";
}

MALTypePtr MALBoolType::deepCopy() {
    return std::shared_ptr<MALBoolType>(new MALBoolType(this->value));
}

bool MALBoolType::isEqualTo(MALTypePtr other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = std::dynamic_pointer_cast<MALBoolType>(other);
    return castOther->value == this->value;
}

std::string MALBoolType::to_string()
{
    return this->value ? "true" : "false";
}

MALTypePtr MalKeywordType::deepCopy() {
    return std::shared_ptr<MalKeywordType>(new MalKeywordType(this->value));
}

bool MalKeywordType::isEqualTo(MALTypePtr other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = std::dynamic_pointer_cast<MalKeywordType>(other);
    return castOther->value == this->value;
}

std::string MalKeywordType::to_string()
{
    return ":" + this->value;
}

MALTypePtr MALVectorType::deepCopy() {
    auto result = std::shared_ptr<MALVectorType>(new MALVectorType());
    for (auto p = this->values.begin(); p != this->values.end(); p++) {
        auto malType = *p;
        result->values.push_back(malType->deepCopy());
    }
    return result;
}

bool MALVectorType::isEqualTo(MALTypePtr other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = std::dynamic_pointer_cast<MALVectorType>(other);
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

MALTypePtr MALHashMapType::deepCopy() {
    auto result = std::shared_ptr<MALHashMapType>(new MALHashMapType());
    for (auto p = this->values.begin(); p != this->values.end(); p++) {
        auto malType = *p;
        result->values.insert(std::pair<MALTypePtr, MALTypePtr>(p->first->deepCopy(), p->second->deepCopy()));
    }
    return result;
}

bool MALHashMapType::isEqualTo(MALTypePtr other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = std::dynamic_pointer_cast<MALHashMapType>(other);
    if (castOther->size() != this->size()) {
        return false;
    }
    auto r = castOther->values.begin();
    for (auto p = this->values.begin(); p != this->values.end(); p++, r++) {
        auto otherSymbol = std::dynamic_pointer_cast<MALType>(r->first);
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

MALTypePtr MALFuncType::deepCopy() {
    return std::shared_ptr<MALFuncType>(new MALFuncType(this->name, this->fn));
}

bool MALFuncType::isEqualTo(MALTypePtr other)
{
    if (other->type() != this->type()) {
        return false;
    }
    auto castOther = std::dynamic_pointer_cast<MALFuncType>(other);

    //TODO TEST THIS:
    return &(*other) == this;
}

std::string MALFuncType::to_string()
{
    return "<function:" + this->name + ">";
}
