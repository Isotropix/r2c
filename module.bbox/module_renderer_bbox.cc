//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_app.h>
#include <of_object_factory.h>
#include <of_time.h>

#include "module_renderer_bbox.h"


IMPLEMENT_CLASS(ModuleRendererBbox, ModuleRenderer)

ModuleRendererBbox::ModuleRendererBbox() : ModuleRenderer(), m_background_color(0.0f) {}

ModuleRendererBbox::~ModuleRendererBbox() {}

template <typename T>
inline T GetValue(OfObject& object, const char *aname, T dvalue, bool force_default, const float& mult)
{
    OfAttr *attr = object.get_attribute(aname);
    const T v = static_cast<T>(mult * (force_default ? dvalue : (attr != nullptr ? attr->get_long() : dvalue)));
    return static_cast<T>(attr != nullptr ? (v < static_cast<T>(attr->get_numeric_range_min()) ? static_cast<T>(attr->get_numeric_range_min()) : v) : v);
}

void
ModuleRendererBbox::sync(const float& sampling_quality)
{
}

// doing nothing there. Just here as an example
void
ModuleRendererBbox::on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    ModuleProjectItem::on_attribute_change(attr, dirtiness, dirtiness_flags);
    if (attr.get_name() == "background_color") {
        m_background_color = static_cast<GMathVec3f>(attr.get_vec3d());
    }
}
