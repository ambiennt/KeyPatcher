cmake_minimum_required(VERSION 3.30.5)

project(KeyPatcher LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

# Remove default optimization flags
if (MSVC)
    string(REGEX REPLACE "/Ob[^ ]*|/O[^ ]*" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REGEX REPLACE "/Ob[^ ]*|/O[^ ]*" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REGEX REPLACE "/Ob[^ ]*|/O[^ ]*" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    string(REGEX REPLACE "/Ob[^ ]*|/O[^ ]*" "" CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC") # Just MSVC
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        /permissive-
        /Zc:__cplusplus
        $<$<CONFIG:Debug>:/Od;/Ob0>
        $<$<CONFIG:Release>:/O2;/Ob2>)
elseif (MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang") # Clang-Cl
    target_compile_options(${PROJECT_NAME} PRIVATE
        /clang:-Wall
        /clang:-Wextra
        /clang:-Wpedantic
        $<$<CONFIG:Debug>:/clang:-O0;/Ob0>
        $<$<CONFIG:Release>:/clang:-O3;/Ob2>)
elseif((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:Debug>:-O0;>
        $<$<CONFIG:Release>:-O3>)
endif()