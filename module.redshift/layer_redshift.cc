//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <of_object.h>
#include <of_app.h>
#include <dso_export.h>

#include <module_layer.h>
#include <module_group.h>
#include <gui_image.h>
#include <image_canvas.h>
#include <image_handle.h>
#include <image_map_channel.h>
#include <gmath_vec3.h>
#include <gmath_vec4.h>


#include <r2c_scene_delegate.h>
#include <r2c_render_buffer.h>

#include "redshift_render_delegate.h"

#include "layer_redshift.cma"

static ImageCanvas *
get_null_image()
{
    static ImageCanvas *null_image = new ImageCanvas(2,2,4);
    return null_image;
}

class LayerRedshiftModuleData {
public:

    LayerRedshiftModuleData(OfApp& app) : application(app) {
        context_geometries = nullptr;
        context_lights = nullptr;
        scene_geometries = nullptr;
        scene_lights = nullptr;
        scene_delegate = nullptr;
        render_delegate = nullptr;
    }

    ~LayerRedshiftModuleData() {
        if (scene_delegate != nullptr) {
            delete render_delegate;
            R2cSceneDelegate::destroy(scene_delegate);
        }
    }

    static OfObject * create_embedded_group(OfObject& object, const CoreString& group_name, const CoreString& filter) {
        OfObject *emb_group = object.add_embedded_object(group_name, "Group");
        ModuleGroup *module_group = static_cast<ModuleGroup *>(emb_group->get_module());
        module_group->set_filter(filter);
        module_group->set_inclusion_rule("./*");
        // set static flag first to avoid future set value on embedded object' attributes to trigger override management mechanism
        emb_group->set_static(true);
        emb_group->set_private(true);
        emb_group->set_read_only(true);
        emb_group->get_attribute("objects")->set_weak_referencing(true);  // done at this level to avoid hacking group behavior for this special use case
        emb_group->get_attribute("references")->set_weak_referencing(true);
        emb_group->set_state(OfItem::STATE_LIVE);
        module_group->set_is_dirty();
        return emb_group;
    }

    R2cSceneDelegate *get_scene_delegate() {
        if (scene_delegate == nullptr) {
            // must initialize first R2cSceneDelegate
            if (!R2cSceneDelegate::is_initialized()) {
                R2cSceneDelegate::initialize(application);
            }
            scene_delegate = R2cSceneDelegate::create();
            render_delegate = new RedshiftRenderDelegate;
            scene_delegate->set_render_delegate(render_delegate);
        }
        return scene_delegate;
    }

    OfApp& application;
    bool dirty;
    OfObject *context_geometries;
    OfObject *context_lights;
    OfAttr *scene_geometries;
    OfAttr *scene_lights;

    static const char *__scene_geometries__;
    static const char *__scene_lights__;

private:

    R2cSceneDelegate *scene_delegate;
    RedshiftRenderDelegate *render_delegate;
};

const char *LayerRedshiftModuleData::__scene_geometries__ = "__scene_geometries__";
const char *LayerRedshiftModuleData::__scene_lights__ = "__scene_lights__";

IX_BEGIN_DECLARE_MODULE_CALLBACKS(ModuleLayerRedshift, ModuleLayerCallbacks)
    static void module_constructor(OfObject& object, ModuleObject *module);
    static void on_deserialize_object(OfObject& object, const CoreVersion& serial_version, const ParserGroup& parser_group, const OfSerialOptions& serial_options);
    static void on_attribute_change(OfObject& object, const OfAttr& attr, int& dirtiness, const int& dirtiness_flags);
    static const ImageCanvas *get_image(OfObject& object, const ModuleImage::Quality& quality, const GMathVec4f* region);
    static void* create_module_data(const OfObject& object);
    static bool destroy_module_data(const OfObject& object, void *module_data);
IX_END_DECLARE_MODULE_CALLBACKS(ModuleLayerRedshift)

void *
IX_MODULE_CLBK::create_module_data(const OfObject& object)
{
    return new LayerRedshiftModuleData(object.get_application());
}

bool
IX_MODULE_CLBK::destroy_module_data(const OfObject& object, void *module_data)
{
    LayerRedshiftModuleData *data = static_cast<LayerRedshiftModuleData *>(module_data);
    delete data;
    return true;
}

void
IX_MODULE_CLBK::module_constructor(OfObject& object, ModuleObject *module)
{
    // might not be needed as it references none or itself embedded groups
    LayerRedshiftModuleData *data = static_cast<LayerRedshiftModuleData *>(object.get_module_data());

    // ideally we should declare tokens and fail at compile time if an attribute is not declared
    data->scene_geometries = object.get_attribute(LayerRedshiftModuleData::__scene_geometries__);
    data->scene_lights = object.get_attribute(LayerRedshiftModuleData::__scene_lights__);

    // creating internal groups to gather geometries and lights that lie under the same context of the image/layer
    data->context_geometries = LayerRedshiftModuleData::create_embedded_group(object, "__context_geometries__", "SceneObject");
    data->context_lights = LayerRedshiftModuleData::create_embedded_group(object, "__context_lights__", "Light");
    data->scene_geometries->set_weak_referencing(true);
    data->scene_geometries->set_object(data->context_geometries);
    data->scene_lights->set_weak_referencing(true);
    data->scene_lights->set_object(data->context_lights);

    // set the scene delegate to the right groups
    data->get_scene_delegate()->set_geometries(data->context_geometries);
    data->get_scene_delegate()->set_lights(data->context_lights);
}

