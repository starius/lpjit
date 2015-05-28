package = "lpjit"
version = "dev-1"
source = {
    url = "git@github.com:starius/lpjit.git",
}
description = {
    summary = "JIT compiler of LPeg patterns",
    homepage = "https://github.com/starius/lpjit",
    license = "MIT",
}
dependencies = {
    "lua >= 5.1",
}
external_dependencies = {
    LPEG = {
        header = "lptypes.h",
    },
}
build = {
    type = "builtin",
    modules = {
        ['lpjit'] = {
            sources = {
                "src/lpjit.c",
                "src/lpjit_compile_posix64.c",
                "src/lpjit_compiler.c",
                "src/lpjit_memory.c",
            },
            incdirs = {"$(LPEG_INCDIR)", '.'},
            defines = {"X64"},
        },
    },
}