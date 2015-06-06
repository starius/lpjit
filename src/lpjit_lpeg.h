#ifndef lpjit_lpeg_h
#define lpjit_lpeg_h

#define LPEG_DEBUG

#include "lpcap.h"
#include "lpcode.h"
#include "lpprint.h"
#include "lptree.h"
#include "lptypes.h"
#include "lpvm.h"

/* initial size for call/backtrack stack */
#if !defined(INITBACK)
#define INITBACK 100
#endif

typedef struct MatchState MatchState;

/*
** Get the initial position for the match, interpreting negative
** values from the end of the subject
*/
size_t initposition(lua_State *L, size_t len);

int lpeg_sizei(const Instruction *i);

int lpeg_runtimecap(MatchState* mstate,
        CapState *cs, Capture *close,
        const char *s, int *rem);
int lpeg_getcaptures(lua_State *L, const char *s,
        const char *r, int ptop);

Capture* lpeg_doubleCap(MatchState* mstate,
        lua_State* L, Capture* cap,
        int captop, int ptop);

int lpeg_maxStackIndex(lua_State* L);

int lpeg_removedyncap(MatchState* mstate,
        lua_State *L, Capture *capture,
        int level, int last);

int lpeg_resdyncaptures(MatchState* mstate,
        lua_State *L, int fr,
        int curr, int limit);

void lpeg_adddyncaptures(MatchState* mstate,
        const char *s, Capture *base,
        int n, int fd);

#endif
