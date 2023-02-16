#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <map>
#include <functional>
#include <unordered_map>

#pragma once

#define MALTypePtr std::shared_ptr<MALType>
#define MALListTypePtr std::shared_ptr<MALListType>
#define MALSymbolTypePtr std::shared_ptr<MALSymbolType>

class Env;

class MALType
{
public:
	enum Types {
		List,
		Number,
		Symbol,
		String,
		Bool,
		Nil,
		Keyword,
		Vector,
		HashMap,
		Function
	};

	static std::string typeToString(Types t);

	virtual std::string to_string() = 0;
	virtual MALType::Types type() const = 0;
	virtual const bool isContainer() const = 0;
	virtual bool isIteratable() { return false; };
	virtual bool isEqualTo(MALTypePtr other) = 0;
	virtual MALTypePtr deepCopy() = 0;
};

class MALAtomType : public MALType
{
public:
	virtual const bool isContainer() const override { return false; };
};

class MALContainerType : public MALType
{
public:
	virtual const bool isContainer() const override { return true; };
	virtual size_t size() = 0;
};

class MALIteratableContainerType : public MALContainerType
{
public:
	virtual bool isIteratable() override { return true; };
	virtual MALTypePtr getAt(size_t pos) = 0;
	virtual void setAt(size_t pos, MALTypePtr value) = 0;
};

class MALListType : public MALIteratableContainerType
{
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	std::vector<MALTypePtr> values;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::List; }
	virtual size_t size() override { return values.size(); };
	virtual MALTypePtr getAt(size_t pos) override { return values[pos]; };
	virtual void setAt(size_t pos, MALTypePtr value) { values[pos] = value; };
};

class MALNumberType : public MALAtomType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	double value;
	MALNumberType(double value) : value(value) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Number; }
};

class MALSymbolType : public MALAtomType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	std::string name;
	MALSymbolType(std::string name) : name(name) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Symbol; }
};

class MALStringType : public MALAtomType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	std::string value;
	MALStringType(std::string value) : value(value) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::String; }
};

class MALNilType : public MALAtomType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Nil; }
};

class MALBoolType : public MALAtomType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	bool value;
	MALBoolType(bool value) : value(value) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Bool; }
};

class MalKeywordType : public MALAtomType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	std::string value;
	MalKeywordType(std::string value) : value(value) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Keyword; }
};

class MALVectorType : public MALIteratableContainerType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	std::vector<MALTypePtr> values;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Vector; }
	virtual size_t size() override { return values.size(); };
	virtual MALTypePtr getAt(size_t pos) override { return values[pos]; };
	virtual void setAt(size_t pos, MALTypePtr value) { values[pos] = value; };
};

struct MALTypeHash {
	std::size_t operator()(MALTypePtr const& key) const noexcept
	{
		return (size_t)&*key;
	}
};
struct MALTypeEqualPred {
	bool operator()(MALTypePtr lhs, MALTypePtr rhs) const
	{
		return lhs->isEqualTo(rhs);
	}
};
class MALHashMapType : public MALContainerType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	std::unordered_map<MALTypePtr, MALTypePtr, MALTypeHash, MALTypeEqualPred> values;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::HashMap; }
	virtual size_t size() override { return values.size(); };
};

using MALFunctor = std::function<MALTypePtr (std::vector<MALTypePtr>)>;

class MALCallableType : public MALAtomType {
public:
	virtual bool isBuiltin() = 0;
	virtual MALType::Types type() const override { return  MALType::Types::Function; }
	std::string name;
	MALCallableType(std::string name) : name(name) {}
};

class MALFuncType : public MALCallableType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	MALFuncType(std::string name, std::shared_ptr<Env> env, MALListTypePtr bindingsList, MALTypePtr funcBody) 
		: MALCallableType(name), env(env), bindingsList(bindingsList), funcBody(funcBody) {}
	virtual std::string to_string() override;
	virtual bool isBuiltin() override { return false; };
	std::shared_ptr<Env> env;
	MALListTypePtr bindingsList;
	MALTypePtr funcBody;
};

class MALBuiltinFuncType : public MALCallableType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	MALFunctor fn;
	MALBuiltinFuncType(std::string name, MALFunctor fn) : MALCallableType(name), fn(fn) {}
	virtual std::string to_string() override;
	virtual bool isBuiltin() override { return true; };
};

