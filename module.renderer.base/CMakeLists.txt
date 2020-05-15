#
# Copyright 2020 - present Isotropix SAS. See License.txt for license information
#

set (SOURCES
    main.cc
    renderer_base.cc
)

set (CID_FILES
    renderer_base.cid
)

add_clarisse_module (renderer_base
    "${SOURCES}"
    ""
    "${CID_FILES}"
    ""
)

target_link_libraries (renderer_base
    PRIVATE
        ${CLARISSE_IX_CORE_LIBRARY}
        ${CLARISSE_IX_MODULE_LIBRARY}
        ${CLARISSE_IX_OF_LIBRARY}
)

# patch a few things needed on Windows to allow building r2c in Debug while still
# working correctly with the release version of Clarisse.
# (in the future this will be done by the add_clarisse_module helper function)
target_compile_definitions (renderer_base PRIVATE $<$<PLATFORM_ID:Windows>:NDEBUG>)
target_compile_options (renderer_base PRIVATE $<$<PLATFORM_ID:Windows>:/MD>)
