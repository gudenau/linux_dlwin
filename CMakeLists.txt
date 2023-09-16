cmake_minimum_required(VERSION 3.26.4)

project(dlwin VERSION 0.0.0 DESCRIPTION "Loading DLLs on Linux")

add_library(dlwin SHARED
    include/pe.h
    include/types.h

    public/dlwin.h

    source/dlwin.c
    source/pe.c
        include/util.h
        source/util.c
        include/headers/dos.h
        include/headers/pe.h
)

set_target_properties(dlwin PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(dlwin PROPERTIES SOVERSION 1)

#set_target_properties(dlwin PROPERTIES PUBLIC_HEADER
#    public/error.h
#    public/misc.h
#    public/pe.h
#    public/types.h
#)

set (CMAKE_C_STANDARD 17)

set (CMAKE_C_COMPILER clang)
set (CMAKE_LINKER clang)

set (CMAKE_C_FLAGS "-fvisibility=hidden -Wall -Wextra -Werror")
set (CMAKE_LINKER_FLAGS "-fuse-ld=lld -fvisibility=hidden")

add_compile_definitions(__dlwin_build__)

target_include_directories(dlwin PRIVATE include)
target_include_directories(dlwin PRIVATE public)

include(GNUInstallDirs)

install(TARGETS dlwin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
