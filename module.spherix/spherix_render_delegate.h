//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

// R2C includes
#include <r2c_render_delegate.h>
#include <r2c_render_buffer.h>

// Local includes
#include "./spherix_utils.h"

class SpherixRenderDelegateImpl;
class SpherixGeometryInfo;
class SpherixInstancerInfo;

/*! \class SpherixRenderDelegate
 *  \brief This class implements a Spehrix render delegate to Clarisse
 *         using the R2C library. While not feature complete, this example
 *         gives a good insight on how to create a 3rd party renderer
 *         in Clarisse. It shows all the different bindings to convert or
 *         describe Clarisse geometries, instancers, materials, lights..
 *         to another rendering engine. It is also handling geometry deduplication.
           For more information please refer to R2cRenderDelegate documentation. */
class SpherixRenderDelegate : public R2cRenderDelegate {
public:

    SpherixRenderDelegate(OfApp *app);
    virtual ~SpherixRenderDelegate() override;

    CoreString get_class_name() const override;

    /*************************************************** BEGIN R2C methods ****************************************************/

    /*! For documentation of these methodes, see \ref r2c_render_delegate.h */
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

    void clear() override;

    /*************************************************** END R2C methods ****************************************************/

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

    /*! \brief Add a new geometry (never been processed yet) to the render scene and synchronize it
     *  \param cgeometryid id of the new geometry in the scene delegate
     *  \param rgeometry spherix geometry handle */
    void sync_new_geometry(R2cItemId cgeometryid, SpherixGeometryInfo& rgeometry) { _sync_geometry(cgeometryid, rgeometry, true); }
    /*! \brief Synchronize an existing render geometry with the one defined in the scene delegate
     *  \param cgeometryid id of the geometry in the scene delegate
     *  \param rgeometry spherix geometry handle */
    void sync_geometry(R2cItemId cgeometryid, SpherixGeometryInfo& rgeometry) { _sync_geometry(cgeometryid, rgeometry, false); }
    /*! \brief Actual implementation of geometry synchronization whereas it is a new or an existing one
     *  \param cgeometryid id of the geometry in the scene delegate
     *  \param rgeometry spherix geometry definition handle
     *  \param is_new set whether the input geometry is new or existing (there are two different codepaths) */
    void _sync_geometry(R2cItemId cgeometryid, SpherixGeometryInfo& rgeometry, const bool& is_new);
    /*! \brief Synchronize all needed geometries with the render scene
     *  \param cleanup output cleanup flags to do post cleanup with the render scene */
    void sync_geometries();

    /*! \brief Add a new instancer (never been processed yet) to the render scene and synchronize it
     *  \param cinstancerid id of the new instancer in the scene delegate
     *  \param rinstancer spherix instancer definition handle */
    inline void sync_new_instancer(R2cItemId cinstancerid, SpherixInstancerInfo& rinstancer) { _sync_instancer(cinstancerid, rinstancer, true); }
    /*! \brief Synchronize an existing instancer with the one defined in the scene delegate
     *  \param cinstancerid id of the instancer in the scene delegate
     *  \param rinstancer spherix instancer definition handle */
    inline void sync_instancer(R2cItemId cinstancerid, SpherixInstancerInfo& rinstancer) { _sync_instancer(cinstancerid, rinstancer, false); }
    /*! \brief Actual implementation of instancer synchronization whereas it is a new or an existing one
     *  \param cinstancerid id of the instancer in the scene delegate
     *  \param rinstancer spherix geometry definition handle
     *  \param is_new set whether the input instancer is new or existing (there are two different codepaths) */
    void _sync_instancer(R2cItemId cinstancerid, SpherixInstancerInfo& rinstancer, const bool& is_new);
    /*! \brief Synchronize all needed instancers with the render scene
     *  \param cleanup output cleanup flags to do post cleanup with the render scene */
    void sync_instancers();
    /*! \brief Synchronize the render scene lights with the scene delegate
     *  \param cleanup output cleanup flags to do post cleanup with the render scene */
    void sync_lights();
    /*! \brief Synchronize the render camera with the scene delegate
     *  \param width width of the rendered image
     *  \param height hight of the rendered image */
    void sync_camera(const unsigned int& width, const unsigned int& height);

    SpherixRenderDelegateImpl *m; // private implementation
    DECLARE_CLASS
};
