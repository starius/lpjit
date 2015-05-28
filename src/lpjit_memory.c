#if _WIN32
#include <Windows.h>
#else
#include <sys/mman.h>
#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#define MAP_ANONYMOUS MAP_ANON
#endif
#endif

#include "lpjit_memory.h"

void* lpjit_allocate(size_t size) {
#ifdef _WIN32
    return VirtualAlloc(0, size,
            MEM_RESERVE | MEM_COMMIT,
            PAGE_READWRITE);
#else
    return mmap(0, size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0);
#endif
}

void lpjit_protect(void* buffer, size_t size) {
#ifdef _WIN32
    DWORD dwOld;
    VirtualProtect(buffer, size, PAGE_EXECUTE_READ, &dwOld);
#else
    mprotect(buffer, size, PROT_READ | PROT_EXEC);
#endif
}

void lpjit_deallocate(void* buffer, size_t size) {
#ifdef _WIN32
    VirtualFree(buffer, 0, MEM_RELEASE);
#else
    munmap(buffer, size);
#endif
}
