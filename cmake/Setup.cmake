#
# Copyright 2020 - present Isotropix SAS. See License.txt for license information
#

# check required variables
if (NOT DEFINED CLARISSE_INSTALL_DIR)
    message (FATAL_ERROR "CLARISSE_INSTALL_DIR not set. This should point to the directory where Clarisse's executable is located.")
endif ()
if (NOT DEFINED CLARISSE_SDK_DIR)
    message (FATAL_ERROR "CLARISSE_SDK_DIR not set. This should point to the directory where the SDK is located.")
endif ()

# avoid CMake conflicts with potentially already installed modules
set (CLARISSE_IGNORED_LIBRARIES IX_R2C)
set (CLARISSE_IGNORED_MODULES RENDERER_BASE REDSHIFT)

# load our helper script and a few required packages
list (APPEND CMAKE_MODULE_PATH ${CLARISSE_SDK_DIR}/cmake)
find_package (ClarisseSDK REQUIRED)
find_package (OpenGL REQUIRED)
find_package (PythonLibs 2.7 REQUIRED)

# this will make CMake output our modules and libraries in a "bin" subdirectory of the build folder, and is needed by our helper scripts
set (IX_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")

# include our helper file
include (${CLARISSE_CMAKE_HELPERS})
