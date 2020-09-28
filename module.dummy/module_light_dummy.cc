//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include "module_light_dummy.h"

// Needs to be kept outside the header
IMPLEMENT_CLASS(ModuleLightDummy, ModuleLight)

static GMathVec3f default_evaluate(OfObject&)
{
    return GMathVec3f(1.0f, 0.0f, 0.0f);
}

ModuleLightDummyCallbacks::ModuleLightDummyCallbacks()
: cb_evaluate(default_evaluate)
{}
