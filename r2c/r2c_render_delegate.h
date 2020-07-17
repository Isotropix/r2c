//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef R2C_RENDER_DELEGATE_H
#define R2C_RENDER_DELEGATE_H

#include <core_basic_array.h>
#include <core_base_object.h>
#include <r2c_common.h>

class ImageCanvas;
class OfObject;
class ModuleLayer;
class ModuleMaterial;

class R2cGeometryResource;
class R2cItemDescriptor;
class R2cSceneDelegate;
class R2cRenderBuffer;

/*! \class R2cRenderDelegate
    \brief This class defines an abstract Clarisse Render Delegate that is fully managed by R2cSceneDelegate.
    \note  You must call R2cSceneDelegate::initialize before creating any instances of R2cRenderDelegate */
class R2C_EXPORT R2cRenderDelegate : public CoreBaseObject {
public:

    R2cRenderDelegate();
    virtual ~R2cRenderDelegate() {}

    /*! \brief Return the name of the clarisse class of the renderer */
    virtual CoreString get_class_name() const = 0;

    /*! \brief Called by the Scene Delegate with a geometry is inserted to the scene
     *  \param item item descriptor of the inserted geometry */
    virtual void insert_geometry(R2cItemDescriptor item) = 0;
    /*! \brief Called by the Scene Delegate with a geometry is removed from the scene
     *  \param item item descriptor of the removed geometry
     *  \note It is also possible that the input item has been removed because it has been destroyed.
     *  In that case R2cItemDescriptor::get_item will return nullptr. Additionally you can check
     *  if the item is destroyed using R2cItemDescriptor::is_destroyed */
    virtual void remove_geometry(R2cItemDescriptor item) = 0;
    /*! \brief Called by the Scene Delegate to propagate the dirtiness of the item
     *  \param item item descriptor of the geometry being modified
     *  \param dirtiness the current dirtiness of the item. Note that it can be a composite of multiple flags
     *  \note  For more information refer R2cSceneDelegate::Dirtiness */
    virtual void dirty_geometry(R2cItemDescriptor item, const int& dirtiness) = 0;

    /*! \brief Called by the Scene Delegate with a geometry is inserted to the scene
     *  \param item item descriptor of the inserted light */
    virtual void insert_light(R2cItemDescriptor item) = 0;
    /*! \brief Called by the Scene Delegate with a light is removed from the scene
     *  \param item item descriptor of the removed light
     *  \note It is also possible that the input item has been removed because it has been destroyed.
     *  In that case R2cItemDescriptor::get_item will return nullptr. Additionally you can check
     *  if the item is destroyed using R2cItemDescriptor::is_destroyed */
    virtual void remove_light(R2cItemDescriptor item) = 0;
    /*! \brief Called by the Scene Delegate to propagate the dirtiness of the item
     *  \param item item descriptor of the geometry being modified
     *  \param dirtiness the current dirtiness of the item. Note that it can be a composite of multiple flags
     *  \note  For more information refer R2cSceneDelegate::Dirtiness */
    virtual void dirty_light(R2cItemDescriptor item, const int& dirtiness) = 0;

    /*! \brief Called by the Scene Delegate with a geometry is inserted to the scene
     *  \param item item descriptor of the inserted light */
    virtual void insert_instancer(R2cItemDescriptor item) = 0;
    /*! \brief Called by the Scene Delegate with a light is removed from the scene
     *  \param item item descriptor of the removed light
     *  \note It is also possible that the input item has been removed because it has been destroyed.
     *  In that case R2cItemDescriptor::get_item will return nullptr. Additionally you can check
     *  if the item is destroyed using R2cItemDescriptor::is_destroyed */
    virtual void remove_instancer(R2cItemDescriptor item) = 0;
    /*! \brief Called by the Scene Delegate to propagate the dirtiness of the item
     *  \param item item descriptor of the geometry being modified
     *  \param dirtiness the current dirtiness of the item. Note that it can be a composite of multiple flags
     *  \note  For more information refer R2cSceneDelegate::Dirtiness */
    virtual void dirty_instancer(R2cItemDescriptor item, const int& dirtiness) = 0;

