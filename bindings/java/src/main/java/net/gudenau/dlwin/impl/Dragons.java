package net.gudenau.dlwin.impl;

import sun.misc.Unsafe;

import java.lang.invoke.MethodHandle;
import java.lang.invoke.MethodHandles;
import java.lang.invoke.MethodType;
import java.lang.reflect.AccessibleObject;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.List;

final class Dragons {
    private static final Unsafe UNSAFE = findUnsafe();
    private static final MethodHandles.Lookup LOOKUP = createLookup();

    private Dragons() {
        throw new AssertionError();
    }

    private static Unsafe findUnsafe() {
        List<Throwable> exceptions = new ArrayList<>();
        for(var field : Unsafe.class.getDeclaredFields()) {
            if(field.getType() == Unsafe.class && Modifier.isStatic(field.getModifiers())) {
                try {
                    field.setAccessible(true);
                    if(field.get(null) instanceof Unsafe unsafe) {
                        return unsafe;
                    }
                } catch(Throwable e) {
                    exceptions.add(e);
                }
            }
        }
        var exception = new RuntimeException("Failed to get Unsafe");
        exceptions.forEach(exception::addSuppressed);
        throw exception;
    }

    private static MethodHandles.Lookup createLookup() {
        long override = -1;
        {
            AccessibleObject object;
            try {
                object = (AccessibleObject) UNSAFE.allocateInstance(AccessibleObject.class);
            } catch(Throwable e) {
                throw new RuntimeException("Failed to find override", e);
            }

            for(long cookie = 4; cookie < 16; cookie++) {
                object.setAccessible(false);
                if(UNSAFE.getBoolean(object, cookie)) {
                    continue;
                }
                object.setAccessible(true);
                if(UNSAFE.getBoolean(object, cookie)) {
                    override = cookie;
                    break;
                }
            }

            if(override == -1) {
                throw new RuntimeException("Failed to find override");
            }
        }

        try {
            var constructor = MethodHandles.Lookup.class.getDeclaredConstructor(Class.class, Class.class, int.class);
            UNSAFE.putBoolean(constructor, override, true);
            return constructor.newInstance(Object.class, null, -1);
        } catch(Throwable e) {
            throw new RuntimeException("Failed to create lookup", e);
        }
    }

    @SuppressWarnings("unchecked")
    public static <T> Class<T> findClass(String name) {
        try {
            return (Class<T>) Class.forName(name);
        } catch(Throwable e) {
            throw new RuntimeException("Failed to find class " + name, e);
        }
    }

    public static MethodHandle findStaticMethod(Class<?> owner, String name, MethodType type) {
        try {
            return LOOKUP.findStatic(owner, name, type);
        } catch(NoSuchMethodException | IllegalAccessException e) {
            throw new RuntimeException("Failed to find " + owner.getName() + '#' + name, e);
        }
    }
}
