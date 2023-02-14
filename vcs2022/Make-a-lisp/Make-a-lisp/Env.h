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

#define EnvPtr std::shared_ptr<Env>
struct SymbolHash {
	std::size_t operator()(MALSymbolTypePtr const& symbolKey) const noexcept
	{
		return std::hash<std::string> {}(symbolKey->name);
	}
};
struct SymbolEqualPred {
	bool operator()(const MALSymbolTypePtr lhs, const MALSymbolTypePtr rhs) const
	{
		return lhs->name == rhs->name;
	}
};

using EnvTable = std::unordered_map<MALSymbolTypePtr, MALTypePtr, SymbolHash, SymbolEqualPred>;

class Env {
	EnvTable data;
	EnvPtr outer = nullptr;
public:
	Env(EnvPtr outer = nullptr);
	Env(EnvPtr outer, MALListTypePtr bindings, std::vector<MALTypePtr> exprs);
	// takes a symbol key and a mal value and adds to the data structure
	void set(MALSymbolTypePtr symbol, MALTypePtr funType);
	/*takes a symbol key and if the current environment contains that key then return the environment. 
	If no key is found and outer is not nil then call find (recurse) on the outer environment.*/
	MALTypePtr find(MALSymbolTypePtr symbol);
	/*takes a symbol key and uses the find method to locate the environment with the key, then returns the matching value. 
	If no key is found up the outer chain, then throws/raises a "not found" error.*/
	MALTypePtr get(MALSymbolTypePtr symbol);
	std::string dataToString();
};
