#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <assert.h>

#include "Reader.h"
#include "Type.h"
#include "Env.h"

void checkArgsIsAtLeast(std::string name, MALType* type, int correctValue, int argCount);

void checkArgsIsAtLeast(std::string name, int correctValue, int argCount);

void checkArgsIs(std::string name, MALType* type, int correctValue, int argCount);

void assertMalType(MALType* element, MALType::Types type);

