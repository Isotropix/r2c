//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include "module_texture_dummy.h"

// Needs to be kept outside the header
IMPLEMENT_CLASS(ModuleTextureDummy, ModuleTextureOperator)

static GMathVec3f evaluate_texture_default(OfObject& /* unused */, const GMathVec3f& ray_direction)
{
    return GMathVec3f(1.0f, 0.0f, 0.0f);
}

ModuleTextureDummyCallbacks::ModuleTextureDummyCallbacks()
    : cb_evaluate(evaluate_texture_default)
{}
