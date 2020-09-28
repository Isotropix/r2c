//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Clarisse includes
#include <of_object.h>

// Local includes
#include "./module_material_dummy.h"
#include "./module_texture_dummy.h"
#include "./material_dummy.cma"

// WARNING: keep those lines for compatibility reasons
#if ISOTROPIX_VERSION_NUMBER >= IX_BUILD_VERSION_NUMBER(4, 0, 2, 0, 0, 0)
#define MODULE_CLASS OfModule
#else
#define MODULE_CLASS ModuleObject
#endif

// Create a class that implements function that will be then pluged to the module callback
class ModuleMaterialDummyCallbackOverrides : public ModuleMaterialDummyCallbacks {
public :
    // Here we store the attributes because calling object.get_attribute("attr") can be very slow
    struct MaterialDummyModuleData {
        const OfAttr *color;
    };

    // Create a module data that will exist during the object's lifetime
    static void * create_module_data(const OfObject& object)
    {
        MaterialDummyModuleData *data = new MaterialDummyModuleData();;
        data->color = object.get_attribute("color");
        return data;
    }

    // Destroy the module data when the object is destroyed
    static bool destroy_module_data(const OfObject& object, void *data)
    {
        delete (MaterialDummyModuleData *)data;
        return true;
    }

    static GMathVec3f shade(OfObject& object, const GMathVec3f& ray_direction, const GMathVec3f& normal)
    {
        // Here we do a very simple example, but you could add arguments to the function and create a complex evaluation method
        OfAttr *attr_color = object.get_attribute("color");
        if (attr_color->is_textured()) {
            // Evaluate the texture and return the result
            ModuleTextureDummy *texture_dummy = (ModuleTextureDummy *)attr_color->get_texture()->get_module();
            return fabs(ray_direction.dot(normal)) * texture_dummy->evaluate(ray_direction);
        } else {
            return fabs(ray_direction.dot(normal)) * GMathVec3f(attr_color->get_vec3d());
        }
    }
};


// WARNING: do not remove this typedef, it is needed by the macro IX_CREATE_MODULE_CLBK
typedef ModuleMaterialDummyCallbackOverrides IX_MODULE_CLBK;

namespace MaterialDummy
{
    // This method is called when opening Clarisse and it register the module
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        // Create the new class
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleMaterialDummy);
        new_classes.add(new_class);

        // Create the ModuleTextureDummyCallbacks and init it
        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)

        // Plug the previous defined function to the module callback created above
        module_callbacks->cb_create_module_data     = IX_MODULE_CLBK::create_module_data;
        module_callbacks->cb_destroy_module_data    = IX_MODULE_CLBK::destroy_module_data;
        module_callbacks->cb_shade                  = IX_MODULE_CLBK::shade;
    }
}
