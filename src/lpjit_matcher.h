#ifndef lpjit_matcher_h
#define lpjit_matcher_h

#include "lpjit.h"
#include "lpjit_dasm.h"

typedef void(*MatcherImpl)(MatchState*);

#define LPJIT_GIVEUP ((const char*)0)
#define LPJIT_STACKOVERFLOW ((const char*)1)

#define LPJIT_FAIL (-1)

typedef struct Matcher {
    void* buffer;
    size_t buffer_size;
    MatcherImpl impl;
    dasm_State* d;
    int pattern_ref;
} Matcher;

#endif
