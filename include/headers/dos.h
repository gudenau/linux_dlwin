#ifndef DLWIN_HEADERS_DOS_H
#define DLWIN_HEADERS_DOS_H

#include "types.h"

#define DOS_HEADER_MAGIC (0x5A4D)

typedef struct {
    u16 magic;
    u16 cblp;
    u16 cp;
    u16 crlc;
    u16 cparhdr;
    u16 minalloc;
    u16 maxalloc;
    u16 ss;
    u16 sp;
    u16 csum;
    u16 ip;
    u16 cs;
    u16 lfarlc;
    u16 ovno;
    u16 res[4];
    u16 oemid;
    u16 oeminfo;
    u16 res2[10];
    u32 lfanew;
} DosHeader;

#endif
