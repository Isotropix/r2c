#
# Copyright 2020 - present Isotropix SAS. See License.txt for license information
#
set (SOURCES
    spherix_layer.cc
    spherix_light.cc
    main.cc
    spherix_material.cc
    spherix_module_light.cc
    spherix_module_material.cc
    spherix_module_renderer.cc
    spherix_render_delegate.cc
    spherix_utils.cc
    spherix_renderer.cc
    spherix_texture.cc
    spherix_module_texture.cc
)

set (HEADERS
    spherix_module_light.h
    spherix_module_material.h
    spherix_module_renderer.h
    spherix_render_delegate.h
    spherix_utils.h
    spherix_module_texture.h
    spherix_external_renderer.h
    spherix_register_shaders.h
)

set (CID_FILES
    spherix_layer.cid
    spherix_light.cid
    spherix_material.cid
    spherix_renderer.cid
    spherix_texture.cid
)

set (CID_DEPENDS
    layer_r2c_scene
)

add_clarisse_module (spherix
    "${SOURCES}"
    "${HEADERS}"
    "${CID_FILES}"
    "${CID_DEPENDS}"
)

target_include_directories (spherix
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries (spherix
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
