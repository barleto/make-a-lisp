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

MALType* EVAL(MALType* ast, Env* env);
MALType* eval_ast(MALType* ast, Env* env);

MALType* handleSpecialForms(MALListType* astList, Env* env, MALSymbolType* lookupSymbol);

