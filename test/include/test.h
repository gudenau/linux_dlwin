#ifndef H_TEST
#define H_TEST

#include <stdint.h>

#ifndef EXPORT
#define EXPORT
#endif

EXPORT void test(void);
EXPORT uint8_t test8(uint8_t a, uint8_t b);
EXPORT uint16_t test16(uint16_t a, uint16_t b);
EXPORT uint32_t test32(uint32_t a, uint32_t b);
EXPORT uint64_t test64(uint64_t a, uint64_t b);

#endif
