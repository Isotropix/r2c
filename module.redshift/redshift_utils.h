//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef REDSHIFT_UTILS_H
#define REDSHIFT_UTILS_H

#include <core_hash_table.h>

#include <gmath_vec3.h>
#include <gmath_bbox3.h>
#include <gmath_matrix4x4.h>

#include <RS.h>

#include <r2c_scene_delegate.h>

/*! \class RSLightInfo
    \brief internal class holding Redshift light data */
class RSLightInfo {
public:
    RSLight *ptr; // pointer to the actual redshift item
    RSShaderNode *shader; // only used for clarisse native lights
    int dirtiness; // dirtiness state of the item
    RSLightInfo() : ptr(nullptr), dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}
};

typedef CoreHashTable<R2cItemId, RSLightInfo> RSLightIndex;

/*! \class RSResourceInfo
    \brief internal class holding the actual geometric resource data */
class RSResourceInfo {
public:
     //! Defines the type of RSMeshBase we are referencing
    enum Type {
        TYPE_MESH,          //!< it's a RSMesh
        TYPE_HAIR,          //!< it's a RSMeshHair
        TYPE_POINT_CLOUD    //!< it's a  RSMeshPointCloud
    };

    RSMeshBase *ptr; //!< pointer to the actual redshift item
    Type type; //!< defines the type of the redshift mesh
    unsigned int refcount; //!< internal refcount used to keep track of the number of requesters
    RSResourceInfo() : ptr(nullptr), refcount(0) {}
};

typedef CoreHashTable<R2cResourceId, RSResourceInfo> RSResourceIndex;

/*! \class RSGeometryInfo
    \brief internal class holding Redshift geometry data (instance pointing to a resource) */
class RSGeometryInfo {
public:
    RSMeshInstance *ptr; //!< pointer to the actual redshift item
    RSInstanceMaterialOverrides *materials; //!< material override definition since everything is considered as an instance
    R2cResourceId resource; //!< id to the actual Clarisse geometry resource
    int dirtiness; //!< dirtiness state of the item
    RSGeometryInfo() : ptr(nullptr), materials(nullptr), resource(nullptr), dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}
};

typedef CoreHashTable<R2cItemId, RSGeometryInfo> RSGeometryIndex;

/*! \class RSInstancerInfo
    \brief internal class holding instancer data which is basically a list of Redshift point clouds instancing a geometry */
class RSInstancerInfo {
public:
    CoreArray<RSPointCloud *> ptrs; //!< list of pointers to the redshift point cloud (one per prototype)
    CoreArray<R2cResourceId> resources; //!< list of unique resources for used by all prototypes the number of resources can be smaller that the number of prototypes if there's deduplication
    int dirtiness; //!< dirtiness state of the item
    RSInstancerInfo() : dirtiness(R2cSceneDelegate::DIRTINESS_ALL) {}
};

typedef CoreHashTable<R2cItemId, RSInstancerInfo> RSInstancerIndex;

class R2cRenderBuffer;

/*! \class RenderingAbortChecker
    \brief Abort checker class which lets Clarisse interrupt the render. */
class RenderingAbortChecker : public RSAbortChecker {
public:

    RenderingAbortChecker(OfApp& app) : RSAbortChecker(), m_application(app) {}
    bool ShouldAbort() override;

private:

    OfApp& m_application;
};

/*! \class RenderingBlockSink
    \brief Redshift render buffer redirection class. */
class RenderingBlockSink : public RSBlockSink {
public:

    RenderingBlockSink();
    // since there's no way to easily remove Blocksink we do extra stuff in the destructor
    virtual ~RenderingBlockSink() override;

    inline bool IsDisplaySink() const override { return true; }

    const char*	GetClassIdentifier() const override { return "Clarisse::RenderingBlockSink"; }
    void OutputBlock(unsigned int layerID, unsigned int denoisePassID, unsigned int offsetX, unsigned int offsetY, unsigned int width, unsigned int height, unsigned int stride, const char* pDataType, const char* pBitDepth, float gamma, bool clamped, const void* data ) override;
    void NotifyWillRenderBlock(unsigned int offsetX, unsigned int offsetY, unsigned int width, unsigned int height ) override;

    // These are called right before/after actual rendering is about to happen (called from withint RS_Renderer_Render()
    void PreRender() override;
    void PostRender() override;

    unsigned int GetWidth() const override;
    unsigned int GetHeight() const override;

