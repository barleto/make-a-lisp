#include "core.h"

MALTypePtr add(std::vector<MALTypePtr> args, EnvPtr env) {
    double result = 0;
    for (auto p = args.begin(); p != args.end(); p++) {
        assertMalType(*p, MALType::Types::Number);
        auto number = std::dynamic_pointer_cast<MALNumberType>(*p);
        result += number->value;
    }
    return std::shared_ptr<MALNumberType>(new MALNumberType(result));
}

MALTypePtr sub(std::vector<MALTypePtr> args, EnvPtr env) {
    double result = args.size() > 0 ? (std::dynamic_pointer_cast<MALNumberType>(args[0]))->value : 0;
    for (auto p = args.begin() + 1; p != args.end(); p++) {
        assertMalType(*p, MALType::Types::Number);
        auto number = std::dynamic_pointer_cast<MALNumberType>(*p);
        result -= number->value;
    }
    return std::shared_ptr<MALNumberType>(new MALNumberType(result));
}

MALTypePtr mult(std::vector<MALTypePtr> args, EnvPtr env) {
    double result = 1;
    for (auto p = args.begin(); p != args.end(); p++) {
        assertMalType(*p, MALType::Types::Number);
        auto number = std::dynamic_pointer_cast<MALNumberType>(*p);
        result *= number->value;
    }
    return std::shared_ptr<MALNumberType>(new MALNumberType(result));
}

MALTypePtr divs(std::vector<MALTypePtr> args, EnvPtr env) {
    double result = args.size() > 0 ? (std::dynamic_pointer_cast<MALNumberType>(args[0]))->value : 0;
    for (auto p = args.begin() + 1; p != args.end(); p++) {
        assertMalType(*p, MALType::Types::Number);
        auto number = std::dynamic_pointer_cast<MALNumberType>(*p);
        result /= number->value;
    }
    return std::shared_ptr<MALNumberType>(new MALNumberType(result));
}

MALTypePtr print(std::vector<MALTypePtr> args, EnvPtr env) {
    auto nil = std::shared_ptr<MALNilType>(new MALNilType());
    for (auto p = args.begin(); p != args.end(); p++) {
        std::cout << pr_str(*p) << " ";
    }
    std::cout << std::endl;
    return std::shared_ptr<MALNilType>(new MALNilType());
}

MALTypePtr println(std::vector<MALTypePtr> args, EnvPtr env) {
    auto nil = std::shared_ptr<MALNilType>(new MALNilType());
    for (auto p = args.begin(); p != args.end(); p++) {
        std::cout << pr_str(*p) << std::endl;
    }
    return std::shared_ptr<MALNilType>(new MALNilType());
}

MALTypePtr list(std::vector<MALTypePtr> args, EnvPtr env) {
    auto result = std::shared_ptr<MALListType>(new MALListType());
    for (auto p = args.begin(); p != args.end(); p++) {
        result->values.push_back(*p);
    }
    return result;
}

MALTypePtr isList(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("list?", 1, args.size());
    return std::shared_ptr<MALBoolType>(new MALBoolType(args[0]->type() == MALType::Types::List));
}

MALTypePtr isEmpty(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("empty?", 1, args.size());
    return std::shared_ptr<MALBoolType>(new MALBoolType(args[0]->isContainer() && (std::dynamic_pointer_cast<MALContainerType>(args[0]))->size() == 0));
}

MALTypePtr count(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("count", 1, args.size());
    if (args[0]->type() == MALType::Types::Nil) {
        return std::shared_ptr<MALNumberType>(new MALNumberType(0));
    }
    if (!args[0]->isContainer()) {
        throw std::runtime_error("Error: Can only count container types. Found: '" + MALType::typeToString(args[0]->type()) + "'");
    }
    return std::shared_ptr<MALNumberType>(new MALNumberType((std::dynamic_pointer_cast<MALContainerType>(args[0]))->size()));
}

MALTypePtr eq(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("=", 2, args.size());
    if (args[0]->type() != args[1]->type()) {
        return std::shared_ptr<MALBoolType>(new MALBoolType(false));
    }
    return std::shared_ptr<MALBoolType>(new MALBoolType(args[0]->isEqualTo(args[1])));
}

MALTypePtr lt(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("<", 2, args.size());
    assertMalType(args[0], MALType::Types::Number);
    assertMalType(args[1], MALType::Types::Number);
    auto l = std::dynamic_pointer_cast<MALNumberType>(args[0]);
    auto r = std::dynamic_pointer_cast<MALNumberType>(args[1]);
    return std::shared_ptr<MALBoolType>(new MALBoolType(l->value < r->value));
}

MALTypePtr lte(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("<=", 2, args.size());
    assertMalType(args[0], MALType::Types::Number);
    assertMalType(args[1], MALType::Types::Number);
    auto l = std::dynamic_pointer_cast<MALNumberType>(args[0]);
    auto r = std::dynamic_pointer_cast<MALNumberType>(args[1]);
    return std::shared_ptr<MALBoolType>(new MALBoolType(l->value <= r->value));
}

