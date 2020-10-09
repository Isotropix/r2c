//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Clarisse includes
#include <of_object.h>

// Local includes
#include "./kubix_module_light.h"
#include "./kubix_light.cma"

// WARNING: keep those lines for compatibility reasons
#if ISOTROPIX_VERSION_NUMBER >= IX_BUILD_VERSION_NUMBER(4, 9)
#define MODULE_CLASS OfModule
#else
#define MODULE_CLASS ModuleObject
#endif

// Create a class that implements function that will be then pluged to the module callback
class ModuleLightKubixCallbackOverrides : public ModuleLightKubixCallbacks {
public:

    // Here we store the attributes because calling object.get_attribute("attr") can be very slow
    struct KubixLightModuleData {
        const OfAttr *color;
    };

    // Create a module data that will exist during the object's lifetime
    static void * create_module_data(const OfObject& object)
    {
        KubixLightModuleData *data = new KubixLightModuleData();
        data->color = object.get_attribute("color");
        return data;
    }

    // Destroy the module data when the object is destroyed
    static bool destroy_module_data(const OfObject& object, void *data)
    {
        delete (KubixLightModuleData *)data;
        return true;
    }

    // Evaluate the light and return the result
    static GMathVec3f evaluate(OfObject& object)
    {
        // Here we do a very simple example, but you could add arguments to the function and create a complex evaluation method
        const KubixLightModuleData *data = static_cast<KubixLightModuleData *>(object.get_module_data());
        return GMathVec3f(data->color->get_vec3d());
    }
};

// WARNING: do not remove this typedef, it is needed by the macro IX_CREATE_MODULE_CLBK
typedef ModuleLightKubixCallbackOverrides IX_MODULE_CLBK;

namespace KubixLight
{
    // This method is called when opening Clarisse and it registers the module
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        // Create the new class
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleLightKubix);
        new_classes.add(new_class);

        // Create the ModuleTextureKubixCallbacks and init it
        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)

        // Plug the previous defined function to the module callback created above
        module_callbacks->cb_create_module_data = IX_MODULE_CLBK::create_module_data;
        module_callbacks->cb_destroy_module_data = IX_MODULE_CLBK::destroy_module_data;
        module_callbacks->cb_evaluate = IX_MODULE_CLBK::evaluate;
    }
}
