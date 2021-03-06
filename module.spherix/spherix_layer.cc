//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// R2C includes
#include <r2c_module_layer_scene.h>

// Local includes
#include "./spherix_render_delegate.h"
#include "./spherix_layer.cma"

IX_BEGIN_DECLARE_MODULE_CALLBACKS(SpherixModuleLayer, ModuleLayerR2cSceneCallbacks)
    static R2cRenderDelegate *get_render_delegate(OfObject& object);
IX_END_DECLARE_MODULE_CALLBACKS(SpherixModuleLayer)

//!
//! This callback must be implemented to specify which Render Delegate to attach to the Scene Delegate
//!
R2cRenderDelegate *
IX_MODULE_CLBK::get_render_delegate(OfObject& object)
{
    return new SpherixRenderDelegate(&object.get_application());
}

namespace SpherixLayer
{
    // This method is called when opening Clarisse and it registers the module
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(SpherixModuleLayer)
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)
        module_callbacks->cb_get_render_delegate = IX_MODULE_CLBK::get_render_delegate;
    }
}
