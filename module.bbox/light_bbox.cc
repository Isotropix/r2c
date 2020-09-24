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

IX_BEGIN_DECLARE_MODULE_CALLBACKS(ModuleLightBbox, ModuleLightBboxCallback)
    static GMathVec3d evaluate(OfObject&object);
IX_END_DECLARE_MODULE_CALLBACKS(ModuleLightBbox)


namespace LightBbox
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleLightBbox);
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)

        module_callbacks->cb_evaluate = IX_MODULE_CLBK::evaluate;
    }
}

GMathVec3d IX_MODULE_CLBK::evaluate(OfObject& object)
{
    return object.get_attribute("color")->get_vec3d();
}
