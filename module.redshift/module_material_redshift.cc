//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_app.h>

#include <RS.h>

#include "module_material_redshift.h"
#include "redshift_utils.h"

IMPLEMENT_CLASS(ModuleMaterialRedshift, ModuleMaterial)

static const char *base_class_name = "MaterialRedshift";

ModuleMaterialRedshift::ModuleMaterialRedshift() : ModuleMaterial()
{
    m_material = nullptr;
}

ModuleMaterialRedshift::~ModuleMaterialRedshift()
{
    if (m_material != nullptr) {
        RSShaderNode *shader = get_material()->GetSurfaceShaderNodeGraph();
        RS_ShaderNode_Release(shader);
        RS_Material_Release(m_material);
    }
}

void
ModuleMaterialRedshift::on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    ModuleMaterial::on_attribute_change(attr, dirtiness, dirtiness_flags);

    if (RSShaderNode *shader = get_material()->GetSurfaceShaderNodeGraph()) {
		RedshiftUtils::on_attribute_change(*shader, attr, dirtiness, dirtiness_flags);
    }
}

void
ModuleMaterialRedshift::on_material_rename(OfObject& object, const EventInfo& evtid, void *data)
{
    if (m_material != nullptr) {
        // update the material resource name if it changed
        m_material->SetResourceName(get_object_name().get_data());
    }
}

CoreString
ModuleMaterialRedshift::mangle_class(const CoreString& class_name)
{
    CoreString name = base_class_name;
    name += class_name;
    return name;
}

void
ModuleMaterialRedshift::module_constructor(OfObject& object)
{
    ModuleMaterial::module_constructor(object);
    m_shader_class_name = object.get_class().get_name().get_data() + 16;
    connect(*get_object(), EVT_ID_OF_OBJECT_RENAME, EVENT_METHOD(ModuleMaterialRedshift::on_material_rename));
    connect(*get_object(), EVT_ID_OF_OBJECT_CONTEXT_CHANGED, EVENT_METHOD(ModuleMaterialRedshift::on_material_rename));

    m_material = RS_Material_Get(get_object_name().get_data());
    CoreString id;
    id += get_of_module_id();
    RSShaderNode *shader = RS_ShaderNode_Get(id.get_data(), m_shader_class_name.get_data());
    if (shader != nullptr) {
        m_material->SetSurfaceShaderNodeGraph(shader);
    }
}
