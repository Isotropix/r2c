//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Local includes
#include "./spherix_external_renderer.h"
#include "./spherix_module_texture.h"
#include "./spherix_utils.h"

// Clarisse includes
#include <module_camera.h>
#include <ray_generator_camera.h>
#include <sampling_image.h>

void SpherixCamera::init_ray_generator(const R2cSceneDelegate& delegate, const unsigned int width, const unsigned int height)
{
    // Extract the ray generator from the scene's camera
    ModuleCamera *current_camera = static_cast<ModuleCamera *>(delegate.get_camera().get_item()->get_module());
    m_ray_generator = current_camera->create_ray_generator();
    m_ray_generator->init(width, height, 1, 1);
}

GMathRay SpherixCamera::generate_ray(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y) const
{
#if ISOTROPIX_VERSION_NUMBER < IX_BUILD_VERSION_NUMBER(4, 5)
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

void SpherixUtils::create_light(const R2cSceneDelegate &render_delegate, R2cItemId item_id, SpherixLightInfo &light_info)
{
    // Get the OfObject of the light
    R2cItemDescriptor idesc = render_delegate.get_render_item(item_id);
    OfObject *item = idesc.get_item();

    // Fill the light data
    light_info.light_data.shader_light = static_cast<ModuleLightSpherix *>(item->get_module())->get_light();
}

void SpherixAttributChange::on_attribute_change(const OfAttr &attr, ExternalShader *shader)
{
    std::string parameter_name = attr.get_name().get_data();

    // Browse all the shader attributes and update the correct one
    for (Parameter *param : shader->parameters) {
        if (param->name == parameter_name) {
            // Update the value
            if (param->type == EXTR_TYPE_DOUBLE) {
                ParameterDouble *param_color = static_cast<ParameterDouble *>(param);
                param_color->value = attr.get_double();
            } else if (param->type == EXTR_TYPE_COLOR) {
                ParameterColor *param_color = static_cast<ParameterColor *>(param);
                GMathVec3d value = attr.get_vec3d();
                param_color->value[0] = value[0];
                param_color->value[1] = value[1];
                param_color->value[2] = value[2];
            } else if(param->type == EXTR_TYPE_BOOL) {
                ParameterBool *param_bool = static_cast<ParameterBool *>(param);
                param_bool->value = attr.get_bool();
            } else {
                CORE_ASSERT(false);
            }

            param->texture = (attr.is_textured()) ? static_cast<ModuleTextureSpherix *>(attr.get_texture()->get_module())->get_texture() : nullptr;
            break;
        }
    }

}

bool SpherixSphere::intersect(const GMathRay &local_ray, double &t, GMathVec3d &normal) const {
    GMathVec3d local_position;

    double t0, t1;
    GMathVec3d origin = local_ray.get_origin();
    double square_radius = m_radius * m_radius;

    const double a = local_ray.get_direction().dot(local_ray.get_direction());
    const double b = 2.0 * local_ray.get_direction().dot(origin);
    const double c = origin.dot(origin) - square_radius;

    double discrim = b * b - 4.0 * a * c;
    if (discrim < 0.0) {
        return false;
    }
    discrim = sqrt(discrim);
    const double q = b < 0 ? -0.5 * (b - discrim) : -0.5 * (b + discrim);
    t0 = q / a;
    t1 = c / q;

    if (t0 > t1) {
        t = t1;
    } else {
        t = t0;
    }

    local_position = local_ray.compute_position(t);

    if (local_position[1] >= -m_radius && local_position[1] <= m_radius) {
        normal = local_position;
        normal.normalize();
        return true;
    }

    return false;
}

GMathVec3d SpherixSphere::get_center() const { return m_center; }
