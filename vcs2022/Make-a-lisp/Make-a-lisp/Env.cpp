#include "Env.h"

std::shared_ptr<MALSymbolType> listArgsSymbol(new MALSymbolType("&"));

Env::Env(EnvPtr outer) : outer(outer) {}

Env::Env(EnvPtr outer, std::shared_ptr<MALSequenceType> bindings, std::vector<MALTypePtr> exprs) : outer(outer)
{
    bool foundSpecialChar = false;
    int realBindingsSize = 0;
    for (int i = 0; i < bindings->size(); i++) {
        auto symbol = bindings->getAt(i)->asSymbol();
        if (symbol->isEqualTo(listArgsSymbol)) {
            break;
        }
        realBindingsSize++;
    }
    if (exprs.size() < realBindingsSize) {
        throw std::runtime_error("Error: Number of parameters don't match function's parameters list size.");
    }
    for (int i = 0; i < bindings->size(); i++) {
        auto symbol = bindings->getAt(i)->asSymbol();
        if (symbol->isEqualTo(listArgsSymbol)) {
            if (i + 1 >= bindings->size()) {
                throw std::runtime_error("Error: Missing binding name after special character '&'");
            }
            auto argsList = std::shared_ptr<MALListType>(new MALListType());
            for (int j = i; j < exprs.size(); j++) {
                argsList->values.push_back(exprs[j]);
            }
            auto argsListSym = bindings->getAt(i + 1)->asSymbol();
            this->set(argsListSym, argsList);
            return;
        }
        this->set(bindings->getAt(i)->asSymbol(), exprs[i]);
    }
}

void Env::set(MALSymbolTypePtr symbol, MALTypePtr malType)
{
    auto got = this->data.find(symbol);
    if (got != this->data.end()) {
        this->data.erase(symbol);
    }
    this->data.insert(std::pair<MALSymbolTypePtr, MALTypePtr>(symbol, malType));
}

MALTypePtr Env::find(MALSymbolTypePtr symbol)
{
    auto got = this->data.find(symbol);
    if (got != this->data.end()) {
        return got->second;
    }
    if (this->outer == nullptr) {
        return nullptr;
    }
    return  this->outer->find(symbol);
}

MALTypePtr Env::get(MALSymbolTypePtr symbol)
{
    auto result = this->find(symbol);
    if (result == nullptr) {
        throw std::runtime_error("ERROR: Env lookup: Symbol definition for '" + symbol->name + "' was not found.");
    }
    return result;
}

std::string Env::print()
{
    std::string result = "";
    auto size = this->data.size();
    int i = 0;
    for (auto p = this->data.begin(); p != this->data.end(); p++, i++) {
        auto pair = *p;
        result += pair.first->to_string(true) + " ";
        result += pair.second->to_string(true);
        if (i != size - 1) {
            result += " ";
        }
    }
    std::cout << "AAAAAAAAAAA" << std::endl;
    return "{" + result + "}";
}
