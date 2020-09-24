//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_app.h>

#include "module_light_bbox.h"

IMPLEMENT_CLASS(ModuleLightBbox, ModuleLight)

static const char *base_class_name = "LightBbox";

ModuleLightBbox::ModuleLightBbox() : ModuleLight()
{
}

ModuleLightBbox::~ModuleLightBbox()
{
}

CoreString
ModuleLightBbox::mangle_class(const CoreString& class_name)
{
    CoreString name = base_class_name;
    name += class_name;
    return name;
}

static GMathVec3f default_evaluate(OfObject&)
{
    return GMathVec3f(0.0,0.0,0.0);
}

ModuleLightBboxCallback::ModuleLightBboxCallback()
{
    cb_evaluate = default_evaluate;
}
