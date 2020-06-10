//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <core_log.h>
#include <sys_thread_lock.h>
#include <of_object.h>

#include <image_canvas.h>
#include <image_map_channel.h>
#include <module_layer.h>
#include <module_camera.h>
#include <module_particle.h>

#include <r2c_scene_delegate.h>
#include <r2c_render_buffer.h>

#include <RS.h>

#include <module_material_redshift.h>
#include "module_renderer_redshift.h"
#include "redshift_utils.h"

#include "redshift_render_delegate.h"

// private implementation
class RSDelegateImpl {
public:

    RSScene *scene; // actual Redshift render scene
    RSCamera *camera; // Redshift render camera
    RenderingBlockSink *sink; // Redshift BlockSink that fills the Clarisse's render buffer
    RenderingAbortChecker *abort_checker; // Redshift AbortChecker that notifies the renderer to stop
    RenderingProgress *progress; // Redshift rendering progress

    struct {
        RSResourceIndex index; // the index of all current resources where we store deduplicated data
    } resources;

    struct {
        RSGeometryIndex index; // index of all render geometries which are mesh instances pointing to a geometry resource
        CoreVector<R2cItemId> inserted; // is filled by RedshiftRenderDelegate::insert_geometry when a geometry is inserted to the scene
        CoreVector<R2cItemId> removed; // is filled by RedshiftRenderDelegate::remove_geometry when a geometry is removed from the scene

        bool dirty; // flag set when we receive dirtiness so that we don't need to iterate the whole index to know that it is dirty
        bool is_dirty() { return inserted.get_count() != 0 || removed.get_count() != 0 || dirty; } // return true is index is dirty

    } geometries;

    struct {
        RSLightIndex index; // index of all render lights
        CoreVector<R2cItemId> inserted; // is filled by RedshiftRenderDelegate::insert_light when a light is inserted to the scene
        CoreVector<R2cItemId> removed; // is filled by RedshiftRenderDelegate::remove_light when a light is removed from the scene
        bool dirty; // flag set when we receive dirtiness so that we don't need to iterate the whole index to know that it is dirty

        bool is_dirty() { return inserted.get_count() != 0 || removed.get_count() != 0 || dirty; } // return true is index is dirty

    } lights;

    struct {
        RSInstancerIndex index; // index of all render instancers
        CoreVector<R2cItemId> inserted; // is filled by RedshiftRenderDelegate::insert_instancer when an instancer is inserted to the scene
        CoreVector<R2cItemId> removed; // is filled by RedshiftRenderDelegate::remove_instancer when an instancer is removed from the scene
        bool dirty; // flag set when we receive dirtiness so that we don't need to iterate the whole index to know that it is dirty

        bool is_dirty() { return inserted.get_count() != 0 || removed.get_count() != 0 || dirty; } // return true is index is dirty

    } instancers;

    inline RSDelegateImpl()
        : scene(nullptr)
        , camera(nullptr)
        , sink(nullptr)
        , abort_checker(nullptr)
        , progress(nullptr) {
    }

    ~RSDelegateImpl() {
    }
};

IMPLEMENT_CLASS(RedshiftRenderDelegate, R2cRenderDelegate);

RedshiftRenderDelegate::RedshiftRenderDelegate() : R2cRenderDelegate()
{
    m = new RSDelegateImpl;
}

RedshiftRenderDelegate::~RedshiftRenderDelegate()
{
    clear();
    delete m;
}

CoreString 
RedshiftRenderDelegate::get_class_name() const
{
    return "RendererRedshift";
}

bool
RedshiftRenderDelegate::sync_render_settings(const float& sampling_quality)
{
    if (!get_scene_delegate()->get_render_settings().is_null()) {
        R2cItemDescriptor renderer = get_scene_delegate()->get_render_settings();
        if (!renderer.is_null() && renderer.get_item()->get_module()->is_kindof(ModuleRendererRedshift::class_info())) {
            ModuleRendererRedshift *settings = static_cast<ModuleRendererRedshift *>(renderer.get_item()->get_module());
            settings->sync(sampling_quality);
            return true;
        }

    }
    return false;
}

void
RedshiftRenderDelegate::insert_light(R2cItemDescriptor item)
{
    m->lights.inserted.add(item.get_id());
}

void
RedshiftRenderDelegate::remove_light(R2cItemDescriptor item)
{
    RSLightInfo *light = m->lights.index.is_key_exists(item.get_id());
    if (light != nullptr) { // make sure it is indeed in our index
        m->lights.removed.add(item.get_id());
    }
}

