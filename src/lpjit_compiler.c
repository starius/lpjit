#include "lpjit_compiler.h"
#include "lpjit_lpeg.h"

void lpjit_compilerInit(CompilerState* cstate,
                        lua_State* L,
                        Pattern* pattern) {
    cstate->L = L;
    cstate->pattern = pattern;
    cstate->instruction = pattern->code;
    cstate->max_stack_size = lpeg_maxStackIndex(L);
}

int lpjit_offsetOf(CompilerState* cstate,
                   Instruction* instruction) {
    return instruction - cstate->pattern->code;
}

Instruction* lpjit_fromOffset(CompilerState* cstate,
                              int offset) {
    return cstate->pattern->code + offset;
}
