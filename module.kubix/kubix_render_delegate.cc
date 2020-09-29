//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#include "./kubix_render_delegate.h"

// Clarisse includes
#include <module_scene_object.h>
#include <of_object.h>
#include <of_app.h>
#include <sys_thread_lock.h>
#include <sys_thread_task_manager.h>

// Local includes
#include "./kubix_module_renderer.h"

// private implementation
class KubixRenderDelegateImpl {
public:
    KubixCamera camera;
    // rendering progress (warning: this variable is updated in a multithreaded context)
    CoreAtomic32 progress;
    // We store this to be able to access the SysThreadTaskManager
    OfApp *app;
    
    struct {
        KubixResourceIndex index; // the index of all current resources where we store deduplicated data
    } resources;

    template<class INDEX>
    struct ClarisseToDummyObjectsMapping {
        INDEX index; // index of all render object (can be geometries, lights or instancers) which are instances pointing to a object resource
        CoreVector<R2cItemId> inserted; // is filled by KubixRenderDelegate::insert_xxx when a object is inserted to the scene
        CoreVector<R2cItemId> removed; // is filled by KubixRenderDelegate::remove_xxx when a object is removed from the scene

        bool dirty; // flag set when we receive dirtiness so that we don't need to iterate the whole index to know that it is dirty
        bool is_dirty() { return inserted.get_count() != 0 || removed.get_count() != 0 || dirty; } // return true is index is dirty
    };

    ClarisseToDummyObjectsMapping<DummyGeometryIndex> geometries;
    ClarisseToDummyObjectsMapping<KubixLightIndex> lights;
    ClarisseToDummyObjectsMapping<KubixInstancerIndex> instancers;
};

IMPLEMENT_CLASS(KubixRenderDelegate, R2cRenderDelegate);

const CoreVector<CoreString> KubixRenderDelegate::s_supported_cameras      = { "CameraAlembic", "CameraUsd", "CameraPerspective", "CameraPerspectiveAdvanced"};
const CoreVector<CoreString> KubixRenderDelegate::s_unsupported_cameras    = {};
const CoreVector<CoreString> KubixRenderDelegate::s_supported_lights       = { "KubixLight" };
const CoreVector<CoreString> KubixRenderDelegate::s_unsupported_lights     = {};
const CoreVector<CoreString> KubixRenderDelegate::s_supported_materials    = { "KubixMaterial" };
const CoreVector<CoreString> KubixRenderDelegate::s_unsupported_materials  = {};
const CoreVector<CoreString> KubixRenderDelegate::s_supported_geometries   = { "SceneObject" };
const CoreVector<CoreString> KubixRenderDelegate::s_unsupported_geometries = { "GeometryBundle" };

KubixRenderDelegate::KubixRenderDelegate(OfApp *app) : R2cRenderDelegate()
{
    m = new KubixRenderDelegateImpl;
    m->app = app;
}

KubixRenderDelegate::~KubixRenderDelegate()
{
    clear();
    delete m;
}

CoreString 
KubixRenderDelegate::get_class_name() const
{
    return "KubixRenderer";
}

void
KubixRenderDelegate::insert_light(R2cItemDescriptor item)
{
    m->lights.inserted.add(item.get_id());
}

void
KubixRenderDelegate::remove_light(R2cItemDescriptor item)
{
    KubixLightInfo *light = m->lights.index.is_key_exists(item.get_id());
    if (light != nullptr) { // make sure it is indeed in our index
        m->lights.removed.add(item.get_id());
    }
}

void
KubixRenderDelegate::dirty_light(R2cItemDescriptor item, const int& dirtiness)
{
    KubixLightInfo *light = m->lights.index.is_key_exists(item.get_id());
    if (light != nullptr) { // make sure it is indeed in our index
        m->lights.dirty = true;
        light->dirtiness |= dirtiness;
    }
}

void
KubixRenderDelegate::insert_instancer(R2cItemDescriptor item)
{
    m->instancers.inserted.add(item.get_id());
}

void
KubixRenderDelegate::remove_instancer(R2cItemDescriptor item)
{
    KubixInstancerInfo *instancer = m->instancers.index.is_key_exists(item.get_id());
    if (instancer != nullptr) { // make sure it is indeed in our index
        m->instancers.removed.add(item.get_id());
    }
}

