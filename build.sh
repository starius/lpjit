git submodule update --init

if [ ! -f minilua ]; then
    gcc -o minilua luajit-2.0/src/host/minilua.c -lm
fi

./minilua ./dynasm/dynasm.lua -D X64 -o src/lpjit_compile_posix64.c src/lpjit_compile.c
luarocks make --local LPEG_INCDIR=lpeg CFLAGS='-g -O0 -shared -fPIC'
