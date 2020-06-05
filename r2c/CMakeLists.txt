#
# Copyright 2020 - present Isotropix SAS. See License.txt for license information
#

set (SOURCES
    r2c_instancer.cc
    r2c_module_layer_scene.cc
    r2c_render_delegate.cc
    r2c_scene_delegate.cc
    r2c_common.cc
    r2c_render_buffer.cc
)

set (HEADERS
    r2c_common.h
    r2c_instancer.h
    r2c_module_layer_scene.h
    r2c_render_delegate.h
    r2c_scene_delegate.h
    r2c_render_buffer.h
    r2c_export.h
)

# fix for a bug in our add_clarisse_library helper macro (without this, the output directory
# is not set and ix_r2c is not built in the correct folder, resulting in potential build
# errors later on)
set (TARGET_NAME ix_r2c)

add_clarisse_library (ix_r2c
    "${SOURCES}"
    "${HEADERS}"
    ""
    ""
)

target_include_directories (ix_r2c
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ix_r2c
    PUBLIC
        ${CLARISSE_IX_CORE_LIBRARY}
        ${CLARISSE_IX_EVENT_LIBRARY}
        ${CLARISSE_IX_GMATH_LIBRARY}
    PRIVATE
        ${CLARISSE_IX_CTX_LIBRARY}
        ${CLARISSE_IX_GEOMETRY_LIBRARY}
        ${CLARISSE_IX_IMAGE_LIBRARY}
        ${CLARISSE_IX_MODULE_LIBRARY}
        ${CLARISSE_IX_OF_LIBRARY}
        ${CLARISSE_IX_SYS_LIBRARY}
)

# small fix for a bug in our helper add_clarisse_library function which will install the runtime libs
# in a lib subdirectory of the install prefix, whereas we want it in the install prefix directly.
install (TARGETS ix_r2c DESTINATION .)

# patch a few things needed on Windows to allow building r2c in Debug while still
# working correctly with the release version of Clarisse.
# (in the future this will be done by the add_clarisse_module helper function)
target_compile_definitions (ix_r2c PRIVATE $<$<PLATFORM_ID:Windows>:NDEBUG>)
target_compile_options (ix_r2c PRIVATE $<$<PLATFORM_ID:Windows>:/MD>)
