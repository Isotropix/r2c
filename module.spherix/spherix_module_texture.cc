//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Local includes
#include "./spherix_module_texture.h"
#include "./spherix_external_renderer.h"
#include "./spherix_register_shaders.h"
#include "./spherix_utils.h"

// Needs to be kept outside the header
IMPLEMENT_CLASS(ModuleTextureSpherix, ModuleTexture)

ModuleTextureSpherix::ModuleTextureSpherix() : ModuleTexture()
{
    m_texture = nullptr;
}

ModuleTextureSpherix::~ModuleTextureSpherix()
{
    if (m_texture != nullptr) {
        delete m_texture;
    }
}

// Create the External Shader associated with the class name
ExternalTextureShader *create_texture_shader(const std::string class_name)
{
    if (class_name == "SpherixTextureColor") {
        return new SpherixTextureColor();
    }

    CORE_ASSERT(false);
    return nullptr;
}

void
ModuleTextureSpherix::module_constructor(OfObject& object)
{
    ModuleTexture::module_constructor(object);
    const CoreString& shader_class_name = object.get_class().get_name();

    // Create the actual External Shader using the object class name
    m_texture = create_texture_shader(shader_class_name.get_data());
}

void
ModuleTextureSpherix::on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    ModuleTexture::on_attribute_change(attr, dirtiness, dirtiness_flags);

    // Synchronizing the external shader automatically when an attribute change
    SpherixAttributChange::on_attribute_change(attr, m_texture);
}
