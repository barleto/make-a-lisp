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
