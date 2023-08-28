#ifndef __dlwin_internal_pe__
#define __dlwin_internal_pe__

#include "types.h"
#include "error.h"
#include "misc.h"

#include "pe/Exports.h"
#include "pe/Segment.h"

#define DOS_MAGIC   (0x5A4D)
#define PE_MAGIC    (0x00004550)

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES (16)

typedef enum {
    IMAGE_FILE_MACHINE_UNKNOWN = 0x0,
    IMAGE_FILE_MACHINE_ALPHA = 0x184,
    IMAGE_FILE_MACHINE_ALPHA64 = 0x284,
    IMAGE_FILE_MACHINE_AM33 = 0x1d3,
    IMAGE_FILE_MACHINE_AMD64 = 0x8664,
    IMAGE_FILE_MACHINE_ARM = 0x1c0,
    IMAGE_FILE_MACHINE_ARM64 = 0xaa64,
    IMAGE_FILE_MACHINE_ARMNT = 0x1c4,
    IMAGE_FILE_MACHINE_AXP64 = 0x284,
    IMAGE_FILE_MACHINE_EBC = 0xebc,
    IMAGE_FILE_MACHINE_I386 = 0x14c,
    IMAGE_FILE_MACHINE_IA64 = 0x200,
    IMAGE_FILE_MACHINE_LOONGARCH32 = 0x6232,
    IMAGE_FILE_MACHINE_LOONGARCH64 = 0x6264,
    IMAGE_FILE_MACHINE_M32R = 0x9041,
    IMAGE_FILE_MACHINE_MIPS16 = 0x266,
    IMAGE_FILE_MACHINE_MIPSFPU = 0x366,
    IMAGE_FILE_MACHINE_MIPSFPU16 = 0x466,
    IMAGE_FILE_MACHINE_POWERPC = 0x1f0,
    IMAGE_FILE_MACHINE_POWERPCFP = 0x1f1,
    IMAGE_FILE_MACHINE_R4000 = 0x166,
    IMAGE_FILE_MACHINE_RISCV32 = 0x5032,
    IMAGE_FILE_MACHINE_RISCV64 = 0x5064,
    IMAGE_FILE_MACHINE_RISCV128 = 0x5128,
    IMAGE_FILE_MACHINE_SH3 = 0x1a2,
    IMAGE_FILE_MACHINE_SH3DSP = 0x1a3,
    IMAGE_FILE_MACHINE_SH4 = 0x1a6,
    IMAGE_FILE_MACHINE_SH5 = 0x1a8,
    IMAGE_FILE_MACHINE_THUMB = 0x1c2,
    IMAGE_FILE_MACHINE_WCEMIPSV2 = 0x169,
} MachineType;

typedef struct {
    u16 e_magic;
    u16 e_cblp;
    u16 e_cp;
    u16 e_crlc;
    u16 e_cparhdr;
    u16 e_minalloc;
    u16 e_maxalloc;
    u16 e_ss;
    u16 e_sp;
    u16 e_csum;
    u16 e_ip;
    u16 e_cs;
    u16 e_lfarlc;
    u16 e_ovno;
    u16 e_res[4];
    u16 e_oemid;
    u16 e_oeminfo;
    u16 e_res2[10];
    u32 e_lfanew;
} DosHeader;

typedef struct {
    MachineType machine : 16;
    u16 numberOfSections;
    u32 timeDateStamp;
    u32 pointerToSymbolTable;
    u32 numberOfSymbols;
    u16 sizeOfOptionalHeader;
    u16 characteristics;
} CoffFileHeader;

typedef struct {
    u32 virtualAddress;
    u32 size;
} DataDirectory;

typedef struct {
    u16 magic;
    u8 majorLinkerVersion;
    u8 minorLinkerVersion;
    u32 sizeOfCode;
    u32 sizeOfInitializedData;
    u32 sizeOfUninitilizedData;
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
    DataDirectory dataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} ImageOptionalHeader;

struct PeFile {
    pe::Segment** sections;
    pe::Exports* exports;
    File* file;

    CoffFileHeader coffHeader;
    ImageOptionalHeader optionalHeader;

    int sectionCount;
};

#endif
