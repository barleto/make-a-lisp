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

void REPL(std::string& input, const char* const& history_path);

std::shared_ptr<HandleSpecialFormResult> handleSpecialForms(MALListTypePtr astList, EnvPtr env);


