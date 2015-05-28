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

// Arguments:
// 1. lpeg Pattern object
// Results:
// 1. Matcher object
int lpjit_pushMatcher(lua_State* L) {
    Pattern* pattern = luaL_checkudata(L, 1, PATTERN_T);
    //
    Matcher* matcher = lua_newuserdata(L, sizeof(Matcher));
    int matcher_index = lua_gettop(L);
    // Matcher depends on Pattern (bitsets for ISet etc)
    lua_pushvalue(L, 1); // pattern
    matcher->pattern_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    //
    matcher->buffer = 0;
    matcher->impl = 0;
    matcher->d = 0;
    if (luaL_newmetatable(L, "lpjit_Matcher")) {
        lua_pushcfunction(L, lpjit_gc);
        lua_setfield(L, -2, "__gc");
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

void lpjit_match(lua_State *L, const Matcher* matcher,
            MatchState* mstate) {
    luaL_argcheck(L, matcher->buffer != 0, 1, "Bad matcher");
    luaL_argcheck(L, matcher->impl != 0, 1, "Bad matcher");
    matcher->impl(L, mstate);
    // result is in mstate->subject_current
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