void
RedshiftRenderDelegate::dirty_light(R2cItemDescriptor item, const int& dirtiness)
{
    RSLightInfo *light = m->lights.index.is_key_exists(item.get_id());
    if (light != nullptr) { // make sure it is indeed in our index
        m->lights.dirty = true;
        light->dirtiness |= dirtiness;
    }
}

void
RedshiftRenderDelegate::insert_instancer(R2cItemDescriptor item)
{
    m->instancers.inserted.add(item.get_id());
}

void
RedshiftRenderDelegate::remove_instancer(R2cItemDescriptor item)
{
    RSInstancerInfo *instancer = m->instancers.index.is_key_exists(item.get_id());
    if (instancer != nullptr) { // make sure it is indeed in our index
        m->instancers.removed.add(item.get_id());
    }
}

void
RedshiftRenderDelegate::dirty_instancer(R2cItemDescriptor item, const int& dirtiness)
{
    RSInstancerInfo *instancer = m->instancers.index.is_key_exists(item.get_id());
    if (instancer != nullptr) { // make sure it is indeed in our index
        m->instancers.dirty = true;
        instancer->dirtiness |= dirtiness;
    }
}

void
RedshiftRenderDelegate::insert_geometry(R2cItemDescriptor item)
{
    m->geometries.inserted.add(item.get_id());
}

void
RedshiftRenderDelegate::remove_geometry(R2cItemDescriptor item)
{
    RSGeometryInfo *geometry = m->geometries.index.is_key_exists(item.get_id());
    if (geometry != nullptr) { // make sure it is indeed in our index
        m->geometries.removed.add(item.get_id());
    }
}

void
RedshiftRenderDelegate::dirty_geometry(R2cItemDescriptor item, const int& dirtiness)
{
    RSGeometryInfo *geometry = m->geometries.index.is_key_exists(item.get_id());
    if (geometry != nullptr) { // make sure it is indeed in our index
        m->geometries.dirty = true;
        geometry->dirtiness |= dirtiness;
    }
}

void
RedshiftRenderDelegate::render(R2cRenderBuffer *render_buffer, const float& sampling_quality)
{
    if (render_buffer != nullptr && sync_render_settings(sampling_quality)) { // make sure we have what we need to render
        const unsigned int w = static_cast<unsigned int>(render_buffer->get_width());
        const unsigned int h = static_cast<unsigned int>(render_buffer->get_height());

        sync_camera(w, h, 0, 0, w, h); // this takes care of creating the camera
        if (m->camera == nullptr) return; // no valid camera is set

        // Create the render scene if it wasn't already created
        if (m->scene == nullptr) {
            m->scene = RS_Scene_New();
        }

        // Create the sink if it wasn't already
        if (m->sink == nullptr) {
            m->sink = new RenderingBlockSink;
            RS_RenderChannel_GetMain()->AddBlockSink(0, m->sink); // just for the beauty for now
        }

        m->sink->SetRenderBuffer(render_buffer); // set the sink to the input render buffer

        // Create the abort checker if it wasn't already
        if (m->abort_checker == nullptr) {
            m->abort_checker = new RenderingAbortChecker(get_scene_delegate()->get_application());
        }

        // Create the progress class if it wasn't already
        if (m->progress == nullptr) {
            m->progress = new RenderingProgress();
        }

        // make sure to synchronize the render scene with the scene delegate
        sync();

        // main rendering call.
        RS_Renderer_Render(m->camera, m->scene, true, m->abort_checker, m->progress);

        // finalize the render buffer
        render_buffer->finalize();
    }
}

float
RedshiftRenderDelegate::get_render_progress() const
{
    return m->progress ? m->progress->get_current_progress() : 0.f;
}

void
RedshiftRenderDelegate::sync()
{
    CleanupFlags cleanup;
    sync_geometries(cleanup);
    sync_instancers(cleanup);
    sync_lights(cleanup);
    // cleanup the scene in the event we removed items
    cleanup_scene(cleanup);
}

