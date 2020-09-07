#
# Copyright 2020 - present Isotropix SAS. See License.txt for license information
#

# check required variables
if (NOT DEFINED REDSHIFT_SDK_DIR)
    message (STATUS "[ module.redshift ] REDSHIFT_SDK_DIR not set, the module will not be built.")
    return ()
endif ()

set (SOURCES
    layer_redshift.cc
    light_redshift.cc
    main.cc
    material_redshift.cc
    module_light_redshift.cc
    module_material_redshift.cc
    module_renderer_redshift.cc
    module_texture_redshift.cc
    redshift_render_delegate.cc
    redshift_utils.cc
    renderer_redshift.cc
    texture_redshift.cc
)

set (HEADERS
    module_light_redshift.h
    module_material_redshift.h
    module_renderer_redshift.h
    module_texture_redshift.h
    redshift_render_delegate.h
    redshift_utils.h
)

set (CID_FILES
    layer_redshift.cid
    light_redshift.cid
    material_redshift.cid
    renderer_redshift.cid
    texture_redshift.cid
)

set (CID_DEPENDS
    layer_r2c_scene
)

add_clarisse_module (redshift
    "${SOURCES}"
    "${HEADERS}"
    "${CID_FILES}"
    "${CID_DEPENDS}"
)

ix_setup_properties (redshift)

target_include_directories (redshift
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}

        # Redshift SDK
        ${REDSHIFT_SDK_DIR}/include
)

target_link_libraries (redshift
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

        # Redshift SDK
        $<$<PLATFORM_ID:Windows>:${REDSHIFT_SDK_DIR}/lib/x64/redshift-core-vc100.lib>
)

# install the Redshift's runtime libs
if (WIN32)
    install (FILES
        ${REDSHIFT_SDK_DIR}/bin/altus-api.dll
        ${REDSHIFT_SDK_DIR}/bin/OpenImageIO-1.6.17-vc100.dll
        ${REDSHIFT_SDK_DIR}/bin/optix.51.dll
        ${REDSHIFT_SDK_DIR}/bin/redshift-core-vc100.dll
        DESTINATION .
    )
endif ()