void
KubixRenderDelegate::dirty_instancer(R2cItemDescriptor item, const int& dirtiness)
{
    KubixInstancerInfo *instancer = m->instancers.index.is_key_exists(item.get_id());
    if (instancer != nullptr) { // make sure it is indeed in our index
        m->instancers.dirty = true;
        instancer->dirtiness |= dirtiness;
    }
}

void
KubixRenderDelegate::insert_geometry(R2cItemDescriptor item)
{
    m->geometries.inserted.add(item.get_id());
}

void
KubixRenderDelegate::remove_geometry(R2cItemDescriptor item)
{
    KubixGeometryInfo *geometry = m->geometries.index.is_key_exists(item.get_id());
    if (geometry != nullptr) { // make sure it is indeed in our index
        m->geometries.removed.add(item.get_id());
    }
}

void
KubixRenderDelegate::dirty_geometry(R2cItemDescriptor item, const int& dirtiness)
{
    KubixGeometryInfo *geometry = m->geometries.index.is_key_exists(item.get_id());
    if (geometry != nullptr) { // make sure it is indeed in our index
        m->geometries.dirty = true;
        geometry->dirtiness |= dirtiness;
    }
}

float
KubixRenderDelegate::get_render_progress() const
{
    return m->progress.get_float();
}

void
KubixRenderDelegate::sync()
{
    // Called before each render
    sync_geometries();
    sync_instancers();
    sync_lights();
}

void
KubixRenderDelegate::clear()
{
    // !!! make sure to clear everything !!!
    // clearing instances
    m->geometries.index.remove_all();
    m->geometries.removed.remove_all();
    m->geometries.inserted.remove_all();
    m->geometries.dirty = R2cSceneDelegate::DIRTINESS_ALL;

    // clearing meshes
    m->resources.index.remove_all();

    // clearing instancers
    m->instancers.index.remove_all();
    m->instancers.removed.remove_all();
    m->instancers.inserted.remove_all();
    m->instancers.dirty = R2cSceneDelegate::DIRTINESS_ALL;

    // clearing lights
    m->lights.index.remove_all();
    m->lights.removed.remove_all();
    m->lights.inserted.remove_all();
    m->lights.dirty = R2cSceneDelegate::DIRTINESS_ALL;
}

void
KubixRenderDelegate::sync_camera(const unsigned int& width, const unsigned int& height)
{
    m->camera.init_ray_generator(*get_scene_delegate(), width, height);
}

void
sync_shading_groups(const R2cSceneDelegate& delegate, R2cItemId cgeometryid, KubixGeometryInfo& rgeometry)
{
    const R2cShadingGroupInfo& shading_group = delegate.get_shading_group_info(cgeometryid, 0);
    if (shading_group.get_material().is_null()) {
        rgeometry.material = nullptr;
    } else {
        rgeometry.material = static_cast<ModuleMaterialKubix *>(shading_group.get_material().get_item()->get_module());
    }
}

