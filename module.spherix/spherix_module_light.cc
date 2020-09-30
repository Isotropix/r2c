//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Local includes
#include "./spherix_module_light.h"
#include "./spherix_external_renderer.h"
#include "./spherix_register_shaders.h"
#include "./spherix_utils.h"

// Needs to be kept outside the header
IMPLEMENT_CLASS(ModuleLightSpherix, ModuleLight)

ModuleLightSpherix::ModuleLightSpherix() : ModuleLight()
{
    m_light = nullptr;
}

ModuleLightSpherix::~ModuleLightSpherix()
{
    if (m_light != nullptr) {
        delete m_light;
    }
}

ExternalLightShader *create_light_shader(const std::string class_name) {
    if (class_name == "SpherixLightDistant") {
        return new SpherixLightDistant();
    }

    CORE_ASSERT(false);
    return nullptr;
}

void
ModuleLightSpherix::module_constructor(OfObject& object)
{
    ModuleLight::module_constructor(object);
    const CoreString& shader_class_name = object.get_class().get_name();

    // Create the actual External Shader using the object class name
    m_light = create_light_shader(shader_class_name.get_data());
}

void
ModuleLightSpherix::on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    ModuleLight::on_attribute_change(attr, dirtiness, dirtiness_flags);

    // Synchronizing the external shader automatically when an attribute change
    SpherixAttributChange::on_attribute_change(attr, m_light);
}
