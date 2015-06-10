#ifndef lpjit_matcher_h
#define lpjit_matcher_h

#include "lpjit.h"
#include "lpjit_dasm.h"

typedef void(*MatchStateMethod)(MatchState*);

#define LPJIT_END 0
#define LPJIT_GIVEUP 1
#define LPJIT_STACKOVERFLOW 2
#define LPJIT_THROW 3

#define LPJIT_FAIL (1)

typedef struct Matcher {
    void* buffer;
    size_t buffer_size;
    MatchStateMethod impl;
    MatchStateMethod error; // doesn't use its argument
    dasm_State* d;
    int pattern_ref;
} Matcher;

#endif
