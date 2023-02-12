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
		HashMap,
		Function
	};

	virtual std::string to_string() = 0;
	virtual MALType::Types type() const = 0;
	virtual const bool isCompound() const = 0;
	virtual bool isIteratable() { return false; };
}; 

class MALAtomType : public MALType
{
public:
	virtual const bool isCompound() const override { return false; };
};

class MALContainerType : public MALType
{
public:
	virtual const bool isCompound() const override { return true; };
	virtual size_t size() = 0;
};

class MALIteratableContainerType : public MALContainerType
{
public:
	virtual bool isIteratable() override { return true; };
	virtual MALType* getAt(size_t pos) = 0;
	virtual void setAt(size_t pos, MALType* value) = 0;
};

class MALListType : public MALIteratableContainerType
{
public:
	std::vector<MALType*> values;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::List; }
	virtual size_t size() override { return values.size(); };
	virtual MALType* getAt(size_t pos) override { return values[pos]; };
	virtual void setAt(size_t pos, MALType* value) { values[pos] = value; };
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

class MALVectorType : public MALIteratableContainerType {
public:
	std::vector<MALType*> values;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Vector; }
	virtual size_t size() override { return values.size(); };
	virtual MALType* getAt(size_t pos) override { return values[pos]; };
	virtual void setAt(size_t pos, MALType* value) { values[pos] = value; };
};

class MALHashMapType : public MALContainerType {
public:
	std::map<MALType*, MALType*> values;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::HashMap; }
	virtual size_t size() override { return values.size(); };
};

using MALFunctor = std::function<MALType* (std::vector<MALType*>)>;

class MALFuncType : public MALAtomType {
public:
	MALFuncType(std::string name, MALFunctor fn) : name(name), fn(fn) {}
	std::string name;
	MALFunctor fn;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Function; }
};

