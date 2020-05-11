//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_app.h>

#include <RS.h>

#include "module_material_redshift.h"

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

inline RSColor
get_color3(const OfAttr& attr)
{
    return RSColor(static_cast<float>(attr.get_double(0)),
                   static_cast<float>(attr.get_double(1)),
                   static_cast<float>(attr.get_double(2)));
}

inline RSColor
get_color4(const OfAttr& attr)
{
    return RSColor(static_cast<float>(attr.get_double(0)),
                   static_cast<float>(attr.get_double(1)),
                   static_cast<float>(attr.get_double(2)),
                   static_cast<float>(attr.get_double(3)));
}

inline RSVector2
get_vec2(const OfAttr& attr)
{
    return RSVector2(static_cast<float>(attr.get_double(0)),
                     static_cast<float>(attr.get_double(1)));
}

inline RSVector3
get_vec3(const OfAttr& attr)
{
    return RSVector3(static_cast<float>(attr.get_double(0)),
                     static_cast<float>(attr.get_double(1)),
                     static_cast<float>(attr.get_double(2)));
}

inline RSVector4
get_vec4(const OfAttr& attr)
{
    return RSVector4(static_cast<float>(attr.get_double(0)),
                     static_cast<float>(attr.get_double(1)),
                     static_cast<float>(attr.get_double(2)),
                     static_cast<float>(attr.get_double(3)));
}

inline float
get_float(const OfAttr& attr) { return static_cast<float>(attr.get_double(0)); }

inline int
get_int(const OfAttr& attr) { return static_cast<int>(attr.get_long(0)); }

inline RSUInt4
get_uint4(const OfAttr& attr) {
    return RSUInt4(static_cast<unsigned int>(attr.get_long(0)),
                   static_cast<unsigned int>(attr.get_long(1)),
                   static_cast<unsigned int>(attr.get_long(2)),
                   static_cast<unsigned int>(attr.get_long(3)));
}

inline const char *
get_string(const OfAttr& attr) { return attr.get_string().get_data(); }

inline bool
get_bool(const OfAttr& attr) { return attr.get_bool(); }

void
ModuleMaterialRedshift::on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    ModuleMaterial::on_attribute_change(attr, dirtiness, dirtiness_flags);

    RSShaderNode *shader = get_material()->GetSurfaceShaderNodeGraph();
    if (shader != nullptr) {
        const char *name = attr.get_name().get_data();
        unsigned int idx = shader->GetParameterIndex(name);
        shader->BeginUpdate();

        switch (attr.get_visual_hint()) {
            case OfAttr::VISUAL_HINT_RGB:
                shader->SetParameterData(idx, get_color3(attr));
                break;
            case OfAttr::VISUAL_HINT_RGBA:
                shader->SetParameterData(idx, get_color4(attr));
                break;
            case OfAttr::VISUAL_HINT_DEFAULT:
                switch (attr.get_type()) {
                    case OfAttr::TYPE_BOOL:
                        shader->SetParameterData(idx, get_bool(attr));
                        break;
                    case OfAttr::TYPE_LONG:
                        if (attr.get_value_count() == 4) {
                            shader->SetParameterData(idx, get_uint4(attr));
                        } else {
                            shader->SetParameterData(idx, get_int(attr));
                        }
                        break;
                    case OfAttr::TYPE_DOUBLE:
                        switch (attr.get_value_count()) {
                            case 2:
                                shader->SetParameterData(idx, get_vec2(attr));
                                break;
                            case 3:
                                shader->SetParameterData(idx, get_vec3(attr));
                                break;
                            case 4:
                                shader->SetParameterData(idx, get_vec4(attr));
                                break;
                            default:
                                shader->SetParameterData(idx, get_float(attr));
                                break;
                        }
                        break;
                    case OfAttr::TYPE_STRING:
                        shader->SetParameterData(idx, get_string(attr));
                        break;
                    default: break;
                }
                break;
            default: break;
        }
        shader->EndUpdate();
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
