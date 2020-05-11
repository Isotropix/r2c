//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_app.h>
#include <of_object_factory.h>
#include <of_time.h>

#include <RS.h>

#include "module_renderer_redshift.h"


IMPLEMENT_CLASS(ModuleRendererRedshift, ModuleProjectItem)

ModuleRendererRedshift::ModuleRendererRedshift() : ModuleProjectItem() {}

ModuleRendererRedshift::~ModuleRendererRedshift() {}

template <typename T>
inline T GetValue(OfObject& object, const char *aname, T dvalue, bool force_default, const float& mult)
{
    OfAttr *attr = object.get_attribute(aname);
    const T v = static_cast<T>(mult * (force_default ? dvalue : (attr != nullptr ? attr->get_long() : dvalue)));
    return static_cast<T>(attr != nullptr ? (v < static_cast<T>(attr->get_numeric_range_min()) ? static_cast<T>(attr->get_numeric_range_min()) : v) : v);
}

// set render option helper for bool
void
render_option_set_bool(OfObject& object, const char *aname, const char *rs_aname, bool dvalue, bool force_default = false)
{
    OfAttr *attr = object.get_attribute(aname);
    RS_RenderOption_SetBool(rs_aname, force_default ? dvalue : attr != nullptr ? attr->get_bool() : dvalue);
}

// set render option helper for uint
inline void
render_option_set_uint(OfObject& object, const char *aname, const char *rs_aname,
                       unsigned int dvalue, bool force_default = false, const float& mult = 1.0f)
{
    RS_RenderOption_SetUInt(rs_aname, GetValue<unsigned int>(object, aname, dvalue, force_default, mult));
}

// set render option helper for string
inline void
render_option_set_float(OfObject& object, const char *aname, const char *rs_aname,
                        float dvalue, bool force_default = false, const float& mult = 1.0f)
{
    RS_RenderOption_SetFloat(rs_aname, GetValue<float>(object, aname, dvalue, force_default, mult));
}


// set render option helper for string
void
render_option_set_string(OfObject& object, const char *aname, const char *rs_aname, const char *dvalue, bool force_default = false)
{
    OfAttr *attr = object.get_attribute(aname);
    RS_RenderOption_SetString(rs_aname, force_default ? dvalue : attr != nullptr ? attr->get_string().get_data() : dvalue);
}


void
ModuleRendererRedshift::sync(const float& sampling_quality)
{
    OfObject& object = *get_object();

    float current_frame = static_cast<float>(get_application().get_factory().get_time().get_current_frame());
    unsigned int frameid = *(unsigned int *) &current_frame; // hackish way to get a unique and consistent frame id from a float
    RS_RenderOption_SetUInt("FrameID", frameid);

    RS_RenderOption_SetBool("MotionBlurEnabled", false);

    render_option_set_uint(object, "min_samples", "UnifiedMinSamples", 8, false, sampling_quality);
    render_option_set_uint(object, "max_samples", "UnifiedMaxSamples", 32, false, sampling_quality);
    render_option_set_float(object, "adaptive_error_threshold", "UnifiedAdaptiveErrorThreshold", 0.005f);

    if (sampling_quality == 0.0f) {
        render_option_set_string(object, "filter_type", "UnifiedFilterType", "RS_AAFILTER_BOX", true);
        render_option_set_float(object, "filter_size", "UnifiedFilterSize", 1.0f, true);
    } else {
        render_option_set_string(object, "filter_type", "UnifiedFilterType", "RS_AAFILTER_GAUSS");
        render_option_set_float(object, "filter_size", "UnifiedFilterSize", 2.0f);
    }
    render_option_set_float(object, "max_subsample_intensity", "UnifiedMaxOverbright", 2.0f);
    render_option_set_bool(object, "randomize_pattern_on_each_frame", "UnifiedRandomizePattern", true);

    RS_Renderer_SetDisplayGamma(1.0f);
    RS_Renderer_SetColorInputGamma(1.0f);
    RS_Renderer_SetSamplingGamma(1.0f);

    render_option_set_bool(object, "enable_progressive_rendering", "ProgressiveRenderingEnabled", false);
    render_option_set_uint(object, "progressive_rendering_samples", "ProgressiveRenderingNumPasses", 64, false, sampling_quality);

    RS_RenderOption_SetString("PrimaryGIEngine", "RS_GIENGINE_BRUTE_FORCE");
    RS_RenderOption_SetString("SecondaryGIEngine", "RS_GIENGINE_BRUTE_FORCE");

    RS_RenderOption_SetUInt("NumGIBounces", 1);
    RS_RenderOption_SetBool("ConserveGIReflectionEnergy", true);

    RS_RenderOption_Validate();
}

// doing nothing there. Just here as an example
void
ModuleRendererRedshift::on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    ModuleProjectItem::on_attribute_change(attr, dirtiness, dirtiness_flags);
    if (attr.get_name() == "min_samples") {
    } else if (attr.get_name() == "max_samples") {
    } else if (attr.get_name() == "adaptive_error_threshold") {
    } else if (attr.get_name() == "randomize_pattern_on_each_frame") {
    } else if (attr.get_name() == "filter_type") {
    } else if (attr.get_name() == "filter_size") {
    } else if (attr.get_name() == "max_subsample_intensity") {
    }
}
