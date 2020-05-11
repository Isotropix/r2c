//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef R2C_EXPORT_H
#define R2C_EXPORT_H

#include <core_platform.h>

#ifdef CORE_WINDOWS
    #ifdef ix_r2c_EXPORTS
        #define R2C_EXPORT __declspec(dllexport)
    #else
        #define R2C_EXPORT __declspec(dllimport)
    #endif
#else
    #define R2C_EXPORT
#endif

#endif
