#ifndef __dlwin_pe__
#define __dlwin_pe__

#include "error.h"
#include "types.h"
#include "misc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PeFile PeFile;

dlwin_export DlWinError dlwin_openPeFile(const char* path, PeFile** file);
dlwin_export DlWinError dlwin_closePeFile(PeFile** file);
dlwin_export void* dlwin_sym(PeFile* file, const char* name);
dlwin_export void* dlwin_ordinal(PeFile* file, u32 ordinal);

#ifdef __cplusplus
}
#endif

#endif
