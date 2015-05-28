#ifndef lpjit_memory_h
#define lpjit_memory_h

void* lpjit_allocate(size_t size);

void lpjit_protect(void* buffer, size_t size);

void lpjit_deallocate(void* buffer, size_t size);

#endif
