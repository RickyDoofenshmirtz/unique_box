cmake_minimum_required(VERSION 4.1.0)
project(UNIQUE_BOX LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Complier path: ${CMAKE_CXX_COMPILER}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wconversion -Wpedantic")
    message(STATUS "C++ Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -w")
    message(STATUS "C++ Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(unique_box ${SRC_FILES})

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*)
add_library(lib_unique_handle ${HEADER_FILES})
target_include_directories(lib_unique_handle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(unique_box PRIVATE stdc++exp)
    message(STATUS "using g++, linking stdc++exp")
endif()
