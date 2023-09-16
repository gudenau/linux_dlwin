#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <sys/mman.h>

#include "headers/dos.h"
#include "headers/pe.h"
#include "types.h"
#include "util.h"

#include "pe.h"

static int readHeaders(int file, PeFile* handle) {
    DosHeader dosHeader;
    if(readFully(file, &dosHeader, sizeof(dosHeader))) {
        return errno;
    }
    if(dosHeader.magic != DOS_HEADER_MAGIC) {
        return ENOEXEC;
    }

    if(lseek(file, dosHeader.lfanew, SEEK_SET) < 0) {
        return errno;
    }

    u32 peMagic;
    if(readFully(file, &peMagic, sizeof(peMagic))) {
        return errno;
    }
    if(peMagic != PE_MAGIC) {
        return ENOEXEC;
    }

    // These are at the same location in memory
    PeHeader* peHeader = &handle->header64.header;
    if(readFully(file, peHeader, sizeof(PeHeader))) {
        return errno;
    }

    size_t optionalLength = peHeader->sizeOfOptionalHeader;
    if(optionalLength == 0) {
        return ENOEXEC;
    }

    u16 optionalMagic;
    if(readFully(file, &optionalMagic, minimum(optionalLength, sizeof(optionalMagic)))) {
        return errno;
    }
    if(optionalMagic == PE_MAGIC_64) {
        handle->type = PE_TYPE_64;
        handle->header64.optional.magic = PE_MAGIC_64;
        if(readFully(file, &handle->header64.optional.majorLinkerVersion, minimum(optionalLength, sizeof(PeOptionalHeader64)) - sizeof(u16))) {
            return errno;
        }
    } else if(optionalMagic == PE_MAGIC_32) {
        handle->type = PE_TYPE_32;
        handle->header32.optional.magic = PE_MAGIC_32;
        if(readFully(file, &handle->header32.optional.majorLinkerVersion, minimum(optionalLength, sizeof(PeOptionalHeader32)) - sizeof(u16))) {
            return errno;
        }
    } else {
        return ENOEXEC;
    }

    // These are both at the same address
    int sectionCount = handle->header64.header.numberOfSections;
    PeSection* sections = (PeSection*) calloc(sectionCount, sizeof(PeSection));
    if(!sections) {
        return errno;
    }
    handle->sectionCount = sectionCount;
    handle->sections = sections;

    for(int i = 0; i < sectionCount; i++) {
        PeSection* section = &sections[i];
        if(readFully(file, &section->header, sizeof(PeSectionHeader))) {
            return errno;
        }

        memcpy(section->name, &section->header.name, PE_SECTION_NAME_LENGTH);
    }

    return 0;
}

static int readSections(int file, PeFile* handle) {
    u64 base = handle->type == PE_TYPE_32 ? handle->header32.optional.imageBase : handle->header64.optional.imageBase;

    u32 sectionCount = handle->sectionCount;
    for(u32 i = 0; i < sectionCount; i++) {
        PeSection* section = &handle->sections[i];
        if(lseek(file, section->header.pointerToRawData, SEEK_SET) < 0) {
            return errno;
        }

        void* pointer = mmap(
                (void*) (section->header.virtualAddress + base),
            section->header.virtualSize,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1,
            0
        );
        if(pointer == MAP_FAILED) {
            return errno;
        }
        section->pointer = pointer;

        if(readFully(file, pointer, minimum(section->header.virtualSize, section->header.sizeOfRawData))) {
            return errno;
        }
    }

    return 0;
}

static void unresolvedImportHandler(void) {
    fprintf(stderr, "An unresolved import from a PE file loaded by dlwin was called!\n");
    abort();
}

//TODO Make this work on i386 DLLs
static int readImportTable(PeFile* handle) {
    PeDataDirectory* directory = PeFile_dataDir(handle, PE_DIRECTORY_IMPORT);
    if(!directory) {
        return 0;
    }

    u32 virtualAddress = directory->virtualAddress;
    PeImageImportDescriptor* importDescriptor = PeFile_resolveAddress(handle, virtualAddress);
    if(!importDescriptor) {
        return EFAULT;
    }

    int dllCount = 0;
    while(
        importDescriptor[dllCount].originalFirstThunk |
        importDescriptor[dllCount].timeDateStamp |
        importDescriptor[dllCount].forwarderChain |
        importDescriptor[dllCount].name |
        importDescriptor[dllCount].firstThunk
    ) {
        dllCount++;
    }

    PeImport *imports = calloc(dllCount, sizeof(PeImport));
    if (!imports) {
        return errno;
    }
    handle->imports = imports;
    handle->importCount = dllCount;

    for(int o = 0; o < dllCount; o++) {
        PeImport* import = &imports[o];

        const char *dll = PeFile_resolveAddress(handle, importDescriptor->name);
        if (!dll) {
            return EFAULT;
        }

        import->dll = dll;

        u64 *thunk = PeFile_resolveAddress(handle, importDescriptor->firstThunk);
        u64 count = 0;
        while (thunk[count]) {
            count++;
        }
        if (!count) {
            return 0;
        }

        PeImportedFunction *functions = calloc(count, sizeof(PeImportedFunction));
        if (!functions) {
            int error = errno;
            free(import);
            handle->imports = NULL;
            handle->importCount = 0;
            return error;
        }
        import->functions = functions;
        import->count = count;

        for (u64 i = 0; i < count; i++) {
            u64 thunkValue = thunk[i];
            PeImportedFunction *function = &functions[i];
            function->address = (void**) &thunk[i];
            if (thunkValue & 0x8000000000000000L) {
                function->ordinal = (u16) (thunkValue & 0xFFFF);
            } else {
                PeImportByName *funcImport = PeFile_resolveAddress(handle, (u32) (thunkValue & 0xFFFFFFFF));
                if (!funcImport) {
                    return ENXIO;
                }

                function->name = funcImport->name;
                function->ordinal = -1;
            }
            *function->address = unresolvedImportHandler;
        }

        importDescriptor++;
    }

    return 0;
}