void
RedshiftRenderDelegate::cleanup_scene(const CleanupFlags& cleanup)
{
    // since we can't remove geometries we must repopulate all geometries etc... :'(
    if (cleanup.mesh_instances) {
        m->scene->ClearInstanceMaterialOverrides();
        m->scene->ClearMeshInstances();
        // adding instances
        for (auto geometry : m->geometries.index) {
            const RSGeometryInfo& geo = geometry.get_value();
            geo.ptr->SetTemplate(geo.materials->GetTemplate(), m->scene->AddInstanceMaterialOverride(geo.materials));
            m->scene->AddMeshInstance(geometry.get_value().ptr);
        }
    }

    if (cleanup.meshes) {
        m->scene->ClearMeshes();
        // adding mesh resources
        for (auto resource: m->resources.index) m->scene->AddMesh(resource.get_value().ptr);
    }

    if (cleanup.point_clouds) {
        m->scene->ClearMeshPointClouds();
        for (auto instancer: m->instancers.index) {
            for (auto point_cloud : instancer.get_value().ptrs) {
                m->scene->AddMeshPointCloud(point_cloud);
            }
        }
    }

    if (cleanup.lights) {
        m->scene->ClearLights();
        for (auto light : m->lights.index) m->scene->AddLight(light.get_value().ptr);
    }
}

void
RedshiftRenderDelegate::clear()
{
    // !!! make sure to clear everything !!!
    if (m->scene != nullptr) {
        // clearing instances
        for (auto geometry : m->geometries.index) {
            const RSGeometryInfo& geo = geometry.get_value();
            RS_InstanceMaterialOverrides_Delete(geo.materials);
            RS_MeshInstance_Delete(geo.ptr);
        }
        m->geometries.index.remove_all();
        m->geometries.removed.clear();
        m->geometries.inserted.clear();
        m->geometries.dirty = R2cSceneDelegate::DIRTINESS_ALL;
        m->scene->ClearInstanceMaterialOverrides();
        m->scene->ClearMeshInstances();

        // clearing meshes
        for (auto resource: m->resources.index) RS_MeshBase_Delete(resource.get_value().ptr);
        m->scene->ClearMeshes();
        m->resources.index.remove_all();

        // clearing point clouds
        for (auto instancer: m->instancers.index) {
            for (auto point_cloud : instancer.get_value().ptrs) {
                RS_PointCloud_Delete(point_cloud);
            }
        }
        m->scene->ClearMeshPointClouds(); // not really necessary since we called ClearMeshes()
        m->instancers.index.remove_all();
        m->instancers.removed.clear();
        m->instancers.inserted.clear();
        m->instancers.dirty = R2cSceneDelegate::DIRTINESS_ALL;

        // clearing lights
        for (auto light : m->lights.index) RS_Light_Delete(light.get_value().ptr);
        m->lights.index.remove_all();
        m->lights.removed.clear();
        m->lights.inserted.clear();
        m->lights.dirty = R2cSceneDelegate::DIRTINESS_ALL;
        m->scene->ClearLights();

        RS_Scene_Delete(m->scene);
        m->scene = nullptr;
    }
    if (m->camera != nullptr) {
        RS_Camera_Delete(m->camera);
        m->camera = nullptr;
    }

    delete m->sink;
    m->sink = nullptr;

    delete m->abort_checker;
    m->abort_checker = nullptr;

    delete m->progress;
    m->progress = nullptr;
}

void
RedshiftRenderDelegate::sync_camera(const unsigned int& w, const unsigned int& h,
                                    const unsigned int& cox, const unsigned int& coy,
                                    const unsigned int& cw, const unsigned int& ch)
{
    if (!get_scene_delegate()->get_camera().is_destroyed()) {
        if (m->camera == nullptr) {
            m->camera = RS_Camera_New();
        }
        ModuleCamera *cam = static_cast<ModuleCamera *>(get_scene_delegate()->get_camera().get_item()->get_module());
        m->camera->SetMatrix(RedshiftUtils::ToRSMatrix4x4(cam->get_global_matrix()));

        m->camera->SetNumStepsFromRendererOptions(); // we need to call this before setting any parameters or matrices. As the name suggests, this reads the transformation blur "num steps" and allocates as many aspect, near/far plane, matrices, etc as needed
        m->camera->SetFramebufferParams(w, h, cox, coy, cw, ch);

        // FIXME: need to support all types of compatible cameras
        m->camera->SetType("RS_CAMERA_TYPE_PERSPECTIVE");

        double ratio = h / static_cast<double>(w), hfov, vfov;
        // FIXME: FOV is only working in vertical. Need to investigate on Clarisse side.
        cam->get_fovs(ratio, hfov, vfov);

        // We need to ensure all steps are cleared because the scene might be using transformation blur!
        for(unsigned int i=0; i< m->camera->GetNumTransformationSteps(); i++) {
            m->camera->SetAspect(static_cast<float>(ratio), i); // height divided by width
            m->camera->SetNear(0.01f, i);
            m->camera->SetFar(100000.0f, i);
            m->camera->SetFOVOrOrthogonalHeight(gmath_radians(static_cast<float>(vfov)), true, i); // FOV to 90 degrees in radians. We want 90 degrees horizontal FOV, hence the false parameter.
        }
    } else if (m->camera != nullptr) { // shouldn't really happen since we can't be called with an empty camera
        RS_Camera_Delete(m->camera);
        m->camera = nullptr;
    }
}

