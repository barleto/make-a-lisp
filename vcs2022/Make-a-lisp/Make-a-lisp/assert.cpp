#include "assert.h"

void checkArgsIsAtLeast(std::string name, MALTypePtr type, int correctValue, int argCount) {
    if (argCount < correctValue) {
        throw std::runtime_error("ERROR: '" + name + "' need at least " + std::to_string(correctValue) + " params. Params found: '" + type->to_string() + "'");
    }
}

void checkArgsIsAtLeast(std::string name, int correctValue, int argCount) {
    if (argCount < correctValue) {
        throw std::runtime_error("ERROR: '" + name + "' need at least " + std::to_string(correctValue) + " params.");
    }
}

void checkArgsIs(std::string name, MALTypePtr type, int correctValue, int argCount) {
    if (argCount != correctValue) {
        throw std::runtime_error("ERROR: '" + name + "' need exactly " + std::to_string(correctValue) + " params. Params found: '" + type->to_string() + "'");
    }
}

void assertMalType(MALTypePtr element, MALType::Types type)
{
    if (element->type() != type) {
        throw std::runtime_error("ERROR: Expected '"+ MALType::typeToString(type) +"', but found '"+ MALType::typeToString(element->type()) +"'");
    }
}
