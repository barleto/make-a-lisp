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

	static std::string typeToString(Types t);

	virtual std::string to_string() = 0;
	virtual MALType::Types type() const = 0;
	virtual const bool isContainer() const = 0;
	virtual bool isIteratable() { return false; };
	virtual bool isEqualTo(MALType* other) = 0;
	virtual MALType* deepCopy() = 0;
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
	virtual MALType* getAt(size_t pos) = 0;
	virtual void setAt(size_t pos, MALType* value) = 0;
};

class MALListType : public MALIteratableContainerType
{
public:
	virtual MALType* deepCopy() override;
	virtual bool isEqualTo(MALType* other) override;
	std::vector<MALType*> values;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::List; }
	virtual size_t size() override { return values.size(); };
	virtual MALType* getAt(size_t pos) override { return values[pos]; };
	virtual void setAt(size_t pos, MALType* value) { values[pos] = value; };
};

class MALNumberType : public MALAtomType {
public:
	virtual MALType* deepCopy() override;
	virtual bool isEqualTo(MALType* other) override;
	float value;
	MALNumberType(float value) : value(value) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Float; }
};

class MALSymbolType : public MALAtomType {
public:
	virtual MALType* deepCopy() override;
	virtual bool isEqualTo(MALType* other) override;
	std::string name;
	MALSymbolType(std::string name) : name(name) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Symbol; }
};

class MALStringType : public MALAtomType {
public:
	virtual MALType* deepCopy() override;
	virtual bool isEqualTo(MALType* other) override;
	std::string value;
	MALStringType(std::string value) : value(value) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::String; }
};

class MALNilType : public MALAtomType {
public:
	virtual MALType* deepCopy() override;
	virtual bool isEqualTo(MALType* other) override;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Nil; }
};

class MALBoolType : public MALAtomType {
public:
	virtual MALType* deepCopy() override;
	virtual bool isEqualTo(MALType* other) override;
	bool value;
	MALBoolType(bool value) : value(value) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Bool; }
};

class MalKeywordType : public MALAtomType {
public:
	virtual MALType* deepCopy() override;
	virtual bool isEqualTo(MALType* other) override;
	std::string value;
	MalKeywordType(std::string value) : value(value) {}
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Keyword; }
};

class MALVectorType : public MALIteratableContainerType {
public:
	virtual MALType* deepCopy() override;
	virtual bool isEqualTo(MALType* other) override;
	std::vector<MALType*> values;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Vector; }
	virtual size_t size() override { return values.size(); };
	virtual MALType* getAt(size_t pos) override { return values[pos]; };
	virtual void setAt(size_t pos, MALType* value) { values[pos] = value; };
};

struct MALTypeHash {
	std::size_t operator()(MALType* const& key) const noexcept
	{
		return (size_t)key;
	}
};
struct MALTypeEqualPred {
	bool operator()(MALType* lhs, MALType* rhs) const
	{
		return lhs->isEqualTo(rhs);
	}
};
class MALHashMapType : public MALContainerType {
public:
	virtual MALType* deepCopy() override;
	virtual bool isEqualTo(MALType* other) override;
	std::unordered_map<MALType*, MALType*, MALTypeHash, MALTypeEqualPred> values;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::HashMap; }
	virtual size_t size() override { return values.size(); };
};

using MALFunctor = std::function<MALType* (std::vector<MALType*>)>;
class MALFuncType : public MALAtomType {
public:
	virtual MALType* deepCopy() override;
	virtual bool isEqualTo(MALType* other) override;
	MALFuncType(std::string name, MALFunctor fn) : name(name), fn(fn) {}
	std::string name;
	MALFunctor fn;
	virtual std::string to_string() override;
	virtual MALType::Types type() const override { return  MALType::Types::Function; }
};
