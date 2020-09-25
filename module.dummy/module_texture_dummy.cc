//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include "module_texture_dummy.h"
#include "dummy_utils.h"

IMPLEMENT_CLASS(ModuleTextureDummy, ModuleTextureOperator)

static const char *base_class_name = "TextureDummy";

static GMathVec3f evaluate_texture_default(OfObject& /* unused */, const GMathVec3f& ray_direction)
{
    return GMathVec3f(1.0f, 0.0f, 0.0f);
}

ModuleTextureDummyCallbacks::ModuleTextureDummyCallbacks()
    : cb_evaluate(evaluate_texture_default)
{
}


ModuleTextureDummy::ModuleTextureDummy() : ModuleTextureOperator()
{
}

void
ModuleTextureDummy::on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    ModuleTextureOperator::on_attribute_change(attr, dirtiness, dirtiness_flags);
}

CoreString
ModuleTextureDummy::mangle_class(const CoreString& class_name)
{
    CoreString name = base_class_name;
    name += class_name;
    return name;
}

void
ModuleTextureDummy::module_constructor(OfObject& object)
{
    ModuleTextureOperator::module_constructor(object);
}

