# include/ldl/CMakeLists.txt
#
# SPDX-License-Identifier: GNU GENERAL PUBLIC LICENSE Version 3 (GNU GPL-3.0)

file(GLOB HELPER_HEADERS "helpers/*.hpp")
target_sources(
    ldl
    PUBLIC
        FILE_SET ldl_headers
        TYPE HEADERS
        FILES object_deserializer.hpp ${HELPER_HEADERS}
)

install(
    TARGETS ldl
    FILE_SET ldl_headers
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT ldl_development
)

install(
    TARGETS ldl
    EXPORT ldl_export
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET ldl_headers
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT ldl_development
)

install(
    EXPORT ldl_export
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ldl/
    NAMESPACE LDL::
    FILE ldl.cmake
    EXPORT_LINK_INTERFACE_LIBRARIES
    COMPONENT ldl_development
)