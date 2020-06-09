#include <r2c_module_layer_scene.h>
#include <r2c_scene_delegate.h>
#include <r2c_render_delegate.h>
#include <module_group.h>
#include <of_app.h>

IMPLEMENT_CLASS(ModuleLayerR2cScene, ModuleLayerScene)

//!
//! Constructor
//!
ModuleLayerR2cScene::ModuleLayerR2cScene() : m_scene_delegate(nullptr), m_render_delegate(nullptr)
{

}

//!
//! Destructor
//!
ModuleLayerR2cScene::~ModuleLayerR2cScene()
{
    delete m_render_delegate;
    if (m_scene_delegate != nullptr) {
        R2cSceneDelegate::destroy(m_scene_delegate);
    }
}

ModuleLayerR2cSceneCallbacks::ModuleLayerR2cSceneCallbacks()
{
    cb_get_render_delegate = nullptr;
}

void
ModuleLayerR2cScene::module_constructor(OfObject& object)
{
    ModuleLayerScene::module_constructor(object);

    // must initialize first R2cSceneDelegate
    if (!R2cSceneDelegate::is_initialized()) {
        R2cSceneDelegate::initialize(object.get_application());
    }

    m_scene_delegate = R2cSceneDelegate::create();

    // Get the Render Delegate to attach to the Scene Delegate
    if (get_callbacks<ModuleLayerR2cSceneCallbacks>()->cb_get_render_delegate != nullptr) {
        m_render_delegate = get_callbacks<ModuleLayerR2cSceneCallbacks>()->cb_get_render_delegate(object);

        const CoreString renderer_class_name = m_render_delegate->get_class_name();
        OfClass *renderer_class = get_application().get_factory().get_classes().get(renderer_class_name);

        if (renderer_class != nullptr && renderer_class->is_kindof("Renderer")) {
            // Specify that the attribute "renderer" supports only the specified renderer class type
            OfAttr *renderer_attr = object.get_attribute("renderer");
            CoreArray<CoreString> filter = { renderer_class_name };
            renderer_attr->set_object_filters(filter);
        } else {
            LOG_WARNING("ModuleLayerR2cScene.module_constructor: The method \'" << m_render_delegate->get_class_info_name() << "::get_class_name()\'"
                        "must return the Clarisse class name of the supported renderer, which have to inherit from the class Renderer.");
        }
    }

    // Attach the Render Delegate to the Scene Delegate
    m_scene_delegate->set_render_delegate(m_render_delegate);
}

void
ModuleLayerR2cScene::on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags)
{
    ModuleLayerScene::on_attribute_change(attr, dirtiness, dirtiness_flags);

    bool dirty = false;
    const CoreString& aname = attr.get_name();

    if (aname == "active_camera") {
        m_scene_delegate->set_camera(attr.get_object());
        dirty = true;
    } else if (aname == "renderer") {
        m_scene_delegate->set_render_settings(attr.get_object());
        dirty = true;
    } else if (aname == "geometries") {
        m_scene_delegate->set_geometries(get_scene_object_group()->get_object());
        dirty = true;
    } else if (aname == "lights") {
        m_scene_delegate->set_lights(get_light_group()->get_object());
        dirty = true;
    } else if (aname == "__geometries__") {
        // embedded group referencing all the scene objects inside the current context when no group is connected into the attribute "geometries"
        dirty = true;
    } else if (aname == "__lights__") {
        // embedded group referencing all the lights inside the current context when no group is connected into the attribute "lights"
        dirty = true;
    } else if (aname == "shading_layer") {
        OfObject *shading_layer = attr.get_object();
        OfObject *current_shading_layer = m_scene_delegate->get_shading_layer().get_item();
        if (attr.get_event_info().type != OfAttrEvent::TYPE_PROPAGATE && shading_layer != current_shading_layer) {
            m_scene_delegate->set_shading_layer(shading_layer);
        }
    }
    if (dirty) {
        // dirty current image buffer to re-evaluate the render
        dirty_layer(true);
    }
}