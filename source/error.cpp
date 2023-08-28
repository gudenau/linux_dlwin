#include <cstring>
#include <cstdio>
#include <cerrno>

#include "error.h"

void dlwin_strerror(DlWinError error, char* buffer, size_t length) {
    if(error == DLWIN_SUCCESS) {
        strncpy(buffer, "Success", length);
        return;
    }

    auto arg = dlwin_errorArg(error);

    switch(dlwin_errorBase(error)) {
        case DLWIN_SUCCESS_BASE: {
            strncpy(buffer, "Success", length);
        } break;
        
        case DLWIN_ERROR_ARG: {
            snprintf(buffer, length, "Bad argument: %d", arg);
        } break;

        case DLWIN_ERROR_NULL_ARG: {
            snprintf(buffer, length, "Null argument: %d", arg);
        } break;

        case DLWIN_ERROR_IO: {
            snprintf(buffer, length, "I/O error: %s", strerror(arg));
        } break;

        case DLWIN_ERROR_MISC: {
            snprintf(buffer, length, "Misc error: %s", strerror(arg));
        } break;

        default: {
            snprintf(buffer, length, "Unknown error (0x%016lX)", error);
        } break;
    }
}
