#ifndef __internal_pe_segment__
#define __internal_pe_segment__

#include "types.h"

#include "File.h"

namespace pe {
    typedef struct {
        char name[8];
        union {
            u32 physicalAddress;
            u32 virtualSize;
        };
        u32 virtualAddress;
        u32 sizeOfRawData;
        u32 pointerToRawData;
        u32 pointerToRelocations;
        u32 pointerToLinenumbers;
        u16 numberOfRelocations;
        u16 numberOfLinenumbers;
        u32 characteristics;
    } SegmentHeader;

    class Segment {
    public:
        Segment(File* file, size_t base, DlWinError* error);
        ~Segment(void);

        void* data(void);
        size_t dataSize(void);
        const char* name(void);
        void* rva(size_t value);
        bool containsRva(size_t value);

        DlWinError read(File* file);
        DlWinError protect(void);

    private:
        void* m_data;
        size_t m_base;
        SegmentHeader m_header;
        char m_name[9];
    };
};

#endif
