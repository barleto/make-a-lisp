#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <map>

enum MALTypes {
	Undefined,
	List,
	Int,
	Symbol,
	String,
	Bool,
	Nil,
	Keyword,
	Vectors,
	HashMap
};

#pragma once
class MALType
{

public: 
	virtual std::string to_string() = 0;
	const MALTypes type = MALTypes::Undefined;
	virtual const bool isCompound() = 0;
}; 

class MALAtomType : public MALType
{
public:
	virtual std::string to_string() = 0;
	virtual const bool isCompound() override;
};

class MALCompoundType : public MALType
{
public:
	virtual std::string to_string() = 0;
	virtual const bool isCompound() override;
};

class MALListType : public MALCompoundType
{
public:
	const MALTypes type = MALTypes::List;
	const bool isCompound = true;
	std::vector<MALType*> values;
	virtual std::string to_string() override;
};

class MALIntType : public MALAtomType {
public:
	const MALTypes type = MALTypes::Int;
	int value;
	MALIntType(int value) : value(value) {}
	virtual std::string to_string() override;
};

class MALSymbolType : public MALAtomType {
public:
	const MALTypes type = MALTypes::Symbol;
	std::string name;
	MALSymbolType(std::string& name) : name(name) {}
	virtual std::string to_string() override;
};

class MALStringType : public MALAtomType {
public:
	const MALTypes type = MALTypes::String;
	std::string value;
	MALStringType(std::string value) : value(value) {}
	virtual std::string to_string() override;
};

class MALNilType : public MALAtomType {
public:
	const MALTypes type = MALTypes::Nil;
	virtual std::string to_string() override;
};

class MALBoolType : public MALAtomType {
public:
	const MALTypes type = MALTypes::Nil;
	bool value;
	MALBoolType(bool value) : value(value) {}
	virtual std::string to_string() override;
};

class MalKeywordType : public MALAtomType {
public:
	const MALTypes type = MALTypes::Keyword;
	std::string value;
	MalKeywordType(std::string value) : value(value) {}
	virtual std::string to_string() override;
};

class MALVectorType : public MALCompoundType {
public:
	const MALTypes type = MALTypes::Vectors;
	std::vector<MALType*> values;
	virtual std::string to_string() override;
};

class MALHashMapType : public MALCompoundType {
public:
	const MALTypes type = MALTypes::Vectors;
	std::map<MALType*, MALType*> values;
	virtual std::string to_string() override;
};