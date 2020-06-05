//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//


#include <dso_export.h>
#include <r2c_module_layer_scene.h>
#include <image_handle.h>

#include <r2c_scene_delegate.h>
#include <r2c_render_delegate.h>
#include <r2c_render_buffer.h>

#include "layer_r2c_scene.cma"

static ImageCanvas *
get_null_image()
{
    static ImageCanvas *null_image = new ImageCanvas(2,2,4);
    return null_image;
}

IX_BEGIN_DECLARE_MODULE_CALLBACKS(ModuleLayerR2CScene, ModuleLayerR2cSceneCallbacks)
static const ImageCanvas *get_image(OfObject& object, const ModuleImage::Quality& quality, const GMathVec4f* region);
static ModuleObject *declare_module(OfObject& object, OfObjectFactory& objects);
static bool destroy_module(OfObject& object, OfObjectFactory& objects, ModuleObject *module);
IX_END_DECLARE_MODULE_CALLBACKS(ModuleLayerR2CScene)

ModuleObject*
IX_MODULE_CLBK::declare_module(OfObject& object, OfObjectFactory& objects)
{
    ModuleLayerR2cScene *layer = new ModuleLayerR2cScene();
    layer->set_object(object);
    return layer;
}

bool
IX_MODULE_CLBK::destroy_module(OfObject& object, OfObjectFactory& objects, ModuleObject *module)
{
    delete static_cast<ModuleLayerR2cScene *>(module);
    return true;
}

const ImageCanvas *
IX_MODULE_CLBK::get_image(OfObject& object, const ModuleImage::Quality& quality, const GMathVec4f *region)
{
    ModuleLayerR2cScene *layer = static_cast<ModuleLayerR2cScene *>(object.get_module());
    R2cSceneDelegate *scene = layer->get_scene_delegate();
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

        //${CLARISSE_IX_RESOURCE_LIBRARY} synching the scene
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

namespace LayerR2cScene
{
    void on_register(OfApp& app, CoreVector<OfClass *>& new_classes)
    {
        OfClass *new_class = IX_DECLARE_MODULE_CLASS(ModuleLayerR2CScene)
        new_classes.add(new_class);

        IX_MODULE_CLBK *module_callbacks;
        IX_CREATE_MODULE_CLBK(new_class, module_callbacks)
        module_callbacks->cb_get_image = IX_MODULE_CLBK::get_image;
        module_callbacks->cb_create_module = IX_MODULE_CLBK::declare_module;
        module_callbacks->cb_destroy_module = IX_MODULE_CLBK::destroy_module;
    }
}
