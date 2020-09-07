#
# Copyright 2020 - present Isotropix SAS. See License.txt for license information
#


if (ISOTROPIX_INTERNAL_BUILD)

    # Basic compilation options used for R2C's libraries and modules.
    if (LINUX_SYSTEM)
        set(COMPILER_RELEASE "-fexpensive-optimizations -O3 -fPIC -ffast-math -mfpmath=sse -msse2 -fno-strict-aliasing")
        set(LINKER_RELEASE "-s")
        set(COMPILER_DEBUG "-g -fPIC")
        set(LINKER_DEBUG "")
    elseif (WIN_SYSTEM)
        set(COMPILER_RELEASE "/MD /fp:fast /EHs /Ox")
        set(LINKER_RELEASE "")
        set(COMPILER_DEBUG "/MD /fp:fast /EHs /Zi")
        set(LINKER_DEBUG "/DEBUG")
    elseif (OSX_SYSTEM)
        set(COMPILER_RELEASE "-O3 -fPIC -ffast-math -mfpmath=sse -msse2 -fno-strict-aliasing")
        set(LINKER_RELEASE "-dynamiclib")
        set(COMPILER_DEBUG "-g -fPIC")
        set(LINKER_DEBUG "-dynamiclib")
    endif()

    # Encapsulate our helper macro
    function (ix_setup_properties NAME)
        set (TARGET_NAME ${NAME})
        ix_auto_setup_properties ()
    endfunction ()

else ()

    # check required variables
    if (NOT DEFINED CLARISSE_INSTALL_DIR)
        message (FATAL_ERROR "CLARISSE_INSTALL_DIR not set. This should point to the directory where Clarisse's executable is located.")
    endif ()
    if (NOT DEFINED CLARISSE_SDK_DIR)
        message (FATAL_ERROR "CLARISSE_SDK_DIR not set. This should point to the directory where the SDK is located.")
    endif ()

    # to mimic our internal build setup
    function (ix_setup_properties NAME)
    endfunction ()

    # avoid CMake conflicts with potentially already installed modules
    list (APPEND CLARISSE_IGNORED_LIBRARIES IX_R2C)
    list (APPEND CLARISSE_IGNORED_MODULES LAYER_R2C_SCENE REDSHIFT)

    # load our helper script and a few required packages
    list (APPEND CMAKE_MODULE_PATH ${CLARISSE_SDK_DIR}/cmake)
    find_package (ClarisseSDK REQUIRED)
    find_package (OpenGL REQUIRED)
    find_package (PythonLibs 2.7 REQUIRED)

    # this will make CMake output our modules and libraries in a "bin" subdirectory of the build folder, and is needed by our helper scripts
    set (IX_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")

    # include our helper file
    include (${CLARISSE_CMAKE_HELPERS})

endif ()
