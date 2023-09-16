#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <dlwin.h>

__attribute__((ms_abi)) static int MessageBoxA(void* window, const char* text, const char* caption, int type) {
    printf(
        "MessageBoxA(0x%016lX, %s, %s, %d)\n",
        (size_t) window,
        text ? text : "null",
        caption ? caption : "null",
        type
    );

    return 0;
}

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    dlwin_handle* handle = dlwin_open("test.dll");
    if(!handle) {
        fprintf(stderr, "Failed to open DLL file: %s\n", strerror(errno));
        return 1;
    }

    DlwinFunction user32[] = {
        {
            .name = "MessageBoxA",
            .function = MessageBoxA,
            .ordinal = -1
        },
        {
            .name = NULL,
            .function = NULL,
            .ordinal = 0
        }
    };
    dlwin_bind(handle, "USER32.dll", user32);

    void (*test)(void) = dlwin_sym(handle, "test");
    uint8_t (*test8)(uint8_t, uint8_t) __attribute__((ms_abi)) = dlwin_sym(handle, "test8");
    uint16_t (*test16)(uint16_t, uint16_t) __attribute__((ms_abi)) = dlwin_sym(handle, "test16");
    uint32_t (*test32)(uint32_t, uint32_t) __attribute__((ms_abi)) = dlwin_sym(handle, "test32");
    uint64_t (*test64)(uint64_t, uint64_t) __attribute__((ms_abi)) = dlwin_sym(handle, "test64");

    test();
    printf("%d\n", test8(12, 21));
    printf("%d\n", test16(4096, 255));
    printf("%d\n", test32(10, 100));
    printf("%ld\n", test64(65536, 256));

    dlwin_close(handle);

    return 0;
}
