//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <core_log.h>
#include <sys_thread_lock.h>
#include <of_object.h>
#include <of_app.h>

#include <app_progress_bar.h>
#include <image_canvas.h>
#include <image_map_channel.h>
#include <module_layer.h>
#include <module_camera.h>
#include <module_particle.h>

#include <r2c_scene_delegate.h>
#include <r2c_instancer.h>

#include <sys_thread_task_manager.h>

#include <module_material_dummy.h>
#include "module_renderer_dummy.h"

#include <module_texture_dummy.h>
#include "dummy_utils.h"

#include "dummy_render_delegate.h"

// private implementation
class BboxDelegateImpl {
public:
    BboxCamera camera; // Bbox render camera
    
    struct {
        BBResourceIndex index; // the index of all current resources where we store deduplicated data
    } resources;

    struct {
        BBGeometryIndex index; // index of all render geometries which are mesh instances pointing to a geometry resource
        CoreVector<R2cItemId> inserted; // is filled by BboxRenderDelegate::insert_geometry when a geometry is inserted to the scene
        CoreVector<R2cItemId> removed; // is filled by BboxRenderDelegate::remove_geometry when a geometry is removed from the scene

        bool dirty; // flag set when we receive dirtiness so that we don't need to iterate the whole index to know that it is dirty
        bool is_dirty() { return inserted.get_count() != 0 || removed.get_count() != 0 || dirty; } // return true is index is dirty

    } geometries;

    struct {
        BboxLightIndex index; // index of all render lights
        CoreVector<R2cItemId> inserted; // is filled by BboxRenderDelegate::insert_light when a light is inserted to the scene
        CoreVector<R2cItemId> removed; // is filled by BboxRenderDelegate::remove_light when a light is removed from the scene
        bool dirty; // flag set when we receive dirtiness so that we don't need to iterate the whole index to know that it is dirty

        bool is_dirty() { return inserted.get_count() != 0 || removed.get_count() != 0 || dirty; } // return true is index is dirty

    } lights;
    
    struct {
        BBInstancerIndex index; // index of all render instancers
        CoreVector<R2cItemId> inserted; // is filled by BboxRenderDelegate::insert_instancer when an instancer is inserted to the scene
        CoreVector<R2cItemId> removed; // is filled by BboxRenderDelegate::remove_instancer when an instancer is removed from the scene
        bool dirty; // flag set when we receive dirtiness so that we don't need to iterate the whole index to know that it is dirty

        bool is_dirty() { return inserted.get_count() != 0 || removed.get_count() != 0 || dirty; } // return true is index is dirty

    } instancers;
};

IMPLEMENT_CLASS(BboxRenderDelegate, R2cRenderDelegate);

const CoreVector<CoreString> BboxRenderDelegate::s_supported_cameras      = { "CameraAlembic", "CameraUsd", "CameraPerspective", "CameraPerspectiveAdvanced"};
const CoreVector<CoreString> BboxRenderDelegate::s_unsupported_cameras    = {};
const CoreVector<CoreString> BboxRenderDelegate::s_supported_lights       = { "LightDummy" };
const CoreVector<CoreString> BboxRenderDelegate::s_unsupported_lights     = {};
const CoreVector<CoreString> BboxRenderDelegate::s_supported_materials    = { "MaterialDummy" };
const CoreVector<CoreString> BboxRenderDelegate::s_unsupported_materials  = {};
const CoreVector<CoreString> BboxRenderDelegate::s_supported_geometries   = { "SceneObject" };
const CoreVector<CoreString> BboxRenderDelegate::s_unsupported_geometries = { "GeometryVolume", "GeometryFur", "GeometryBundle", "GeometrySphere", "GeometryCylinder", "GeometryPointArray" };

BboxRenderDelegate::BboxRenderDelegate(OfApp *app) : R2cRenderDelegate()
{
    m = new BboxDelegateImpl;
    m_app = app;
}

BboxRenderDelegate::~BboxRenderDelegate()
{
    clear();
    delete m;
}

