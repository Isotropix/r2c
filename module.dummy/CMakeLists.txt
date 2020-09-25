#
# Copyright 2020 - present Isotropix SAS. See License.txt for license information
#
set (SOURCES
    layer_dummy.cc
    light_dummy.cc
    main.cc
    material_dummy.cc
    module_light_dummy.cc
    module_material_dummy.cc
    module_renderer_dummy.cc
    dummy_render_delegate.cc
    dummy_utils.cc
    renderer_dummy.cc
    texture_dummy.cc
    module_texture_dummy.cc
)

set (HEADERS
    module_light_dummy.h
    module_material_dummy.h
    module_renderer_dummy.h
    dummy_render_delegate.h
    dummy_utils.h
    module_texture_dummy.h
)

set (CID_FILES
    layer_dummy.cid
    light_dummy.cid
    material_dummy.cid
    renderer_dummy.cid
    texture_dummy.cid
)

set (CID_DEPENDS
    layer_r2c_scene
)

add_clarisse_module (dummy
    "${SOURCES}"
    "${HEADERS}"
    "${CID_FILES}"
    "${CID_DEPENDS}"
)

target_include_directories (dummy
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries (dummy
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
