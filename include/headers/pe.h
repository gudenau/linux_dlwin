#ifndef DLWIN_HEADERS_PE_H
#define DLWIN_HEADERS_PE_H

#include "types.h"

#define PE_DATA_DIRECTORY_COUNT (16)

#define PE_MAGIC    (0x00004550)
#define PE_MAGIC_32 (0x010B)
#define PE_MAGIC_64 (0x020B)

typedef enum {
    PE_DIRECTORY_EXPORT = 0,
    PE_DIRECTORY_IMPORT = 1,
    PE_DIRECTORY_RESOURCE = 2,
    PE_DIRECTORY_EXCEPTION = 3,
    PE_DIRECTORY_SECURITY = 4,
    PE_DIRECTORY_BASERELOC = 5,
    PE_DIRECTORY_DEBUG = 6,
    PE_DIRECTORY_ARCHITECTURE = 7,
    PE_DIRECTORY_GLOBALPTR = 8,
    PE_DIRECTORY_TLS = 9,
    PE_DIRECTORY_LOAD_CONFIG = 10,
    PE_DIRECTORY_BOUND_IMPORT = 11,
    PE_DIRECTORY_IAT = 12,
    PE_DIRECTORY_DELAY_IMPORT = 13,
    PE_DIRECTORY_COM_DESCRIPTOR = 14,
} PeDataDir;

typedef struct {
    u16 machine;
    u16 numberOfSections;
    u32 timeDateStamp;
    u32 pointerToSymbolTable;
    u32 numberOfSymbols;
    u16 sizeOfOptionalHeader;
    u16 characteristics;
} PeHeader;

typedef struct {
    u32 virtualAddress;
    u32 size;
} PeDataDirectory;

typedef struct {
    u16 magic;
    u8 majorLinkerVersion;
    u8 minorLinkerVersion;
    u32 sizeOfCode;
    u32 sizeOfInitializedData;
    u32 sizeOfUninitializedData;
    u32 addressOfEntryPoint;
    u32 baseOfCode;
    u32 baseOfData;

    u32 imageBase;
    u32 sectionAlignment;
    u32 fileAlignment;
    u16 majorOperatingSystemVersion;
    u16 minorOperatingSystemVersion;
    u16 majorImageVersion;
    u16 minorImageVersion;
    u16 majorSubsystemVersion;
    u16 minorSubsystemVersion;
    u32 win32VersionValue;
    u32 sizeOfImage;
    u32 sizeOfHeaders;
    u32 checksum;
    u16 subsystem;
    u16 dllCharacteristics;
    u32 sizeOfStackReserve;
    u32 sizeOfStackCommit;
    u32 sizeOfHeapReserve;
    u32 sizeOfHeapCommit;
    u32 loaderFlags;
    u32 numberOfRvaAndSizes;
    PeDataDirectory dataDirectory[PE_DATA_DIRECTORY_COUNT];
} PeOptionalHeader32;

typedef struct {
    u32 magic;
    PeHeader header;
    PeOptionalHeader32 optional;
} PeHeaders32;

typedef struct {
    u16 magic;
    u8 majorLinkerVersion;
    u8 minorLinkerVersion;
    u32 sizeOfCode;
    u32 sizeOfInitializedData;
    u32 addressOfEntryPoint;
    u32 baseOfCode;
    u64 imageBase;
    u32 sectionAlignment;
    u32 fileAlignment;
    u16 majorOperatingSystemVersion;
    u16 minorOperatingSystemVersion;
    u16 majorImageVersion;
    u16 minorImageVersion;
    u16 majorSubsystemVersion;
    u16 minorSubsystemVersion;
    u32 win32VersionValue;
    u32 sizeOfImage;
    u32 sizeOfHeaders;
    u32 checksum;
    u16 subsystem;
    u16 dllCharacteristics;
    u64 sizeOfStackReserve;
    u64 sizeOfStackCommit;
    u64 sizeOfHeapReserve;
    u64 sizeOfHeapCommit;
    u32 loaderFlags;
    u32 numberOfRvaAndSizes;
    PeDataDirectory dataDirectory[PE_DATA_DIRECTORY_COUNT];
} PeOptionalHeader64;

typedef struct {
    u32 magic;
    PeHeader header;
    PeOptionalHeader64 optional;
} PeHeaders64;

#define PE_SECTION_NAME_LENGTH (8)

typedef struct {
    char name[PE_SECTION_NAME_LENGTH];
    union {
        u32 physicalAddress;
        u32 virtualSize;
    };
    u32 virtualAddress;
    u32 sizeOfRawData;
    u32 pointerToRawData;
    u32 pointerToRelocations;
    u32 pointerToLineNumbers;
    u16 numberOfRelocations;
    u16 numberOfLineNumbers;
    u32 characteristics;
} PeSectionHeader;

typedef struct {
    u16 hint;
    char name[];
} PeImportByName;

typedef struct {
    union {
        u32 characteristics;
        u32 originalFirstThunk;
    };
    u32 timeDateStamp;
    u32 forwarderChain;
    u32 name;
    u32 firstThunk;
} PeImageImportDescriptor;

typedef struct {
    u32 flags;
    u32 timeDateStamp;
    u16 majorVersion;
    u16 minorVersion;
    u32 name;
    u32 ordinalBase;
    u32 addressCount;
    u32 nameCount;
    u32 exportPointer;
    u32 namePointer;
    u32 ordinalPointer;
} PeExportHeader;

#endif //DLWIN_HEADERS_PE_H
