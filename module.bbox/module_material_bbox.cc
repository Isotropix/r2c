//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_app.h>

#include "module_material_bbox.h"
#include "bbox_utils.h"

IMPLEMENT_CLASS(ModuleMaterialBbox, ModuleMaterial)

static GMathVec3f default_shade(OfObject& /* unused */)
{
    return GMathVec3f(1.0f, 0.0f, 0.0f);
}

ModuleMaterialBboxCallbacks::ModuleMaterialBboxCallbacks()
: cb_shade(default_shade)
{}

ModuleMaterialBbox::ModuleMaterialBbox()
    : ModuleMaterial()
{}

ModuleMaterialBbox::~ModuleMaterialBbox()
{
}
