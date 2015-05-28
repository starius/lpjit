#include "lpjit_compiler.h"

void lpjit_compilerInit(CompilerState* cstate,
                        lua_State* L,
                        Pattern* pattern) {
    cstate->L = L;
    cstate->nlabels = 0;
    cstate->pattern = pattern;
    cstate->instruction = pattern->code;
}

int lpjit_offsetOf(CompilerState* cstate,
                   Instruction* instruction) {
    return instruction - cstate->pattern->code;
}

Instruction* lpjit_fromOffset(CompilerState* cstate,
                              int offset) {
    return cstate->pattern->code + offset;
}
