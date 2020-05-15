//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_app.h>

#include "module_light_redshift.h"

IMPLEMENT_CLASS(ModuleLightRedshift, ModuleLight)

static const char *base_class_name = "LightRedshift";

ModuleLightRedshift::ModuleLightRedshift() : ModuleLight()
{
}

ModuleLightRedshift::~ModuleLightRedshift()
{
}

CoreString
ModuleLightRedshift::mangle_class(const CoreString& class_name)
{
    CoreString name = base_class_name;
    name += class_name;
    return name;
}
