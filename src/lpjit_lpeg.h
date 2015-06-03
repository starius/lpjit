#ifndef lpjit_lpeg_h
#define lpjit_lpeg_h

#define LPEG_DEBUG

#include "lpcap.h"
#include "lpcode.h"
#include "lpprint.h"
#include "lptree.h"
#include "lptypes.h"
#include "lpvm.h"

/*
** Get the initial position for the match, interpreting negative
** values from the end of the subject
*/
size_t initposition(lua_State *L, size_t len);

int lpeg_sizei(const Instruction *i);

int lpeg_runtimecap(CapState *cs, Capture *close,
        const char *s, int *rem);
int lpeg_getcaptures(lua_State *L, const char *s,
        const char *r, int ptop);
int lpeg_finddyncap(Capture *cap, Capture *last);

Capture* lpeg_doubleCap(lua_State* L, Capture* cap,
        int captop, int ptop);

int lpeg_maxStackIndex(lua_State* L);

#endif
