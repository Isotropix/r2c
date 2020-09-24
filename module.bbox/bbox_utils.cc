//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include "bbox_utils.h"

#include <core_log.h>
#include <gmath_matrix3x3.h>
#include <image_canvas.h>
#include <image_map_channel.h>
#include <module_camera.h>
#include <module_geometry.h>
#include <module_layer.h>
#include <module_light_bbox.h>
#include <module_material_bbox.h>
#include <module_scene_object_tree.h>
#include <of_context.h>
#include <poly_mesh_smoothed.h>
#include <poly_mesh.h>
#include <r2c_instancer.h>
#include <r2c_render_buffer.h>
#include <sys_globals.h>
#include <sys_thread_lock.h>
#include <ray_generator_camera.h>
#include <sampling_image.h>

void BboxUtils::create_light(const R2cSceneDelegate &render_delegate, R2cItemId item_id, BboxLightInfo &light_info)
{
    // Get the OfObject of the light
    R2cItemDescriptor idesc = render_delegate.get_render_item(item_id);
    OfObject *item = idesc.get_item();

    // Fill the light data
    light_info.light_data.light_module = static_cast<ModuleLightBbox *>(item->get_module());
}

void BboxCamera::init_ray_generator(const R2cSceneDelegate& delegate, const unsigned int width, const unsigned int height)
{
    ModuleCamera *current_camera = static_cast<ModuleCamera *>(delegate.get_camera().get_item()->get_module());
    m_ray_generator = current_camera->create_ray_generator();
    m_ray_generator->init(width, height, 1, 1);
}

GMathRay BboxCamera::generate_ray(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y)
{
    GMathRay ray;

    // Create image sampler
    GMathVec2d image_sample, min, max;
    ImagePixelSample pixel_sample;
    ImageSampler image_sampler;
    image_sampler.init(width, height);
    image_sampler.get_pixel_samples(x, y, &image_sample, &pixel_sample, min, max);

    // Compute the ray
    unsigned int index = 0;
    m_ray_generator->get_rays(&image_sample, &pixel_sample, 1, &ray, &index);

    return ray;
}
