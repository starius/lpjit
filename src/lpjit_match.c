#include "lpjit_match.h"
#include "lpjit_lpeg.h"

void lpjit_doubleCap(MatchState* mstate) {
    mstate->capture = lpeg_doubleCap(
            mstate->L,
            mstate->capture,
            mstate->cap_top,
            mstate->ptop);
    mstate->capsize = 2 * mstate->cap_top;
}

int lpjit_removedyncap(MatchState* mstate) {
    return lpeg_removedyncap(
            mstate->L,
            mstate->capture,
            mstate->cap_level,
            mstate->cap_top);
}
