package net.gudenau.dlwin.impl;

import net.gudenau.dlwin.DlWin;
import net.gudenau.dlwin.DlWinFunction;

import java.lang.foreign.Arena;
import java.lang.foreign.FunctionDescriptor;
import java.lang.foreign.MemoryLayout;
import java.lang.foreign.MemorySegment;
import java.lang.invoke.MethodHandle;
import java.lang.invoke.VarHandle;
import java.nio.file.Path;

import static java.lang.foreign.MemoryLayout.paddingLayout;
import static java.lang.foreign.ValueLayout.ADDRESS;
import static java.lang.foreign.ValueLayout.JAVA_INT;

public record DlWinImpl(
    Arena arena,
    MemorySegment handle
) implements DlWin {
    private static RuntimeException exception(String message) {
        //TODO errno
        return exception(message, -1);
    }

    private static RuntimeException exception(String message, int error) {
        //TODO errno
        return new RuntimeException(message);
    }

    public static DlWin open(Path path) {
        var result = Natives.dlwin_open(path.toAbsolutePath().toString());
        if(result.equals(MemorySegment.NULL)) {
            throw exception("Failed to open " + path);
        }
        return new DlWinImpl(Arena.ofShared(), result);
    }

    private static final MemoryLayout DlWinFunc = MemoryLayout.structLayout(
        ADDRESS.withName("name"),
        ADDRESS.withName("function"),
        JAVA_INT.withName("ordinal"),
        paddingLayout(Integer.BYTES)
    );
    private static final VarHandle DlWinFunc$name = DlWinFunc.varHandle(MemoryLayout.PathElement.groupElement("name"));
    private static final VarHandle DlWinFunc$function = DlWinFunc.varHandle(MemoryLayout.PathElement.groupElement("function"));
    private static final VarHandle DlWinFunc$ordinal = DlWinFunc.varHandle(MemoryLayout.PathElement.groupElement("ordinal"));

    @Override
    public void bind(String dll, DlWinFunction... functions) {
        try(var arena = Arena.ofConfined()) {
            var count = functions.length;
            var array = arena.allocateArray(DlWinFunc, count + 1);
            for(int i = 0; i < count; i++) {
                var func = functions[i];
                var entry = array.asSlice(i * DlWinFunc.byteSize(), DlWinFunc.byteSize());
                DlWinFunc$name.set(entry, this.arena.allocateUtf8String(func.name()));
                DlWinFunc$function.set(entry, Natives.WIN_LINKER.upcallStub(func.handle(), func.descriptor(), this.arena));
                DlWinFunc$ordinal.set(entry, func.ordinal());
            }
            var result = Natives.dlwin_bind(handle, dll, array);
            if(result != 0) {
                throw exception("Failed to bind symbols", result);
            }
        }
    }

    @Override
    public MethodHandle symbol(String name, FunctionDescriptor descriptor) {
        var result = Natives.dlwin_sym(handle, name);
        if(result.equals(MemorySegment.NULL)) {
            throw exception("Failed to find symbol " + name);
        }

        return Natives.WIN_LINKER.downcallHandle(result, descriptor);
    }

    @Override
    public void close() {
        Natives.dlwin_close(handle);
    }
}
