#include <cstdlib>
#include <cerrno>

extern "C" {
    #include <fcntl.h>
    #include <unistd.h>
}

#include "error.h"
#include "types.h"
#include "util.h"

#include "File.h"

File::File(const char* path, DlWinError* error) {
    auto result = nullCheck(error, 2);
    if(result) {
        abort();
    }

    result = nullCheck(path, 1);
    if(result) {
        *error = result;
        return;
    }

    m_handle = open(path, O_RDONLY);
    if(m_handle == -1) {
        *error = dlwin_error(DLWIN_ERROR_IO, errno);
    }
}

File::~File(void) {
    if(m_handle != -1) {
        close(m_handle);
    }
}

DlWinError File::read(void* buffer, size_t length) {
    u8* pointer = (u8*) buffer;
    ssize_t transfered;

    while(length && (transfered = ::read(m_handle, pointer, length)) > 0) {
        pointer += transfered;
        length -= transfered;
    }

    return transfered < 0 ? dlwin_error(DLWIN_ERROR_IO, errno) : DLWIN_SUCCESS;
}

DlWinError File::write(const void* buffer, size_t length) {
    u8* pointer = (u8*) buffer;
    ssize_t transfered;

    while(length && (transfered = ::write(m_handle, pointer, length)) > 0) {
        pointer += transfered;
        length -= transfered;
    }

    return transfered < 0 ? dlwin_error(DLWIN_ERROR_IO, errno) : DLWIN_SUCCESS;
}

DlWinError File::seek(size_t offset) {
    auto result = ::lseek(m_handle, offset, SEEK_SET);
    return result < 0 ? dlwin_error(DLWIN_ERROR_IO, errno) : DLWIN_SUCCESS;
}
