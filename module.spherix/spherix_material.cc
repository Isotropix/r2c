//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Clarisse includes
#include <of_object.h>

// Local includes
#include "./spherix_module_material.h"
#include "./spherix_module_texture.h"
#include "./spherix_material.cma"

// WARNING: keep those lines for compatibility reasons
#if ISOTROPIX_VERSION_NUMBER >= IX_VERSION_NUMBER(5, 0)
#define MODULE_CLASS OfModule
#else
#define MODULE_CLASS ModuleObject
#endif

// Create a class that implements function that will be then pluged to the module callback
class ModuleMaterialSpherixCallbackOverrides : public ModuleMaterialCallbacks {
public :
    static MODULE_CLASS *declare_module(OfObject& object, OfObjectFactory& objects)
    {
        ModuleMaterialSpherix *material = new ModuleMaterialSpherix;
        material->set_object(object);
        return material;
    }
};


// WARNING: do not remove this typedef, it is needed by the macro IX_CREATE_MODULE_CLBK
typedef ModuleMaterialSpherixCallbackOverrides IX_MODULE_CLBK;

namespace SpherixMaterial
{
    // This method is called when opening Clarisse and it register the module
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        // Create the new class
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(SpherixMaterial);
        new_classes.add(new_class);

        // Create the ModuleTextureSpherixCallbacks and init it
        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)

        // Plug the previous defined function to the module callback created above
        module_callbacks->cb_create_module = IX_MODULE_CLBK::declare_module;
    }
}
