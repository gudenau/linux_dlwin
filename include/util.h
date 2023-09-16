#ifndef DLWIN_UTIL_H
#define DLWIN_UTIL_H

#include <stddef.h>

int readFully(int handle, void* buffer, size_t length);
int writeFully(int handle, const void* buffer, size_t length);
size_t minimum(size_t a, size_t b);

#endif //DLWIN_UTIL_H