void
sync_shading_groups(const R2cSceneDelegate& delegate, R2cItemId cgeometryid, RSGeometryInfo& rgeometry)
{
    for (unsigned int i = 0; i < rgeometry.materials->GetNumMaterials(); i++) {
        const R2cShadingGroupInfo&  shading_group = delegate.get_shading_group_info(cgeometryid, i);
        rgeometry.materials->SetMaterial(i, shading_group.get_material().is_null() ? RedshiftUtils::get_default_material() : static_cast<ModuleMaterialRedshift *>(shading_group.get_material().get_item()->get_module())->get_material());
    }
}

RSCachedMeshFlags
RS_CachedMeshFlag_Hidden()
{
    static RSCachedMeshFlags hidden = RS_CachedMeshFlag_GetDefault();
    static bool init = false;
    if (!init) {
        RS_CachedMeshFlag_Set(hidden, "MESHFLAG_PRIMARYRAYVISIBLE", false);
        RS_CachedMeshFlag_Set(hidden, "MESHFLAG_AOCASTER", false);
        RS_CachedMeshFlag_Set(hidden, "MESHFLAG_SHADOWCASTER", false);
        RS_CachedMeshFlag_Set(hidden, "MESHFLAG_REFLECTIONVISIBLE", false);
        RS_CachedMeshFlag_Set(hidden, "MESHFLAG_REFRACTIONVISIBLE", false);
        RS_CachedMeshFlag_Set(hidden, "MESHFLAG_GIVISIBLE", false);
        RS_CachedMeshFlag_Set(hidden, "MESHFLAG_CAUSTICVISIBLE", false);
        RS_CachedMeshFlag_Set(hidden, "MESHFLAG_FGVISIBLE", false);
        init = true;
    }
    return hidden;
}

void
RedshiftRenderDelegate::_sync_geometry(R2cItemId cgeometryid, RSGeometryInfo& rgeometry, const bool& is_new)
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
            RSResourceInfo *stored_resource = m->resources.index.is_key_exists(cresource.get_id());
            RSMeshBase *mesh = nullptr;

            if (stored_resource == nullptr) { // the resource doesn't exists so let's create it
                // create corresponding geometry resource according to the Clarisse geometry
                RSResourceInfo new_resource;
                mesh = new_resource.ptr = RedshiftUtils::CreateGeometry(*get_scene_delegate(), cgeometryid, RedshiftUtils::get_default_material(), new_resource.type);
                new_resource.refcount = 1;
                // adding the new resource
                m->resources.index.add(cresource.get_id(), new_resource);
                // we need to add the new mesh to instanciate it
                m->scene->AddMesh(new_resource.ptr);
            } else {
                mesh = stored_resource->ptr;
                stored_resource->refcount++;
            }

            // generating the instance to the resource
            rgeometry.ptr = RS_MeshInstance_New();
            rgeometry.ptr->SetIsTransformationBlurred(false);
            // we need to create the material overrides for the instance since we can freely assign
            // materials to instances in Clarisse
            rgeometry.materials = RS_InstanceMaterialOverrides_New();
            rgeometry.materials->SetTemplate(mesh);
            rgeometry.materials->SetNumMaterials(mesh->GetNumMaterials());
            // back pointer to the clarisse resource since when we are dirty it's too late to get it back
            rgeometry.resource = cresource.get_id();

            rgeometry.ptr->SetTemplate(mesh, m->scene->AddInstanceMaterialOverride(rgeometry.materials));
            // since that was a new geometry we will need to set the matrix, materials and visibility flags
            rgeometry.dirtiness = R2cSceneDelegate::DIRTINESS_KINEMATIC |
                                  R2cSceneDelegate::DIRTINESS_SHADING_GROUP |
                                  R2cSceneDelegate::DIRTINESS_VISIBILITY;
        }
    }

    if (rgeometry.dirtiness & R2cSceneDelegate::DIRTINESS_KINEMATIC) {
        rgeometry.ptr->SetMatrix(RedshiftUtils::ToRSMatrix4x4(get_scene_delegate()->get_transform(cgeometryid)));
    }

    if (rgeometry.dirtiness & R2cSceneDelegate::DIRTINESS_SHADING_GROUP) {
        sync_shading_groups(*get_scene_delegate(), cgeometryid, rgeometry);
    }

    if (rgeometry.dirtiness & R2cSceneDelegate::DIRTINESS_VISIBILITY) {
        const bool visibility = get_scene_delegate()->get_visible(cgeometryid);
        rgeometry.ptr->SetCachedMeshFlags(visibility ? RS_CachedMeshFlag_GetDefault() : RS_CachedMeshFlag_Hidden());
    }

    // setting the dirtiness back to none since the geometry is fully synched
    rgeometry.dirtiness = R2cSceneDelegate::DIRTINESS_NONE;
}


