#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <memory>

extern "C" {
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/mman.h>
}

#include "util.h"

#include "File.h"

#include "pe.h"
#include "internalpe.h"

using namespace pe;

using std::make_unique;
using std::unique_ptr;

DlWinError dlwin_openPeFile(const char* path, PeFile** file) {
    auto result = nullCheck(path, 1);
    if(result) return result;

    result = nullCheck(file, 2);
    if(result) return result;

    // Forward declerations for gotos
    int sectionCount = 0;
    Segment** sections = nullptr;
    Exports* exports = nullptr;

    auto handle = new File(path, &result);
    if(result) return result;

    // Read the DOS header, ensure it's valid and seek to the PE header.
    DosHeader dosHeader;
    result = handle->read(&dosHeader, sizeof(dosHeader));
    if(result) goto cleanup;

    if(dosHeader.e_magic != DOS_MAGIC) {
        result = dlwin_error(DLWIN_ERROR_BAD_FILE, EINVAL);
        goto cleanup;
    }

    result = handle->seek(dosHeader.e_lfanew);
    if(result) goto cleanup;

    u32 magic;
    result = handle->read(&magic, sizeof(magic));
    if(result) goto cleanup;

    if(magic != PE_MAGIC) {
        return dlwin_error(DLWIN_ERROR_BAD_FILE, EINVAL);
    }

    CoffFileHeader coffHeader;
    result = handle->read(&coffHeader, sizeof(coffHeader));
    if(result) goto cleanup;

    // TODO Support i386
    if(coffHeader.machine != IMAGE_FILE_MACHINE_AMD64) {
        return dlwin_error(DLWIN_ERROR_BAD_FILE, EINVAL);
    }

    // This magic value is half the size, ensure the upper half is clear
    magic = 0;
    result = handle->read(&magic, sizeof(u16));
    if(result) goto cleanup;

    // TODO This shouldn't be a naked magic constant and we should support i386
    if(magic != 0x020B) {
        return dlwin_error(DLWIN_ERROR_BAD_FILE, EINVAL);
    }

    ImageOptionalHeader optionalHeader;
    memset(&optionalHeader, 0, sizeof(optionalHeader));
    optionalHeader.magic = magic;
    result = handle->read(&optionalHeader.majorLinkerVersion, minimum(sizeof(optionalHeader), coffHeader.sizeOfOptionalHeader) - 2);
    if(result) goto cleanup;

    sectionCount = coffHeader.numberOfSections;
    sections = (Segment**) calloc(sectionCount, sizeof(Segment*));
    for(int i = 0; i < sectionCount; i++) {
        sections[i] = new Segment(handle, optionalHeader.imageBase, &result);
        if(result) goto cleanup;
    }
    for(int i = 0; i < sectionCount; i++) {
        result = sections[i]->read(handle);
        if(result) goto cleanup;
    }

    size_t dataAddress;
    dataAddress = optionalHeader.dataDirectory[0].virtualAddress;
    Segment* exportSegment;
    exportSegment = nullptr;
    for(int i = 0; i < sectionCount; i++) {
        if(sections[i]->containsRva(dataAddress)) {
            exportSegment = sections[i];
            break;
        }
    }
    exports = new Exports(exportSegment, exportSegment->rva(dataAddress), &result);

    for(int i = 0; i < sectionCount; i++) {
        result = sections[i]->protect();
        if(result) goto cleanup;
    }

    PeFile* data;
    data = new PeFile;
    memcpy(&data->coffHeader, &coffHeader, sizeof(coffHeader));
    memcpy(&data->optionalHeader, &optionalHeader, sizeof(optionalHeader));
    data->sections = sections;
    data->sectionCount = sectionCount;
    data->exports = exports;
    *file = data;

    return DLWIN_SUCCESS;

cleanup:
    if(exports) {
        delete exports;
    }

    for(int i = 0; i < sectionCount; i++) {
        if(sections[i]) {
            delete sections[i];
        }
    }

    if(handle) {
        delete handle;
    }

    return result;
}

dlwin_export DlWinError dlwin_closePeFile(PeFile** file) {
    auto result = nullCheck(file, 1);
    if(result) return result;

    auto data = *file;
    result = nullCheck(data, 2);
    if(result) return result;

    if(data->exports) {
        delete data->exports;
    }

    auto sections = data->sections;
    if(sections) {
        auto sectionCount = data->sectionCount;
        for(int i = 0; i < sectionCount; i++) {
            if(sections[i]) {
                delete sections[i];
            }
        }
        free(data->sections);
    }

    if(data->file) {
        delete data->file;
    }

    delete data;
    *file = nullptr;

    return DLWIN_SUCCESS;
}

dlwin_export void* dlwin_sym(PeFile* file, const char* name) {
    if(!file || !name) {
        return nullptr;
    }

    auto exports = file->exports;
    if(!exports) {
        return nullptr;
    }

    return exports->symbol(name);
}

dlwin_export void* dlwin_ordinal(PeFile* file, u32 ordinal) {
    if(!file) {
        return nullptr;
    }

    auto exports = file->exports;
    if(!exports) {
        return nullptr;
    }

    return exports->symbol(ordinal);
}
