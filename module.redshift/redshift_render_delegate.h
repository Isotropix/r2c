//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef REDSHIFT_RENDER_DELEGATE_H
#define REDSHIFT_RENDER_DELEGATE_H

#include <r2c_render_delegate.h>

class RSDelegateImpl;
class RSGeometryInfo;
class RSInstancerInfo;

/*! \class RedshiftRenderDelegate
 *  \brief This class implements a Redshift render delegate to Clarisse
 *         using the R2C library. While not feature complete, this example
 *         gives a good insight on how to integrate a 3rd party renderer
 *         to Clarisse. It shows all the different bindings to convert or
 *         describe Clarisse geometries, instancers, materials, lights..
 *         to another rendering engine. It is also handling geometry deduplication.
           For more information please refer to R2cRenderDelegate documentation. */
class RedshiftRenderDelegate : public R2cRenderDelegate {
public:

    struct CleanupFlags { //!< Defines cleanup info used to cleanup the scene after a sync
        CleanupFlags() : hairs(false), point_clouds(false), meshes(false), mesh_instances(false) {}
        bool hairs; //!< true if curve/mesh geometry have been removed from the scene
        bool point_clouds; //!< true if point clouds geometry have been removed from the scene
        bool meshes; //!< true if meshes have been removed from the scene
        bool mesh_instances; //!< true if mesh instances have been removed from the scene
        bool lights; //!< true if lights have been removed from the scene
    };

    RedshiftRenderDelegate();
    virtual ~RedshiftRenderDelegate() override;

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

    CoreVector<CoreString> get_supported_cameras() const override;
    CoreVector<CoreString> get_supported_lights() const override;
    CoreVector<CoreString> get_supported_materials() const override;
    CoreVector<CoreString> get_supported_geometries() const override;

    void clear() override;

private:

    /*! \brief Synchronize the internal render scene with the scene delegate */
    void sync();
    /*! \brief Synchronize the render settings */
    bool sync_render_settings(const float& sampling_quality);

    /*! \brief Add a new geometry (never been processed yet) to the render scene and synchronize it
     *  \param cgeometryid id of the new geometry in the scene delegate
     *  \param rgeometry redshift geometry handle */
    void sync_new_geometry(R2cItemId cgeometryid, RSGeometryInfo& rgeometry) { _sync_geometry(cgeometryid, rgeometry, true); }
    /*! \brief Synchronize an existing render geometry with the one defined in the scene delegate
     *  \param cgeometryid id of the geometry in the scene delegate
     *  \param rgeometry redshift geometry handle */
    void sync_geometry(R2cItemId cgeometryid, RSGeometryInfo& rgeometry) { _sync_geometry(cgeometryid, rgeometry, false); }
    /*! \brief Actual implementation of geometry synchronization whereas it is a new or an existing one
     *  \param cgeometryid id of the geometry in the scene delegate
     *  \param rgeometry redshift geometry definition handle
     *  \param is_new set whether the input geometry is new or existing (there are two different codepaths) */
    void _sync_geometry(R2cItemId cgeometryid, RSGeometryInfo& rgeometry, const bool& is_new);
    /*! \brief Synchronize all needed geometries with the render scene
     *  \param cleanup output cleanup flags to do post cleanup with the render scene */
    void sync_geometries(CleanupFlags& cleanup);

    /*! \brief Add a new instancer (never been processed yet) to the render scene and synchronize it
     *  \param cinstancerid id of the new instancer in the scene delegate
     *  \param rinstancer redshift instancer definition handle */
    inline void sync_new_instancer(R2cItemId cinstancerid, RSInstancerInfo& rinstancer) { _sync_instancer(cinstancerid, rinstancer, true); }
    /*! \brief Synchronize an existing instancer with the one defined in the scene delegate
     *  \param cinstancerid id of the instancer in the scene delegate
     *  \param rinstancer redshift instancer definition handle */
    inline void sync_instancer(R2cItemId cinstancerid, RSInstancerInfo& rinstancer) { _sync_instancer(cinstancerid, rinstancer, false); }
    /*! \brief Actual implementation of instancer synchronization whereas it is a new or an existing one
     *  \param cinstancerid id of the instancer in the scene delegate
     *  \param rinstancer redshift geometry definition handle
     *  \param is_new set whether the input instancer is new or existing (there are two different codepaths) */
    void _sync_instancer(R2cItemId cinstancerid, RSInstancerInfo& rinstancer, const bool& is_new);
    /*! \brief Synchronize all needed instancers with the render scene
     *  \param cleanup output cleanup flags to do post cleanup with the render scene */
    void sync_instancers(CleanupFlags& cleanup);
    /*! \brief Synchronize the render scene lights with the scene delegate
     *  \param cleanup output cleanup flags to do post cleanup with the render scene */
    void sync_lights(CleanupFlags& cleanup);
    /*! \brief Synchronize the render camera with the scene delegate
     *  \param w width of the rendered image
     *  \param h hight of the rendered image
     *  \param cox x offset of the render region
     *  \param coy y offset of the render region
     *  \param cw width of the render region
     *  \param ch height of the render region */
    void sync_camera(const unsigned int& w, const unsigned int& h,
                     const unsigned int& cox, const unsigned int& coy,
                     const unsigned int& cw, const unsigned int& ch);
    /*! \brief Cleanup the render scene according to the specified flags
     *  \note This post cleanup is there to rebuild the render scene since redshift can only
     *        add new items not remove them. We are then obliged to remove the corresponding
              item collections (mesh, lights...) if an item has been removed from the scene. */
    void cleanup_scene(const CleanupFlags& cleanup);

    RSDelegateImpl *m; // private implementation
    DECLARE_CLASS
};

#endif
