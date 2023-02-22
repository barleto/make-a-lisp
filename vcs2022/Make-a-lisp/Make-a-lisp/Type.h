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
class MALSymbolType;
class MALListType;
class MALSequenceType;
class MALCallableType;
class MALBoolType;

class MALType : public std::enable_shared_from_this<MALType>
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
		Function,
		Atom,
	};

	static std::string typeToString(Types t);

	bool is_macro;
	virtual std::string to_string(bool print_readably) = 0;
	virtual MALType::Types type() const = 0;
	virtual const bool isContainer() const = 0;
	virtual bool isSequence() { return false; };
	virtual bool isEqualTo(MALTypePtr other) = 0;
	virtual MALTypePtr deepCopy() = 0;

	bool tryAsSymbol(std::shared_ptr<MALSymbolType>& ptr) {
		if (type() != Types::Symbol) {
			ptr = nullptr;
			return false;
		}
		ptr = std::dynamic_pointer_cast<MALSymbolType>(this->shared_from_this());
		return true;
	}

	bool tryAsList(std::shared_ptr<MALListType>& ptr) {
		if (type() != Types::List) {
			ptr = nullptr;
			return false;
		}
		ptr = std::dynamic_pointer_cast<MALListType>(this->shared_from_this());
		return true;
	}

	bool tryAsCallable(std::shared_ptr<MALCallableType>& ptr) {
		if (type() != Types::Function) {
			ptr = nullptr;
			return false;
		}
		ptr = std::dynamic_pointer_cast<MALCallableType>(this->shared_from_this());
		return true;
	}

	bool tryAsSequence(std::shared_ptr<MALSequenceType>& ptr) {
		if (!isSequence()) {
			ptr = nullptr;
			return false;
		}
		ptr = std::dynamic_pointer_cast<MALSequenceType>(this->shared_from_this());
		return true;
	}

	bool tryAsBool(std::shared_ptr<MALBoolType>& ptr) {
		if (type() != Types::Bool) {
			ptr = nullptr;
			return false;
		}
		ptr = std::dynamic_pointer_cast<MALBoolType>(this->shared_from_this());
		return true;
	}

	std::shared_ptr<MALSymbolType> asSymbol() {
		if (type() != Types::Symbol) {
			return nullptr;
		}
		return std::dynamic_pointer_cast<MALSymbolType>(this->shared_from_this());
	}

	std::shared_ptr<MALSequenceType> asSequence() {
		if (!isSequence()) {
			return nullptr;
		}
		return std::dynamic_pointer_cast<MALSequenceType>(this->shared_from_this());
	}

	std::shared_ptr<MALListType> asList() {
		if (type() != Types::List) {
			return nullptr;
		}
		return std::dynamic_pointer_cast<MALListType>(this->shared_from_this());
	}
};

class MALLeafType : public MALType
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

class MALSequenceType : public MALContainerType
{
public:
	virtual bool isSequence() override { return true; };
	virtual MALTypePtr getAt(size_t pos) = 0;
	virtual void setAt(size_t pos, MALTypePtr value) = 0;
	virtual void push_back(MALTypePtr value) = 0;
};

class MALListType : public MALSequenceType
{
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	std::vector<MALTypePtr> values;
	virtual std::string to_string(bool print_readably) override;
	virtual MALType::Types type() const override { return  MALType::Types::List; }
	virtual size_t size() override { return values.size(); };
	virtual MALTypePtr getAt(size_t pos) override { return values[pos]; };
	virtual void setAt(size_t pos, MALTypePtr value) { values[pos] = value; };
	virtual void push_back(MALTypePtr value) override {
		this->values.push_back(value);
	}
};

class MALNumberType : public MALLeafType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	double value;
	MALNumberType(double value) : value(value) {}
	virtual std::string to_string(bool print_readably) override;
	virtual MALType::Types type() const override { return  MALType::Types::Number; }
};

class MALSymbolType : public MALLeafType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	std::string name;
	MALSymbolType(std::string name) : name(name) {}
	virtual std::string to_string(bool print_readably) override;
	virtual MALType::Types type() const override { return  MALType::Types::Symbol; }
};

class MALStringType : public MALLeafType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	std::string value;
	MALStringType(std::string value) : value(value) {}
	virtual std::string to_string(bool print_readably) override;
	virtual MALType::Types type() const override { return  MALType::Types::String; }
};

class MALNilType : public MALLeafType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	virtual std::string to_string(bool print_readably) override;
	virtual MALType::Types type() const override { return  MALType::Types::Nil; }
	static std::shared_ptr<MALNilType> Nil;
};

class MALBoolType : public MALLeafType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	bool value;
	MALBoolType(bool value) : value(value) {}
	virtual std::string to_string(bool print_readably) override;
	virtual MALType::Types type() const override { return  MALType::Types::Bool; }
};

class MalKeywordType : public MALLeafType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	std::string value;
	MalKeywordType(std::string value) : value(value) {}
	virtual std::string to_string(bool print_readably) override;
	virtual MALType::Types type() const override { return  MALType::Types::Keyword; }
};

class MALVectorType : public MALSequenceType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	std::vector<MALTypePtr> values;
	virtual std::string to_string(bool print_readably) override;
	virtual MALType::Types type() const override { return  MALType::Types::Vector; }
	virtual size_t size() override { return values.size(); };
	virtual MALTypePtr getAt(size_t pos) override { return values[pos]; };
	virtual void setAt(size_t pos, MALTypePtr value) { values[pos] = value; };
	virtual void push_back(MALTypePtr value) override {
		this->values.push_back(value);
	}
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
	virtual std::string to_string(bool print_readably) override;
	virtual MALType::Types type() const override { return  MALType::Types::HashMap; }
	virtual size_t size() override { return values.size(); };
};

using MALFunctor = std::function<MALTypePtr (std::vector<MALTypePtr>, std::shared_ptr<Env>)>;

class MALCallableType : public MALLeafType {
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
	MALFuncType(std::string name, std::shared_ptr<Env> env, std::shared_ptr<MALSequenceType> bindingsList, MALTypePtr funcBody)
		: MALCallableType(name), env(env), bindingsList(bindingsList), funcBody(funcBody) {}
	virtual std::string to_string(bool print_readably) override;
	virtual bool isBuiltin() override { return false; };
	std::shared_ptr<Env> env;
	std::shared_ptr<MALSequenceType> bindingsList;
	MALTypePtr funcBody;
};

class MALBuiltinFuncType : public MALCallableType {
public:
	virtual MALTypePtr deepCopy() override;
	virtual bool isEqualTo(MALTypePtr other) override;
	MALFunctor fn;
	MALBuiltinFuncType(std::string name, MALFunctor fn) : MALCallableType(name), fn(fn) {}
	virtual std::string to_string(bool print_readably) override;
	virtual bool isBuiltin() override { return true; };
};

class MALAtomType : public MALLeafType {
public:
	virtual std::string to_string(bool print_readably) override ;
	virtual bool isEqualTo(MALTypePtr other) override;
	virtual MALTypePtr deepCopy() override;
	virtual MALType::Types type() const override { return  MALType::Types::Atom; };
	MALTypePtr ref;
	MALAtomType(MALTypePtr ref) : ref(ref) {};
};

class MALException{
public: 
	MALTypePtr errorValue;
	MALException(MALTypePtr errorVal) : errorValue(errorVal) {};
};

