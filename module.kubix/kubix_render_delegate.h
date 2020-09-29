//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

// R2C includes
#include <r2c_render_delegate.h>
#include <r2c_render_buffer.h>

// Local includes
#include "./kubix_utils.h"

class KubixRenderDelegateImpl;
class KubixGeometryInfo;
class KubixInstancerInfo;

/*! \class KubixRenderDelegate
 *  \brief This class implements a Dummy render delegate to Clarisse
 *         using the R2C library. While not feature complete, this example
 *         gives a good insight on how to create a 3rd party renderer
 *         in Clarisse. It shows all the different bindings to convert or
 *         describe Clarisse geometries, instancers, materials, lights..
 *         to another rendering engine. It is also handling geometry deduplication.
           For more information please refer to R2cRenderDelegate documentation. */
class KubixRenderDelegate : public R2cRenderDelegate {
public:

    KubixRenderDelegate(OfApp *app);
    virtual ~KubixRenderDelegate() override;

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

    struct RenderData {
        RenderData(): region(0,0,0,0) {}
        // Sub-image related data
        unsigned int width;
        unsigned int height;
        R2cRenderBuffer::Region region;

        // Shading data
        GMathVec3f light_contribution;
        GMathVec3f background_color;

        // Buffers
        R2cRenderBuffer *render_buffer; // <-- used to interface with Clarisse image view
        float* buffer_ptr;

    };

    /*! Used to trace rays through the scene and render a region of the final image (see \ref RenderData). This is thread safe. */
    void render_region(RenderData& render_data, const unsigned int& thread_id) const;

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
     *  \param rgeometry dummy geometry handle */
    void sync_new_geometry(R2cItemId cgeometryid, KubixGeometryInfo& rgeometry) { _sync_geometry(cgeometryid, rgeometry, true); }
    /*! \brief Synchronize an existing render geometry with the one defined in the scene delegate
     *  \param cgeometryid id of the geometry in the scene delegate
     *  \param rgeometry dummy geometry handle */
    void sync_geometry(R2cItemId cgeometryid, KubixGeometryInfo& rgeometry) { _sync_geometry(cgeometryid, rgeometry, false); }
    /*! \brief Actual implementation of geometry synchronization whereas it is a new or an existing one
     *  \param cgeometryid id of the geometry in the scene delegate
     *  \param rgeometry dummy geometry definition handle
     *  \param is_new set whether the input geometry is new or existing (there are two different codepaths) */
    void _sync_geometry(R2cItemId cgeometryid, KubixGeometryInfo& rgeometry, const bool& is_new);
    /*! \brief Synchronize all needed geometries with the render scene
     *  \param cleanup output cleanup flags to do post cleanup with the render scene */
    void sync_geometries();

    /*! \brief Add a new instancer (never been processed yet) to the render scene and synchronize it
     *  \param cinstancerid id of the new instancer in the scene delegate
     *  \param rinstancer dummy instancer definition handle */
    inline void sync_new_instancer(R2cItemId cinstancerid, KubixInstancerInfo& rinstancer) { _sync_instancer(cinstancerid, rinstancer, true); }
    /*! \brief Synchronize an existing instancer with the one defined in the scene delegate
     *  \param cinstancerid id of the instancer in the scene delegate
     *  \param rinstancer dummy instancer definition handle */
    inline void sync_instancer(R2cItemId cinstancerid, KubixInstancerInfo& rinstancer) { _sync_instancer(cinstancerid, rinstancer, false); }
    /*! \brief Actual implementation of instancer synchronization whereas it is a new or an existing one
     *  \param cinstancerid id of the instancer in the scene delegate
     *  \param rinstancer dummy geometry definition handle
     *  \param is_new set whether the input instancer is new or existing (there are two different codepaths) */
    void _sync_instancer(R2cItemId cinstancerid, KubixInstancerInfo& rinstancer, const bool& is_new);
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

    KubixRenderDelegateImpl *m; // private implementation
    DECLARE_CLASS
};
