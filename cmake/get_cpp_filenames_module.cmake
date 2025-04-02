# get_cpp_filenames_module.cmake
#
# SPDX-License-Identifier: GNU GENERAL PUBLIC LICENSE Version 3 (GNU GPL-3.0)

function(get_filenames_without_extensions folder_name extension variable_name)
    file(GLOB_RECURSE files "${folder_name}/*${extension}")

    set(file_list "")  # Temporary local variable to store filenames
    foreach(file ${files})
        get_filename_component(filename_no_ext ${file} NAME_WE)
        list(APPEND file_list "${filename_no_ext}")
    endforeach()
    list(REMOVE_DUPLICATES file_list)

    # Correctly set the variable in the parent scope
    set(${variable_name} "${file_list}" PARENT_SCOPE)
endfunction()
