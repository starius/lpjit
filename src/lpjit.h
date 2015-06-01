#ifndef lpjit_h
#define lpjit_h

// lua
#include <lua.h>

#include "lpjit_lpeg.h"

typedef struct MatchState {
    const char* subject_begin; // o
    const char* subject_current; // s, start position
    const char* subject_end; // e
    lua_State* L;
    Capture* capture;
    long long int capsize;
    long long int ptop;
    long long int cap_top; // ASM->C
    void* stack_pos; // ASM -> ASM
} MatchState;

typedef struct Matcher Matcher;

int lpjit_pushMatcher(lua_State *L);

Matcher* lpjit_checkMatcher(lua_State *L, int index);

void lpjit_match(lua_State *L, const Matcher* matcher,
                 MatchState* state);

int lua_lpjit_match(lua_State* L);

#endif