void
IX_MODULE_CLBK::on_attribute_change(OfObject& object, const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    bool dirty = false;
    LayerRedshiftModuleData *data = static_cast<LayerRedshiftModuleData *>(object.get_module_data());
    const CoreString& aname = attr.get_name();

    R2cSceneDelegate *scene = data->get_scene_delegate();
    if (aname == "camera") {
        scene->set_camera(attr.get_object());
        dirty = true;
    } else if (aname == "renderer") {
        scene->set_render_settings(attr.get_object());
        dirty = true;
    } else if (aname == "geometries") {
        // Checking if we changed the value of the geometries group
        // in that case we must update the scene geometries accordingly
        // to reflect the changes. When null we use the internal group gathering
        // all geometries from the context. Otherwise we just use the ones defined
        // in the group referenced by geometries.
        if (attr.get_object() == nullptr) {
            data->scene_geometries->set_object(data->context_geometries);
            scene->set_geometries(data->context_geometries);
        } else {
            data->scene_geometries->set_object(attr.get_object());
            scene->set_geometries(attr.get_object());
        }
        dirty = true;
    } else if (aname == "lights") {
        // Checking if we changed the value of the lights group
        // in that case we must update the scene geometries accordingly
        // to reflect the changes. When null we use the internal group gathering
        // all lights from the context. Otherwise we just use the ones defined
        // in the group referenced by lights.
        if (attr.get_object() == nullptr) {
            data->scene_lights->set_object(data->context_lights);
            scene->set_lights(data->context_lights);
        } else {
            data->scene_lights->set_object(attr.get_object());
            scene->set_lights(attr.get_object());
        }
        dirty = true;
    } else if (aname == LayerRedshiftModuleData::__scene_geometries__) {
        if (attr.get_event_info().type == OfAttrEvent::TYPE_PROPAGATE) {
            // dirtiness propagation from geometries
        }

        dirty = true;
    } else if (aname == LayerRedshiftModuleData::__scene_lights__) {
        if (attr.get_event_info().type == OfAttrEvent::TYPE_PROPAGATE) {
            // dirtiness propagation from lights
        }
        dirty = true;
    } else {
    }
    if (dirty) {
        ModuleLayer *layer = static_cast<ModuleLayer *>(object.get_module());
        // dirty current image buffer to re-evaluate the render
        layer->dirty_layer(true);
    }
}

const ImageCanvas *
IX_MODULE_CLBK::get_image(OfObject& object, const ModuleImage::Quality& quality, const GMathVec4f *region)
{
    ModuleLayer *layer = static_cast<ModuleLayer *>(object.get_module());
    LayerRedshiftModuleData *data = static_cast<LayerRedshiftModuleData *>(object.get_module_data());
    R2cSceneDelegate *scene = data->get_scene_delegate();
    if (scene->get_render_settings().is_null() || scene->get_camera().is_null()) {
        return get_null_image();
    } else {
        // FIXME: should we create an evaluator and sync inside?

        ModuleImage *parent_image = layer->get_parent_image();
        OfAttr *quality_attr = parent_image->get_object()->get_attribute("sampling_quality");
        const double iq = ModuleImage::get_quality_level(quality);
        const double sq =  quality_attr == nullptr ? 1.0 : quality_attr->get_double();
        float sampling_quality = static_cast<float>(sq * iq * iq);

        if (quality != ModuleImage::Quality::QUALITY_FULL) {
            sampling_quality = 0.0;
        }

        const int w = static_cast<int>(ceil(layer->get_width() * iq));
        const int h = static_cast<int>(ceil(layer->get_height() * iq));

        // synching the scene
        scene->sync();

        ImageHandle *cur_handle = layer->get_pyramid_source_image(quality);
        ImageCanvas *canvas = &cur_handle->get_canvas();
        canvas->clear();
        // resize the canvas to the proper resolution
        ImageCanvas::resize(*get_null_image(), *canvas, w, h);
        canvas->get_pyramid()->set_repeat_mode(ImagePixel::RESET, ImagePixel::RESET);
        layer->start_progress(quality);

        // synching is now done let's call the render
        ClarisseLayerRenderBuffer render_buffer(*layer, *canvas);
        scene->get_render_delegate()->render(&render_buffer, sampling_quality);
        const bool is_interrupted = object.get_application().must_stop_evaluation();
        canvas->finalize(is_interrupted == false);
        layer->stop_progress(cur_handle);
        return canvas;
    }
}

namespace LayerRedshift
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleLayerRedshift)
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)
        module_callbacks->cb_module_constructor = IX_MODULE_CLBK::module_constructor;
        module_callbacks->cb_on_attribute_change = IX_MODULE_CLBK::on_attribute_change;
        module_callbacks->cb_get_image = IX_MODULE_CLBK::get_image;
        module_callbacks->cb_create_module_data = IX_MODULE_CLBK::create_module_data;
        module_callbacks->cb_destroy_module_data = IX_MODULE_CLBK::destroy_module_data;
    }
}
