#ifndef lpjit_h
#define lpjit_h

// lua
#include <lua.h>

#include "lpjit_lpeg.h"

#define LPJIT_STACKFRAME_SIZE 3 * sizeof(void*)

typedef struct MatchState {
    const char* subject_begin; // o
    const char* subject_current; // s, start position, ASM<->C
    const char* subject_end; // e
    lua_State* L;
    Capture* capture;
    long long int capsize; // C->ASM (stored only in C)
    long long int ptop; // C
    long long int cap_top; // ASM<->C
    long long int cap_level; // ASM->C
    long long int n_dyncap; // ASM<->C
    long long int stacksize; // ASM
    void* stack_pos; // ASM -> ASM
    void* return_address; // ASM
    void* stack_backup;
    long long int runtime_result;
    int result; // ASM->C
    int inside; // in JITed code

    void(*error)(MatchState*);
} MatchState;

typedef struct Matcher Matcher;

int lpjit_pushMatcher(lua_State *L);

Matcher* lpjit_checkMatcher(lua_State *L, int index);

void lpjit_match(const Matcher* matcher, MatchState* state);

int lua_lpjit_match(lua_State* L);

#endif
