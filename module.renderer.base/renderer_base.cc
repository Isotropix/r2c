//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <dso_export.h>
#include <module_project_item.h>
#include <of_app.h>
#include <of_object.h>

#include "renderer_base.cma"


IX_BEGIN_DECLARE_MODULE_CALLBACKS(ModuleRendererBase, ModuleObjectCallbacks)
    static ModuleObject *declare_module(OfObject& object, OfObjectFactory& objects);
IX_END_DECLARE_MODULE_CALLBACKS(ModuleRendererBase)

ModuleObject *
IX_MODULE_CLBK::declare_module(OfObject& object, OfObjectFactory& objects)
{
    ModuleProjectItem *renderer = new ModuleProjectItem;
    renderer->set_object(object);
    return renderer;
}

namespace RendererBase
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleRendererBase);
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)
        module_callbacks->cb_create_module = IX_MODULE_CLBK::declare_module;
    }
}