CoreString 
BboxRenderDelegate::get_class_name() const
{
    return "RendererDummy";
}

bool
BboxRenderDelegate::sync_render_settings(const float& sampling_quality)
{
    if (!get_scene_delegate()->get_render_settings().is_null()) {
        R2cItemDescriptor renderer = get_scene_delegate()->get_render_settings();
        if (!renderer.is_null() && renderer.get_item()->get_module()->is_kindof(ModuleRendererDummy::class_info())) {
            ModuleRendererDummy *settings = static_cast<ModuleRendererDummy *>(renderer.get_item()->get_module());
            settings->sync(sampling_quality);
            return true;
        }
    }
    return false;
}

void
BboxRenderDelegate::insert_light(R2cItemDescriptor item)
{
    m->lights.inserted.add(item.get_id());
}

void
BboxRenderDelegate::remove_light(R2cItemDescriptor item)
{
    BboxLightInfo *light = m->lights.index.is_key_exists(item.get_id());
    if (light != nullptr) { // make sure it is indeed in our index
        m->lights.removed.add(item.get_id());
    }
}

void
BboxRenderDelegate::dirty_light(R2cItemDescriptor item, const int& dirtiness)
{
    BboxLightInfo *light = m->lights.index.is_key_exists(item.get_id());
    if (light != nullptr) { // make sure it is indeed in our index
        m->lights.dirty = true;
        light->dirtiness |= dirtiness;
    }
}

void
BboxRenderDelegate::insert_instancer(R2cItemDescriptor item)
{
    m->instancers.inserted.add(item.get_id());
}

void
BboxRenderDelegate::remove_instancer(R2cItemDescriptor item)
{
    BboxInstancerInfo *instancer = m->instancers.index.is_key_exists(item.get_id());
    if (instancer != nullptr) { // make sure it is indeed in our index
        m->instancers.removed.add(item.get_id());
    }
}

void
BboxRenderDelegate::dirty_instancer(R2cItemDescriptor item, const int& dirtiness)
{
    BboxInstancerInfo *instancer = m->instancers.index.is_key_exists(item.get_id());
    if (instancer != nullptr) { // make sure it is indeed in our index
        m->instancers.dirty = true;
        instancer->dirtiness |= dirtiness;
    }
}

void
BboxRenderDelegate::insert_geometry(R2cItemDescriptor item)
{
    m->geometries.inserted.add(item.get_id());
}

void
BboxRenderDelegate::remove_geometry(R2cItemDescriptor item)
{
    BboxGeometryInfo *geometry = m->geometries.index.is_key_exists(item.get_id());
    if (geometry != nullptr) { // make sure it is indeed in our index
        m->geometries.removed.add(item.get_id());
    }
}

void
BboxRenderDelegate::dirty_geometry(R2cItemDescriptor item, const int& dirtiness)
{
    BboxGeometryInfo *geometry = m->geometries.index.is_key_exists(item.get_id());
    if (geometry != nullptr) { // make sure it is indeed in our index
        m->geometries.dirty = true;
        geometry->dirtiness |= dirtiness;
    }
}

class RenderRegionTask : public SysThreadTask {
public :
    RenderRegionTask(): reg(0,0,0,0) {}

    virtual void execution_entry(const unsigned int& id) {
        dummy_render_delegate->render_region(buffer_ptr, render_buffer, total_width, total_height, reg, light_contribution, background_color);
    }

    unsigned int total_width;
    unsigned int total_height;
    R2cRenderBuffer::Region reg;
    GMathVec3f light_contribution;
    GMathVec3f background_color;
    R2cRenderBuffer *render_buffer;
    const BboxRenderDelegate *dummy_render_delegate;
    float* buffer_ptr;
};


