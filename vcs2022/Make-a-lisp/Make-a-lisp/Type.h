#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>

enum MALTypes {
	Undefined,
	List,
	Int,
	Symbol,
};

#pragma once
class MALType
{

public: 
	virtual std::string to_string() = 0;
	const MALTypes type = MALTypes::Undefined;
	const bool isCompound = false;
}; 

class MALListType : public MALType
{
public:
	const MALTypes type = MALTypes::List;
	const bool isCompound = true;
	std::vector<MALType*> values;
	virtual std::string to_string() override;
};

class MALIntType : public MALType {
public:
	const MALTypes type = MALTypes::Int;
	int value;
	MALIntType(int value) : value(value) {}
	virtual std::string to_string() override;
};

class MALSymbolType : public MALType {
public:
	const MALTypes type = MALTypes::Symbol;
	std::string name;
	MALSymbolType(std::string& name) : name(name) {}
	virtual std::string to_string() override;
};
