# dlwin
A way to load Windows PE files under Linux.

---
### API

```C
dlwin_export dlwin_handle* dlwin_open(const char* filename);
```
Opens a PE file and maps it into memory.

```C
dlwin_export void* dlwin_sym(dlwin_handle* handle, const char* symbol);
```
Retrieves an exported symbol from the opened PE file.

```C
dlwin_export int dlwin_bind(dlwin_handle* handle, const char* dll, DlwinFunction* functions);
```
Binds imported functions to the opened PE file. The functions are a null terminated list.

```C
dlwin_export int dlwin_close(dlwin_handle* handle);
```
Unmaps the PE file, do not call any exported functions from the file after calling this.

---

Example dll source:
```C
#include <stdint.h>

#include <windows.h>

#define EXPORT __declspec(dllexport)

EXPORT void test() {
    MessageBoxA(NULL, "This is a test", NULL, MB_OK);
}

EXPORT uint64_t divide(uint64_t a, uint64_t b) {
    return a / b;
}
```

Example dlwin usage:
```C
#include <stdint.h>
#include <stdlib.h>

#include <dlwin.h>

#define WINAPI __attribute__((ms_abi))

typedef void (*TestFunc)(void) WINAPI;
typedef uint64_t (*DivideFunc)(uint64_t, uint64_t) WINAPI;

WINAPI static int messageBoxHandler(void* window, const char* text, const char* caption, int type) {
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

    TestFunc test = dlwin_sym(handle, "test");
    DivideFunc divide = dlwin_sym(handle, "divide");

    test();
    printf("%ld\n", divide(65536, 256));

    dlwin_close(handle);

    return 0;
}
```