void
RedshiftRenderDelegate::sync_geometries(CleanupFlags& cleanup)
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
        // check if geometries have been removed and in that case we will have to
        // rebuild the render scene since we can't remove items from the scene using
        // the Redshift API
        cleanup.mesh_instances = m->geometries.removed.get_count() > 0;
        // let's see if we have to remove geometries from the scene
        for (auto removed_item : m->geometries.removed) {
            RSGeometryInfo *geometry = m->geometries.index.is_key_exists(removed_item);
            // check the current geometry exists in the scene
            if (geometry != nullptr) {
                // doing proper cleanup. Let's cleanup the resource
                // get the resource if it exists
                RSResourceInfo *stored_resource = m->resources.index.is_key_exists(geometry->resource);
                if (stored_resource != nullptr) { // there's a resource bound to the current geometry
                    stored_resource->refcount--;
                    if (stored_resource->refcount == 0) { // no one is using that resource anymore so let's delete it
                        RS_MeshBase_Delete(stored_resource->ptr);
                        m->resources.index.remove(geometry->resource);
                        switch(stored_resource->type) {
                            case RSResourceInfo::TYPE_POINT_CLOUD:
                                cleanup.point_clouds |= true;
                                break;
                            case RSResourceInfo::TYPE_HAIR:
                                cleanup.hairs |= true;
                                break;
                            default:
                                cleanup.meshes |= true;
                                break;
                        }
                    }
                }
                RS_MeshInstance_Delete(geometry->ptr);
                RS_InstanceMaterialOverrides_Delete(geometry->materials);

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
        RSGeometryInfo geometry;
        CoreVector<RSGeometryInfo> new_geometries(0, m->geometries.inserted.get_count());
        for (auto inserted_item : m->geometries.inserted) {
            // initializing the new geometry
            geometry.ptr = nullptr;
            geometry.dirtiness = R2cSceneDelegate::DIRTINESS_ALL;
            // synching the new geometry
            sync_new_geometry(inserted_item, geometry);
            // adding it to our geometry index
            m->geometries.index.add(inserted_item, geometry);
            new_geometries.add(geometry);
        }
        // since we processed all pending inserted geometries we have to clear the array
        m->geometries.inserted.remove_all();

        if (!cleanup.mesh_instances) {
            // we can just add new geometries to the scene since they are already synched!
            for (auto new_geometry : new_geometries) {
               m->scene->AddMeshInstance(new_geometry.ptr);
            }
        }
    }
    // our geometries are now perfectly synched
    m->geometries.dirty = false;
}

/*! \brief shading group synchronization helper */
void
sync_shading_groups(const R2cSceneDelegate& delegate, R2cItemId cinstancerid, RSInstancerInfo& rinstancer)
{
    // retreive the clarisse instancer
    R2cItemDescriptor cinstancer = delegate.get_render_item(cinstancerid);
    // since the redshift instancer is mimicing Clarisse, shading groups/material association are perfect match
    unsigned int shading_group_offset = 0;
    for (auto instancer : rinstancer.ptrs) {
        for (unsigned int sgindex = 0; sgindex < instancer->GetNumMaterials(); sgindex++) {
            const R2cShadingGroupInfo& shading_group = delegate.get_shading_group_info(cinstancerid, sgindex + shading_group_offset);
            instancer->SetMaterial(sgindex, shading_group.get_material().is_null() ? RedshiftUtils::get_default_material() : static_cast<ModuleMaterialRedshift *>(shading_group.get_material().get_item()->get_module())->get_material());
        }
        shading_group_offset += instancer->GetNumMaterials();
    }
}

