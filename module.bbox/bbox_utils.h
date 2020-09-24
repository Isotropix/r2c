//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef REDSHIFT_UTILS_H
#define REDSHIFT_UTILS_H

#include <core_hash_table.h>
#include <gmath_bbox3.h>
#include <gmath_matrix4x4.h>
#include <gmath_vec3.h>
#include <r2c_scene_delegate.h>
#include <module_light_bbox.h>

class OfAttr;
class ModuleMaterial;

struct LightData {
    ModuleLightBbox *light_module;
};

/*! \class BboxLightInfo
    \brief internal class holding Bbox light data */
class BboxLightInfo {
public:
    int dirtiness; // dirtiness state of the item
    BboxLightInfo() : dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}
    LightData light_data;

};

typedef CoreHashTable<R2cItemId, BboxLightInfo> BboxLightIndex;

/*! \class bboxResourceInfo
    \brief internal class holding the actual geometric resource data */
class BboxResourceInfo {
public:
    unsigned int refcount; //!< internal refcount used to keep track of the number of requesters
    GMathBbox3d bbox;
    BboxResourceInfo() : refcount(0) {}
};

typedef CoreHashTable<R2cResourceId, BboxResourceInfo> BBResourceIndex;

/*! \class BBGeometryInfo
    \brief internal class holding Bbox geometry data (instance pointing to a resource) */
class BboxGeometryInfo {
public:
    bool visibility;
    GMathMatrix4x4d transform;
    R2cResourceId resource; //!< id to the actual Clarisse geometry resource
    int dirtiness; //!< dirtiness state of the item
    BboxGeometryInfo() : resource(nullptr), dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}
};

typedef CoreHashTable<R2cItemId, BboxGeometryInfo> BBGeometryIndex;

/*! \class BBInstancerInfo
    \brief internal class holding instancer data which is basically a list of Bbox point clouds instancing a geometry */
class BboxInstancerInfo {
public:
    bool visibility;
    GMathMatrix4x4d transform;
    R2cResourceId resource; //!< id to the actual Clarisse geometry resource
    int dirtiness; //!< dirtiness state of the item
    BboxInstancerInfo() : dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}
};

typedef CoreHashTable<R2cItemId, BboxInstancerInfo> BBInstancerIndex;


class BboxUtils {
public :
    static void create_light(const R2cSceneDelegate& render_delegate, R2cItemId item_id, BboxLightInfo& light_info);
};

#endif
