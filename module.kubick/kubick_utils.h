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
#include "./kubick_module_material.h"
#include "./kubick_module_light.h"

// Forward declaration
class RayGeneratorCamera;


/*********************************** CUSTOM GEOMETRY ***********************************/

// Example of simple custom geometry. Here we simply raytrace against bounding boxes.
class KubickBbox {
public:
    KubickBbox() {}

    template <class U>
    inline KubickBbox(const GMathBbox3<U>& clarisse_bbox)
        : m_params{ clarisse_bbox[0], clarisse_bbox[1] } {
    }

    inline bool intersect(const GMathRay& ray, double& tmin, double& tmax, GMathVec3d& normal) const {
        double tymin, tymax;
        normal = GMathVec3d(1.0, 0.0, 0.0);
        tmin = (m_params[ray.get_sign()[0]][0] - ray.get_position()[0]) * ray.get_inverse_direction()[0];
        tmax = (m_params[1 - ray.get_sign()[0]][0] - ray.get_position()[0]) * ray.get_inverse_direction()[0];

        tymin = (m_params[ray.get_sign()[1]][1] - ray.get_position()[1]) * ray.get_inverse_direction()[1];
        tymax = (m_params[1-ray.get_sign()[1]][1] - ray.get_position()[1]) * ray.get_inverse_direction()[1];

        if ((tmin > tymax) || (tymin > tmax)) return false;
        if (tymin > tmin) { tmin = tymin; normal = GMathVec3d(0.0, 1.0, 0.0); }
        if (tymax < tmax) tmax = tymax;

        tymin = (m_params[ray.get_sign()[2]][2] - ray.get_position()[2]) * ray.get_inverse_direction()[2];
        tymax = (m_params[1 - ray.get_sign()[2]][2] - ray.get_position()[2]) * ray.get_inverse_direction()[2];

        if ((tmin > tymax) || (tymin > tmax)) return false;
        if (tymin > tmin) { tmin = tymin; normal = GMathVec3d(0.0, 0.0, 1.0); }
        if (tymax < tmax) tmax = tymax;
        return ((tmin < gmath_infinity) && (tmax > gmath_epsilon));
    }
    
    inline void get_corner_vertices(GMathVec3d *vertices) const {
        // bottom quad
        vertices[0][0] = m_params[0][0]; vertices[0][1] = m_params[0][1]; vertices[0][2] = m_params[0][2];
        vertices[1][0] = m_params[1][0]; vertices[1][1] = m_params[0][1]; vertices[1][2] = m_params[0][2];
        vertices[2][0] = m_params[1][0]; vertices[2][1] = m_params[0][1]; vertices[2][2] = m_params[1][2];
        vertices[3][0] = m_params[0][0]; vertices[3][1] = m_params[0][1]; vertices[3][2] = m_params[1][2];
        // upper quad
        vertices[4][0] = m_params[0][0]; vertices[4][1] = m_params[1][1]; vertices[4][2] = m_params[0][2];
        vertices[5][0] = m_params[1][0]; vertices[5][1] = m_params[1][1]; vertices[5][2] = m_params[0][2];
        vertices[6][0] = m_params[1][0]; vertices[6][1] = m_params[1][1]; vertices[6][2] = m_params[1][2];
        vertices[7][0] = m_params[0][0]; vertices[7][1] = m_params[1][1]; vertices[7][2] = m_params[1][2];
    }

    inline void transform_bbox_and_get_bbox(const GMathMatrix4x4d& matrix, KubickBbox& result) const {
        GMathVec3d bbox_vertices[9];
        get_corner_vertices(bbox_vertices);
        result[0][0] = result[0][1] = result[0][2] = gmath_infinity;
        result[1][0] = result[1][1] = result[1][2] = -gmath_infinity;
        unsigned int i, j;
        for (i = 0; i < 8; i++) {
            GMathMatrix4x4d::multiply(bbox_vertices[8], bbox_vertices[i], matrix);
            for (j = 0; j < 3; j++) {
                if (bbox_vertices[8][j] < result[0][j]) {
                    result[0][j] = bbox_vertices[8][j];
                }
                if (bbox_vertices[8][j] > result[1][j]) {
                    result[1][j] = bbox_vertices[8][j];
                }
            }
        }
    }

	inline GMathVec3d& operator[](const unsigned int& index) { return m_params[index]; }
    inline const GMathVec3d& operator[](const unsigned int& index) const { return m_params[index]; }

private:
    GMathVec3d m_params[2];
};


/*********************************** CAMERA ***********************************/

class KubickCamera {
public:
    void init_ray_generator(const R2cSceneDelegate &delegate, const unsigned int width, const unsigned int height);
    GMathRay generate_ray(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y);

private :
    RayGeneratorCamera *m_ray_generator;
};


/*********************************** LIGHT ***********************************/

struct LightData {
    ModuleLightKubick *light_module;
};

/*! \class KubickLightInfo
    \brief internal class holding Bbox light data */
class KubickLightInfo {
public:
    KubickLightInfo() : dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}

    LightData light_data;
    int dirtiness; // dirtiness state of the item
};

typedef CoreHashTable<R2cItemId, KubickLightInfo> KubickLightIndex;


/*********************************** MATERIAL ***********************************/

struct MaterialData {
    MaterialData(): material_module(nullptr) {}
    MaterialData(ModuleMaterialKubick* module): material_module(module) {}
    ModuleMaterialKubick *material_module;
};

/*! \class KubickResourceInfo
    \brief internal class holding the actual geometric resource data */
class KubickResourceInfo {
public:
    unsigned int refcount; //!< internal refcount used to keep track of the number of requesters
    KubickBbox bbox;
    KubickResourceInfo() : refcount(0) {}
};

typedef CoreHashTable<R2cResourceId, KubickResourceInfo> KubickResourceIndex;


/*********************************** GEOMETRY ***********************************/

/*! \class KubickGeometryInfo
    \brief internal class holding Bbox geometry data (instance pointing to a resource) */
class KubickGeometryInfo {
public:
    bool visibility;
    GMathMatrix4x4d transform;
    R2cResourceId resource; //!< id to the actual Clarisse geometry resource
    MaterialData material;
    int dirtiness; //!< dirtiness state of the item
    KubickGeometryInfo() : resource(nullptr), dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}
};

typedef CoreHashTable<R2cItemId, KubickGeometryInfo> DummyGeometryIndex;

/*! \class BBInstancerInfo
    \brief internal class holding instancer data which is basically a list of Bbox point clouds instancing a geometry */
class KubickInstancerInfo {
public:
    bool visibility;
    GMathMatrix4x4d transform;
    R2cResourceId resource; //!< id to the actual Clarisse geometry resource
    MaterialData material;
    int dirtiness; //!< dirtiness state of the item
    KubickInstancerInfo() : resource(nullptr), dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}
};

typedef CoreHashTable<R2cItemId, KubickInstancerInfo> KubickInstancerIndex;


/*********************************** HELPERS ***********************************/

namespace KubickUtils {
    void create_light(const R2cSceneDelegate& render_delegate, R2cItemId item_id, KubickLightInfo& light_info);
};