void
KubixRenderDelegate::_sync_geometry(R2cItemId cgeometryid, KubixGeometryInfo& rgeometry, const bool& is_new)
{
    if (rgeometry.dirtiness & R2cSceneDelegate::DIRTINESS_GEOMETRY) {
        if (!is_new) {
            // mark as removed since we will need to recreate it
            m->geometries.removed.add(cgeometryid);
            rgeometry.dirtiness = R2cSceneDelegate::DIRTINESS_NONE;
            // reinsert the removed geometry so it is added after the cleanup
            m->geometries.inserted.add(cgeometryid);
        } else {
            // it's a new geometry so let's first see if the geometry already defined a resource
            R2cGeometryResource cresource = get_scene_delegate()->get_geometry_resource(cgeometryid);
            rgeometry.resource = cresource.get_id();
            
            // Create or increment ref count of stored resource
            KubixResourceInfo *stored_resource = m->resources.index.is_key_exists(rgeometry.resource);
            if (stored_resource == nullptr) { // the resource doesn't exists so let's create it
                // create corresponding geometry resource according to the Clarisse geometry
                KubixResourceInfo new_resource;

                // Extract its bbox
                R2cItemDescriptor idesc = get_scene_delegate()->get_render_item(cgeometryid);
                ModuleSceneObject *module = static_cast<ModuleSceneObject *>(idesc.get_item()->get_module());
                new_resource.bbox = module->get_bbox();

                new_resource.refcount = 1;
                // adding the new resource
                m->resources.index.add(cresource.get_id(), new_resource);
            } else {
                stored_resource->refcount++;
            }

            // since that was a new geometry we will need to set the matrix, materials and visibility flags
            rgeometry.dirtiness =   R2cSceneDelegate::DIRTINESS_KINEMATIC |
                                    R2cSceneDelegate::DIRTINESS_SHADING_GROUP |
                                    R2cSceneDelegate::DIRTINESS_VISIBILITY;
        }
    }

    if (rgeometry.dirtiness & R2cSceneDelegate::DIRTINESS_KINEMATIC) {
        rgeometry.transform = get_scene_delegate()->get_transform(cgeometryid);
    }
    if (rgeometry.dirtiness & R2cSceneDelegate::DIRTINESS_SHADING_GROUP) {
        sync_shading_groups(*get_scene_delegate(), cgeometryid, rgeometry);
    }
    if (rgeometry.dirtiness & R2cSceneDelegate::DIRTINESS_VISIBILITY) {
        rgeometry.visibility = get_scene_delegate()->get_visible(cgeometryid);
    }

    // setting the dirtiness back to none since the geometry is fully synched
    rgeometry.dirtiness = R2cSceneDelegate::DIRTINESS_NONE;
}

void
KubixRenderDelegate::sync_geometries()
{
    if (m->geometries.is_dirty()) {
        // iterating through geometries to see if we need to sync any of them
        // it's VERY IMPORTANT to do this before everything else since if any
        // items received DIRTINESS_GEOMETRY, we need to remove it from the
        // scene to rebuild it!!!
        for (auto geometry : m->geometries.index) {
            if (geometry.get_value().dirtiness != R2cSceneDelegate::DIRTINESS_NONE) {
                sync_geometry(geometry.get_key(), geometry.get_value());
            }
        }
        // let's see if we have to remove geometries from the scene
        for (auto removed_item : m->geometries.removed) {
            KubixGeometryInfo *geometry = m->geometries.index.is_key_exists(removed_item);
            // check the current geometry exists in the scene
            if (geometry != nullptr) {
                // doing proper cleanup. Let's cleanup the resource
                // get the resource if it exists
                KubixResourceInfo *stored_resource = m->resources.index.is_key_exists(geometry->resource);
                if (stored_resource != nullptr) { // there's a resource bound to the current geometry
                    stored_resource->refcount--;
                    if (stored_resource->refcount == 0) { // no one is using that resource anymore so let's delete it
                        m->resources.index.remove(geometry->resource);
                    }
                }
                m->geometries.index.remove(removed_item);
                if (m->geometries.index.get_count() == 0) break; // finished
            }
        }
        // since we processed all pending removed geometries we have to clear our array
        m->geometries.removed.remove_all();

        // let's see if new geometries have been added. Interestingly it's possible
        // that sync_geometry remove and add items if the topology changes. This
        // is why it is very important to first sync the index, remove and finally add.
        // let's see if we have to create new geometries
        KubixGeometryInfo geometry;
        CoreVector<KubixGeometryInfo> new_geometries(0, m->geometries.inserted.get_count());
        for (auto inserted_item : m->geometries.inserted) {
            // initializing the new geometry
            geometry.dirtiness = R2cSceneDelegate::DIRTINESS_ALL;
            // synching the new geometry
            sync_new_geometry(inserted_item, geometry);
            // adding it to our geometry index
            m->geometries.index.add(inserted_item, geometry);
            new_geometries.add(geometry);
        }
        // since we processed all pending inserted geometries we have to clear the array
        m->geometries.inserted.remove_all();
    }
    // our geometries are now perfectly synched
    m->geometries.dirty = false;
}

