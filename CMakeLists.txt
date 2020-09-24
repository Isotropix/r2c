#-------------------------------------------------------------------------------
# Copyright 2020 - present Isotropix SAS. See License.txt for license information
#-------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.7)

project (R2C)

#-------------------------------------------------------------------------------
# Setup everthing needed to build R2C
#-------------------------------------------------------------------------------

include (cmake/Setup.cmake)

#-------------------------------------------------------------------------------
# R2C required library and module
#-------------------------------------------------------------------------------

add_subdirectory (r2c)
add_subdirectory (module.layer.r2c.scene)

#-------------------------------------------------------------------------------
# R2c Bbox example
#-------------------------------------------------------------------------------
add_subdirectory (module.dummy)

#-------------------------------------------------------------------------------
# Documentation
#-------------------------------------------------------------------------------

option (R2C_BUILD_DOC "Build documentation with doxygen (if available). ON by default." ON)
if (R2C_BUILD_DOC)
    add_subdirectory (docs)
endif ()

#-------------------------------------------------------------------------------
# Optional Redshift example
#-------------------------------------------------------------------------------

option (R2C_BUILD_REDSHIFT "Build the Redshift example integration (needs the Redshift SDK). ON by default." ON)
if (R2C_BUILD_REDSHIFT)
    add_subdirectory (module.redshift)
endif ()