MALTypePtr gt(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast(">", 2, args.size());
    assertMalType(args[0], MALType::Types::Number);
    assertMalType(args[1], MALType::Types::Number);
    auto l = std::dynamic_pointer_cast<MALNumberType>(args[0]);
    auto r = std::dynamic_pointer_cast<MALNumberType>(args[1]);
    return std::shared_ptr<MALBoolType>(new MALBoolType(l->value > r->value));
}

MALTypePtr gte(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast(">=", 2, args.size());
    assertMalType(args[0], MALType::Types::Number);
    assertMalType(args[1], MALType::Types::Number);
    auto l = std::dynamic_pointer_cast<MALNumberType>(args[0]);
    auto r = std::dynamic_pointer_cast<MALNumberType>(args[1]);
    return std::shared_ptr<MALBoolType>(new MALBoolType(l->value >= r->value));
}

MALTypePtr readString(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("read-string", 1, args.size());
    assertMalType(args[0], MALType::Types::String);
    auto stringType = std::dynamic_pointer_cast<MALStringType>(args[0]);
    auto readString = stringType->value;
    auto result = read_str(readString);
    return result;
}

MALTypePtr slurp(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("slurp", 1, args.size());
    assertMalType(args[0], MALType::Types::String);
    auto stringType = std::dynamic_pointer_cast<MALStringType>(args[0]);
    auto fileName = stringType->value;

    std::ifstream file(fileName);
    if (!file) {
        throw std::runtime_error("Error: File named '" + fileName + "' could not be opened.");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    return std::shared_ptr<MALStringType>(new MALStringType(buffer.str()));
}

MALTypePtr concatStr(std::vector<MALTypePtr> args, EnvPtr env) {
    std::string result = "";
    //TODO
    for (auto p = args.begin(); p != args.end(); p++) {
        assertMalType(*p, MALType::Types::String);
        auto stringType = std::dynamic_pointer_cast<MALStringType>(*p);
        result += stringType->value;
    }
    return std::shared_ptr<MALStringType>(new MALStringType(result));
}

MALTypePtr atom(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsNumber("atom", 1, args.size());
    return std::shared_ptr<MALAtomType>(new MALAtomType(args[0]));
}

MALTypePtr isAtom(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsNumber("atom?", 1, args.size());
    return std::shared_ptr<MALBoolType>(new MALBoolType(args[0]->type() == MALType::Types::Atom));
}

MALTypePtr deref(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsNumber("deref", 1, args.size());
    assertMalType(args[0], MALType::Types::Atom);
    auto atom = std::dynamic_pointer_cast<MALAtomType>(args[0]);
    if (atom->ref == nullptr) {
        return std::shared_ptr<MALNilType>(new MALNilType());
    }
    return atom->ref;
}

MALTypePtr resetBang(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsNumber("reset!", 2, args.size());
    assertMalType(args[0], MALType::Types::Atom);
    auto atom = std::dynamic_pointer_cast<MALAtomType>(args[0]);
    atom->ref = args[1];
    return args[1];
}

MALTypePtr swapBang(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("reset!", 2, args.size());
    assertMalType(args[0], MALType::Types::Atom);
    assertMalType(args[1], MALType::Types::Function);
    auto atom = std::dynamic_pointer_cast<MALAtomType>(args[0]);
    auto func = std::dynamic_pointer_cast<MALCallableType>(args[1]);
    std::shared_ptr<MALListType> callFuncAst(new MALListType());
    callFuncAst->values.push_back(func);
    callFuncAst->values.push_back(atom->ref);
    for (int i = 2; i < args.size(); i++) {
        callFuncAst->values.push_back(args[i]);
    }
    auto result = EVAL(callFuncAst,env);
    atom->ref = result;
    return result;
}

MALTypePtr evalSpecialForm(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("eval", 1, args.size());
    auto res = EVAL(args[0], nullptr);
    return res;
}


MALTypePtr cons(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsNumber("cons", 2, args.size());
    assertMalType(args[1], MALType::Types::List);
    auto arg2List = args[1]->asList();
    MALListTypePtr newList(new MALListType());
    newList->values.push_back(args[0]);
    newList->values.insert(newList->values.end(), arg2List->values.begin(), arg2List->values.end());
    return newList;
}

MALTypePtr concatList(std::vector<MALTypePtr> args, EnvPtr env) {
    MALListTypePtr newList(new MALListType());
    for (auto p = args.begin(); p != args.end(); p++) {
        assertMalType(*p, MALType::Types::List);
        auto arg2List = std::dynamic_pointer_cast<MALListType>(*p);
        for (auto pp = arg2List->values.begin(); pp != arg2List->values.end(); pp++) {
            newList->values.push_back(*pp);
        }
    }
    return newList;
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
    {"read-string", readString},
    {"slurp", slurp},
    {"str", concatStr},
    {"atom", atom},
    {"atom?", isAtom},
    {"deref", deref},
    {"reset!", resetBang},
    {"swap!", swapBang},
    {"eval", evalSpecialForm},
    {"cons", cons},
    {"concat", concatList},
};

void addBuiltInOperationsToEnv(EnvPtr env)
{
    for (auto p = ns.begin(); p != ns.end(); p++) {
        env->set(std::shared_ptr<MALSymbolType>(new MALSymbolType(p->first)), std::shared_ptr<MALBuiltinFuncType>(new MALBuiltinFuncType(p->first, p->second)));
    }
}
