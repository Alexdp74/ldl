# examples/CMakeLists.txt
#
# SPDX-License-Identifier: GNU GENERAL PUBLIC LICENSE Version 3 (GNU GPL-3.0)

set(LDL_LIBRARY "ldl")

include(GNUInstallDirs)
include(${CMAKE_SOURCE_DIR}/cmake/get_cpp_filenames_module.cmake)

# Get the list of all buildable examples in the current folder.
get_filenames_without_extensions(
    "./"
    ".cpp"
    EXAMPLES
)

foreach(example ${EXAMPLES})
    # Add example executable.
    add_executable(${example} "")

    # Add example source file.
    target_sources(${example} PRIVATE ${example}.cpp)

    # Link example with the library.
    target_link_libraries(${example} "${LDL_LIBRARY}")

    # Install .
    install(
        TARGETS ${example}
            COMPONENT ldl_examples
            DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endforeach()