    /*! \brief Called when a requested a render
     *  \param render_buffer Clarisse render buffer
     *  \param sampling_quality a percentage that defines a global multiplier to all sampling values (lights, material, AA etc...)
     *  \note  The scene descriptor is synched prior the render call. */
    virtual void render(R2cRenderBuffer *render_buffer, const float& sampling_quality) = 0;
    /*! \brief Return the current rendering progress, between 0 and 1. */
    virtual float get_render_progress() const = 0;

    /*! \brief Return the names of the clarisse camera classes supported by the render delegate
	 *  \param supported_cameras The list of camera classes supported by the renderer
	 *  \param unsupported_cameras The list of camera classes not supported by the renderer
     *  \note  Items for unsupported classes are simply skipped and not passed to the render delegate.
     *  Instead of specifying many classes, you can specify a base class as supported and optionally some of its children as unsupported. */
	virtual void get_supported_cameras(CoreVector<CoreString>& supported_cameras, CoreVector<CoreString>& unsupported_cameras) const = 0;
    /*! \brief Return the names of the clarisse light classes supported by the render delegate
	 *  \param supported_lights The list of light classes supported by the renderer
	 *  \param unsupported_lights The list of light classes not supported by the renderer
     *  \note  The scene descriptor is synched prior the render call.
     *  Instead of specifying many classes, you can specify a base class as supported and optionally some of its children as unsupported. */
	virtual void get_supported_lights(CoreVector<CoreString>& supported_lights, CoreVector<CoreString>& unsupported_lights) const = 0;
    /*! \brief Return the names of the clarisse material classes supported by the render delegate
	 *  \param supported_materials The list of material classes supported by this renderer
	 *  \param unsupported_materials The list of material classes not supported by this renderer
     *  \note  The scene descriptor is synched prior the render call.
     *  Instead of specifying many classes, you can specify a base class as supported and optionally some of its children as unsupported. */
	virtual void get_supported_materials(CoreVector<CoreString>& supported_materials, CoreVector<CoreString>& unsupported_materials) const = 0;
    /*! \brief Return the names of the clarisse geometry classes supported by the render delegate
	 *  \param supported_geometries The list of geometry classes supported by this renderer
	 *  \param unsupported_geometries The list of geometry classes not supported by this renderer
     *  \note  The scene descriptor is synched prior the render call.
     *  Instead of specifying many classes, you can specify a base class as supported and optionally some of its children as unsupported. */
	virtual void get_supported_geometries(CoreVector<CoreString>& supported_geometries, CoreVector<CoreString>& unsupported_geometries) const = 0;

	/*! \brief Return the material that will be evaluated when the default material is connected to the current shading group evaluated */
	virtual ModuleMaterial *get_default_material() const { return nullptr; }

	/*! \brief Return the material that will be evaluated when a non supported material is connected to the current shading group evaluated */
	virtual ModuleMaterial *get_error_material() const { return nullptr; }

    /*! \brief Return the scene delegate associated to the render delegate */
    const R2cSceneDelegate *get_scene_delegate() const { return m_scene_delegate; }

    /*! \brief Must be overloaded to deallocated all data from the render delegate */
    virtual void clear() = 0;

private:

    R2cRenderDelegate(const R2cRenderDelegate&) = delete;
    R2cRenderDelegate& operator=(const R2cRenderDelegate&) = delete;
    // internal management
    friend class R2cSceneDelegate;
    inline void set_scene_delegate(const R2cSceneDelegate *delegate) { m_scene_delegate = delegate; }

    const R2cSceneDelegate *m_scene_delegate;

    DECLARE_CLASS
};

#endif