/*! \brief shading group synchronization helper */
void
sync_shading_groups(const R2cSceneDelegate& delegate, R2cItemId cinstancerid, KubixInstancerInfo& rinstancer)
{
    // The material is stored statically inside the instance
    const R2cShadingGroupInfo& shading_group = delegate.get_shading_group_info(cinstancerid, 0);
    if (shading_group.get_material().is_null()) {
        // When the material is null, we draw the object with a default color (1.0, 0.0, 1.0)
        // We could also asign a default material
        rinstancer.material = nullptr;
    } else {
        rinstancer.material = static_cast<ModuleMaterialKubix *>(shading_group.get_material().get_item()->get_module());
    }
}

void
KubixRenderDelegate::_sync_instancer(R2cItemId cinstancerid, KubixInstancerInfo& rinstancer, const bool& is_new)
{
    // WARNING: this method is very similar (if not identical) to sync_geometry
    // This is only to show the intended use of R2C
    if (rinstancer.dirtiness & R2cSceneDelegate::DIRTINESS_GEOMETRY) {
        if (!is_new) { // existing instancer
            // mark as removed since we will need to recreate it
            m->instancers.removed.add(cinstancerid);
            // reinsert the removed instancer so it is added after the cleanup
            m->instancers.inserted.add(cinstancerid);
            // mark it as clean since we will rebuild it anyway
            rinstancer.dirtiness = R2cSceneDelegate::DIRTINESS_NONE;
        } else {
            // it's a new instancer so let's first see if the instancer already defined a resource
            R2cGeometryResource cresource = get_scene_delegate()->get_geometry_resource(cinstancerid);
            rinstancer.resource = cresource.get_id();

            KubixResourceInfo *stored_resource = m->resources.index.is_key_exists(rinstancer.resource);
            if (stored_resource == nullptr) { // the resource doesn't exists so let's create it
                // create corresponding geometry resource according to the Clarisse geometry
                KubixResourceInfo new_resource;

                // Extract its bbox
                R2cItemDescriptor idesc = get_scene_delegate()->get_render_item(cinstancerid);
                ModuleSceneObject *module = static_cast<ModuleSceneObject *>(idesc.get_item()->get_module());
                new_resource.bbox = module->get_bbox();

                new_resource.refcount = 1;
                // adding the new resource
                m->resources.index.add(cresource.get_id(), new_resource);
            } else {
                stored_resource->refcount++;
            }
            // since that was a new geometry we will need to set the matrix, materials and visibility flags
            rinstancer.dirtiness = R2cSceneDelegate::DIRTINESS_KINEMATIC |
                                    R2cSceneDelegate::DIRTINESS_SHADING_GROUP |
                                    R2cSceneDelegate::DIRTINESS_VISIBILITY;
        }
    }

    if (rinstancer.dirtiness & R2cSceneDelegate::DIRTINESS_KINEMATIC) {
        rinstancer.transform = get_scene_delegate()->get_transform(cinstancerid);
    }
    if (rinstancer.dirtiness & R2cSceneDelegate::DIRTINESS_SHADING_GROUP) {
        sync_shading_groups(*get_scene_delegate(), cinstancerid, rinstancer);
    }
    if (rinstancer.dirtiness & R2cSceneDelegate::DIRTINESS_VISIBILITY) {
        rinstancer.visibility = get_scene_delegate()->get_visible(cinstancerid);
    }
    // setting the dirtiness back to none since the instancer is synched
    rinstancer.dirtiness = R2cSceneDelegate::DIRTINESS_NONE;
}

