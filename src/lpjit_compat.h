#ifndef lpjit_compat_h
#define lpjit_compat_h

#if LUA_VERSION_NUM == 501
#define compat_rawlen lua_objlen
#define compat_setfuncs(L, funcs) luaL_register(L, NULL, funcs)
#else
#define compat_rawlen lua_rawlen
#define compat_setfuncs(L, funcs) luaL_setfuncs(L, funcs, 0)
#endif

#if LUA_VERSION_NUM >= 502
#define lua_equal(L,idx1,idx2) \
    lua_compare(L,(idx1),(idx2),LUA_OPEQ)
#endif

#endif
