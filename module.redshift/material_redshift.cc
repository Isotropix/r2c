//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_object.h>
#include <of_app.h>
#include <dso_export.h>

#include <module_scene_item.h>

#include "module_material_redshift.h"
#include "material_redshift.cma"

IX_BEGIN_DECLARE_MODULE_CALLBACKS(ModuleMaterialRedshift, ModuleMaterialCallbacks)
    static ModuleObject *declare_module(OfObject& object, OfObjectFactory& objects);
IX_END_DECLARE_MODULE_CALLBACKS(ModuleMaterialRedshift)

ModuleObject *
IX_MODULE_CLBK::declare_module(OfObject& object, OfObjectFactory& objects)
{
    ModuleMaterialRedshift *material = new ModuleMaterialRedshift;
    material->set_object(object);
    return material;
}

namespace MaterialRedshift
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleMaterialRedshift);
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)
        module_callbacks->cb_create_module = IX_MODULE_CLBK::declare_module;
    }
}
