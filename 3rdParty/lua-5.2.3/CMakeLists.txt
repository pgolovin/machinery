cmake_minimum_required(VERSION 2.8.11)
project(lua C)

update_compiler_settings()

if(MSVC)
    add_definitions("/wd4334 /wd4146 /wd4267")
else()
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif()

if(APPLE)
    add_definitions(-DLUA_USE_POSIX -DLUA_USE_DLOPEN)
elseif(WIN32)
    add_definitions(-DLUA_BUILD_AS_DLL -DLUA_COMPAT_MODULE -D_CRT_SECURE_NO_WARNINGS)
else() # Linux
    add_definitions(-DLUA_USE_POSIX -DLUA_COMPAT_ALL)
endif()

# list all header files from src and include folders automatically 
file(GLOB HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

# list all source files from src and test folders automatically 
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src/  SOURCE_FILES)

# Form binaries
add_library(lua
    src/lapi.c
    src/lcode.c
    src/lctype.c
    src/ldebug.c
    src/ldo.c
    src/ldump.c
    src/lfunc.c
    src/lgc.c
    src/llex.c
    src/lmem.c
    src/lobject.c
    src/lopcodes.c
    src/lparser.c
    src/lstate.c
    src/lstring.c
    src/ltable.c
    src/ltm.c
    src/lundump.c
    src/lvm.c
    src/lzio.c
    src/lauxlib.c
    src/lbaselib.c
    src/lbitlib.c
    src/lcorolib.c
    src/ldblib.c
    src/liolib.c
    src/lmathlib.c
    src/loslib.c
    src/lstrlib.c
    src/ltablib.c
    src/loadlib.c
    src/linit.c
)
