git submodule update --init
luajit ./dynasm/dynasm.lua -D X64 -o src/lpjit_compile_posix64.c src/lpjit_compile.c
luarocks make --local LPEG_INCDIR=lpeg
