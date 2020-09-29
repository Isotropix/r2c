//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Local includes
#include "./kubix_module_material.h"

// Needs to be kept outside the header
IMPLEMENT_CLASS(ModuleMaterialKubix, ModuleMaterial)

static GMathVec3f default_shade(OfObject& /* object */, const GMathVec3f& /* ray_dir */, const GMathVec3f& /* normal */ )
{
    return GMathVec3f(1.0f, 0.0f, 0.0f);
}

ModuleMaterialKubixCallbacks::ModuleMaterialKubixCallbacks()
: cb_shade(default_shade)
{}
