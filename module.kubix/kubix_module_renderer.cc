//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Local includes
#include "./kubix_module_renderer.h"

// Needs to be kept outside the header
IMPLEMENT_CLASS(ModuleRendererKubix, ModuleRenderer)

ModuleRendererKubix::ModuleRendererKubix() : ModuleRenderer(), m_background_color(0.0f) {}

void
ModuleRendererKubix::on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    ModuleProjectItem::on_attribute_change(attr, dirtiness, dirtiness_flags);
    if (attr.get_name() == "background_color") {
        m_background_color = static_cast<GMathVec3f>(attr.get_vec3d());
    }
}
