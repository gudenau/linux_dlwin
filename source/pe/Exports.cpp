#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "error.h"
#include "types.h"
#include "util.h"

#include "pe/Exports.h"

using namespace pe;

Exports::Exports(Segment* segment, const void* rawDirectory, DlWinError* error) {
    if(nullCheck(error, 3)) {
        abort();
    }

    auto result = nullCheck(segment, 1);
    if(result) {
        *error = result;
        return;
    }

    result = nullCheck(rawDirectory, 2);
    if(result) {
        *error = result;
        return;
    }

    auto directory = (ImageExportDirectory*) rawDirectory;

    m_ordinalBase = directory->base;
    m_exportCount = directory->numberOfFunctions < directory->numberOfNames ? directory->numberOfFunctions : directory->numberOfNames;
    auto functions = (u32*) segment->rva(directory->addressOfFunctions);
    auto names = (u32*) segment->rva(directory->addressOfNames);
    auto ordinals = (u32*) segment->rva(directory->addressOfNameOrdinals);

    m_exports = new Export[m_exportCount];

    // TODO Better data structure
    for(u32 i = 0; i < m_exportCount; i++) {
        auto current = &m_exports[i];
        current->name = (const char*) segment->rva(names[i]);
        current->pointer = segment->rva(functions[i]);
        current->ordinal = ordinals[i];
    }
}

Exports::~Exports(void) {
    if(m_exports) {
        delete m_exports;
    }
}

void* Exports::symbol(const char* name) {
    if(!name) {
        return nullptr;
    }

    for(u32 i = 0; i < m_exportCount; i++) {
        auto current = &m_exports[i];
        if(strcmp(current->name, name) == 0) {
            return current->pointer;
        }
    }

    return nullptr;
}

void* Exports::symbol(u32 ordinal) {
    if(ordinal < m_ordinalBase || ordinal >= m_ordinalBase + m_exportCount) {
        return nullptr;
    }

    return m_exports[ordinal - m_ordinalBase].pointer;
}