void
KubixRenderDelegate::sync_instancers()
{
    if (m->instancers.is_dirty()) {
        // iterating through instancers to see if we need to sync any of them
        // it's VERY IMPORTANT to do this before everything else since if any
        // items received DIRTINESS_GEOMETRY, we need to remove it from the
        // scene to rebuild it!!!
        for (auto instancer : m->instancers.index) {
            if (instancer.get_value().dirtiness != R2cSceneDelegate::DIRTINESS_NONE) {
                sync_instancer(instancer.get_key(), instancer.get_value());
            }
        }

        // let's see if we have to remove instancers from the scene
        for (auto removed_item : m->instancers.removed) {
            KubixInstancerInfo *instancer = m->instancers.index.is_key_exists(removed_item);
            // check the current instancer exists in the scene
            if (instancer != nullptr) {
                // now doing proper cleanup. Let's cleanup resources
                // get the resources if they exist
                KubixResourceInfo *stored_resource = m->resources.index.is_key_exists(instancer->resource);
                if (stored_resource != nullptr) { // there's a resource bound to the current geometry
                    stored_resource->refcount--;
                    if (stored_resource->refcount == 0) { // no one is using that resource anymore so let's delete it
                        m->resources.index.remove(instancer->resource);
                    }
                }
                m->instancers.index.remove(removed_item);
                if (m->instancers.index.get_count() == 0) break; // finished
            }
        }
        // since we processed all pending removed instancers we have to clear our array
        m->instancers.removed.remove_all();

        // let's see if new instancers have been added. Interestingly it's possible
        // that sync_instancers remove and add items if the topology changes. This
        // is why it is very important to first sync the index, remove and finally add.
        // let's see if we have to create new geometries
        KubixInstancerInfo instancer;
        CoreVector<KubixInstancerInfo> new_instancers(0, m->instancers.inserted.get_count());
        for (auto inserted_item : m->instancers.inserted) {
            // since we create them we need to make them as fully dirty
            instancer.dirtiness = R2cSceneDelegate::DIRTINESS_ALL;
            // synching the new instancer
            sync_new_instancer(inserted_item, instancer);
            // adding it to our instancer index
            m->instancers.index.add(inserted_item, instancer);
            new_instancers.add(instancer);
        }
        // since we processed all pending inserted instancers we have to clear the array
        m->instancers.inserted.remove_all();
    }
    // our instancers are now perfectly synched
    m->instancers.dirty = false;
}

/*! \brief light synchronization helper */
void
sync_light(const R2cSceneDelegate& delegate, R2cItemId clightid, KubixLightInfo& rlight)
{
    // Our lights only have a color, so there's nothing to do here...
    rlight.dirtiness = R2cSceneDelegate::DIRTINESS_NONE;
}

void
KubixRenderDelegate::sync_lights()
{
    if (m->lights.is_dirty()) {
        // remove lights first
        for (auto removed_item : m->lights.removed) {
            KubixLightInfo *light = m->lights.index.is_key_exists(removed_item);
            // check the current light exists in the scene
            if (light != nullptr) {
                m->lights.index.remove(removed_item);
                if (m->lights.index.get_count() == 0) break; // finished
            }
        }
        m->lights.removed.remove_all();

        LOG_INFO_VAR(m->lights.inserted.get_count());
        // creating new lights
        KubixLightInfo light;
        CoreVector<KubixLightInfo> new_lights(0, m->lights.inserted.get_count());
        for (auto inserted_item : m->lights.inserted) {
            // create corresponding light according to the Clarisse light
            KubixUtils::create_light(*get_scene_delegate(), inserted_item, light);
            light.dirtiness = R2cSceneDelegate::DIRTINESS_ALL;
            // and sync it to its attributes
            // synching the new light
            sync_light(*get_scene_delegate(), inserted_item, light);
            // adding it to our light index
            m->lights.index.add(inserted_item, light);
            new_lights.add(light);
        }
        m->lights.inserted.remove_all();

        // iterating through lights to see if we need to sync any of them
        for (auto light : m->lights.index) {
            if (light.get_value().dirtiness != R2cSceneDelegate::DIRTINESS_NONE) {
                sync_light(*get_scene_delegate(), light.get_key(), light.get_value());
            }
        }
    }
    // our lights are now perfectly synched
    m->lights.dirty = false;
}

void
KubixRenderDelegate::get_supported_cameras(CoreVector<CoreString>& supported_cameras, CoreVector<CoreString>& unsupported_cameras) const
{
	supported_cameras = s_supported_cameras;
	unsupported_cameras = s_unsupported_cameras;
}

void
KubixRenderDelegate::get_supported_materials(CoreVector<CoreString>& supported_materials, CoreVector<CoreString>& unsupported_materials) const
{
	supported_materials = s_supported_materials;
	unsupported_materials = s_unsupported_materials;
}

void
KubixRenderDelegate::get_supported_lights(CoreVector<CoreString>& supported_lights, CoreVector<CoreString>& unsupported_lights) const
{
	supported_lights = s_supported_lights;
	unsupported_lights = s_unsupported_lights;
}

