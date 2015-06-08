cmake_minimum_required(VERSION 3.0.2)
#project(Constructor CXX)

update_compiler_settings()

# list all header files from src and include folders automatically 
file(GLOB HEADER_FILES         ${CMAKE_CURRENT_LIST_DIR}/include/*.h)
file(GLOB PRIVATE_HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

# list all source files from src and test folders automatically 
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src/  SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/test/ TEST_FILES)

# Form binaries
add_library(Constructor STATIC
    ${HEADER_FILES}
    ${PRIVATE_HEADER_FILES}
    ${SOURCE_FILES}
)

add_executable(ConstructorTest
    ${TEST_FILES}
)

set_property (TARGET Constructor      PROPERTY FOLDER "Libraries")
set_property (TARGET ConstructorTest  PROPERTY FOLDER "UnitTests")

# set include directories
# set property for library 
set_property(
    TARGET Constructor
    PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/src
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/Utils
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/Utils/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include
)

#set property for the test executable
set_property(
    TARGET ConstructorTest 
    PROPERTY INCLUDE_DIRECTORIES ${gtest_SOURCE_DIR}/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/src
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/Utils
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/Utils/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include
)

# exclude file from build. this cpp files are included to another cpp
set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/src/ConstructionLibraryPrimitives.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/MeshLibraryPrimitives.cpp
    PROPERTIES
    HEADER_FILE_ONLY true
)

# add required definitions (tr1 tuple is not supported under VS11)
set_target_properties(ConstructorTest         PROPERTIES COMPILE_FLAGS -DGTEST_HAS_TR1_TUPLE=0)

add_dependencies(ConstructorTest Constructor gtest)
target_link_libraries(ConstructorTest Constructor gtest)

# files layout for all targets
source_group ("Headers"             FILES ${HEADER_FILES})
source_group ("Sources"             FILES ${SOURCE_FILES})
source_group ("Private headers"     FILES ${PRIVATE_HEADER_FILES})
source_group ("Tests"               FILES ${TEST_FILES})

# add tests
add_test(
    NAME    ConstructorUnitTest
    WORKING_DIRECTORY ConstructorTest_BINARY_DIR
    COMMAND ConstructorTest
)

