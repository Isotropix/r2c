//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <geometry_object.h>

#include "r2c_common.h"

const GeometryObject *
R2cGeometryResource::get_geometry() const
{
    return static_cast<const GeometryObject *>(m_id);
}

OfObject *
R2cItemDescriptor::get_item() const
{
    return is_destroyed() ? nullptr : static_cast<OfObject *>(m_id);
}

CoreString
R2cItemDescriptor::get_type_name(const Type& type)
{
    switch(type) {
        case TYPE_INSTANCER:
            return "TYPE_INSTANCER";
        case TYPE_GEOMETRY:
            return "TYPE_GEOMETRY";
        case TYPE_LIGHT:
            return "TYPE_LIGHT";
        case TYPE_MATERIAL:
            return "TYPE_MATERIAL";
        case TYPE_CAMERA:
            return "TYPE_CAMERA";
        case TYPE_GROUP:
            return "TYPE_GROUP";
        default:
            return "TYPE_UNKNOWN";
    }
}
