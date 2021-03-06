//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Local includes
#include "./kubix_utils.h"

// Clarisse includes
#include <module_camera.h>
#include <ray_generator_camera.h>
#include <sampling_image.h>

void KubixCamera::init_ray_generator(const R2cSceneDelegate& delegate, const unsigned int width, const unsigned int height)
{
    // Extract the ray generator from the scene's camera
    ModuleCamera *current_camera = static_cast<ModuleCamera *>(delegate.get_camera().get_item()->get_module());
    m_ray_generator = current_camera->create_ray_generator();
    m_ray_generator->init(width, height, 1, 1);
}

GMathRay KubixCamera::generate_ray(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y)
{
#if ISOTROPIX_VERSION_NUMBER < IX_VERSION_NUMBER(4, 5)
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
#else
    // Create image sampler
    ImageSample image_sample;
    GMathVec2d min, max;
    ImageSampler image_sampler;
    image_sampler.init(width, height);
    image_sampler.get_pixel_samples(x, y, &image_sample, min, max);

    // Compute the ray
    GMathRay ray;
    unsigned int index = 0;
    m_ray_generator->get_rays(&image_sample, 1, &ray, &index);
#endif
    return ray;
}

void KubixUtils::create_light(const R2cSceneDelegate &render_delegate, R2cItemId item_id, KubixLightInfo &light_info)
{
    // Get the OfObject of the light
    R2cItemDescriptor idesc = render_delegate.get_render_item(item_id);
    OfObject *item = idesc.get_item();

    // Fill the light data
    light_info.light_data.light_module = static_cast<ModuleLightKubix *>(item->get_module());
}
