# CMakeLists.txt
#
# SPDX-License-Identifier: GNU GENERAL PUBLIC LICENSE Version 3 (GNU GPL-3.0)

cmake_minimum_required(VERSION 3.25)

project(
    ldl
    VERSION 1.0.0
    DESCRIPTION "Little Deserialization Library (LDL)"
    LANGUAGES CXX
)

# [CMAKE.SKIP_TESTS]
option(
    LDL_BUILD_TESTS
    "Enable building tests. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

# [CMAKE.SKIP_EXAMPLES]
option(
    LDL_BUILD_EXAMPLES
    "Enable building examples. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

include(CTest)
include(FetchContent)
include(GNUInstallDirs)

set(CMAKE_VERIFY_INTERFACE_HEADER_SETS ON)

add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(ldl INTERFACE cxx_std_20)
target_sources(
    ldl
    PUBLIC FILE_SET ldl_headers
    TYPE HEADERS
    BASE_DIRS include
)
add_subdirectory(include/ldl)

if(LDL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(LDL_BUILD_TESTS)
    add_subdirectory(tests/ldl)
endif()
