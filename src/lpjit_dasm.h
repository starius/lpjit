#ifndef lpjit_dasm_h
#define lpjit_dasm_h

// set dasm options and include dasm headers

typedef struct CompilerState CompilerState;

#define Dst_DECL CompilerState* cstate
#define Dst_REF cstate->d

#include "dynasm/dasm_proto.h"

#endif