static int readExportTable(PeFile* handle) {
    PeDataDirectory* exportDirectory = PeFile_dataDir(handle, PE_DIRECTORY_EXPORT);
    if(!exportDirectory) {
        return 0;
    }

    PeExportHeader* exportHeader = PeFile_resolveAddress(handle, exportDirectory->virtualAddress);
    if(!exportHeader) {
        return EINVAL;
    }

    if(exportHeader->nameCount != exportHeader->addressCount) {
        //TODO Support this
        return EINVAL;
    }

    u32 count = exportHeader->addressCount;
    PeExportedFunction* functions = calloc(count, sizeof(PeExportedFunction));
    if(!functions) {
        return errno;
    }
    handle->exportCount = count;
    handle->exports = functions;

    //TODO Error handling
    u16* ordinals = PeFile_resolveAddress(handle, exportHeader->ordinalPointer);
    u32* names = PeFile_resolveAddress(handle, exportHeader->namePointer);
    u32* addresses = PeFile_resolveAddress(handle, exportHeader->exportPointer);
    int ordinalBase = (int) exportHeader->ordinalBase;

    //TODO Handle forwarder RVAs (https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#export-address-table)
    for(u32 i = 0; i < count; i++) {
        PeExportedFunction* function = &functions[i];
        function->name = PeFile_resolveAddress(handle, names[i]);
        function->ordinal = ordinalBase + ordinals[i];
        function->address = PeFile_resolveAddress(handle, addresses[i]);
    }

    return 0;
}

//TODO Ensure the machine is compatible
PeFile* PeFile_open(const char* path) {
    int error;

    PeFile* handle = calloc(1, sizeof(PeFile));

    int file = open(path, O_RDONLY);
    if(file == -1) {
        error = errno;
        goto cleanup;
    }

    if((error = readHeaders(file, handle))) {
        goto cleanup;
    }

    if((error = readSections(file, handle))) {
        goto cleanup;
    }

    close(file);

    if((error = readImportTable(handle))) {
        goto cleanup;
    }

    if((error = readExportTable(handle))) {
        goto cleanup;
    }

    return handle;

cleanup:
    if(file != -1) {
        close(file);
    }
    PeFile_close(handle);
    errno = error;
    return NULL;
}

void PeFile_close(PeFile* file) {
    free(file->exports);

    PeImport* imports = file->imports;
    if(imports) {
        u32 importCount = file->importCount;
        for(u32 i = 0; i < importCount; i++) {
            free(imports[i].functions);
        }
        free(imports);
    }

    u32 sectionCount = file->sectionCount;
    for(u32 i = 0; i < sectionCount; i++) {
        PeSection* section = &file->sections[i];
        if(section->pointer) {
            munmap(section->pointer, section->header.virtualSize);
        }
    }

    free(file->sections);

    memset(file, 0, sizeof(PeFile));

    free(file);
}

PeDataDirectory* PeFile_dataDir(PeFile* file, PeDataDir directory) {
    PeDataDirectory* dir = file->type == PE_TYPE_32 ?
        &file->header32.optional.dataDirectory[directory] :
        &file->header64.optional.dataDirectory[directory];

    if(dir->virtualAddress | dir->size) {
        return dir;
    } else {
        return NULL;
    }
}

PeSection* PeFile_section(PeFile* file, const char* name) {
    u32 sectionCount = file->sectionCount;
    for(u32 i = 0; i < sectionCount; i++) {
        if(strcmp(file->sections[i].name, name) == 0) {
            return &file->sections[i];
        }
    }

    return NULL;
}

void* PeFile_resolveAddress(PeFile* file, u32 rva) {
    u32 sectionCount = file->sectionCount;
    for(u32 i = 0; i < sectionCount; i++) {
        PeSection* section = &file->sections[i];
        u32 virtualAddress = section->header.virtualAddress;
        if(rva >= virtualAddress && rva < virtualAddress + section->header.virtualSize) {
            return (void*)(((size_t) section->pointer) + (rva - virtualAddress));
        }
    }

    return NULL;
}

int PeFile_reprotectMemory(PeFile* file) {
    if(file->reprotected) {
        return 0;
    }

    file->reprotected = true;

    u32 count = file->sectionCount;
    for(u32 i = 0; i < count; i++) {
        PeSection* section = &file->sections[i];
        u32 characteristics = section->header.characteristics;
        int result = mprotect(
            section->pointer,
            section->header.virtualSize,
            (characteristics & 0x40000000 ? PROT_READ : 0) |
                (characteristics & 0x80000000 ? PROT_WRITE : 0) |
                (characteristics & 0x20000000 ? PROT_EXEC : 0)
        );
        if(result) {
            return result;
        }
    }

    return 0;
}
