//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

//#include <of_object.h>
//#include <of_app.h>
//#include <dso_export.h>

#include "module_texture_dummy.h"
#include "texture_dummy.cma"

#if ISOTROPIX_VERSION_NUMBER >= IX_BUILD_VERSION_NUMBER(4, 0, 2, 0, 0, 0)
#define MODULE_CLASS OfModule
#else
#define MODULE_CLASS ModuleObject
#endif

// Create a class that implements functoin that will be then pluged to the module callback
class ModuleTextureDummyCallbacksOverrides : public ModuleTextureDummyCallbacks {
public :
    // Here we store the attributes because calling object.get_attribute("attr") can be very slow
    struct TextureDummyModuleData {
        const OfAttr *color;
    };

    // Create a module data that will exist during the life object
    static void * create_module_data(const OfObject& object)
    {
        TextureDummyModuleData *data = new TextureDummyModuleData();;
        data->color = object.get_attribute("color");
        return data;
    }

    // Destroy the module data when the object is destroyed
    static bool destroy_module_data(const OfObject& object, void *data)
    {
        delete (TextureDummyModuleData *)data;
        return true;
    }

    // Evaluate the texture and return the result
    static  GMathVec3f evaluate(OfObject& object, const GMathVec3f& ray_direction)
    {
        // Here we do a very simple example, but you could add arguments to the function and create a complex evaluation method
        TextureDummyModuleData *data = static_cast<TextureDummyModuleData *>(object.get_module_data());
        return GMathVec3f(data->color->get_vec3d()) * ray_direction;
    }
};

// WARNING: do not remove this typedef, it is needed by the macro IX_CREATE_MODULE_CLBK
typedef ModuleTextureDummyCallbacksOverrides IX_MODULE_CLBK;

namespace TextureDummy
{
    // This method is called when opening Clarisse and it register the module
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        // Create the new class
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleTextureDummy);
        new_classes.add(new_class);

        // Create the ModuleTextureDummyCallbacks and init it
        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)

        // Plug the previous defined function to the module callback created above
        module_callbacks->cb_create_module_data = IX_MODULE_CLBK::create_module_data;
        module_callbacks->cb_destroy_module_data = IX_MODULE_CLBK::destroy_module_data;
        module_callbacks->cb_evaluate = IX_MODULE_CLBK::evaluate;
    }
}
