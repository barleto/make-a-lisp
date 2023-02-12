#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <map>
#include <functional>
#include <unordered_map>

#pragma once
class MALType
{
public: 
	enum Types {
		List,
		Float,
		Symbol,
		String,
		Bool,
		Nil,
		Keyword,
		Vector,
		HashMap
	};

	virtual std::string to_string() = 0;
	virtual MALType::Types type() const = 0;
	virtual const bool isCompound() const = 0;
}; 

class MALAtomType : public MALType
{
public:
	virtual const bool isCompound() const override { return false; };
};

class MALCompoundType : public MALType
{
public:
	virtual const bool isCompound() const override { return true; };
};

class MALListType : public MALCompoundType
{
public:
	std::vector<MALType*> values;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::List; }
	~MALListType() {
		for (auto p = values.begin(); p != values.end(); p++) {
			delete *p;
		}
	}
};

class MALNumberType : public MALAtomType {
public:
	float value;
	MALNumberType(float value) : value(value) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Float; }
};

class MALSymbolType : public MALAtomType {
public:
	std::string name;
	MALSymbolType(std::string& name) : name(name) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Symbol; }
};

class MALStringType : public MALAtomType {
public:
	std::string value;
	MALStringType(std::string value) : value(value) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::String; }
};

class MALNilType : public MALAtomType {
public:
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Nil; }
};

class MALBoolType : public MALAtomType {
public:
	bool value;
	MALBoolType(bool value) : value(value) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Bool; }
};

class MalKeywordType : public MALAtomType {
public:
	std::string value;
	MalKeywordType(std::string value) : value(value) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Keyword; }
};

class MALVectorType : public MALCompoundType {
public:
	std::vector<MALType*> values;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Vector; }
};

class MALHashMapType : public MALCompoundType {
public:
	std::map<MALType*, MALType*> values;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::HashMap; }
};

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

using MALFunc = std::function<MALType* (std::vector<MALType*>)>;
using Env = std::unordered_map<MALSymbolType*, MALFunc, SymbolHash, SymbolEqualPred>;