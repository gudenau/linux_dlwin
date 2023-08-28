#include <cerrno>

extern "C" {
    #include <unistd.h>
}

#include "error.h"

void cleanFdHandle(int* handle) {
    if(!handle) {
        return;
    }

    auto value = *handle;
    if(value != -1) {
        close(value);
    }
}

DlWinError nullCheck(const void* pointer, int arg) {
    if(pointer) {
        return DLWIN_SUCCESS;
    } else {
        return dlwin_error(DLWIN_ERROR_NULL_ARG, arg);
    }
}

DlWinError readFully(int handle, void* pointer, size_t length) {
    ssize_t transfered;

    u8* buffer = (u8*) pointer;
    while(length && (transfered = read(handle, buffer, length)) > 0) {
        length -= transfered;
        buffer += transfered;
    }

    return transfered < 0 ? dlwin_error(DLWIN_ERROR_IO, errno) : DLWIN_SUCCESS;
}

DlWinError writeFully(int handle, void* pointer, size_t length) {
    ssize_t transfered;

    u8* buffer = (u8*) pointer;
    while(length && (transfered = write(handle, buffer, length)) > 0) {
        length -= transfered;
        buffer += transfered;
    }

    return transfered < 0 ? dlwin_error(DLWIN_ERROR_IO, errno) : DLWIN_SUCCESS;
}
