#
# Copyright 2020 - present Isotropix SAS. See License.txt for license information
#
set (SOURCES
    layer_bbox.cc
    light_bbox.cc
    main.cc
    material_bbox.cc
    module_light_bbox.cc
    module_material_bbox.cc
    module_renderer_bbox.cc
    bbox_render_delegate.cc
    bbox_utils.cc
    renderer_bbox.cc
)

set (HEADERS
    module_light_bbox.h
    module_material_bbox.h
    module_renderer_bbox.h
    bbox_render_delegate.h
    bbox_utils.h
)

set (CID_FILES
    layer_bbox.cid
    light_bbox.cid
    material_bbox.cid
    renderer_bbox.cid
)

set (CID_DEPENDS
    layer_r2c_scene
)

add_clarisse_module (bbox
    "${SOURCES}"
    "${HEADERS}"
    "${CID_FILES}"
    "${CID_DEPENDS}"
)

target_include_directories (bbox
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries (bbox
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
)
