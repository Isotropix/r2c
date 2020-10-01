//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Clarisse includes
#include <of_object.h>
#include <module_light.h>

// Local includes
#include "./spherix_module_light.h"
#include "./spherix_module_texture.h"
#include "./spherix_light.cma"

// WARNING: keep those lines for compatibility reasons
#if ISOTROPIX_VERSION_NUMBER >= IX_BUILD_VERSION_NUMBER(4, 0, 2, 0, 0, 0)
#define MODULE_CLASS OfModule
#else
#define MODULE_CLASS ModuleObject
#endif

// Create a class that implements function that will be then pluged to the module callback
class ModuleLightSpherixCallbackOverrides : public ModuleLightCallbacks {
public :
    static MODULE_CLASS *declare_module(OfObject& object, OfObjectFactory& objects)
    {
        ModuleLightSpherix *light = new ModuleLightSpherix;
        light->set_object(object);
        return light;
    }
};


// WARNING: do not remove this typedef, it is needed by the macro IX_CREATE_MODULE_CLBK
typedef ModuleLightSpherixCallbackOverrides IX_MODULE_CLBK;

namespace SpherixLight
{
    // This method is called when opening Clarisse and it register the module
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        // Create the new class
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(SpherixLight);
        new_classes.add(new_class);

        // Create the ModuleTextureSpherixCallbacks and init it
        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)

        // Plug the previous defined function to the module callback created above
        module_callbacks->cb_create_module = IX_MODULE_CLBK::declare_module;
    }
}
