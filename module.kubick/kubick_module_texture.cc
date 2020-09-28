//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Local includes
#include "./kubick_module_texture.h"

// Needs to be kept outside the header
IMPLEMENT_CLASS(ModuleTextureKubick, ModuleTextureOperator)

static GMathVec3f evaluate_texture_default(OfObject& /* unused */)
{
    return GMathVec3f(1.0f, 0.0f, 0.0f);
}

ModuleTextureKubickCallbacks::ModuleTextureKubickCallbacks()
    : cb_evaluate(evaluate_texture_default)
{}