void
BboxRenderDelegate::render(R2cRenderBuffer *render_buffer, const float& sampling_quality)
{
    const unsigned int width = render_buffer->get_width();
    const unsigned int height = render_buffer->get_height();

    sync_camera(width, height);
    sync();

    // Browse all the light in the scene and compute the light contribution
    GMathVec3f light_contribution = GMathVec3f(0.0f, 0.0f, 0.0f);
    for (auto light : m->lights.index) {
        const LightData& light_data = light.get_value().light_data;
        light_contribution += light_data.light_module->evaluate();
    }

    // Get the background color from the renderer
    R2cItemDescriptor renderer = get_scene_delegate()->get_render_settings();
    ModuleRendererDummy *settings = static_cast<ModuleRendererDummy *>(renderer.get_item()->get_module());
    GMathVec3f background_color = settings->get_background_color();
    
    const unsigned int task_w = gmath_min(32u, width);
    const unsigned int task_h = gmath_min(32u, height);
    const unsigned int bucket_count_x = (unsigned int)gmath_ceil((float)width / task_w);
    const unsigned int bucket_count_y = (unsigned int)gmath_ceil((float)height / task_h);
    const unsigned int task_count = bucket_count_x * bucket_count_y;

    SysThreadTaskManager task_manager(&m_app->get_thread_manager());
    CoreVector<RenderRegionTask> tasks(task_count);

    unsigned int task_id = 0;
    float* image_buffer = new float[width * height * 4];
    float *next_buffer_entry = image_buffer;
    for(unsigned int j = 0; j < bucket_count_y; ++j) {
        unsigned int offset_y = j * task_h;
        unsigned int bucket_height = gmath_min(task_h, height - offset_y);
        for(unsigned int i = 0; i < bucket_count_x; ++i) {
            unsigned int offset_x = i * task_w;
            unsigned int bucket_width = gmath_min(task_w, width - offset_x);

            tasks[task_id].total_width           = width;
            tasks[task_id].total_height          = height;
            tasks[task_id].reg                   = R2cRenderBuffer::Region(offset_x, offset_y, bucket_width, bucket_height);
            tasks[task_id].light_contribution    = light_contribution;
            tasks[task_id].background_color      = background_color;
            tasks[task_id].render_buffer         = render_buffer;
            tasks[task_id].dummy_render_delegate = this;
            tasks[task_id].buffer_ptr            = next_buffer_entry;
            task_manager.add_task(tasks[task_id], false);
            ++task_id;
            next_buffer_entry += bucket_width * bucket_height * 4;
            LOG_INFO_VAR4(offset_x, offset_y, bucket_width, bucket_height);
        }
    }
    task_manager.wait_until_completed();
    render_buffer->finalize();
    delete[] image_buffer;
}

void
BboxRenderDelegate::render_region(float* result_buffer, R2cRenderBuffer *render_buffer, unsigned int width, unsigned int height, const R2cRenderBuffer::Region& reg, const GMathVec3f& light_contribution, const GMathVec3f& background_color) const
{
    render_buffer->notify_start_render_region(reg, true);
    // Browse our image and for each pixel we compute a ray and raytrace the scene
    for (unsigned int pixel_y = 0; pixel_y < reg.height; ++pixel_y) {
        for (unsigned int pixel_x = 0; pixel_x < reg.width; ++pixel_x) {
            // Compute ray for the pixel [X, Y]
            GMathRay ray = m->camera.generate_ray(width, height, pixel_x + reg.offset_x, pixel_y + reg.offset_y);
            
            GMathVec3f final_color = background_color;

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
                if (closest_hit_material.material_module)
                    final_color = closest_hit_material.material_module->shade(GMathVec3f(ray.get_direction()), GMathVec3f(closest_hit_normal)) * light_contribution;
                else
                    final_color = GMathVec3f(1.0f, 0.0f, 1.0f) * light_contribution;
            }

            // Pixel index
            const unsigned int pixel_index = (pixel_y * reg.width + pixel_x) * 4;
            // const unsigned int pixel_index = ((reg.offset_y + pixel_y) * width + reg.offset_x + pixel_x) * 4;
            // Set the color for the pixel [X,Y]
            result_buffer[pixel_index + 0] = final_color[0];
            result_buffer[pixel_index + 1] = final_color[1];
            result_buffer[pixel_index + 2] = final_color[2];
            result_buffer[pixel_index + 3] = 1.0f;
        }
    }
    // Write the new buffer to the image
    render_buffer->fill_rgba_region(result_buffer, reg.width, reg, true);
}

