#
# Copyright 2020 - present Isotropix SAS. See License.txt for license information
#

find_package (Doxygen)

if (DOXYGEN_FOUND)

    add_custom_command (
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/doxydocs/html/index.html
        DEPENDS ${DOXYGEN_EXECUTABLE} Doxyfile index.md
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_custom_target (r2c_documentation ALL
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/doxydocs/html/index.html
        COMMENT "Generating documentation..."
    )

else ()

    message (WARNING "Doxygen not found, the documentation will not be generated.")

endif ()
