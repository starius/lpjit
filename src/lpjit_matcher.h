#ifndef lpjit_matcher_h
#define lpjit_matcher_h

#include "lpjit.h"
#include "lpjit_dasm.h"

typedef void(*MatcherImpl)(MatchState*);

#define LPJIT_GIVEUP ((const char*)0)

typedef struct Matcher {
    void* buffer;
    size_t buffer_size;
    MatcherImpl impl;
    dasm_State* d;
    int pattern_ref;
} Matcher;

#endif
