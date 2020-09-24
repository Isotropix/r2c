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

class OfAttr;
class ModuleMaterial;


/*! \class BboxLightInfo
    \brief internal class holding Bbox light data */
class BboxLightInfo {
public:
    int dirtiness; // dirtiness state of the item
    BboxLightInfo() : dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}
};

typedef CoreHashTable<R2cItemId, BboxLightInfo> BBLightIndex;

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
    CoreArray<GMathMatrix4x4d> transforms;
    CoreArray<R2cResourceId> resources; //!< list of unique resources for used by all prototypes the number of resources can be smaller that the number of prototypes if there's deduplication
    int dirtiness; //!< dirtiness state of the item
    BboxInstancerInfo() : dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}
};

typedef CoreHashTable<R2cItemId, BboxInstancerInfo> BBInstancerIndex;

#endif