void
RedshiftRenderDelegate::_sync_instancer(R2cItemId cinstancerid, RSInstancerInfo& rinstancer, const bool& is_new)
{
    if (rinstancer.dirtiness & R2cSceneDelegate::DIRTINESS_GEOMETRY) {
        if (!is_new) { // existing instancer
            // mark as removed since we will need to recreate it
            m->instancers.removed.add(cinstancerid);
            // reinsert the removed instancer so it is added after the cleanup
            m->instancers.inserted.add(cinstancerid);
            // mark it as clean since we will rebuild it anyway
            rinstancer.dirtiness = R2cSceneDelegate::DIRTINESS_NONE;
        } else { // it's a new instancer and let's create its resources
            RedshiftUtils::CreateInstancer(rinstancer, m->resources.index, *get_scene_delegate(), m->scene, cinstancerid);
        }
    }

    if (rinstancer.dirtiness & R2cSceneDelegate::DIRTINESS_KINEMATIC) {
        const GMathMatrix4x4d& transform = get_scene_delegate()->get_transform(cinstancerid);
        for (auto ptc : rinstancer.ptrs) ptc->SetMatrix(RedshiftUtils::ToRSMatrix4x4(transform));
    }

    if (rinstancer.dirtiness & R2cSceneDelegate::DIRTINESS_SHADING_GROUP) {
        sync_shading_groups(*get_scene_delegate(), cinstancerid, rinstancer);
    }

    if (rinstancer.dirtiness & R2cSceneDelegate::DIRTINESS_VISIBILITY) {
        const bool visibility = get_scene_delegate()->get_visible(cinstancerid);
        for (auto ptc : rinstancer.ptrs) ptc->SetCachedMeshFlags(visibility ? RS_CachedMeshFlag_GetDefault() : RS_CachedMeshFlag_Hidden());
    }
    // setting the dirtiness back to none since the instancer is synched
    rinstancer.dirtiness = R2cSceneDelegate::DIRTINESS_NONE;
}

