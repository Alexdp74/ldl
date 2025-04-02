# examples/CMakeLists.txt
#
# SPDX-License-Identifier: GNU GENERAL PUBLIC LICENSE Version 3 (GNU GPL-3.0)

find_package(GTest QUIET)
if (NOT GTest_FOUND)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY  https://github.com/google/googletest.git
        GIT_TAG         v1.16.0  # Or a specific tag/branch
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()
enable_testing()
include(GoogleTest)

# Get the list of all buildable examples in the current folder.
file(GLOB TESTS_SOURCES "./*.cpp")

add_executable(ldl_test)
target_sources(ldl_test PRIVATE ${TESTS_SOURCES})

target_link_libraries(
    ldl_test
    PRIVATE ldl GTest::gtest GTest::gtest_main
)
