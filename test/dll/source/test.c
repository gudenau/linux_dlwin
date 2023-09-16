#include <stdint.h>
#include <stddef.h>

#include <windows.h>

#define EXPORT __declspec(dllexport)
#include "test.h"

void test(void) {
    MessageBoxA(NULL, "This is a test", NULL, MB_OK);
}

uint8_t test8(uint8_t a, uint8_t b) {
    return a + b;
}

uint16_t test16(uint16_t a, uint16_t b) {
    return a - b;
}

uint32_t test32(uint32_t a, uint32_t b) {
    return a * b;
}

uint64_t test64(uint64_t a, uint64_t b) {
    return a / b;
}
