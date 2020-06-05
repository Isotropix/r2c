//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef R2C_LAYER_SCENE_H
#define R2C_LAYER_SCENE_H

#include <r2c_export.h>
#include <module_layer_scene.h>

class R2cRenderDelegate;
class R2cSceneDelegate;

//! \class ModuleLayerR2cSceneCallbacks
//! \brief Definition of the callbacks used by the ModuleLayerR2cScene class
//! \note All the callbacks must be implemented to integrate correctly an external renderer in Clarisse
class R2C_EXPORT ModuleLayerR2cSceneCallbacks : public ModuleLayerCallbacks {
public:
    ModuleLayerR2cSceneCallbacks();

    virtual void init_callbacks(OfClassCallbacks& callbacks) {
        ModuleLayerCallbacks::init_callbacks(callbacks);
        ModuleLayerR2cSceneCallbacks& cb = (ModuleLayerR2cSceneCallbacks&)callbacks;
        cb.cb_get_render_delegate = cb_get_render_delegate;
    }
    
    //! Callback allowing to specify which Render Delegate to attach to the Scene Delegate
    typedef R2cRenderDelegate * (*GetRenderDelegateCallback) (OfObject& object);

    GetRenderDelegateCallback cb_get_render_delegate;
};

//! \class ModuleLayerR2cScene
//! \brief Base class for all layers with an attached scene to render with an external renderer.
class R2C_EXPORT ModuleLayerR2cScene : public ModuleLayerScene {
public:

    ModuleLayerR2cScene();
    virtual ~ModuleLayerR2cScene();

    void module_constructor(OfObject& object) override;
    void on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags) override;

    /*! \brief Returns the Scene Delegate attached to this layer. */
    R2cSceneDelegate *get_scene_delegate() { return m_scene_delegate; }

private:

    //! The Scene Delegate attached to this layer
    R2cSceneDelegate *m_scene_delegate;

    //! The Render Delegate attached to this layer
    R2cRenderDelegate *m_render_delegate;

    DECLARE_CLASS
};

#endif