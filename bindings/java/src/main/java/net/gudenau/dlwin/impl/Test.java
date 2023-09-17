package net.gudenau.dlwin.impl;

import net.gudenau.dlwin.DlWin;
import net.gudenau.dlwin.DlWinFunction;

import java.lang.foreign.FunctionDescriptor;
import java.lang.foreign.MemorySegment;
import java.nio.file.Path;

import static java.lang.foreign.ValueLayout.*;

public class Test {
    private static int MessageBoxA(MemorySegment window, MemorySegment text, MemorySegment caption, int type) {
        System.out.printf("MessageBoxA(0x%016X, 0x%016X, 0x%016X, %d)\n", window.address(), text.address(), caption.address(), type);
        return 0;
    }

    public static void main(String[] args) throws Throwable {
        try(var dlwin = DlWin.open(Path.of("../../test/test.dll"))) {
            var messageBoxDesc = FunctionDescriptor.of(JAVA_INT, ADDRESS, ADDRESS, ADDRESS, JAVA_INT);
            dlwin.bind(
                "USER32.dll",
                new DlWinFunction(
                    "MessageBoxA",
                    Dragons.findStaticMethod(Test.class, "MessageBoxA", messageBoxDesc.toMethodType()),
                    messageBoxDesc,
                    -1
                )
            );

            var divide = dlwin.symbol("test64", FunctionDescriptor.of(JAVA_LONG, JAVA_LONG, JAVA_LONG));
            var result = (long) divide.invokeExact(65536L, 256L);
            System.out.println(result);

            var test = dlwin.symbol("test", FunctionDescriptor.ofVoid());
            test.invokeExact();
        }
    }
}
