//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <dso_export.h>
#include <r2c_module_layer_scene.h>
#include <redshift_render_delegate.h>

#include "layer_redshift.cma"

IX_BEGIN_DECLARE_MODULE_CALLBACKS(ModuleLayerRedshift, ModuleLayerR2cSceneCallbacks)
    static R2cRenderDelegate *get_render_delegate(OfObject& object);
IX_END_DECLARE_MODULE_CALLBACKS(ModuleLayerRedshift)

//!
//! This callback must be implemented to specify which Render Delegate to attach to the Scene Delegate
//!
R2cRenderDelegate *
IX_MODULE_CLBK::get_render_delegate(OfObject& object)
{
    return new RedshiftRenderDelegate;
}

namespace LayerRedshift
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleLayerRedshift)
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)
        module_callbacks->cb_get_render_delegate = IX_MODULE_CLBK::get_render_delegate;
    }
}
