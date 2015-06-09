cmake_minimum_required(VERSION 3.0.2)
update_compiler_settings()

# list all header files from src and include folders automatically 
file(GLOB HEADER_FILES         ${CMAKE_CURRENT_LIST_DIR}/include/*.h)
file(GLOB PRIVATE_HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
file(GLOB INTERNAL_HEADER_FILES ${PROJECT_SOURCE_DIR}/internal/*.h)
file(GLOB TEST_SCRIPTS         ${CMAKE_CURRENT_LIST_DIR}/test_scripts/*.lua)

# list all source files from src and test folders automatically 
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src/  SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/test/ TEST_FILES)

# files layout for all targets
source_group ("Private headers"     FILES ${PRIVATE_HEADER_FILES})
source_group ("Tests"               FILES ${TEST_FILES})
source_group ("Scripts"             FILES ${TEST_SCRIPTS})

# generator setup
set(GENERATOR_PATH ${CMAKE_CURRENT_LIST_DIR}/generator/script_generator.py)
set(GENERATED_HEADER ${PROJECT_BINARY_DIR}/ScriptEngine_gen.h)
set(GENERATED_SOURCE ${PROJECT_BINARY_DIR}/ScriptEngine_gen.cpp)
file(GLOB GENERATOR_SOURCES ${PROJECT_SOURCE_DIR}/include/*.h)


add_custom_command(
    PRE_BUILD 
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT ${GENERATED_HEADER} ${GENERATED_SOURCE}
    COMMAND python ${GENERATOR_PATH} -ho ${GENERATED_HEADER} -so ${GENERATED_SOURCE} -fl ${GENERATOR_SOURCES}
    DEPENDS ${GENERATOR_SOURCES} ${GENERATOR_PATH}
    COMMENT "--------------> Processing headers. C++ Lua translation <--------------"
)

set_source_files_properties(
    ${GENERATED_HEADER}
    ${GENERATED_SOURCE}
    PROPERTIES GENERATED 1
)

# Form binaries
add_library(Overmind STATIC
    ${HEADER_FILES}
    ${PRIVATE_HEADER_FILES}
    ${INTERNAL_HEADER_FILES}
    ${SOURCE_FILES}
    ${GENERATED_HEADER}
    ${GENERATED_SOURCE}
    ${GENERATOR_PATH}
)

add_executable(OvermindTest
    ${TEST_FILES}
    ${TEST_SCRIPTS}
)

set_property (TARGET Overmind      PROPERTY FOLDER "Libraries")
set_property (TARGET OvermindTest  PROPERTY FOLDER "UnitTests")

# set include directories
# set property for library 
set_property(
    TARGET Overmind
    PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${lua_SOURCE_DIR}/src
    APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/src
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/Utils
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/Utils/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_BINARY_DIR}
    APPEND PROPERTY INCLUDE_DIRECTORIES ${GLM_DIR}
)

#set property for the test executable
set_property(
    TARGET OvermindTest 
    PROPERTY INCLUDE_DIRECTORIES ${gtest_SOURCE_DIR}/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${lua_SOURCE_DIR}/src
    APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/src
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/Utils
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/Utils/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include
    APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_BINARY_DIR}
    APPEND PROPERTY INCLUDE_DIRECTORIES ${GLM_DIR}
)

# add required definitions (tr1 tuple is not supported under VS11)
set_target_properties(OvermindTest         PROPERTIES COMPILE_FLAGS -DGTEST_HAS_TR1_TUPLE=0)

add_dependencies(Overmind Constructor lua)
target_link_libraries(Overmind Constructor lua)

add_dependencies(OvermindTest Overmind gtest)
target_link_libraries(OvermindTest Overmind gtest)

add_custom_command(TARGET OvermindTest
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/test_scripts/ ${CMAKE_CURRENT_BINARY_DIR}/test_scripts/
    COMMENT "Copying test_scripts folder to ${CMAKE_CURRENT_BINARY_DIR}/test_scripts/" VERBATIM
)

# add tests
add_test(
    NAME    OvermindUnitTest
    WORKING_DIRECTORY OvermindTest_BINARY_DIR
    COMMAND OvermindTest
)

