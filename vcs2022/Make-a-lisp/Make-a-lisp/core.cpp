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

MALTypePtr prn(std::vector<MALTypePtr> args, EnvPtr env) {
    auto nil = MALNilType::Nil;
    if (args.size() <= 0) {
        return nil;
    }
    std::string result = "";
    int i = 0;
    for (i = 0; i < args.size() - 1; i++) {
        result += pr_str(args[i], true) + " ";
    }
    result += pr_str(args[i], true);
    std::cout << result;
    return nil;
}

MALTypePtr println(std::vector<MALTypePtr> args, EnvPtr env) {
    auto nil = MALNilType::Nil;
    if (args.size() <= 0) {
        return nil;
    }
    std::string result = "";
    int i = 0;
    for (i = 0; i < args.size() - 1; i++) {
        result += pr_str(args[i], false) + " ";
    }
    result += pr_str(args[i], false);
    std::cout << result << std::endl;
    return nil;
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
        return MALNilType::Nil;
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
    if (!args[1]->isSequence()) {
        throw std::runtime_error("Error: Second parameter of 'cons' must be a sequence (e.g. list or vector). Found: " + args[1]->to_string(true));
    }
    auto argAsSequence = args[1]->asSequence();
    MALListTypePtr newList(new MALListType());
    newList->values.push_back(args[0]);
    for (int i = 0; i < argAsSequence->size(); i++) {
        newList->values.push_back(argAsSequence->getAt(i));
    }
    return newList;
}

MALTypePtr concatList(std::vector<MALTypePtr> args, EnvPtr env) {
    MALListTypePtr newList(new MALListType());
    for (auto p = args.begin(); p != args.end(); p++) {
        if (!(*p)->isSequence()) {
            throw std::runtime_error("Error: All parameters of 'concat' must be a sequence (e.g. list or vector). Found: " + (*p)->to_string(true));
        }
        auto argAsSequence = (*p)->asSequence();
        for (int i = 0; i < argAsSequence->size(); i++) {
            newList->values.push_back(argAsSequence->getAt(i));
        }
    }
    return newList;
}

MALTypePtr pr_str_func(std::vector<MALTypePtr> args, EnvPtr env) {
    if (args.size() <= 0) {
        return std::shared_ptr<MALStringType>(new MALStringType(""));
    }
    std::string result = "";
    int i = 0;
    for (i = 0; i < args.size() - 1; i++) {
        result += pr_str(args[i], true) + " ";
    }
    result += pr_str(args[i], true);
    return std::shared_ptr<MALStringType>(new MALStringType(result));
}

MALTypePtr str(std::vector<MALTypePtr> args, EnvPtr env) {
    if (args.size() <= 0) {
        return std::shared_ptr<MALStringType>(new MALStringType(""));
    }
    std::string result = "";
    int i = 0;
    for (i = 0; i < args.size() - 1; i++) {
        result += pr_str(args[i], false);
    }
    result += pr_str(args[i], false);
    return std::shared_ptr<MALStringType>(new MALStringType(result));
}

MALTypePtr vec(std::vector<MALTypePtr> args, EnvPtr env) {
    auto result = std::shared_ptr<MALVectorType>(new MALVectorType());
    if (args.size() <= 0) {
        return result;
    }

    if (!args[0]->isSequence()) {
        throw std::runtime_error("Error: Parameter of 'vec' must be a sequence (e.g. list or vector). Found: " + args[0]->to_string(true));
    }
    auto argAsSequence = args[0]->asSequence();
    for (int i = 0; i < argAsSequence->size(); i++) {
        result->values.push_back(argAsSequence->getAt(i));
    }
    return result;
}

MALTypePtr nthFunc(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("nthFunc", 2, args.size());
    assertMalType(args[1], MALType::Types::Number);
    auto index = std::dynamic_pointer_cast<MALNumberType>(args[1])->value;
    std::shared_ptr<MALSequenceType> astAsSequence;
    if (!args[0]->tryAsSequence(astAsSequence)) {
        throw std::runtime_error("Error: First parameter of 'nth' must be a sequence (e.g. list or vector). Found: " + args[0]->to_string(true));
    }
    if (index >= astAsSequence->size() || index < 0) {
        throw std::runtime_error("Error: Index '"+std::to_string((int)index)+"' is not a valid index of sequence '" + astAsSequence->to_string(true) + "'");
    }
    return astAsSequence->getAt(index);
}

MALTypePtr firstFunc(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("firstFunc", 1, args.size());
    std::shared_ptr<MALSequenceType> astAsSequence;
    if (args[0]->type() == MALType::Types::Nil) {
        return MALNilType::Nil;
    }
    if (!args[0]->tryAsSequence(astAsSequence)) {
        throw std::runtime_error("Error: Parameter of 'first' must be a sequence (e.g. list or vector). Found: " + args[0]->to_string(true));
    }
    if (astAsSequence->size() <= 0) {
        return MALNilType::Nil;
    }
    return astAsSequence->getAt(0);
}

