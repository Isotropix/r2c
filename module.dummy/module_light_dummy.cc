//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_app.h>

#include "module_light_dummy.h"

IMPLEMENT_CLASS(ModuleLightDummy, ModuleLight)

static const char *base_class_name = "LightDummy";

static GMathVec3f default_evaluate(OfObject&)
{
    return GMathVec3f(1.0f, 0.0f, 0.0f);
}

ModuleLightDummyCallbacks::ModuleLightDummyCallbacks()
: cb_evaluate(default_evaluate)
{
}

ModuleLightDummy::ModuleLightDummy() : ModuleLight()
{
}

ModuleLightDummy::~ModuleLightDummy()
{
}

// TODO: is this usefull ?
CoreString
ModuleLightDummy::mangle_class(const CoreString& class_name)
{
    CoreString name = base_class_name;
    name += class_name;
    return name;
}