void
RedshiftRenderDelegate::sync_instancers(CleanupFlags& cleanup)
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
        // the Redshift API

        cleanup.point_clouds |= m->instancers.removed.get_count() > 0;

        // let's see if we have to remove instancers from the scene
        for (auto removed_item : m->instancers.removed) {
            RSInstancerInfo *instancer = m->instancers.index.is_key_exists(removed_item);
            // check the current instancer exists in the scene
            if (instancer != nullptr) {
                // removing all point clouds representing the current instancer
                for (unsigned int i = 0; i < instancer->ptrs.get_count(); i++) RS_PointCloud_Delete(instancer->ptrs[i]);
                // now doing proper cleanup. Let's cleanup resources
                // get the resources if they exist

                for (unsigned int i = 0; i < instancer->resources.get_count(); i++) {
                    R2cResourceId resource_id = instancer->resources[i];
                    RSResourceInfo *stored_resource = m->resources.index.is_key_exists(resource_id);
                    if (stored_resource != nullptr) { // there's a resource bound to the current instancer
                        stored_resource->refcount--;
                        if (stored_resource->refcount == 0) { // no one is using that resource anymore so let's delete it
                            RS_MeshBase_Delete(stored_resource->ptr);
                            m->resources.index.remove(resource_id);
                            switch(stored_resource->type) {
                                case RSResourceInfo::TYPE_POINT_CLOUD: // spheres are point clouds....
                                    cleanup.point_clouds |= true;
                                    break;
                                case RSResourceInfo::TYPE_HAIR:
                                    cleanup.hairs |= true;
                                    break;
                                default:
                                    cleanup.meshes |= true;
                                    break;
                            }
                        }
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
        RSInstancerInfo instancer;
        CoreVector<RSInstancerInfo> new_instancers(0, m->instancers.inserted.get_count());
        for (auto inserted_item : m->instancers.inserted) {
            // initializing the new instancer
            instancer.ptrs.remove_all();
            instancer.resources.remove_all();
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

        if (!cleanup.point_clouds) {
            // we can just add new geometries to the scene since they are already synched!
            for (auto new_instancer : new_instancers) {
                for (auto point_cloud : new_instancer.ptrs) {
                    m->scene->AddMeshPointCloud(point_cloud);
                }
            }
        }
    }
    // our instancers are now perfectly synched
    m->instancers.dirty = false;
}

/*! \brief light synchronization helper */
void
sync_light(const R2cSceneDelegate& delegate, R2cItemId clightid, RSLightInfo& rlight)
{
    if (rlight.dirtiness & R2cSceneDelegate::DIRTINESS_KINEMATIC) {
        rlight.ptr->SetMatrix(RedshiftUtils::ToRSMatrix4x4(delegate.get_transform(clightid)));
    }

    if (rlight.dirtiness & R2cSceneDelegate::DIRTINESS_LIGHT) {
        OfObject *clight = delegate.get_render_item(clightid).get_item();
        // synching light's attributes only
        OfAttr *color = clight->attribute_exists("color");
        rlight.shader->BeginUpdate();
        rlight.shader->SetParameterData("color", color != nullptr ? RedshiftUtils::ToRSColor(color->get_vec3d()) : RSColor(1.0,1.0,1.0));

        OfAttr *radius = clight->attribute_exists("radius");
        if (radius != nullptr) {
            float value =  static_cast<float>(radius->get_double());
            rlight.ptr->SetAreaScaling(RSVector3(value, value, value));
        }
        rlight.shader->EndUpdate();
    }
    // setting the dirtiness back to none since the light is synched
    rlight.dirtiness = R2cSceneDelegate::DIRTINESS_NONE;
}

void
RedshiftRenderDelegate::sync_lights(CleanupFlags& cleanup)
{
    if (m->lights.is_dirty()) {
        cleanup.lights = m->lights.removed.get_count() > 0;
        // remove lights first
        for (auto removed_item : m->lights.removed) {
            RSLightInfo *light = m->lights.index.is_key_exists(removed_item);
            // check the current light exists in the scene
            if (light != nullptr) {
                RS_Light_Delete(light->ptr);
                RS_ShaderNode_Release(light->shader);
                m->lights.index.remove(removed_item);
                if (m->lights.index.get_count() == 0) break; // finished
            }
        }
        m->lights.removed.remove_all();

        // creating new lights
        RSLightInfo light;
        CoreVector<RSLightInfo> new_lights(0, m->lights.inserted.get_count());
        for (auto inserted_item : m->lights.inserted) {
            // create corresponding light according to the Clarisse light
            RedshiftUtils::CreateLight(*get_scene_delegate(), inserted_item, light);
            light.dirtiness = R2cSceneDelegate::DIRTINESS_ALL;
            // and sync it to its attributes
            // synching the new light
            sync_light(*get_scene_delegate(), inserted_item, light);
            // adding it to our light index
            m->lights.index.add(inserted_item, light);
            new_lights.add(light);
        }
        m->lights.inserted.remove_all();

        if (!cleanup.lights) {
            // we can just add new lights since they are already synched!
            for (auto new_light : new_lights) {
               m->scene->AddLight(new_light.ptr);
            }
        }
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

CoreVector<CoreString>
RedshiftRenderDelegate::get_supported_cameras() const
{
    CoreVector<CoreString> result;
    result.add("CameraAlembic");
    result.add("CameraUsd");
    result.add("CameraPerspective");
    result.add("CameraPerspectiveAdvanced");
    return result;
}


CoreVector<CoreString>
RedshiftRenderDelegate::get_supported_materials() const
{
    CoreVector<CoreString> result;
    result.add("MaterialRedshift"); // we only support redshift materials
    return result;
}

CoreVector<CoreString>
RedshiftRenderDelegate::get_supported_lights() const
{
    CoreVector<CoreString> result;
    result.add("Light"); // lights are not really supported right now
    return result;
}

CoreVector<CoreString>
RedshiftRenderDelegate::get_supported_geometries() const
{
    CoreVector<CoreString> result;
    result.add("GeometryPolymesh");
    //result.add("GeometrySphere"); // skip for now since the current implementation creates a point cloud for spheres...
    result.add("GeometryBox");
    result.add("GeometryUsdCube");
    //result.add("GeometryUsdSphere"); // same as the implicit sphere
    //result.add("GeometryBundle"); // bundle (not supported but should be treated pretty much like instancers)
    result.add("SceneObjectTree"); // instancer
    return result;
}
