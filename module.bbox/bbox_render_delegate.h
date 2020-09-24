//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef REDSHIFT_RENDER_DELEGATE_H
#define REDSHIFT_RENDER_DELEGATE_H

#include <r2c_render_delegate.h>

class BboxDelegateImpl;
class BboxGeometryInfo;
class BboxInstancerInfo;

/*! \class BboxRenderDelegate
 *  \brief This class implements a Bbox render delegate to Clarisse
 *         using the R2C library. While not feature complete, this example
 *         gives a good insight on how to integrate a 3rd party renderer
 *         to Clarisse. It shows all the different bindings to convert or
 *         describe Clarisse geometries, instancers, materials, lights..
 *         to another rendering engine. It is also handling geometry deduplication.
           For more information please refer to R2cRenderDelegate documentation. */
class BboxRenderDelegate : public R2cRenderDelegate {
public:

    struct CleanupFlags { //!< Defines cleanup info used to cleanup the scene after a sync
        CleanupFlags() : hairs(false), point_clouds(false), meshes(false), mesh_instances(false) {}
        bool hairs; //!< true if curve/mesh geometry have been removed from the scene
        bool point_clouds; //!< true if point clouds geometry have been removed from the scene
        bool meshes; //!< true if meshes have been removed from the scene
        bool mesh_instances; //!< true if mesh instances have been removed from the scene
        bool lights; //!< true if lights have been removed from the scene
    };

    BboxRenderDelegate();
    virtual ~BboxRenderDelegate() override;

    CoreString get_class_name() const override;

    void insert_geometry(R2cItemDescriptor item) override;
    void remove_geometry(R2cItemDescriptor item) override;
    void dirty_geometry(R2cItemDescriptor item, const int& dirtiness) override;

    void insert_light(R2cItemDescriptor item) override;
    void remove_light(R2cItemDescriptor item) override;
    void dirty_light(R2cItemDescriptor item, const int& dirtiness) override;

    void insert_instancer(R2cItemDescriptor item) override;
    void remove_instancer(R2cItemDescriptor item) override;
    void dirty_instancer(R2cItemDescriptor item, const int& dirtiness) override;

    void render(R2cRenderBuffer *render_buffer, const float& sampling_quality) override;
    float get_render_progress() const override;

    void get_supported_cameras(CoreVector<CoreString>& supported_cameras, CoreVector<CoreString>& unsupported_cameras) const override;
    void get_supported_lights(CoreVector<CoreString>& supported_lights, CoreVector<CoreString>& unsupported_lights) const override;
    void get_supported_materials(CoreVector<CoreString>& supported_materials, CoreVector<CoreString>& unsupported_materials) const override;
    void get_supported_geometries(CoreVector<CoreString>& supported_geometries, CoreVector<CoreString>& unsupported_geometries) const override;

	ModuleMaterial * get_default_material() const override;
	ModuleMaterial * get_error_material() const override;

    void clear() override;

	static const CoreVector<CoreString> s_supported_cameras;
	static const CoreVector<CoreString> s_unsupported_cameras;
	static const CoreVector<CoreString> s_supported_lights;
	static const CoreVector<CoreString> s_unsupported_lights;
	static const CoreVector<CoreString> s_supported_materials;
	static const CoreVector<CoreString> s_unsupported_materials;
	static const CoreVector<CoreString> s_supported_geometries;
	static const CoreVector<CoreString> s_unsupported_geometries;

private:

    /*! \brief Synchronize the internal render scene with the scene delegate */
    void sync();
    /*! \brief Synchronize the render settings */
    bool sync_render_settings(const float& sampling_quality);

    /*! \brief Add a new geometry (never been processed yet) to the render scene and synchronize it
     *  \param cgeometryid id of the new geometry in the scene delegate
     *  \param rgeometry bbox geometry handle */
    void sync_new_geometry(R2cItemId cgeometryid, BboxGeometryInfo& rgeometry) { _sync_geometry(cgeometryid, rgeometry, true); }
    /*! \brief Synchronize an existing render geometry with the one defined in the scene delegate
     *  \param cgeometryid id of the geometry in the scene delegate
     *  \param rgeometry bbox geometry handle */
    void sync_geometry(R2cItemId cgeometryid, BboxGeometryInfo& rgeometry) { _sync_geometry(cgeometryid, rgeometry, false); }
    /*! \brief Actual implementation of geometry synchronization whereas it is a new or an existing one
     *  \param cgeometryid id of the geometry in the scene delegate
     *  \param rgeometry bbox geometry definition handle
     *  \param is_new set whether the input geometry is new or existing (there are two different codepaths) */
    void _sync_geometry(R2cItemId cgeometryid, BboxGeometryInfo& rgeometry, const bool& is_new);
    /*! \brief Synchronize all needed geometries with the render scene
     *  \param cleanup output cleanup flags to do post cleanup with the render scene */
    void sync_geometries(CleanupFlags& cleanup);

    /*! \brief Add a new instancer (never been processed yet) to the render scene and synchronize it
     *  \param cinstancerid id of the new instancer in the scene delegate
     *  \param rinstancer bbox instancer definition handle */
    inline void sync_new_instancer(R2cItemId cinstancerid, BboxInstancerInfo& rinstancer) { _sync_instancer(cinstancerid, rinstancer, true); }
    /*! \brief Synchronize an existing instancer with the one defined in the scene delegate
     *  \param cinstancerid id of the instancer in the scene delegate
     *  \param rinstancer bbox instancer definition handle */
    inline void sync_instancer(R2cItemId cinstancerid, BboxInstancerInfo& rinstancer) { _sync_instancer(cinstancerid, rinstancer, false); }
    /*! \brief Actual implementation of instancer synchronization whereas it is a new or an existing one
     *  \param cinstancerid id of the instancer in the scene delegate
     *  \param rinstancer bbox geometry definition handle
     *  \param is_new set whether the input instancer is new or existing (there are two different codepaths) */
    void _sync_instancer(R2cItemId cinstancerid, BboxInstancerInfo& rinstancer, const bool& is_new);
    /*! \brief Synchronize all needed instancers with the render scene
     *  \param cleanup output cleanup flags to do post cleanup with the render scene */
    void sync_instancers(CleanupFlags& cleanup);
    /*! \brief Synchronize the render scene lights with the scene delegate
     *  \param cleanup output cleanup flags to do post cleanup with the render scene */
    void sync_lights(CleanupFlags& cleanup);
    /*! \brief Synchronize the render camera with the scene delegate
     *  \param width width of the rendered image
     *  \param height hight of the rendered image */
    void sync_camera(const unsigned int& width, const unsigned int& height);
    /*! \brief Cleanup the render scene according to the specified flags
     *  \note This post cleanup is there to rebuild the render scene since bbox can only
     *        add new items not remove them. We are then obliged to remove the corresponding
              item collections (mesh, lights...) if an item has been removed from the scene. */
    void cleanup_scene(const CleanupFlags& cleanup);

    BboxDelegateImpl *m; // private implementation
    DECLARE_CLASS
};

#endif
