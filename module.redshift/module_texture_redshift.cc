//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_app.h>

#include <RS.h>

#include "module_texture_redshift.h"
#include "redshift_utils.h"

IMPLEMENT_CLASS(ModuleTextureRedshift, ModuleTextureOperator)

static const char *base_class_name = "TextureRedshift";

ModuleTextureRedshift::ModuleTextureRedshift() : ModuleTextureOperator()
{
    m_shader = nullptr;
}

ModuleTextureRedshift::~ModuleTextureRedshift()
{
    if (m_shader != nullptr) {
        RS_ShaderNode_Release(m_shader);
    }
}

void
ModuleTextureRedshift::on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    ModuleTextureOperator::on_attribute_change(attr, dirtiness, dirtiness_flags);

    if (m_shader != nullptr) {
        RedshiftUtils::on_attribute_change(*m_shader, attr, dirtiness, dirtiness_flags);
    }
}

CoreString
ModuleTextureRedshift::mangle_class(const CoreString& class_name)
{
    CoreString name = base_class_name;
    name += class_name;
    return name;
}

void
ModuleTextureRedshift::module_constructor(OfObject& object)
{
    ModuleTextureOperator::module_constructor(object);
    m_shader_class_name = object.get_class().get_name().get_data() + 15;

    CoreString id;
    id += get_object()->get_factory_id();
    m_shader = RS_ShaderNode_Get(id.get_data(), m_shader_class_name.get_data());
}
