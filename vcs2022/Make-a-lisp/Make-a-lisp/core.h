#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <assert.h>

#include "Reader.h"
#include "Type.h"
#include "Env.h"
#include "assert.h"
#include "assert.h"

MALTypePtr EVAL(MALTypePtr ast, EnvPtr env);

void addBuiltInOperationsToEnv(EnvPtr env);

