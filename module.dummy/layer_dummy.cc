//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <dso_export.h>
#include <r2c_module_layer_scene.h>
#include <dummy_render_delegate.h>

#include "layer_dummy.cma"

IX_BEGIN_DECLARE_MODULE_CALLBACKS(ModuleLayerDummy, ModuleLayerR2cSceneCallbacks)
    static R2cRenderDelegate *get_render_delegate(OfObject& object);
IX_END_DECLARE_MODULE_CALLBACKS(ModuleLayerDummy)

//!
//! This callback must be implemented to specify which Render Delegate to attach to the Scene Delegate
//!
R2cRenderDelegate *
IX_MODULE_CLBK::get_render_delegate(OfObject& object)
{
    return new BboxRenderDelegate;
}

namespace LayerDummy
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleLayerDummy)
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)
        module_callbacks->cb_get_render_delegate = IX_MODULE_CLBK::get_render_delegate;
    }
}
