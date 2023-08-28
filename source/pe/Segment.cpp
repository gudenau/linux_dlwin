#include <cstring>
#include <cstdlib>
#include <cerrno>

#include <memory>

extern "C" {
    #include <sys/mman.h>
}

#include "error.h"
#include "types.h"
#include "util.h"

#include "File.h"

#include "pe/Segment.h"

using namespace pe;

Segment::Segment(File* file, size_t base, DlWinError* error) {
    if(nullCheck(error, 2)) {
        abort();
    }

    m_data = MAP_FAILED;

    auto result = file->read(&m_header, sizeof(m_header));
    if(result) {
        *error = result;
        return;
    }

    memcpy(m_name, &m_header.name, 8);
    m_name[8] = 0;

    void* prefered = (void*) (m_header.virtualAddress + base);
    m_data = mmap(prefered, m_header.virtualSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
    if(m_data == MAP_FAILED) {
        *error = dlwin_error(DLWIN_ERROR_ALLOC, errno);
        return;
    }
}

Segment::~Segment(void) {
    if(m_data != MAP_FAILED) {
        munmap(this->data(), this->dataSize());
    }
}

DlWinError Segment::read(File* file) {
    auto fileSize = m_header.sizeOfRawData;
    if(fileSize) {
        auto result = file->seek(m_header.pointerToRawData);
        if(result) return result;

        result = file->read(m_data, fileSize);
        if(result) return result;
    }

    return DLWIN_SUCCESS;
}

DlWinError Segment::protect(void) {
    auto characteristics = m_header.characteristics;
    int isExecutable = (characteristics & 0x20000000) != 0;
    int isReadable = (characteristics & 0x40000000) != 0;
    int isWritable = (characteristics & 0x80000000) != 0;

    auto result = mprotect(
        this->data(),
        this->dataSize(),
        (isExecutable ? PROT_EXEC : 0) |
            (isReadable ? PROT_READ : 0) |
            (isWritable ? PROT_WRITE : 0)
    );
    
    return result < 0 ? dlwin_error(DLWIN_ERROR_MISC, errno) : DLWIN_SUCCESS;
}

void* Segment::data(void) {
    return this->m_data;
}

size_t Segment::dataSize(void) {
    return m_header.virtualSize;
}

const char* Segment::name(void) {
    return this->m_name;
}
