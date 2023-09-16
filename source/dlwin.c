#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "pe.h"
#include "types.h"

#define dlwin_export __attribute__ ((visibility ("default")))

#include "dlwin.h"

struct dlwin_handle {
    PeFile* file;
};

static void cleanup(dlwin_handle* handle) {
    if(handle->file) {
        PeFile_close(handle->file);
    }
    free(handle);
}

dlwin_handle* dlwin_open(const char* filename) {
    if (!filename) {
        errno = EINVAL;
        return NULL;
    }

    dlwin_handle* handle = calloc(1, sizeof(dlwin_handle));
    if(!handle) return NULL;

    handle->file = PeFile_open(filename);
    if(!handle->file) goto error;

    return handle;

error: {}
    int error = errno;
    cleanup(handle);
    errno = error;

    return NULL;
}

void* dlwin_sym(dlwin_handle* handle, const char* symbol) {
    if (!handle || !symbol) {
        errno = EINVAL;
        return NULL;
    }

    PeFile* file = handle->file;
    int error;
    if((error = PeFile_reprotectMemory(file))) {
        errno = error;
        return NULL;
    }

    u32 count = file->exportCount;
    PeExportedFunction* functions = file->exports;
    for(u32 i = 0; i < count; i++) {
        if(strcmp(symbol, functions[i].name) == 0) {
            return functions[i].address;
        }
    }

    errno = ENOENT;
    return NULL;
}

int dlwin_bind(dlwin_handle* handle, const char* dll, DlwinFunction* functions) {
    if(!handle || !dll || !functions) {
        return EINVAL;
    }

    int importCount = handle->file->importCount;
    if(!importCount) {
        return EINVAL;
    }
    PeImport* imports = handle->file->imports;
    PeImport* import = NULL;
    for(int i = 0; i < importCount; i++) {
        if(strcmp(dll, imports[i].dll) == 0) {
            import = &imports[i];
            break;
        }
    }
    if(!import) {
        return EINVAL;
    }

    u64 importSize = import->count;
    while(
        functions->name != NULL ||
        functions->function != NULL ||
        functions->ordinal != 0
    ) {
        const char* name = functions->name;
        bool found = false;

        for(u64 i = 0; i < importSize; i++) {
            PeImportedFunction* importedFunction = &import->functions[i];

            if(strcmp(name, importedFunction->name) == 0) {
                *importedFunction->address = functions->function;
                found = true;
            }
        }

        if(!found) {
            return EINVAL;
        }

        functions++;
    }

    return 0;
}

int dlwin_close(dlwin_handle* handle) {
    if (!handle) {
        return EINVAL;
    }

    cleanup(handle);

    return 0;
}
