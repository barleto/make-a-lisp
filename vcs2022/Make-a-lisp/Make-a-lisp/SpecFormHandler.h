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

MALTypePtr EVAL(MALTypePtr ast, EnvPtr env);
MALTypePtr eval_ast(MALTypePtr ast, EnvPtr env);

std::shared_ptr<SpecFormResult> handleSpecialForms(MALListTypePtr astList, EnvPtr env, MALSymbolTypePtr lookupSymbol);


