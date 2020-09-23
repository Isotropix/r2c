//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_object.h>
#include <of_app.h>
#include <dso_export.h>

#include <module_scene_item.h>

#include "module_light_bbox.h"
#include "light_bbox.cma"

#if ISOTROPIX_VERSION_NUMBER >= IX_BUILD_VERSION_NUMBER(4, 0, 2, 0, 0, 0)
#define MODULE_CLASS OfModule
#else
#define MODULE_CLASS ModuleObject
#endif

IX_BEGIN_DECLARE_MODULE_CALLBACKS(ModuleLightBbox, ModuleLightCallbacks)
    static MODULE_CLASS *declare_module(OfObject& object, OfObjectFactory& objects);
IX_END_DECLARE_MODULE_CALLBACKS(ModuleLightBbox)

MODULE_CLASS *
IX_MODULE_CLBK::declare_module(OfObject& object, OfObjectFactory& objects)
{
    ModuleLightBbox *light = new ModuleLightBbox;
    light->set_object(object);
    return light;
}

namespace LightBbox
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleLightBbox);
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)
        module_callbacks->cb_create_module = IX_MODULE_CLBK::declare_module;
    }
}
