//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_app.h>

#include "module_material_bbox.h"
#include "bbox_utils.h"

IMPLEMENT_CLASS(ModuleMaterialBbox, ModuleMaterial)

static const char *base_class_name = "MaterialBbox";

ModuleMaterialBbox::ModuleMaterialBbox() : ModuleMaterial()
{
    m_material = nullptr;
}

ModuleMaterialBbox::~ModuleMaterialBbox()
{
}

void
ModuleMaterialBbox::on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    ModuleMaterial::on_attribute_change(attr, dirtiness, dirtiness_flags);
}

void
ModuleMaterialBbox::on_material_rename(OfObject& object, const EventInfo& evtid, void *data)
{
}

CoreString
ModuleMaterialBbox::mangle_class(const CoreString& class_name)
{
    CoreString name = base_class_name;
    name += class_name;
    return name;
}

void
ModuleMaterialBbox::module_constructor(OfObject& object)
{
    ModuleMaterial::module_constructor(object);
}
