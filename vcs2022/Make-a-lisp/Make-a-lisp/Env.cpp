#include "Env.h"

void Env::set(MALSymbolType* symbol, MALType* malType)
{
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
