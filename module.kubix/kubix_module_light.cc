//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Local includes
#include "./kubix_module_light.h"

// Needs to be kept outside the header
IMPLEMENT_CLASS(ModuleLightKubix, ModuleLight)

static GMathVec3f default_evaluate(OfObject&)
{
    return GMathVec3f(1.0f, 0.0f, 0.0f);
}

ModuleLightKubixCallbacks::ModuleLightKubixCallbacks()
: cb_evaluate(default_evaluate)
{}
