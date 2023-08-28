#ifndef __internal_file__
#define __internal_file__

#include "error.h"
#include "types.h"

class File {
public:
    File(const char* path, DlWinError* result);
    ~File(void);

    DlWinError read(void* buffer, size_t length);
    DlWinError write(const void* buffer, size_t length);
    DlWinError seek(size_t offset);

private:
    int m_handle;
};

#endif
