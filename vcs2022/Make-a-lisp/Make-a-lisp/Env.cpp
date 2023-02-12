#include "Env.h"

Env::Env(Env* outer) : outer(outer) {}

Env::Env(Env* outer, MALListType* bindings, std::vector<MALType*> exprs) : outer(outer)
{
    if (exprs.size() < bindings->values.size()) {
        throw std::runtime_error("Error: Number of parameters don't match function's parameters list size.");
    }
    for (int i = 0; i < bindings->values.size(); i++) {
        this->set((MALSymbolType*)bindings->values[i], exprs[i]);
    }
}

void Env::set(MALSymbolType* symbol, MALType* malType)
{
    auto got = this->data.find(symbol);
    if (got != this->data.end()) {
        this->data.erase(symbol);
    }
    this->data.insert(std::pair<MALSymbolType*, MALType*>(symbol, malType));
}

MALType* Env::find(MALSymbolType* symbol)
{
    auto got = this->data.find(symbol);
    if (got != this->data.end()) {
        return got->second;
    }
    if (this->outer == nullptr) {
        return new MALNilType();
    }
    return  this->outer->find(symbol);
}

MALType* Env::get(MALSymbolType* symbol)
{
    auto result = this->find(symbol);
    if (result->type() == MALType::Types::Nil) {
        throw std::runtime_error("ERROR: Env lookup: Symbol definition for '" + symbol->name + "' was not found.");
    }
    return result;
}

std::string Env::dataToString()
{
    std::string result = "";
    auto size = this->data.size();
    int i = 0;
    for (auto p = this->data.begin(); p != this->data.end(); p++, i++) {
        auto pair = *p;
        result += pair.first->to_string() + " ";
        result += pair.second->to_string();
        if (i != size - 1) {
            result += " ";
        }
    }
    return "{" + result + "}";
}
