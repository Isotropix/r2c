#
# Copyright 2020 - present Isotropix SAS. See License.txt for license information
#
set (SOURCES
    kubix_layer.cc
    kubix_light.cc
    main.cc
    kubix_material.cc
    kubix_module_light.cc
    kubix_module_material.cc
    kubix_module_renderer.cc
    kubix_render_delegate.cc
    kubix_utils.cc
    kubix_renderer.cc
    kubix_texture.cc
    kubix_module_texture.cc
)

set (HEADERS
    kubix_module_light.h
    kubix_module_material.h
    kubix_module_renderer.h
    kubix_render_delegate.h
    kubix_utils.h
    kubix_module_texture.h
)

set (CID_FILES
    kubix_layer.cid
    kubix_light.cid
    kubix_material.cid
    kubix_renderer.cid
    kubix_texture.cid
)

set (CID_DEPENDS
    layer_r2c_scene
    light
    material
    renderer
    texture_operator
)

add_clarisse_module (kubix
    "${SOURCES}"
    "${HEADERS}"
    "${CID_FILES}"
    "${CID_DEPENDS}"
)

target_include_directories (kubix
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries (kubix
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
