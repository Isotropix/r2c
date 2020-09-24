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

add_clarisse_library (ix_r2c
    "${SOURCES}"
    "${HEADERS}"
    ""
    ""
)

ix_setup_properties (ix_r2c)

target_include_directories (ix_r2c
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ix_r2c
#    PUBLIC
#        ${CLARISSE_IX_CORE_LIBRARY}
#        ${CLARISSE_IX_EVENT_LIBRARY}
#        ${CLARISSE_IX_GMATH_LIBRARY}
#    PRIVATE
#        ${CLARISSE_IX_CTX_LIBRARY}
#        ${CLARISSE_IX_GEOMETRY_LIBRARY}
#        ${CLARISSE_IX_IMAGE_LIBRARY}
#        ${CLARISSE_IX_MODULE_LIBRARY}
#        ${CLARISSE_IX_OF_LIBRARY}
#        ${CLARISSE_IX_SYS_LIBRARY}
)
