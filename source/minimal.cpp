#include <cstdlib>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <new>

extern "C" {
    #include <malloc.h>
}

void* operator new(size_t count) {
    auto pointer = calloc(1, count);
    if(pointer) {
        return pointer;
    } else {
        fprintf(stderr, "DlWin failed to allocate %ld bytes of memory: %s (%d)\n", count, strerror(errno), errno);
        abort();
    }
}

void* operator new[](size_t count) {
    return ::operator new(count);
}

void* operator new(size_t count, const std::nothrow_t& tag) noexcept {
    (void) tag;

    return calloc(1, count);
}

void* operator new[](size_t count, const std::nothrow_t& tag) noexcept {
    (void) tag;

    return calloc(1, count);
}

void operator delete(void* pointer) noexcept {
    free(pointer);
}
