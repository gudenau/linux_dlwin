#ifndef __dlwin_error__
#define __dlwin_error__

#include "types.h"
#include "misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DLWIN_SUCCESS ((DlWinError) (DLWIN_SUCCESS_BASE))

typedef u64 DlWinError;

typedef enum {
    DLWIN_SUCCESS_BASE = 0,
    DLWIN_ERROR_ARG = 1,
    DLWIN_ERROR_NULL_ARG = 2,
    DLWIN_ERROR_IO = 3,
    DLWIN_ERROR_BAD_FILE = 3,
    DLWIN_ERROR_MISC = 4,
    DLWIN_ERROR_ALLOC = 4,
} DlWinErrorBase;

static inline DlWinError dlwin_error(DlWinErrorBase error, u32 argument) {
    return (error & 0xFF) | (argument << 8);
}

static inline DlWinErrorBase dlwin_errorBase(DlWinError error) {
    return (DlWinErrorBase)(error & 0xFF);
}

static inline u16 dlwin_errorArg(DlWinError error) {
    return (error >> 8) & 0xFFFFFFFF;
}

dlwin_export void dlwin_strerror(DlWinError error, char* buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif
