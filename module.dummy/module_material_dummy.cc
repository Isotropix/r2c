//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_app.h>

#include "module_material_dummy.h"
#include "dummy_utils.h"

IMPLEMENT_CLASS(ModuleMaterialDummy, ModuleMaterial)

static GMathVec3f default_shade(OfObject& /* unused */)
{
    return GMathVec3f(1.0f, 0.0f, 0.0f);
}

ModuleMaterialDummyCallbacks::ModuleMaterialDummyCallbacks()
: cb_shade(default_shade)
{}

ModuleMaterialDummy::ModuleMaterialDummy()
    : ModuleMaterial()
{}

ModuleMaterialDummy::~ModuleMaterialDummy()
{
}
