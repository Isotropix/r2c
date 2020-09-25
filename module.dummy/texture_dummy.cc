//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_object.h>
#include <of_app.h>
#include <dso_export.h>

#include "module_texture_dummy.h"
#include "texture_dummy.cma"

#if ISOTROPIX_VERSION_NUMBER >= IX_BUILD_VERSION_NUMBER(4, 0, 2, 0, 0, 0)
#define MODULE_CLASS OfModule
#else
#define MODULE_CLASS ModuleObject
#endif

IX_BEGIN_DECLARE_MODULE_CALLBACKS(ModuleTextureDummy, ModuleTextureDummyCallbacks)

    struct TextureDummyModuleData {
        const OfAttr *color;
    };

    static MODULE_CLASS *declare_module(OfObject& object, OfObjectFactory& objects);
    static GMathVec3f evaluate(OfObject&, const GMathVec3f& ray_direction);
    static void * create_module_data(const OfObject& object);
    static bool destroy_module_data(const OfObject&object, void *);
IX_END_DECLARE_MODULE_CALLBACKS(ModuleTextureDummy)

MODULE_CLASS *
IX_MODULE_CLBK::declare_module(OfObject& object, OfObjectFactory& objects)
{
    ModuleTextureDummy *material = new ModuleTextureDummy;
    material->set_object(object);
    return material;
}

namespace TextureDummy
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleTextureDummy);
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;

        module_callbacks = new IX_MODULE_CLBK();
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)

        module_callbacks->cb_create_module = IX_MODULE_CLBK::declare_module;
        module_callbacks->cb_create_module_data = IX_MODULE_CLBK::create_module_data;
        module_callbacks->cb_destroy_module_data = IX_MODULE_CLBK::destroy_module_data;
        module_callbacks->cb_evaluate = IX_MODULE_CLBK::evaluate;
    }
}

void * IX_MODULE_CLBK::create_module_data(const OfObject& object)
{
    TextureDummyModuleData *data = new TextureDummyModuleData();;
    data->color = object.get_attribute("color");
    return data;
}

bool IX_MODULE_CLBK::destroy_module_data(const OfObject& object, void *data)
{
    delete (TextureDummyModuleData *)data;
    return true;
}


GMathVec3f IX_MODULE_CLBK::evaluate(OfObject& object, const GMathVec3f& ray_direction)
{
    TextureDummyModuleData *data = static_cast<TextureDummyModuleData *>(object.get_module_data());
    return GMathVec3f(data->color->get_vec3d()) * ray_direction;
}
