//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

// Clarisse includes
#include <core_hash_table.h>
#include <gmath_matrix4x4.h>
#include <gmath_vec3.h>
#include <gmath_bbox3.h>

// R2C includes
#include <r2c_scene_delegate.h>

// Local includes
#include "./spherix_module_material.h"
#include "./spherix_module_light.h"

// Forward declaration
class RayGeneratorCamera;
class ExternalShader;
class ExternalLightShader;
class ExternalMaterialShader;
class ExternalTextureShader;


/*********************************** CUSTOM GEOMETRY ***********************************/

// Example of simple custom geometry. Here we simply raytrace against a sphere using the bounding box diagonal as a radius.
class SpherixSphere {
public:
    SpherixSphere() {}

    template <class U>
    inline SpherixSphere(const GMathBbox3<U>& clarisse_bbox)
    {
        m_radius = (clarisse_bbox.get_max() - clarisse_bbox.get_min()).get_length()/2.0;
        m_inv_radius = 1.0/m_radius;
        m_center = (clarisse_bbox.get_max() + clarisse_bbox.get_min())/2.0;
    }

    void compute_normal(const GMathVec3d& pos, GMathVec3d& normal) const;
    bool intersect(const GMathRay& local_ray, double& t, GMathVec3d& normal) const;
    GMathVec3d get_center() const;

private:
    double m_radius;
    double m_inv_radius;
    GMathVec3d m_center;
};


/*********************************** CAMERA ***********************************/

class SpherixCamera {
public:
    void init_ray_generator(const R2cSceneDelegate &delegate, const unsigned int width, const unsigned int height);
    GMathRay generate_ray(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y);

private :
    RayGeneratorCamera *m_ray_generator;
};


/*********************************** LIGHT ***********************************/

struct LightData {
    ExternalLightShader *shader_light;
};

/*! \class SpherixLightInfo
    \brief internal class holding Spherix light data */
class SpherixLightInfo {
public:
    SpherixLightInfo() : dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}

    LightData light_data;
    int dirtiness; // dirtiness state of the item
};

typedef CoreHashTable<R2cItemId, SpherixLightInfo> SpherixLightIndex;


/*********************************** MATERIAL ***********************************/

struct MaterialData {
    MaterialData(): material(nullptr) {}
    MaterialData(ModuleMaterialSpherix* module): material((module == nullptr) ? nullptr : module->get_material()) {}
    ExternalMaterialShader *material;
};

/*! \class SpherixResourceInfo
    \brief internal class holding the actual geometric resource data */
class SpherixResourceInfo {
public:
    unsigned int refcount; //!< internal refcount used to keep track of the number of requesters
    SpherixSphere sphere;
    SpherixResourceInfo() : refcount(0) {}
};

typedef CoreHashTable<R2cResourceId, SpherixResourceInfo> SpherixResourceIndex;


/*********************************** GEOMETRY ***********************************/

/*! \class SpherixGeometryInfo
    \brief internal class holding Spherix geometry data (instance pointing to a resource) */
class SpherixGeometryInfo {
public:
    bool visibility;
    GMathMatrix4x4d transform;
    R2cResourceId resource; //!< id to the actual Clarisse geometry resource
    MaterialData material;
    int dirtiness; //!< dirtiness state of the item
    SpherixGeometryInfo() : resource(nullptr), dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}
};

typedef CoreHashTable<R2cItemId, SpherixGeometryInfo> SpherixGeometryIndex;

/*! \class SpherixInstancerInfo
    \brief internal class holding instancer data which is basically a list of Spherix point clouds instancing a geometry */
class SpherixInstancerInfo {
public:
    bool visibility;
    GMathMatrix4x4d transform;
    R2cResourceId resource; //!< id to the actual Clarisse geometry resource
    MaterialData material;
    int dirtiness; //!< dirtiness state of the item
    SpherixInstancerInfo() : resource(nullptr), dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}
};

typedef CoreHashTable<R2cItemId, SpherixInstancerInfo> SpherixInstancerIndex;


/*********************************** HELPERS ***********************************/

namespace SpherixUtils {
    void create_light(const R2cSceneDelegate& render_delegate, R2cItemId item_id, SpherixLightInfo& light_info);
}

class SpherixAttributChange {
public :
    static void on_attribute_change(const OfAttr& attr, ExternalShader *shader);
};
