#include "core.h"
MALTypePtr add(std::vector<MALTypePtr> args) {
    double result = 0;
    for (auto p = args.begin(); p != args.end(); p++) {
        assertMalType(*p, MALType::Types::Number);
        auto number = std::dynamic_pointer_cast<MALNumberType>(*p);
        result += number->value;
    }
    return std::shared_ptr<MALNumberType>(new MALNumberType(result));
}

MALTypePtr sub(std::vector<MALTypePtr> args) {
    double result = args.size() > 0 ? (std::dynamic_pointer_cast<MALNumberType>(args[0]))->value : 0;
    for (auto p = args.begin() + 1; p != args.end(); p++) {
        assertMalType(*p, MALType::Types::Number);
        auto number = std::dynamic_pointer_cast<MALNumberType>(*p);
        result -= number->value;
    }
    return std::shared_ptr<MALNumberType>(new MALNumberType(result));
}

MALTypePtr mult(std::vector<MALTypePtr> args) {
    double result = 1;
    for (auto p = args.begin(); p != args.end(); p++) {
        assertMalType(*p, MALType::Types::Number);
        auto number = std::dynamic_pointer_cast<MALNumberType>(*p);
        result *= number->value;
    }
    return std::shared_ptr<MALNumberType>(new MALNumberType(result));
}

MALTypePtr divs(std::vector<MALTypePtr> args) {
    double result = args.size() > 0 ? (std::dynamic_pointer_cast<MALNumberType>(args[0]))->value : 0;
    for (auto p = args.begin() + 1; p != args.end(); p++) {
        assertMalType(*p, MALType::Types::Number);
        auto number = std::dynamic_pointer_cast<MALNumberType>(*p);
        result /= number->value;
    }
    return std::shared_ptr<MALNumberType>(new MALNumberType(result));
}

MALTypePtr print(std::vector<MALTypePtr> args) {
    auto nil = std::shared_ptr<MALNilType>(new MALNilType());
    for (auto p = args.begin(); p != args.end(); p++) {
        std::cout << pr_str(*p) << " ";
    }
    std::cout << std::endl;
    return std::shared_ptr<MALNilType>(new MALNilType());
}

MALTypePtr println(std::vector<MALTypePtr> args) {
    auto nil = std::shared_ptr<MALNilType>(new MALNilType());
    for (auto p = args.begin(); p != args.end(); p++) {
        std::cout << pr_str(*p) << std::endl;
    }
    return std::shared_ptr<MALNilType>(new MALNilType());
}

MALTypePtr list(std::vector<MALTypePtr> args) {
    auto result = std::shared_ptr<MALListType>(new MALListType());
    for (auto p = args.begin(); p != args.end(); p++) {
        result->values.push_back(*p);
    }
    return result;
}

MALTypePtr isList(std::vector<MALTypePtr> args) {
    checkArgsIsAtLeast("list?", 1, args.size());
    return std::shared_ptr<MALBoolType>(new MALBoolType(args[0]->type() == MALType::Types::List));
}

MALTypePtr isEmpty(std::vector<MALTypePtr> args) {
    checkArgsIsAtLeast("empty?", 1, args.size());
    return std::shared_ptr<MALBoolType>(new MALBoolType(args[0]->isContainer() && (std::dynamic_pointer_cast<MALContainerType>(args[0]))->size() == 0));
}

MALTypePtr count(std::vector<MALTypePtr> args) {
    checkArgsIsAtLeast("count", 1, args.size());
    if (args[0]->type() == MALType::Types::Nil) {
        return std::shared_ptr<MALNumberType>(new MALNumberType(0));
    }
    if (!args[0]->isContainer()) {
        throw std::runtime_error("Error: Can only count container types. Found: '" + MALType::typeToString(args[0]->type()) + "'");
    }
    return std::shared_ptr<MALNumberType>(new MALNumberType((std::dynamic_pointer_cast<MALContainerType>(args[0]))->size()));
}

MALTypePtr eq(std::vector<MALTypePtr> args) {
    checkArgsIsAtLeast("=", 2, args.size());
    if (args[0]->type() != args[1]->type()) {
        return std::shared_ptr<MALBoolType>(new MALBoolType(false));
    }
    return std::shared_ptr<MALBoolType>(new MALBoolType(args[0]->isEqualTo(args[1])));
}

MALTypePtr lt(std::vector<MALTypePtr> args) {
    checkArgsIsAtLeast("<", 2, args.size());
    assertMalType(args[0], MALType::Types::Number);
    assertMalType(args[1], MALType::Types::Number);
    auto l = std::dynamic_pointer_cast<MALNumberType>(args[0]);
    auto r = std::dynamic_pointer_cast<MALNumberType>(args[1]);
    return std::shared_ptr<MALBoolType>(new MALBoolType(l->value < r->value));
}

MALTypePtr lte(std::vector<MALTypePtr> args) {
    checkArgsIsAtLeast("<=", 2, args.size());
    assertMalType(args[0], MALType::Types::Number);
    assertMalType(args[1], MALType::Types::Number);
    auto l = std::dynamic_pointer_cast<MALNumberType>(args[0]);
    auto r = std::dynamic_pointer_cast<MALNumberType>(args[1]);
    return std::shared_ptr<MALBoolType>(new MALBoolType(l->value <= r->value));
}

MALTypePtr gt(std::vector<MALTypePtr> args) {
    checkArgsIsAtLeast(">", 2, args.size());
    assertMalType(args[0], MALType::Types::Number);
    assertMalType(args[1], MALType::Types::Number);
    auto l = std::dynamic_pointer_cast<MALNumberType>(args[0]);
    auto r = std::dynamic_pointer_cast<MALNumberType>(args[1]);
    return std::shared_ptr<MALBoolType>(new MALBoolType(l->value > r->value));
}

MALTypePtr gte(std::vector<MALTypePtr> args) {
    checkArgsIsAtLeast(">=", 2, args.size());
    assertMalType(args[0], MALType::Types::Number);
    assertMalType(args[1], MALType::Types::Number);
    auto l = std::dynamic_pointer_cast<MALNumberType>(args[0]);
    auto r = std::dynamic_pointer_cast<MALNumberType>(args[1]);
    return std::shared_ptr<MALBoolType>(new MALBoolType(l->value >= r->value));
}

MALTypePtr dlog(std::vector<MALTypePtr> args) {
    auto nil = std::shared_ptr<MALNilType>(new MALNilType());
    checkArgsIsAtLeast("dlog", std::shared_ptr<MALNilType>(new MALNilType()), 1, args.size());
    std::cout << pr_str(args[0]) << std::endl;
    return args[0];
}

std::map<std::string, MALFunctor> ns = {
    {"+", add},
    {"-", sub},
    {"*", mult},
    {"/", divs},
    {"print", print},
    {"prn", print},
    {"println", println},
    {"list", list},
    {"list?", isList},
    {"empty?", isEmpty},
    {"count", count},
    {"=", eq},
    {"==", eq},
    {"<", lt},
    {"<=", lte},
    {">", gt},
    {">=", gte},
    {"dlog", dlog},
};

void addBuiltInOperationsToEnv(EnvPtr env)
{
    for (auto p = ns.begin(); p != ns.end(); p++) {
        env->set(std::shared_ptr<MALSymbolType>(new MALSymbolType(p->first)), std::shared_ptr<MALBuiltinFuncType>(new MALBuiltinFuncType(p->first, p->second)));
    }
}
