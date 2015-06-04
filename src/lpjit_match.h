#ifndef lpjit_match_h
#define lpjit_match_h

#include "lpjit.h"
#include "lpjit_lpeg.h"

void lpjit_doubleCap(MatchState* mstate);

int lpjit_removedyncap(MatchState* mstate);

// if returns LPJIT_FAIL, goto fail
int lpjit_ICloseRunTime(MatchState* mstate);

#endif