MALTypePtr restFunc(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("rest", 1, args.size());
    std::shared_ptr<MALSequenceType> astAsSequence;
    MALListTypePtr newList = MALListTypePtr(new MALListType());
    if (args[0]->type() == MALType::Types::Nil) {
        return newList;
    }
    if (!args[0]->tryAsSequence(astAsSequence)) {
        throw std::runtime_error("Error: Parameter of 'rest' must be a sequence (e.g. list or vector). Found: " + args[0]->to_string(true));
    }
    if (astAsSequence->size() <= 0) {
        return newList;
    }
    for (int i = 1; i < astAsSequence->size(); i++) {
        newList->push_back(astAsSequence->getAt(i));
    }
    return newList;
}

MALTypePtr throwFunc(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsNumber("throw", 1, args.size());
    throw MALException(args[0]);
}

MALTypePtr applyFunc(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsIsAtLeast("apply", 1, args.size());
    assertMalType(args[0], MALType::Types::Function);
    MALListTypePtr result(new MALListType());
    result->values.push_back(args[0]);
    std::shared_ptr<MALSequenceType> astAsSequence;
    for (int i = 1; i < args.size(); i++) {
        if (args[i]->tryAsSequence(astAsSequence)) {
            for (int j = 0; j < astAsSequence->size(); j++) {
                result->values.push_back(astAsSequence->getAt(j));
            }
        }
        else {
            result->values.push_back(args[i]);
        }
    }
    return EVAL(result, env);
}

MALTypePtr mapFunc(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsNumber("map", 2, args.size());
    assertMalType(args[0], MALType::Types::Function);
    std::shared_ptr<MALSequenceType> astAsSequence;
    if (!args[1]->tryAsSequence(astAsSequence)) {
        throw std::runtime_error("ERROR: Second parameter of 'map' must be a sequence.");
    }
    MALListTypePtr result(new MALListType());
    MALListTypePtr funcCall(new MALListType());
    funcCall->values.push_back(args[0]);
    funcCall->values.push_back(args[0]);
    for (int i = 0; i < astAsSequence->size(); i++) {
        funcCall->setAt(1, astAsSequence->getAt(i)); //set correct argument to func call
        result->values.push_back(EVAL(funcCall,env)); //call fun on element at position i
    }
    return result;
}

MALTypePtr isNilFunc(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsNumber("nil?", 1, args.size());
    return std::shared_ptr<MALBoolType>(new MALBoolType(args[0]->type() == MALType::Types::Nil));
}

MALTypePtr isTrueFunc(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsNumber("true?", 1, args.size());
    std::shared_ptr<MALBoolType> astAsBool;
    bool result = args[0]->tryAsBool(astAsBool) && astAsBool->value == true;
    return std::shared_ptr<MALBoolType>(new MALBoolType(result));
}

MALTypePtr isFalseFunc(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsNumber("false?", 1, args.size());
    std::shared_ptr<MALBoolType> astAsBool;
    bool result = args[0]->tryAsBool(astAsBool) && astAsBool->value == false;
    return std::shared_ptr<MALBoolType>(new MALBoolType(result));
}

MALTypePtr isSymbolFunc(std::vector<MALTypePtr> args, EnvPtr env) {
    checkArgsNumber("symbol?", 1, args.size());
    return std::shared_ptr<MALBoolType>(new MALBoolType(args[0]->type() == MALType::Types::Symbol));
}

std::map<std::string, MALFunctor> ns = {
    {"+", add},
    {"-", sub},
    {"*", mult},
    {"/", divs},
    {"prn", prn},
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
    {"atom", atom},
    {"atom?", isAtom},
    {"deref", deref},
    {"reset!", resetBang},
    {"swap!", swapBang},
    {"eval", evalSpecialForm},
    {"cons", cons},
    {"concat", concatList},
    {"pr-str", pr_str_func},
    {"str", str},
    {"vec", vec},
    {"nth", nthFunc},
    {"first", firstFunc},
    {"rest", restFunc},
    {"throw", throwFunc},
    {"apply", applyFunc},
    {"map", mapFunc},
    {"nil?", isNilFunc},
    {"true?", isTrueFunc},
    {"false?", isFalseFunc},
    {"symbol?", isSymbolFunc},
};

void addBuiltInOperationsToEnv(EnvPtr env)
{
    for (auto p = ns.begin(); p != ns.end(); p++) {
        env->set(std::shared_ptr<MALSymbolType>(new MALSymbolType(p->first)), std::shared_ptr<MALBuiltinFuncType>(new MALBuiltinFuncType(p->first, p->second)));
    }
}
