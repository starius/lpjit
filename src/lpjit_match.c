#include "lpjit_match.h"
#include "lpjit_matcher.h"
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

int lpjit_ICloseRunTime(MatchState* mstate) {
    const char* o = mstate->subject_begin;
    const char* s = mstate->subject_current;
    const char* e = mstate->subject_end;
    int fr = lua_gettop(mstate->L) + 1;
    CapState cs;
    cs.s = o;
    cs.L = mstate->L;
    cs.ocap = mstate->capture;
    cs.ptop = mstate->ptop;
    Capture* top_capture = mstate->capture + mstate->cap_top;
    int rem;
    int n = lpeg_runtimecap(&cs, top_capture,
            mstate->subject_current, &rem);
    mstate->cap_top -= n;
    fr -= rem;
    int res = lpeg_resdyncaptures(mstate->L, fr, s - o, e - o);
    if (res == -1) {
        return LPJIT_FAIL;
    }
    mstate->subject_current = o + res;
    n = lua_gettop(mstate->L) - fr + 1;
    mstate->n_dyncap += n - rem;
    if (n > 0) {
        mstate->cap_top += n + 2;
        if (mstate->cap_top >= mstate->capsize) {
            lpjit_doubleCap(mstate);
        }
        Capture* top_capture2 = mstate->capture +
                mstate->cap_top - n - 2;
        lpeg_adddyncaptures(s, top_capture2, n, fr);
    }
    return 0; // Ok
}
