#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <map>
#include <functional>
#include <unordered_map>

#include "Type.h"

struct SymbolHash {
	std::size_t operator()(MALSymbolType* const& symbolKey) const noexcept
	{
		return std::hash<std::string> {}(symbolKey->name);
	}
};

struct SymbolEqualPred {
	bool operator()(const MALSymbolType* lhs, const MALSymbolType* rhs) const
	{
		return lhs->name == rhs->name;
	}
};

using EnvTable = std::unordered_map<MALSymbolType*, MALType*, SymbolHash, SymbolEqualPred>;

class Env {
	EnvTable data;
	Env* outer;
public:
	Env(Env* outer = nullptr) : outer(outer) {}
	// takes a symbol key and a mal value and adds to the data structure
	void set(MALSymbolType* symbol, MALType* funType);
	/*takes a symbol key and if the current environment contains that key then return the environment. 
	If no key is found and outer is not nil then call find (recurse) on the outer environment.*/
	MALType* find(MALSymbolType* symbol);
	/*takes a symbol key and uses the find method to locate the environment with the key, then returns the matching value. 
	If no key is found up the outer chain, then throws/raises a "not found" error.*/
	MALType* get(MALSymbolType* symbol);
};