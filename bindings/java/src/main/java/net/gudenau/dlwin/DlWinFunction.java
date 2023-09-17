package net.gudenau.dlwin;

import java.lang.foreign.FunctionDescriptor;
import java.lang.invoke.MethodHandle;
import java.util.Objects;

public record DlWinFunction(
    String name,
    MethodHandle handle,
    FunctionDescriptor descriptor,
    int ordinal
) {
    public  DlWinFunction {
        Objects.requireNonNull(name, "name can't be null");
        Objects.requireNonNull(handle, "handle can't be null");
        Objects.requireNonNull(descriptor, "descriptor can't be null");
    }
}
