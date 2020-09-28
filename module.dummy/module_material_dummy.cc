//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include "module_material_dummy.h"

// Needs to be kept outside the header
IMPLEMENT_CLASS(ModuleMaterialDummy, ModuleMaterial)

static GMathVec3f default_shade(OfObject& /* object */, const GMathVec3f& /* ray_dir */, const GMathVec3f& /* normal */ )
{
    return GMathVec3f(1.0f, 0.0f, 0.0f);
}

ModuleMaterialDummyCallbacks::ModuleMaterialDummyCallbacks()
: cb_shade(default_shade)
{}
