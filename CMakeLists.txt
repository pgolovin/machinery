cmake_minimum_required(VERSION 3.0.2)
set_property (GLOBAL PROPERTY USE_FOLDERS ON)
project(Machinery)

include(cmake/useful_functions.cmake)

update_compiler_settings()
#check_apple_platform()

# Global variables
set(3RDPARTY_UNPACK_DIR ${PROJECT_SOURCE_DIR}/.unpack)
set(3RDPARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/3rdParty)
set(SDL_DIR ${3RDPARTY_UNPACK_DIR}/SDL/SDL2)
set(ANGLE_DIR ${3RDPARTY_DIR}/angle)
set(GLM_DIR ${3RDPARTY_UNPACK_DIR}/GLM)

if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
    set(ARCH_64 TRUE) 
else() 
    set(ARCH_64 FALSE) 
endif()

if (WIN32)
    set (DYNAMIC_LIB_SUFIX .dll)
    set (STATIC_LIB_SUFIX .lib)
    set (BIN2CPP ${3RDPARTY_DIR}/bin2cpp/bin2cpp.exe)
elseif(APPLE)
    set (DYNAMIC_LIB_SUFIX .dylib)
    set (STATIC_LIB_SUFIX .a)
    set (BIN2CPP ${3RDPARTY_DIR}/bin2cpp/bin2cpp_mac)
else()
    set (DYNAMIC_LIB_SUFIX .so)
    set (STATIC_LIB_SUFIX .a)
    set (BIN2CPP ${3RDPARTY_DIR}/bin2cpp/bin2cpp)
endif()

if (WIN32)
    add_definitions(-DSDL_VIDEO_OPENGL_EGL)
    add_definitions(-DSDL_VIDEO_OPENGL_ES2)
endif()

find_package(OpenGL REQUIRED)

# Set global build variables
set (ROOT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

# Set output folder
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add projects
add_subdirectory(3rdParty)
add_subdirectory(Constructor)
add_subdirectory(Overmind)
add_subdirectory(Utils)

# Set additional targets' flags
set_target_properties(gtest         PROPERTIES COMPILE_FLAGS -DGTEST_HAS_TR1_TUPLE=0)
