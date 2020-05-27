//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_object.h>
#include <of_app.h>
#include <dso_export.h>

#include "module_texture_redshift.h"
#include "texture_redshift.cma"

IX_BEGIN_DECLARE_MODULE_CALLBACKS(ModuleTextureRedshift, ModuleGlObjectCallbacks)
    static ModuleObject *declare_module(OfObject& object, OfObjectFactory& objects);
IX_END_DECLARE_MODULE_CALLBACKS(ModuleTextureRedshift)

ModuleObject *
IX_MODULE_CLBK::declare_module(OfObject& object, OfObjectFactory& objects)
{
    ModuleTextureRedshift *material = new ModuleTextureRedshift;
    material->set_object(object);
    return material;
}

namespace TextureRedshift
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleTextureRedshift);
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)
        module_callbacks->cb_create_module = IX_MODULE_CLBK::declare_module;
    }
}
