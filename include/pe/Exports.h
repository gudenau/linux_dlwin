#ifndef __internal_pe_exports__
#define __internal_pe_exports__

#include "types.h"
#include "error.h"

#include "pe/Segment.h"

namespace pe {
    typedef struct {
        u32 characteristics;
        u32 timeDateStamp;
        u16 majorVersion;
        u16 minorVersion;
        u32 name;
        u32 base;
        u32 numberOfFunctions;
        u32 numberOfNames;
        u32 addressOfFunctions;
        u32 addressOfNames;
        u32 addressOfNameOrdinals;
    } ImageExportDirectory;

    typedef struct {
        const char* name;
        void* pointer;
        u32 ordinal;
    } Export;

    class Exports {
    public:
        Exports(Segment* segment, const void* directory, DlWinError* error);
        ~Exports(void);

        void* symbol(const char* name);
        void* symbol(u32 ordinal);

    private:
        Export* m_exports;
        u32 m_exportCount;
        u32 m_ordinalBase;
    };
};

#endif
