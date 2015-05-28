#ifndef lpjit_compiler_h
#define lpjit_compiler_h

#include "lpjit.h"
#include "lpjit_dasm.h"

typedef struct CompilerState {
    dasm_State* d;
    lua_State* L;
    int nlabels;
    Pattern* pattern;
    Instruction* instruction;
} CompilerState;

void lpjit_compilerInit(CompilerState* cstate,
                        lua_State* L,
                        Pattern* pattern);

int lpjit_offsetOf(CompilerState* cstate,
                   Instruction* instruction);

Instruction* lpjit_fromOffset(CompilerState* cstate,
                              int offset);

#endif
