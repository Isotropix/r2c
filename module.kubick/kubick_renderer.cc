//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Clarisse includes
#include <of_object.h>

// Local includes
#include "./kubick_render_delegate.h"
#include "./kubick_module_renderer.h"
#include "./kubick_renderer.cma"

// WARNING: keep those lines for compatibility reasons
#if ISOTROPIX_VERSION_NUMBER >= IX_BUILD_VERSION_NUMBER(4, 0, 2, 0, 0, 0)
#define MODULE_CLASS OfModule
#else
#define MODULE_CLASS ModuleObject
#endif

IX_BEGIN_DECLARE_MODULE_CALLBACKS(ModuleRendererKubick, ModuleRendererCallbacks)
    static MODULE_CLASS *declare_module(OfObject& object, OfObjectFactory& objects);
    static void get_supported_lights(OfObject& object, CoreVector<CoreString>& supported_lights, CoreVector<CoreString>& unsupported_lights);
	static void get_supported_geometries(OfObject& object, CoreVector<CoreString>& supported_geometries, CoreVector<CoreString>& unsupported_geometries);
	static void get_supported_materials(OfObject& object, CoreVector<CoreString>& supported_materials, CoreVector<CoreString>& unsupported_materials);
IX_END_DECLARE_MODULE_CALLBACKS(ModuleRendererKubick)

MODULE_CLASS *
IX_MODULE_CLBK::declare_module(OfObject& object, OfObjectFactory& objects)
{
    ModuleRendererKubick *settings = new ModuleRendererKubick;
    settings->set_object(object);
    return settings;
}

// Implementing this callback allows to specify to the Renderer to not update the render when a non-supported light is added/removed/updated
void 
IX_MODULE_CLBK::get_supported_lights(OfObject& object, CoreVector<CoreString>& supported_lights, CoreVector<CoreString>& unsupported_lights)
{
	supported_lights = KubickRenderDelegate::s_supported_lights;
	unsupported_lights = KubickRenderDelegate::s_unsupported_lights;
}

// Implementing this callback allows to specify to the Renderer to not update the render when a non-supported geometry is added/removed/updated
void 
IX_MODULE_CLBK::get_supported_geometries(OfObject& object, CoreVector<CoreString>& supported_geometries, CoreVector<CoreString>& unsupported_geometries)
{
	supported_geometries = KubickRenderDelegate::s_supported_geometries;
	unsupported_geometries = KubickRenderDelegate::s_unsupported_geometries;
}

// Implementing this callback allows to specify to the Renderer to not update the render when a non-supported material is added/removed/updated
void 
IX_MODULE_CLBK::get_supported_materials(OfObject& object, CoreVector<CoreString>& supported_materials, CoreVector<CoreString>& unsupported_materials)
{
	supported_materials = KubickRenderDelegate::s_supported_materials;
	unsupported_materials = KubickRenderDelegate::s_unsupported_materials;
}

namespace KubickRenderer
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleRendererKubick)
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)
        module_callbacks->cb_create_module              = IX_MODULE_CLBK::declare_module;
		module_callbacks->cb_get_supported_lights       = IX_MODULE_CLBK::get_supported_lights;
		module_callbacks->cb_get_supported_geometries   = IX_MODULE_CLBK::get_supported_geometries;
		module_callbacks->cb_get_supported_materials    = IX_MODULE_CLBK::get_supported_materials;
    }
}
