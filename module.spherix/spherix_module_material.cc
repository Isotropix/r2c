//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Local includes
#include "./spherix_module_material.h"
#include "./spherix_external_renderer.h"
#include "./spherix_register_shaders.h"
#include "./spherix_utils.h"

// Needs to be kept outside the header
IMPLEMENT_CLASS(ModuleMaterialSpherix, ModuleMaterial)

ModuleMaterialSpherix::ModuleMaterialSpherix() : ModuleMaterial()
{
    m_material = nullptr;
}

ModuleMaterialSpherix::~ModuleMaterialSpherix()
{
    delete m_material;
}

// Create the External Shader associated with the class name
ExternalMaterialShader *create_material_shader(const std::string class_name)
{
    if (class_name == "SpherixMaterialDiffuse") {
        return new SpherixMaterialDiffuse();
    } else if (class_name == "SpherixMaterialReflection") {
        return new SpherixMaterialReflection();
    }

    CORE_ASSERT(false);
    return nullptr;
}

void
ModuleMaterialSpherix::module_constructor(OfObject& object)
{
    ModuleMaterial::module_constructor(object);
    const CoreString& shader_class_name = object.get_class().get_name();

    // Create the actual External Shader using the object class name
    m_material = create_material_shader(shader_class_name.get_data());
}

void
ModuleMaterialSpherix::on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    ModuleMaterial::on_attribute_change(attr, dirtiness, dirtiness_flags);

    // Synchronizing the external shader automatically when an attribute change
    SpherixAttributChange::on_attribute_change(attr, m_material);
}