float
BboxRenderDelegate::get_render_progress() const
{
    return 1.0f;
}

void
BboxRenderDelegate::sync()
{
    sync_geometries();
    sync_instancers();
    sync_lights();
}

void
BboxRenderDelegate::clear()
{
//    // !!! make sure to clear everything !!!
//    if (m->scene != nullptr) {
//        // clearing instances
//        for (auto geometry : m->geometries.index) {
//            const BboxGeometryInfo& geo = geometry.get_value();
//            BB_InstanceMaterialOverrides_Delete(geo.materials);
//            BB_MeshInstance_Delete(geo.ptr);
//        }
//        m->geometries.index.remove_all();
//        m->geometries.removed.remove_all();
//        m->geometries.inserted.remove_all();
//        m->geometries.dirty = R2cSceneDelegate::DIRTINESS_ALL;
//        m->scene->ClearInstanceMaterialOverrides();
//        m->scene->ClearMeshInstances();

//        // clearing meshes
//        for (auto resource: m->resources.index) BB_MeshBase_Delete(resource.get_value().ptr);
//        m->scene->ClearMeshes();
//        m->resources.index.remove_all();

//        // clearing point clouds
//        for (auto instancer: m->instancers.index) {
//            for (auto point_cloud : instancer.get_value().ptrs) {
//                BB_PointCloud_Delete(point_cloud);
//            }
//        }
//        m->scene->ClearMeshPointClouds(); // not really necessary since we called ClearMeshes()
//        m->instancers.index.remove_all();
//        m->instancers.removed.remove_all();
//        m->instancers.inserted.remove_all();
//        m->instancers.dirty = R2cSceneDelegate::DIRTINESS_ALL;

//        // clearing lights
//        for (auto light : m->lights.index) BB_Light_Delete(light.get_value().ptr);
//        m->lights.index.remove_all();
//        m->lights.removed.remove_all();
//        m->lights.inserted.remove_all();
//        m->lights.dirty = R2cSceneDelegate::DIRTINESS_ALL;
//        m->scene->ClearLights();

//        BB_Scene_Delete(m->scene);
//        m->scene = nullptr;
//    }
//    if (m->camera != nullptr) {
//        BB_Camera_Delete(m->camera);
//        m->camera = nullptr;
//    }

//    delete m->sink;
//    m->sink = nullptr;

//    delete m->abort_checker;
//    m->abort_checker = nullptr;

//    delete m->progress;
//    m->progress = nullptr;
}

void
BboxRenderDelegate::sync_camera(const unsigned int& width, const unsigned int& height)
{
    m->camera.init_ray_generator(*get_scene_delegate(), width, height);
}

void
sync_shading_groups(const R2cSceneDelegate& delegate, R2cItemId cgeometryid, BboxGeometryInfo& rgeometry)
{
    const R2cShadingGroupInfo& shading_group = delegate.get_shading_group_info(cgeometryid, 0);
    if (shading_group.get_material().is_null()) {
        rgeometry.material = nullptr;
    } else {
        rgeometry.material = static_cast<ModuleMaterialDummy *>(shading_group.get_material().get_item()->get_module());
    }
}

