#include <unistd.h>

#include "types.h"

#include "util.h"

int readFully(int handle, void* buffer, size_t length) {
    u8* byteBuffer = (u8*) buffer;

    while(length) {
        ssize_t transferred = read(handle, byteBuffer, length);
        if(transferred <= 0) {
            return 1;
        }
        byteBuffer += transferred;
        length -= transferred;
    }

    return 0;
}

int writeFully(int handle, const void* buffer, size_t length) {
    const u8* byteBuffer = (const u8*) buffer;

    while(length) {
        ssize_t transferred = write(handle, byteBuffer, length);
        if(transferred <= 0) {
            return 1;
        }
        byteBuffer += transferred;
        length -= transferred;
    }

    return 0;
}

size_t minimum(size_t a, size_t b) {
    return a < b ? a : b;
}
