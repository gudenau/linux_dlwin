#ifndef DLWIN_PE_H
#define DLWIN_PE_H

#include "headers/pe.h"

typedef enum {
    PE_TYPE_32,
    PE_TYPE_64,
} PeType;

typedef struct {
    PeSectionHeader header;
    char name[PE_SECTION_NAME_LENGTH + 1];
    void* pointer;
} PeSection;

typedef struct {
    const char* name;
    void* address;
    int ordinal;
} PeExportedFunction;

typedef struct {
    const char* name;
    void** address;
    int ordinal;
} PeImportedFunction;

typedef struct {
    const char* dll;
    u64 count;
    PeImportedFunction* functions;
} PeImport;

typedef struct {
    PeType type;
    union {
        PeHeaders32 header32;
        PeHeaders64 header64;
    };

    bool reprotected;

    u32 sectionCount;
    PeSection* sections;

    u32 importCount;
    PeImport* imports;

    u32 exportCount;
    PeExportedFunction* exports;
} PeFile;

PeFile* PeFile_open(const char* path);
void PeFile_close(PeFile* file);
PeDataDirectory* PeFile_dataDir(PeFile* file, PeDataDir directory);
PeSection* PeFile_section(PeFile* file, const char* name);
void* PeFile_resolveAddress(PeFile* file, u32 rva);
int PeFile_reprotectMemory(PeFile* file);

#endif
