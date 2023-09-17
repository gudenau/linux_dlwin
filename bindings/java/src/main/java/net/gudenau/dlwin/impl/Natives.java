package net.gudenau.dlwin.impl;

import java.lang.foreign.*;
import java.lang.invoke.MethodHandle;
import java.lang.invoke.MethodType;
import java.nio.file.Path;

import static java.lang.foreign.ValueLayout.*;

final class Natives {
    private Natives() {
        throw new AssertionError();
    }

    private static final Linker LINKER = Linker.nativeLinker();
    public static final Linker WIN_LINKER;
    static {
        try {
            var Windowsx64Linker = Dragons.findClass("jdk.internal.foreign.abi.x64.windows.Windowsx64Linker");
            var getInstance = Dragons.findStaticMethod(Windowsx64Linker, "getInstance", MethodType.methodType(Windowsx64Linker));
            WIN_LINKER = (Linker) getInstance.invoke();
        } catch(Throwable e) {
            throw new RuntimeException("Failed to get Windows linker", e);
        }
    }
    private static final SymbolLookup LOOKUP = SymbolLookup.libraryLookup(Path.of("../../cmake-build-debug/libdlwin.so"), Arena.global());

    private static MethodHandle function(String name, MemoryLayout result, MemoryLayout... args) {
        var symbol = LOOKUP.find(name)
            .orElseThrow(() -> new RuntimeException("Failed to find " + name));
        var descriptor = result == null ?
            FunctionDescriptor.ofVoid(args) :
            FunctionDescriptor.of(result, args);
        return LINKER.downcallHandle(symbol, descriptor);
    }

    private static final MethodHandle dlwin_open = function("dlwin_open", ADDRESS, ADDRESS);
    public static MemorySegment dlwin_open(String filename) {
        try(var arena = Arena.ofConfined()) {
            return (MemorySegment) dlwin_open.invokeExact(
                arena.allocateUtf8String(filename)
            );
        } catch(Throwable e) {
            throw new RuntimeException("Failed to invoke dlwin_open", e);
        }
    }

    private static final MethodHandle dlwin_sym = function("dlwin_sym", ADDRESS, ADDRESS, ADDRESS);
    public static MemorySegment dlwin_sym(MemorySegment handle, String symbol) {
        try(var arena = Arena.ofConfined()) {
            return (MemorySegment) dlwin_sym.invokeExact(
                handle,
                arena.allocateUtf8String(symbol)
            );
        } catch(Throwable e) {
            throw new RuntimeException("Failed to invoke dlwin_sym", e);
        }
    }

    private static final MethodHandle dlwin_bind = function("dlwin_bind", JAVA_INT, ADDRESS, ADDRESS, ADDRESS);
    public static int dlwin_bind(MemorySegment handle, String dll, MemorySegment functions) {
        try(var arena = Arena.ofConfined()) {
            return (int) dlwin_bind.invokeExact(
                handle,
                arena.allocateUtf8String(dll),
                functions
            );
        } catch(Throwable e) {
            throw new RuntimeException("Failed to invoke dlwin_bind", e);
        }
    }

    private static final MethodHandle dlwin_close = function("dlwin_close", JAVA_INT, ADDRESS);
    public static int dlwin_close(MemorySegment handle) {
        try {
            return (int) dlwin_close.invokeExact(
                handle
            );
        } catch(Throwable e) {
            throw new RuntimeException("Failed to invoke dlwin_close", e);
        }
    }
}
