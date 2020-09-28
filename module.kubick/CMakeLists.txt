#
# Copyright 2020 - present Isotropix SAS. See License.txt for license information
#
set (SOURCES
    kubick_layer.cc
    kubick_light.cc
    main.cc
    kubick_material.cc
    kubick_module_light.cc
    kubick_module_material.cc
    kubick_module_renderer.cc
    kubick_render_delegate.cc
    kubick_utils.cc
    kubick_renderer.cc
    kubick_texture.cc
    kubick_module_texture.cc
)

set (HEADERS
    kubick_module_light.h
    kubick_module_material.h
    kubick_module_renderer.h
    kubick_render_delegate.h
    kubick_utils.h
    kubick_module_texture.h
)

set (CID_FILES
    kubick_layer.cid
    kubick_light.cid
    kubick_material.cid
    kubick_renderer.cid
    kubick_texture.cid
)

set (CID_DEPENDS
    layer_r2c_scene
)

add_clarisse_module (kubick
    "${SOURCES}"
    "${HEADERS}"
    "${CID_FILES}"
    "${CID_DEPENDS}"
)

target_include_directories (kubick
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries (kubick
    PRIVATE
        # helper library
        ix_r2c

        # Clarisse SDK
        ${CLARISSE_IX_CORE_LIBRARY}
        ${CLARISSE_IX_GEOMETRY_LIBRARY}
        ${CLARISSE_IX_MODULE_LIBRARY}
        ${CLARISSE_IX_OF_LIBRARY}
        ${CLARISSE_IX_POLY_LIBRARY}
        ${CLARISSE_IX_SYS_LIBRARY}
        ${CLARISSE_IX_RAYTRACE_LIBRARY}
        ${CLARISSE_IX_SAMPLING_LIBRARY}
        ${CLARISSE_IX_SYS_LIBRARY}
        ${CLARISSE_IX_APP_BASE_LIBRARY}
        ${CLARISSE_IX_SHADING_VARIABLE_LIBRARY}
        ${CLARISSE_IX_CTX_LIBRARY}
)
