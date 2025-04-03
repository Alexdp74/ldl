# examples/CMakeLists.txt
#
# SPDX-License-Identifier: GNU GENERAL PUBLIC LICENSE Version 3 (GNU GPL-3.0)

enable_testing()

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
include(GoogleTest)

# Get the list of all buildable examples, without the extension, in the current folder.
get_filenames_without_extensions(
    "./"
    ".cpp"
    TESTS
)
file(GLOB_RECURSE TESTS_HEADERS "*.hpp")

foreach(test ${TESTS})
    set(TEST_NAME "gtest_${test}")
    # Add test executable.
    add_executable(${TEST_NAME} "")

    # Add test source file and headers.
    target_sources(${TEST_NAME} PRIVATE ${test}.cpp)
    target_sources(
        ${TEST_NAME}
        PRIVATE
            FILE_SET ldl_test_headers
            TYPE HEADERS
            FILES ${TESTS_HEADERS}
    )

    # Link test with the library and GTest.
    target_link_libraries(${TEST_NAME} PRIVATE ldl GTest::gtest GTest::gtest_main)

    # Add the test
    add_test("${test}_test" ${TEST_NAME})
endforeach()