void
BboxRenderDelegate::_sync_geometry(R2cItemId cgeometryid, BboxGeometryInfo& rgeometry, const bool& is_new)
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
            BboxResourceInfo *stored_resource = m->resources.index.is_key_exists(cresource.get_id());
            
            if (stored_resource == nullptr) { // the resource doesn't exists so let's create it
                // create corresponding geometry resource according to the Clarisse geometry
                BboxResourceInfo new_resource;

                // Extract the dummy
                R2cItemDescriptor idesc = get_scene_delegate()->get_render_item(cgeometryid);
                ModuleSceneObject *module = static_cast<ModuleSceneObject *>(idesc.get_item()->get_module());
                new_resource.bbox = module->get_bbox();

                new_resource.refcount = 1;
                // adding the new resource
                m->resources.index.add(cresource.get_id(), new_resource);
                // we need to add the new mesh to instanciate it
            //    m->scene->AddMesh(new_resource.ptr);
            } else {
                stored_resource->refcount++;
            }

            // generating the instance to the resource
            // we need to create the material overrides for the instance since we can freely assign
            // materials to instances in Clarisse
        //    rgeometry.materials = BB_InstanceMaterialOverrides_New();
        //    rgeometry.materials->SetTemplate(mesh);
        //    rgeometry.materials->SetNumMaterials(mesh->GetNumMaterials());
            // back pointer to the clarisse resource since when we are dirty it's too late to get it back
            rgeometry.resource = cresource.get_id();

        //    rgeometry.ptr->SetTemplate(mesh, m->scene->AddInstanceMaterialOverride(rgeometry.materials));
            // since that was a new geometry we will need to set the matrix, materials and visibility flags
            rgeometry.dirtiness = R2cSceneDelegate::DIRTINESS_KINEMATIC |
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
BboxRenderDelegate::sync_geometries()
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
            BboxGeometryInfo *geometry = m->geometries.index.is_key_exists(removed_item);
            // check the current geometry exists in the scene
            if (geometry != nullptr) {
                // doing proper cleanup. Let's cleanup the resource
                // get the resource if it exists
                BboxResourceInfo *stored_resource = m->resources.index.is_key_exists(geometry->resource);
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
        BboxGeometryInfo geometry;
        CoreVector<BboxGeometryInfo> new_geometries(0, m->geometries.inserted.get_count());
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
sync_shading_groups(const R2cSceneDelegate& delegate, R2cItemId cinstancerid, BboxInstancerInfo& rinstancer)
{
    const R2cShadingGroupInfo& shading_group = delegate.get_shading_group_info(cinstancerid, 0);
    if (shading_group.get_material().is_null()) {
        rinstancer.material = nullptr;
    } else {
        rinstancer.material = static_cast<ModuleMaterialDummy *>(shading_group.get_material().get_item()->get_module());
    }
}

void
BboxRenderDelegate::_sync_instancer(R2cItemId cinstancerid, BboxInstancerInfo& rinstancer, const bool& is_new)
{
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
            BboxResourceInfo *stored_resource = m->resources.index.is_key_exists(cresource.get_id());
            
            if (stored_resource == nullptr) { // the resource doesn't exists so let's create it
                // create corresponding geometry resource according to the Clarisse geometry
                BboxResourceInfo new_resource;

                // Extract the dummy
                R2cItemDescriptor idesc = get_scene_delegate()->get_render_item(cinstancerid);
                ModuleSceneObject *module = static_cast<ModuleSceneObject *>(idesc.get_item()->get_module());
                new_resource.bbox = module->get_bbox();

                new_resource.refcount = 1;
                // adding the new resource
                m->resources.index.add(cresource.get_id(), new_resource);
                // we need to add the new mesh to instanciate it
            //    m->scene->AddMesh(new_resource.ptr);
            } else {
                stored_resource->refcount++;
            }

            // generating the instance to the resource
            // we need to create the material overrides for the instance since we can freely assign
            // materials to instances in Clarisse
        //    rgeometry.materials = BB_InstanceMaterialOverrides_New();
        //    rgeometry.materials->SetTemplate(mesh);
        //    rgeometry.materials->SetNumMaterials(mesh->GetNumMaterials());
            // back pointer to the clarisse resource since when we are dirty it's too late to get it back
            rinstancer.resource = cresource.get_id();

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
BboxRenderDelegate::sync_instancers()
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
        // check if instancers have been removed and in that case we will have to
        // rebuild the render scene since we can't remove items from the scene using
        // the Bbox API

        // let's see if we have to remove instancers from the scene
        for (auto removed_item : m->instancers.removed) {
            BboxInstancerInfo *instancer = m->instancers.index.is_key_exists(removed_item);
            // check the current instancer exists in the scene
            if (instancer != nullptr) {
                // now doing proper cleanup. Let's cleanup resources
                // get the resources if they exist
                BboxResourceInfo *stored_resource = m->resources.index.is_key_exists(instancer->resource);
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
        BboxInstancerInfo instancer;
        CoreVector<BboxInstancerInfo> new_instancers(0, m->instancers.inserted.get_count());
        for (auto inserted_item : m->instancers.inserted) {
            // initializing the new instancer
            // instancer.transforms.remove_all();
            // instancer.resources.remove_all();
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

    //    if (!cleanup.point_clouds) {
    //        // we can just add new geometries to the scene since they are already synched!
    //        for (auto new_instancer : new_instancers) {
    //            for (auto point_cloud : new_instancer.ptrs) {
    //                m->scene->AddMeshPointCloud(point_cloud);
    //            }
    //        }
    //    }
    }
    // our instancers are now perfectly synched
    m->instancers.dirty = false;
}

/*! \brief light synchronization helper */
void
sync_light(const R2cSceneDelegate& delegate, R2cItemId clightid, BboxLightInfo& rlight)
{
    if (rlight.dirtiness & R2cSceneDelegate::DIRTINESS_KINEMATIC) {
    }

    if (rlight.dirtiness & R2cSceneDelegate::DIRTINESS_LIGHT) {

    }
    // setting the dirtiness back to none since the light is synched
    rlight.dirtiness = R2cSceneDelegate::DIRTINESS_NONE;
}

void
BboxRenderDelegate::sync_lights()
{
    if (m->lights.is_dirty()) {
        // remove lights first
        for (auto removed_item : m->lights.removed) {
            BboxLightInfo *light = m->lights.index.is_key_exists(removed_item);
            // check the current light exists in the scene
            if (light != nullptr) {
                m->lights.index.remove(removed_item);
                if (m->lights.index.get_count() == 0) break; // finished
            }
        }
        m->lights.removed.remove_all();

        LOG_INFO_VAR(m->lights.inserted.get_count());
        // creating new lights
        BboxLightInfo light;
        CoreVector<BboxLightInfo> new_lights(0, m->lights.inserted.get_count());
        for (auto inserted_item : m->lights.inserted) {
            // create corresponding light according to the Clarisse light
            BboxUtils::create_light(*get_scene_delegate(), inserted_item, light);
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
BboxRenderDelegate::get_supported_cameras(CoreVector<CoreString>& supported_cameras, CoreVector<CoreString>& unsupported_cameras) const
{
	supported_cameras = s_supported_cameras;
	unsupported_cameras = s_unsupported_cameras;
}


void
BboxRenderDelegate::get_supported_materials(CoreVector<CoreString>& supported_materials, CoreVector<CoreString>& unsupported_materials) const
{
	supported_materials = s_supported_materials;
	unsupported_materials = s_unsupported_materials;
}

void
BboxRenderDelegate::get_supported_lights(CoreVector<CoreString>& supported_lights, CoreVector<CoreString>& unsupported_lights) const
{
	supported_lights = s_supported_lights;
	unsupported_lights = s_unsupported_lights;
}

void
BboxRenderDelegate::get_supported_geometries(CoreVector<CoreString>& supported_geometries, CoreVector<CoreString>& unsupported_geometries) const
{
	supported_geometries = s_supported_geometries;
	unsupported_geometries = s_unsupported_geometries;
}

ModuleMaterial * 
BboxRenderDelegate::get_default_material() const
{
    return nullptr;
}

ModuleMaterial * 
BboxRenderDelegate::get_error_material() const
{
    return nullptr;
}