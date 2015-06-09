cmake_minimum_required(VERSION 3.0.2)

include_directories(include)
include_directories(${PROJECT_SOURCE_DIR}/include)

# list all header files from src and include folders automatically 
file(GLOB HEADER_FILES         ${CMAKE_CURRENT_LIST_DIR}/include/*.h)
file(GLOB PRIVATE_HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

# list all source files from src and test folders automatically 
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src/  SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/test/  TEST_FILES)

# Form binaries
#add_library(Utils
#    ${HEADER_FILES}
#    ${PRIVATE_HEADER_FILES}
#    ${SOURCE_FILES}
#)
#set_property (TARGET Utils       PROPERTY FOLDER "Libraries")

# this test is just an application to check created algorithms
add_executable(UtilsTests
    ${TEST_FILES}
)

set_property (TARGET UtilsTests       PROPERTY FOLDER "UnitTests")

set_property(
    TARGET UtilsTests 
    PROPERTY INCLUDE_DIRECTORIES ${gtest_SOURCE_DIR}/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/src
)

set_target_properties(UtilsTests         PROPERTIES COMPILE_FLAGS -DGTEST_HAS_TR1_TUPLE=0)

add_dependencies(UtilsTests gtest)
target_link_libraries(UtilsTests gtest)

source_group ("Tests"               FILES ${TEST_FILES})

# add tests
add_test(
    NAME    UtilsUnitTest
    WORKING_DIRECTORY UtilsTests_BINARY_DIR
    COMMAND UtilsTests
)