    inline R2cRenderBuffer *GetRenderBuffer() const { return m_render_buffer; }
    inline void SetRenderBuffer(R2cRenderBuffer *render_buffer) { m_render_buffer = render_buffer; }

private:

    R2cRenderBuffer *m_render_buffer;
};

// log redirection class
class ClarisseLogSink : public RSLogSink {
    void Log(RSLogLevel level, const char *msg, size_t nCharsMSG) override;
};

class PolyMesh;
class GeometryObject;

/*! \namespace RedshiftUtils
    \brief A set of useful function helpers used to implement the RedshiftRenderDelegate. */
namespace RedshiftUtils {

    // initialize Redshift
    /*! \brief initialize Redshift engine. */
    bool initialize(OfApp& application);
    /*! \brief return true if Redshift is initialized. */
    bool is_initialized();
    /*! \brief Generates a unique name for Redshift shaders. */
    CoreString get_new_unique_name(const CoreString& prefix = "");

    /*! \brief Clarisse to Redshift data structure and math helpers */
    RSMatrix4x4 ToRSMatrix4x4(const GMathMatrix4x4d& m);
    inline RSColor ToRSColor(const GMathVec4d& v) { return RSColor(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2]), static_cast<float>(v[3])); }
    inline RSColor ToRSColor(const GMathVec4f& v) { return RSColor(v[0], v[1], v[2], v[3]); }
    inline RSColor ToRSColor(const GMathVec3d& v) { return RSColor(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2])); }
    inline RSColor ToRSColor(const GMathVec3f& v) { return RSColor(v[0], v[1], v[2]); }
    inline RSVector4 ToRSVector4(const GMathVec4d& v) { return RSVector4(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2]), static_cast<float>(v[3])); }
    inline RSVector4 ToRSVector4(const GMathVec4f& v) { return RSVector4(v[0], v[1], v[2], v[3]); }
    inline RSVector3 ToRSVector3(const GMathVec3d& v) { return RSVector3(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2])); }
    inline RSVector3 ToRSVector3(const GMathVec3f& v) { return RSVector3(v[0], v[1], v[2]); }
    inline RSVector2 ToRSVector2(const GMathVec2d& v) { return RSVector2(static_cast<float>(v[0]), static_cast<float>(v[1])); }
    inline RSVector2 ToRSVector2(const GMathVec2f& v) { return RSVector2(v[0], v[1]); }
    inline RSNormal ToRSNormal(const GMathVec3d& v) { return RSNormal(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2])); }
    inline RSNormal ToRSNormal(const GMathVec3f& v) { return RSNormal(v[0], v[1], v[2]); }

    //
    /*! \brief Return the default material which is set to look like the default Clarisse one. */
    RSMaterial *get_default_material();

    /*! \brief Create a Redshift geometry from an input Clarisse geometry. */
    RSMeshBase *CreateGeometry(const R2cSceneDelegate& delegate, R2cItemId geometry, RSMaterial * material, RSResourceInfo::Type& type);
    /*! \brief Create a Redshift instancer from an input Clarisse instancer */
    void CreateInstancer(RSInstancerInfo& instancer, RSResourceIndex& resource_index, const R2cSceneDelegate& delegate, RSScene *scene, R2cItemId cinstancer);
    /*! \brief Create a Redshift mesh from a Clarisse Polymesh  */
    RSMesh *CreatePolymesh(const PolyMesh& polymesh, RSMaterial *material);
    /*! \brief Create a Redshift mesh from an abstract geometry defined by a GeometryPolymesh class  */
    RSMesh *CreateGeometryPolymesh(const GeometryObject& polymesh, RSMaterial *material);
    /*! \brief Create a Redshift sphere instancer representing a single Clarisse implit sphere. This method shouldn't be used  */
    RSPointCloud *CreateSphere(const float& radius, RSMaterial *material);
    /*! \brief Create a Redshift box mesh used to represent a Clarisse implicit box  */
    RSMesh *CreateBox(const GMathVec3d& size, RSMaterial *material);
    /*! \brief Create a Redshift box mesh used to represent a Clarisse bounding box (useful for debugging)  */
    RSMesh *CreateBbox(const GMathBbox3d& bbox, RSMaterial *material);
    /*! \brief Create a Redshift light from a Clarisse light. This is really a barebone implementation  */
    void CreateLight(const R2cSceneDelegate& delegate, R2cItemId lightid, RSLightInfo& light);
};

#endif
