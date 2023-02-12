#include "core.h"
MALType* add(std::vector<MALType*> args) {
    float result = 0;
    for (auto p = args.begin(); p != args.end(); p++) {
        result += ((MALNumberType*)*p)->value;
    }
    return new MALNumberType(result);
}

MALType* sub(std::vector<MALType*> args) {
    float result = args.size() > 0 ? ((MALNumberType*)args[0])->value : 0;
    for (auto p = args.begin() + 1; p != args.end(); p++) {
        result -= ((MALNumberType*)*p)->value;
    }
    return new MALNumberType(result);
}

MALType* mult(std::vector<MALType*> args) {
    float result = 1;
    for (auto p = args.begin(); p != args.end(); p++) {
        result *= ((MALNumberType*)*p)->value;
    }
    return new MALNumberType(result);
}

MALType* divs(std::vector<MALType*> args) {
    float result = args.size() > 0 ? ((MALNumberType*)args[0])->value : 0;
    for (auto p = args.begin() + 1; p != args.end(); p++) {
        result /= ((MALNumberType*)*p)->value;
    }
    return new MALNumberType(result);
}

MALType* print(std::vector<MALType*> args) {
    auto nil = new MALNilType();
    for (auto p = args.begin(); p != args.end(); p++) {
        std::cout << pr_str(*p) << " ";
    }
    std::cout << std::endl;
    return new MALNilType();
}

MALType* println(std::vector<MALType*> args) {
    auto nil = new MALNilType();
    for (auto p = args.begin(); p != args.end(); p++) {
        std::cout << pr_str(*p) << std::endl;
    }
    return new MALNilType();
}

MALType* list(std::vector<MALType*> args) {
    auto result = new MALListType();
    for (auto p = args.begin(); p != args.end(); p++) {
        result->values.push_back(*p);
    }
    return result;
}

MALType* isList(std::vector<MALType*> args) {
    checkArgsIsAtLeast("list?", 1, args.size());
    return new MALBoolType(args[0]->type() == MALType::Types::List);
}

MALType* isEmpty(std::vector<MALType*> args) {
    checkArgsIsAtLeast("empty?", 1, args.size());
    return new MALBoolType(args[0]->isContainer() && ((MALContainerType*)args[0])->size() == 0);
}

MALType* count(std::vector<MALType*> args) {
    checkArgsIsAtLeast("count", 1, args.size());
    if (args[0]->type() == MALType::Types::Nil) {
        return new MALNumberType(0);
    }
    if (!args[0]->isContainer()) {
        throw std::runtime_error("Error: Can only count container types. Found: '" + MALType::typeToString(args[0]->type()) + "'");
    }
    return new MALNumberType(((MALContainerType*)args[0])->size());
}

MALType* eq(std::vector<MALType*> args) {
    checkArgsIsAtLeast("=", 2, args.size());
    if (args[0]->type() != args[1]->type()) {
        return new MALBoolType(false);
    }
    return new MALBoolType(args[0]->isEqualTo(args[1]));
}

MALType* lt(std::vector<MALType*> args) {
    checkArgsIsAtLeast("<", 2, args.size());
    assertMalType(args[0], MALType::Types::Float);
    assertMalType(args[1], MALType::Types::Float);
    return new MALBoolType(((MALNumberType*)args[0])->value < ((MALNumberType*)args[1])->value);
}

MALType* lte(std::vector<MALType*> args) {
    checkArgsIsAtLeast("<=", 2, args.size());
    assertMalType(args[0], MALType::Types::Float);
    assertMalType(args[1], MALType::Types::Float);
    return new MALBoolType(((MALNumberType*)args[0])->value <= ((MALNumberType*)args[1])->value);
}

MALType* gt(std::vector<MALType*> args) {
    checkArgsIsAtLeast(">", 2, args.size());
    assertMalType(args[0], MALType::Types::Float);
    assertMalType(args[1], MALType::Types::Float);
    return new MALBoolType(((MALNumberType*)args[0])->value > ((MALNumberType*)args[1])->value);
}

MALType* gte(std::vector<MALType*> args) {
    checkArgsIsAtLeast(">=", 2, args.size());
    assertMalType(args[0], MALType::Types::Float);
    assertMalType(args[1], MALType::Types::Float);
    return new MALBoolType(((MALNumberType*)args[0])->value >= ((MALNumberType*)args[1])->value);
}

MALType* dlog(std::vector<MALType*> args) {
    auto nil = new MALNilType();
    checkArgsIsAtLeast("dlog", new MALNilType(), 1, args.size());
    std::cout << pr_str(args[0]) << std::endl;
    return args[0];
}

std::map<std::string, MALFunctor> ns = {
    {"+", add},
    {"-", sub},
    {"*", mult},
    {"/", divs},
    {"print", print},
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

void addBuiltInOperationsToEnv(Env* env)
{
    for (auto p = ns.begin(); p != ns.end(); p++) {
        env->set(new MALSymbolType(p->first), new MALFuncType(p->first, p->second));
    }
}
