#ifndef lpjit_compile_h
#define lpjit_compile_h

#include "lpjit.h"

void lpjit_compile(lua_State* L,
                   Matcher* matcher,
                   Pattern* pattern);

#endif