void
KubixRenderDelegate::get_supported_geometries(CoreVector<CoreString>& supported_geometries, CoreVector<CoreString>& unsupported_geometries) const
{
	supported_geometries = s_supported_geometries;
	unsupported_geometries = s_unsupported_geometries;
}

// Multithread task to render a region of the image
class RenderRegionTask : public SysThreadTask {
public :
    RenderRegionTask(): progress(nullptr) {}

    virtual void execution_entry(const unsigned int& id) {
        dummy_render_delegate->render_region(data, id);
        if (progress)
            progress->add_float(progress_increment);
    }
    KubixRenderDelegate::RenderData data;
    const KubixRenderDelegate *dummy_render_delegate;

    // To show the overall render progress
    CoreAtomic32 *progress;
    float progress_increment;
};

void
KubixRenderDelegate::render(R2cRenderBuffer *render_buffer, const float& sampling_quality)
{
    // Reset the rendering progress
    m->progress.set_float(0.0f);

    // Extract the image dimensions and synchronize our internal scene representation
    R2cRenderBuffer::Region render_region = render_buffer->get_render_region();
    const unsigned int total_width  = render_buffer->get_width();
    const unsigned int total_height = render_buffer->get_height();

    sync_camera(total_width, total_height);
    sync();

    // Browse all the light in the scene and compute the light contribution (very simple lighting)
    GMathVec3f light_contribution = GMathVec3f(0.0f, 0.0f, 0.0f);
    for (auto light : m->lights.index) {
        const LightData& light_data = light.get_value().light_data;
        light_contribution += light_data.light_module->evaluate();
    }

    // Get the background color from the renderer, to demonstrate render settings usage
    R2cItemDescriptor renderer = get_scene_delegate()->get_render_settings();
    ModuleRendererKubix *settings = static_cast<ModuleRendererKubix *>(renderer.get_item()->get_module());
    GMathVec3f background_color = settings->get_background_color();
    
    /************************ Create render tasks ************************/
    const unsigned int task_w = gmath_min(64u, render_region.width);
    const unsigned int task_h = gmath_min(64u, render_region.height);
    const unsigned int bucket_count_x = (unsigned int)gmath_ceil((float)render_region.width / task_w);
    const unsigned int bucket_count_y = (unsigned int)gmath_ceil((float)render_region.height / task_h);
    const unsigned int task_count = bucket_count_x * bucket_count_y;
    const float progress_increment = 1.0f / task_count;

    // To use Clarisse's multi threading capabilities, we create a list of tasks
    // and feed them to the task manager
    // Our tasks only consists of a set of data, and a execution_entry() method.
    SysThreadTaskManager task_manager(&m->app->get_thread_manager());
    CoreVector<RenderRegionTask> tasks(task_count);

    // This should be created the least amount of times (when the image size is updated for example)
    float* image_buffer = new float[render_region.width * render_region.height * 4];
    float *next_buffer_entry = image_buffer;

    unsigned int task_id = 0;
    for(unsigned int j = 0; j < bucket_count_y; ++j) {
        unsigned int offset_y = render_region.offset_y + j * task_h;
        unsigned int bucket_height = gmath_min(task_h, render_region.offset_y + render_region.height - offset_y);
        for(unsigned int i = 0; i < bucket_count_x; ++i) {
            unsigned int offset_x = render_region.offset_x + i * task_w;
            unsigned int bucket_width = gmath_min(task_w, render_region.offset_x + render_region.width - offset_x);

            // Fill task data
            tasks[task_id].data.width                 = total_width;
            tasks[task_id].data.height                = total_height;
            tasks[task_id].data.region                = R2cRenderBuffer::Region(offset_x, offset_y, bucket_width, bucket_height);
            tasks[task_id].data.light_contribution    = light_contribution;
            tasks[task_id].data.background_color      = background_color;
            tasks[task_id].data.render_buffer         = render_buffer;
            tasks[task_id].data.buffer_ptr            = next_buffer_entry;

            tasks[task_id].dummy_render_delegate = this;
            tasks[task_id].progress             = &m->progress;
            tasks[task_id].progress_increment   = progress_increment;

            // Give it to the task manager
            task_manager.add_task(tasks[task_id], false);
            next_buffer_entry += bucket_width * bucket_height * 4;
            ++task_id;
        }
    }
    // Join threads
    task_manager.wait_until_completed();
    render_buffer->finalize();
    delete[] image_buffer;
}

