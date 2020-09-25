//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_object.h>
#include <of_app.h>
#include <dso_export.h>

#include <module_scene_item.h>

#include <module_texture_dummy.h>
#include "module_material_dummy.h"
#include "material_dummy.cma"

#if ISOTROPIX_VERSION_NUMBER >= IX_BUILD_VERSION_NUMBER(4, 0, 2, 0, 0, 0)
#define MODULE_CLASS OfModule
#else
#define MODULE_CLASS ModuleObject
#endif

class ModuleMaterialDummyCallbackOverrides : public ModuleMaterialDummyCallbacks {
};

// WARNING: do not remove this typedef, it is needed by the macro IX_CREATE_MODULE_CLBK
typedef ModuleMaterialDummyCallbackOverrides IX_MODULE_CLBK;

MODULE_CLASS *
declare_module(OfObject& object, OfObjectFactory& objects)
{
    ModuleMaterialDummy *material = new ModuleMaterialDummy;
    material->set_object(object);
    return material;
}

GMathVec3f
shade(OfObject& object, const GMathVec3f& ray_direction, const GMathVec3f& normal)
{
    OfAttr *attr_color = object.get_attribute("color");
    if (attr_color->is_textured()) {
        ModuleTextureDummy *texture_dummy = (ModuleTextureDummy *)attr_color->get_texture()->get_module();
        return ray_direction.dot(normal) * texture_dummy->evaluate(ray_direction);
    } else {
        return ray_direction.dot(normal) * GMathVec3f(attr_color->get_vec3d());
    }
}

namespace MaterialDummy
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleMaterialDummy);
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)
        module_callbacks->cb_create_module = declare_module;
        module_callbacks->cb_shade = shade;
    }
}
