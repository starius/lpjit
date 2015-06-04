#include <lua.h>
#include <lauxlib.h>

#include "lpjit.h"
#include "lpjit_lpeg.h"
#include "lpjit_dasm.h"
#include "lpjit_matcher.h"
#include "lpjit_compiler.h"
#include "lpjit_compat.h"

int lpjit_gc(lua_State* L) {
    Matcher* m = lpjit_checkMatcher(L, 1);
    if (m->buffer) {
        lpjit_deallocate(m->buffer, m->buffer_size);
    }
    if (m->d) {
        CompilerState cstate;
        cstate.d = m->d;
        dasm_free(&cstate);
        m->d = NULL;
    }
    luaL_unref(L, LUA_REGISTRYINDEX, m->pattern_ref);
    return 0;
}

static const luaL_Reg matcher_methods[] = {
    {"match", lua_lpjit_match},
    {NULL, NULL}
};

// Arguments:
// 1. lpeg Pattern object
// Results:
// 1. Matcher object
int lpjit_pushMatcher(lua_State* L) {
    Pattern* pattern = luaL_checkudata(L, 1, PATTERN_T);
    if (!pattern->code) {
        // Pattern is not compiled, call match with ''
        lua_getfield(L, 1, "match");
        lua_pushvalue(L, 1);
        lua_pushliteral(L, "");
        if (lua_pcall(L, 2, 0, 0)) {
            if (pattern->code) {
                // compiled - discard the error
                // probably this error was thrown by match(),
                // not by compile()
                lua_pop(L, 1);
            } else {
                // not compiled -- rethrow the error
                return lua_error(L);
            }
        }
        luaL_argcheck(L, pattern->code, 1,
                "Pattern is not compiled");
    }
    //
    Matcher* matcher = lua_newuserdata(L, sizeof(Matcher));
    int matcher_index = lua_gettop(L);
    // Matcher depends on Pattern (bitsets for ISet etc)
    lua_pushvalue(L, 1); // pattern
    matcher->pattern_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_getfenv(L, 1); // ktable
    lua_setfenv(L, matcher_index); // ktable
    //
    matcher->buffer = 0;
    matcher->impl = 0;
    matcher->d = 0;
    if (luaL_newmetatable(L, "lpjit_Matcher")) {
        lua_pushcfunction(L, lpjit_gc);
        lua_setfield(L, -2, "__gc");
        lua_newtable(L); // __index
        compat_setfuncs(L, matcher_methods);
        lua_setfield(L, -2, "__index");
    }
    lua_setmetatable(L, matcher_index);
    // magic starts here
    lpjit_compile(L, matcher, pattern);
    lua_pushvalue(L, matcher_index);
    return 1; // matcher
}

Matcher* lpjit_checkMatcher(lua_State* L, int index) {
    return luaL_checkudata(L, index, "lpjit_Matcher");
}

void lpjit_match(const Matcher* matcher, MatchState* mstate) {
    lua_State* L = mstate->L;
    luaL_argcheck(L, matcher->buffer != 0, 1, "Bad matcher");
    luaL_argcheck(L, matcher->impl != 0, 1, "Bad matcher");
    matcher->impl(mstate);
    // result is in mstate->subject_current
}

// Arguments:
// 1. lpjit Matcher
// 2. string (subject)
// 3. init position (optional)
// Results:
// TODO
int lua_lpjit_match(lua_State* L) {
    Matcher* matcher = lpjit_checkMatcher(L, 1);
    size_t l;
    const char *s = luaL_checklstring(L, 2, &l);
    size_t i = lpeg_initposition(L, l);
    int ptop = lua_gettop(L);
    /* initialize subscache */
    lua_pushnil(L);
    Capture capture[INITCAPSIZE];
    /* initialize caplistidx */
    lua_pushlightuserdata(L, capture);
    /* initialize penvidx */
    lua_getfenv(L, 1);
    MatchState mstate;
    mstate.subject_begin = s;
    mstate.subject_current = s + i;
    mstate.subject_end = s + l;
    mstate.L = L;
    mstate.capture = capture;
    mstate.capsize = INITCAPSIZE;
    mstate.ptop = ptop;
    //mstate.cap_top = 0; // is not used by ASM
    lpjit_match(matcher, &mstate);
    const char* r = mstate.subject_current;
    if (r == LPJIT_GIVEUP) {
        lua_pushnil(L);
        return 1;
    }
    if (r == LPJIT_STACKOVERFLOW) {
        return luaL_error(L, "too many pending calls/choices");
    }
    return lpeg_getcaptures(L, s, r, ptop);
}

static const luaL_Reg lpjit_functions[] = {
    {"compile", lpjit_pushMatcher},
    {NULL, NULL}
};

int luaopen_lpjit(lua_State* L) {
    lua_newtable(L);
    compat_setfuncs(L, lpjit_functions);
    return 1;
}