template<class OBJECT_INFO>
void raytrace_objects(const GMathRay& ray, const CoreHashTable<R2cItemId, OBJECT_INFO>& index, const KubixResourceIndex& resources_index, double &closest_hit_t, GMathVec3d &closest_hit_normal, MaterialData& closest_hit_material)
{
    // Very simple linear raytracer, only supporting bboxes
    for (const auto object: index)
    {
        const OBJECT_INFO& object_info = object.get_value();
        const KubixResourceInfo *resource_info = resources_index.is_key_exists(object_info.resource);

        // Transform ray to object space
        GMathMatrix4x4d inverse_transform;
        GMathRay transformed_ray;
        GMathMatrix4x4d::get_inverse(object_info.transform, inverse_transform);
        transformed_ray.transform(ray, inverse_transform);

        // Test intersection with bbox
        double tmin, tmax;
        GMathVec3d normal;
        if (resource_info->bbox.intersect(transformed_ray, tmin, tmax, normal))
        {
            // If hit closer than closest, record hit infos
            if (tmin < closest_hit_t)
            {
                GMathMatrix4x4d inverse_transpose_transform;
                GMathMatrix4x4d::transpose(inverse_transform, inverse_transpose_transform);
                GMathVec3d transformed_normal;
                GMathMatrix4x4d::multiply(transformed_normal, normal, inverse_transpose_transform);

                closest_hit_t = tmin;
                closest_hit_normal = transformed_normal;
                closest_hit_material = object_info.material;
            }
        }
    }
}

void
KubixRenderDelegate::render_region(RenderData& render_data, const unsigned int& thread_id) const
{
    // Used to display a green box around the rendered region
    render_data.render_buffer->notify_start_render_region(render_data.region, true, thread_id);

    // Browse our image and for each pixel we compute a ray and raytrace the scene
    for (unsigned int pixel_y = 0; pixel_y < render_data.region.height; ++pixel_y) {
        for (unsigned int pixel_x = 0; pixel_x < render_data.region.width; ++pixel_x) {
            // Compute ray for the pixel [X, Y]
            GMathRay ray = m->camera.generate_ray(render_data.width,
                                                  render_data.height,
                                                  pixel_x + render_data.region.offset_x,
                                                  pixel_y + render_data.region.offset_y);
            
            GMathVec3f final_color = render_data.background_color;

            // Use this ray to raytrace the scene
            // If we hit something we take the color from the intersected material BBox and multiply it per all the light contrinutions
            // If nothing is hit we return the background renderer color
            double closest_hit_t = gmath_infinity;
            GMathVec3d closest_hit_normal;
            MaterialData closest_hit_material;

            // For simplicity, we handle instancers and geometries the same way
            raytrace_objects(ray, m->geometries.index, m->resources.index, closest_hit_t, closest_hit_normal, closest_hit_material);
            raytrace_objects(ray, m->instancers.index, m->resources.index, closest_hit_t, closest_hit_normal, closest_hit_material);

            if (closest_hit_t != gmath_infinity)
            {
                // If the object doesn't have an assigned material, use default color
                if (closest_hit_material.material_module)
                    final_color = closest_hit_material.material_module->shade(GMathVec3f(ray.get_direction()), GMathVec3f(closest_hit_normal)) * render_data.light_contribution;
                else
                    final_color = GMathVec3f(1.0f, 0.0f, 1.0f) * render_data.light_contribution;
            }

            const unsigned int pixel_index = (pixel_y * render_data.region.width + pixel_x) * 4;
            render_data.buffer_ptr[pixel_index + 0] = final_color[0];
            render_data.buffer_ptr[pixel_index + 1] = final_color[1];
            render_data.buffer_ptr[pixel_index + 2] = final_color[2];
            render_data.buffer_ptr[pixel_index + 3] = 1.0f;
        }
    }
    // Write the new buffer to the image
    render_data.render_buffer->fill_rgba_region(render_data.buffer_ptr, render_data.region.width, render_data.region, true);
}