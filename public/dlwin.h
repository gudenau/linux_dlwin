#ifndef DLWIN_HEADER_H
#define DLWIN_HEADER_H

#ifdef __cplusplus
#include <cstdint>

extern "C" {
#else
#include <stdint.h>
#endif

#ifndef dlwin_export
#define dlwin_export
#endif

typedef struct dlwin_handle dlwin_handle;

typedef struct {
    const char* name;
    void* function;
    int ordinal;
} DlwinFunction;

dlwin_export dlwin_handle* dlwin_open(const char* filename);
dlwin_export void* dlwin_sym(dlwin_handle* handle, const char* symbol);
dlwin_export int dlwin_bind(dlwin_handle* handle, const char* dll, DlwinFunction* functions);
dlwin_export int dlwin_close(dlwin_handle* handle);

#ifdef __cplusplus
}
#endif

#endif
