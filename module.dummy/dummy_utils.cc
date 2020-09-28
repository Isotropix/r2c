//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include "dummy_utils.h"

// Clarisse includes
#include <module_camera.h>
#include <ray_generator_camera.h>
#include <sampling_image.h>

void DummyCamera::init_ray_generator(const R2cSceneDelegate& delegate, const unsigned int width, const unsigned int height)
{
    // Extract the ray generator from the scene's camera
    ModuleCamera *current_camera = static_cast<ModuleCamera *>(delegate.get_camera().get_item()->get_module());
    m_ray_generator = current_camera->create_ray_generator();
    m_ray_generator->init(width, height, 1, 1);
}

GMathRay DummyCamera::generate_ray(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y)
{
    // Create image sampler
    GMathVec2d image_sample, min, max;
    ImagePixelSample pixel_sample;
    ImageSampler image_sampler;
    image_sampler.init(width, height);
    image_sampler.get_pixel_samples(x, y, &image_sample, &pixel_sample, min, max);

    // Compute the ray
    GMathRay ray;
    unsigned int index = 0;
    m_ray_generator->get_rays(&image_sample, &pixel_sample, 1, &ray, &index);

    return ray;
}

void DummyUtils::create_light(const R2cSceneDelegate &render_delegate, R2cItemId item_id, DummyLightInfo &light_info)
{
    // Get the OfObject of the light
    R2cItemDescriptor idesc = render_delegate.get_render_item(item_id);
    OfObject *item = idesc.get_item();

    // Fill the light data
    light_info.light_data.light_module = static_cast<ModuleLightDummy *>(item->get_module());
}
