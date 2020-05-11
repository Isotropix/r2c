//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_object.h>
#include <of_app.h>
#include <dso_export.h>

#include <module_renderer_redshift.h>

#include "renderer_redshift.cma"

IX_BEGIN_DECLARE_MODULE_CALLBACKS(ModuleRendererRedshift, ModuleObjectCallbacks)
    static ModuleObject *declare_module(OfObject& object, OfObjectFactory& objects);
IX_END_DECLARE_MODULE_CALLBACKS(ModuleRendererRedshift)

ModuleObject *
IX_MODULE_CLBK::declare_module(OfObject& object, OfObjectFactory& objects)
{
    ModuleRendererRedshift *settings = new ModuleRendererRedshift;
    settings->set_object(object);
    return settings;
}

namespace RendererRedshift
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleRendererRedshift)
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)
        module_callbacks->cb_create_module = IX_MODULE_CLBK::declare_module;
    }
}
