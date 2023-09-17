package net.gudenau.dlwin;

import net.gudenau.dlwin.impl.DlWinImpl;

import java.lang.foreign.FunctionDescriptor;
import java.lang.invoke.MethodHandle;
import java.nio.file.Path;

public sealed interface DlWin extends AutoCloseable permits DlWinImpl {
    static DlWin open(Path path) {
        return DlWinImpl.open(path);
    }

    void bind(String dll, DlWinFunction... functions);
    MethodHandle symbol(String name, FunctionDescriptor descriptor);
    @Override
    void close();
}
