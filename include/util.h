#ifndef __dlwin_util__
#define __dlwin_util__

#include "types.h"
#include "error.h"

#define CLEANER(function)   __attribute__((cleanup(function)))
#define FD_CLEANER          CLEANER(cleanFdHandle)

static inline size_t minimum(size_t a, size_t b) {
    return a < b ? a : b;
}

void cleanFdHandle(int* handle);

DlWinError nullCheck(const void* pointer, int arg);

DlWinError readFully(int handle, void* pointer, size_t length);
DlWinError writeFully(int handle, void* pointer, size_t length);

#endif
