# examples/CMakeLists.txt
#
# SPDX-License-Identifier: GNU GENERAL PUBLIC LICENSE Version 3 (GNU GPL-3.0)

function(get_cpp_filenames_without_extensions folder variable_name)
    file(GLOB_RECURSE files "${folder}/*.cpp")
    set(${variable_name} "")
    foreach(file ${files})
        get_filename_component(filename_no_ext ${file} NAME_WE)
        list(APPEND ${variable_name} "${filename_no_ext}")
    endforeach()
    list(REMOVE_DUPLICATES ${variable_name})
    set(${variable_name} "${${variable_name}}" PARENT_SCOPE)
endfunction()

set(LDL_LIBRARY "ldl")

include(GNUInstallDirs)

# Get the list of all buildable examples in the current folder.
get_cpp_filenames_without_extensions(
    "./"
    EXAMPLES
)
message("EXAMPLES: ${EXAMPLES}")

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
