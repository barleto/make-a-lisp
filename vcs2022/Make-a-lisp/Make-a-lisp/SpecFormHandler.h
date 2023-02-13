#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <assert.h>

#include "Reader.h"
#include "Type.h"
#include "Env.h"

MALType* EVAL(MALType* ast, EnvPtr env);
MALType* eval_ast(MALType* ast, EnvPtr env);

MALType* handleSpecialForms(MALListType* astList, EnvPtr env, MALSymbolType* lookupSymbol);

MALType* handleClosure(MALListType* astList, EnvPtr env);

