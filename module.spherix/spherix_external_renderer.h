//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

#include <string>

// Clarisse includes
#include <core_string.h>
#include <core_vector.h>
#include <gmath_vec3.h>

// In this file we are simulated an external Renderer with his own structures, types and shader

class ExternalTextureShader;


// An enum representing the external renderer type, this is important because it will be used to match out type when creating the object's attributes
enum ExternalRendererType {
    EXTR_TYPE_DOUBLE = 0,
    EXTR_TYPE_COLOR,
    EXTR_TYPE_BOOL,
};


/*! \class Parameter
    \brief internal class used to create an interface between the external renderer and the Clarisse attributes
    There is a lot of attributes that defines how the Parameter will be represent in a Clarisse Object GUI
*/
class Parameter {
public :
    Parameter(const std::string& attr_group_name, const std::string& attr_name, ExternalRendererType attr_type) :
        name(attr_name),
        group_name(attr_group_name),
        enable_numeric_range(false), min_numeric_range(0.0), max_numeric_range(0.0),
        enable_numeric_ui_range(false), min_numeric_ui_range(0.0), max_numeric_ui_range(0.0),
        is_texturable(false),
        type(attr_type),
        texture(nullptr)
    {}

    virtual ~Parameter() {}

    // GUI attributes : Attributes used to populate a GUI Clarisse object
    // (note : you could add a lot more attributes, like a documentation, an expression etc...

    // Attribute name is the name displayed for the attribute
    std::string name;

    // Group name is the category in which the attribute will be
    std::string group_name;

    // Those attributes are used to defined if our attribute will have a numeric range,
    // it means that the value will be clamped between [min_numeric_range, max_numeric_range]
    bool enable_numeric_range;
    double min_numeric_range;
    double max_numeric_range;

    // Those attributes are used to defined if our attribute will have a numeric ui range,
    // it means that the slider will have values between [min_numeric_ui_range, max_numeric_ui_range]
    bool enable_numeric_ui_range;
    double min_numeric_ui_range;
    double max_numeric_ui_range;

    // Tell if the Clarisse object is texturable
    bool is_texturable;

    // External Renderer attributes
    ExternalRendererType type;

    // A pointer on a external texture shader in case of the attribute is texturable
    ExternalTextureShader *texture;
};

/********************* EXTERNAL SHADER *******************/

/*! \class ExternalShader
    \brief internal class used to create a shader that will create an interface between Clarisse shaders and the external ones.
    It is in this class that the shader will define how they are evaluated.
*/
class ExternalShader {
public :
    ExternalShader(){}
    virtual ~ExternalShader()
    {
        for (Parameter *param : parameters) {
            delete param;
        }
    }
    ExternalShader(std::string ext_base_name, std::string name, unsigned int parameter_count) : ext_class_base_name(ext_base_name), class_name(name) { parameters.resize(parameter_count); }

    // Clarisse important attributes that store the module names
    // It is important to store only XXX instead of ModuleXXX for compability reasons

    // ext_class_base_name is the class name of the new External Clarisse module from which the external shader will inherit (ModuleMaterialSpherix, ModuleLightSpherix, ModuleTextureSpherix)
    std::string ext_class_base_name;

    // class_name is the actual class name of the new External Clarisse module (SpherixMaterialDiffuse, SpherixMaterialReflection, SpherixTextureColor)
    std::string class_name;

    // A list of parameters used to compute the shader
    CoreArray<Parameter *> parameters;

};

/******************************* Material, Light, Texture shader *********************************/

/**
 * Here we defined some class that will be used to represent an external shader.
 * In this example we create 3 external shaders to represent the Materials, Lights and Textures.
 * The actual Materials, Lights and Texture will inherit from those.
 */

class ExternalMaterialShader : public ExternalShader {
public :
    ExternalMaterialShader() : ExternalShader() {}
    ExternalMaterialShader(std::string name, unsigned int parameter_count) : ExternalShader("MaterialSpherix", name, parameter_count) {}

    virtual GMathVec3f evaluate(const double *ray_direction, const double *normal) const {return GMathVec3f(0.0f);}
};

class ExternalLightShader : public ExternalShader {
public :
    ExternalLightShader() : ExternalShader() {}
    ExternalLightShader(std::string name, unsigned int parameter_count) : ExternalShader("LightSpherix", name, parameter_count) {}

    virtual GMathVec3f evaluate() const { return GMathVec3f(0.0f); }
};

class ExternalTextureShader : public ExternalShader {
public :
    ExternalTextureShader() : ExternalShader() {}
    ExternalTextureShader(std::string name, unsigned int parameter_count) : ExternalShader("TextureSpherix", name, parameter_count) {}

    virtual double *evaluate(const double *ray_direction, const double *normal) {return value;}

    double value[3];
};

/************************************* Parameter *******************************/

/**
  * Here we define several Parameters to make an interface between the Clarisse attributes and the External Renderer attributes
  * Note : The default value is the value used to initialize the Clarisse attribute
 */

class ParameterDouble : public Parameter
{
public :
    ParameterDouble(const std::string& attr_group_name, const std::string& name, double attr_default_value) : Parameter(attr_group_name, name, ExternalRendererType::EXTR_TYPE_DOUBLE)
    {
        value = default_value = attr_default_value;
    }

    const double get_value() const { return value; }
    const double get_default_value() const { return default_value; }
    const double evaluate_double() const { return value; }

    double value;
    double default_value;
};

class ParameterColor : public Parameter
{
public :
    ParameterColor(const std::string& attr_group_name, const std::string& name, double *attr_default_value) : Parameter(attr_group_name, name, ExternalRendererType::EXTR_TYPE_COLOR)
    {
        value[0] = default_value[0] = attr_default_value[0];
        value[1] = default_value[1] = attr_default_value[1];
        value[2] = default_value[2] = attr_default_value[2];
    }

    const double *get_value() const { return value; }
    const double *get_default_value() const { return default_value; }

    // The evaluate will return the value if the attribute is not textures else it will return the value return per the shader texture
    const double *evaluate(const double *ray_direction, const double *normal) const
    {
        return (texture == nullptr) ? value : texture->evaluate(ray_direction, normal);
    }

    double value[3];
    double default_value[3];
};

class ParameterBool : public Parameter
{
public :
    ParameterBool(const std::string& attr_group_name, const std::string& name, bool attr_default_value) : Parameter(attr_group_name, name, ExternalRendererType::EXTR_TYPE_BOOL)
    {
        value = default_value = attr_default_value;
    }

    bool get_bool() { return value; }
    bool get_default_value() { return default_value; }

    bool value;
    bool default_value;
};

/********************* MATERIALS ***********************/

/**
 * Here we defined some materials with different parameters and evaluate functions.
 * The Shaders (Material, Light and Texture) are very similar so we will only detail the first one
 * Important : To use the shaders you need to create the class corresponding to a Clarisse Module and register this class (see : register_shaders in spherix_register_shaders.h)
 */

// This materiall will have 2 attributes one color and one boolean, according to the boolean it will return a different color
class SpherixMaterialDiffuse : public ExternalMaterialShader {
public :
    SpherixMaterialDiffuse() : ExternalMaterialShader("SpherixMaterialDiffuse", 2)
    {
        // Init the parameters
        // Those paremeters will be created when registering our shaders (see : register_shaders in spherix_register_shaders.h)
        // Also thanks to the function on attribute change and how the module are organized,
        // they will always be updated when the parameter value changed (see : on_attribute_change of the spherix_module_material.h for instance)

        // Color
        double default_value[3] = {1,0,0};
        parameters[0] = new ParameterColor("Shading", "color", default_value);
        parameters[0]->is_texturable = true;

        // Boolean
        parameters[1] = new ParameterBool("spherix_category", "spherix_boolean", false);
    }

    // Here we are evaluating the shader with the stored parameters that are automatically updated when they changed
    // Note : We are passing a ray_direction and a normal but we could add more arguments or remove them like in the SpherixLightDistant
    GMathVec3f evaluate(const double *ray_direction, const double *normal) const
    {
        // Get the parameters
        ParameterColor *attr_color = static_cast<ParameterColor *>(parameters[0]);
        ParameterBool  *attr_spherix_bool = static_cast<ParameterBool *>(parameters[1]);

        // Get the value parameters
        const double *color = attr_color->evaluate(ray_direction, normal);
        const bool spherix_bool = attr_spherix_bool->get_bool();

        GMathVec3f final_color;
        // Compute the final value with a simple shading
        if (spherix_bool) {
            final_color = GMathVec3f(color[0] * ray_direction[0], color[1] * ray_direction[1], color[2] * ray_direction[2]);
        } else {
            final_color = GMathVec3f(color[0], color[1], color[2]);
        }

        double dot = ray_direction[0] * normal[0] + ray_direction[1] * normal[1] + ray_direction[2] * normal[2];
        return fabs(dot) * final_color;
    }
};

class SpherixMaterialReflection : public ExternalMaterialShader {
public :
    SpherixMaterialReflection() : ExternalMaterialShader("SpherixMaterialReflection", 1)
    {
        // Color
        double default_value[3] = {1,1,1};
        parameters[0] = new ParameterColor("Shading", "color", default_value);
        parameters[0]->is_texturable = true;
    }

    virtual GMathVec3f evaluate(const double *ray_direction, const double *normal) const final
    {
        // Get the parameters
        ParameterColor *attr_color = static_cast<ParameterColor *>(parameters[0]);

        // Get the value parameters
        const double *color = attr_color->evaluate(ray_direction, normal);

        // Compute the final value
        const double dot = ray_direction[0] * normal[0] + ray_direction[1] * normal[1] + ray_direction[2] * normal[2];
        if (dot < -0.8) {
            return GMathVec3f(color[0], color[1], color[2]);
        } else {
            return GMathVec3f(0.0f);
        }
    }
};

/********************* LIGHTS *************************/
class SpherixLightDistant : public ExternalLightShader {
public :
    SpherixLightDistant() : ExternalLightShader("SpherixLightDistant", 2)
    {
        // Color
        double default_value[3] = {1,1,1};
        parameters[0] = new ParameterColor("Shading", "color", default_value);
        parameters[0]->is_texturable = true;

        // Intensity
        parameters[1] = new ParameterDouble("Shading", "intensity", 1);

        parameters[1]->enable_numeric_range = true;
        parameters[1]->min_numeric_range = 0.0;
        parameters[1]->max_numeric_range = 10000.0;

        parameters[1]->enable_numeric_ui_range = true;
        parameters[1]->min_numeric_ui_range = 0.0;
        parameters[1]->max_numeric_ui_range = 10.0;
    }

    virtual GMathVec3f evaluate() const final
    {
        // Get the parameters
        ParameterColor  *attr_color        = static_cast<ParameterColor *>(parameters[0]);
        ParameterDouble *attr_spherix_double = static_cast<ParameterDouble *>(parameters[1]);

        // Get the value parameters
        const double *color = attr_color->get_value();
        const double intensity = attr_spherix_double->get_value();

        return GMathVec3f(color[0], color[1], color[2]) * intensity;
    }
};

/********************* TEXTURES ***********************/

class SpherixTextureColor : public ExternalTextureShader {
public :
    SpherixTextureColor() : ExternalTextureShader("SpherixTextureColor", 1)
    {
        // Color
        double default_value[3] = {1,1,1};
        parameters[0] = new ParameterColor("Shading", "color", default_value);
    }

    virtual double *evaluate(const double *ray_direction, const double *normal) final
    {
        // Get the parameters
        ParameterColor  *attr_color = static_cast<ParameterColor *>(parameters[0]);

        // Get the value parameters
        const double *color = attr_color->get_value();
        value[0] = color[0];
        value[1] = color[1];
        value[2] = color[2];

        return value;
    }
};

/********************* RENDERER ***********************/

// Here we are simulating an external renderer that will raytrace and shade the object using the external shaders
// To simplify we are using Clarisse camera and Clarisse thread management

#include <of_app.h>
#include <sys_thread_lock.h>
#include <sys_thread_task_manager.h>
#include <r2c_render_buffer.h>
#include <spherix_render_delegate.h>

struct RenderData {
    RenderData(): region(0,0,0,0) {}
    // Sub-image related data
    unsigned int width;
    unsigned int height;
    R2cRenderBuffer::Region region;

    // Shading data
    GMathVec3f light_contribution;
    GMathVec3f background_color;

    // Buffers
    R2cRenderBuffer *render_buffer; // <-- used to interface with Clarisse image view
    float* buffer_ptr;

    // Camera
    const SpherixCamera *camera;
};

// Function that will raytrace the geometries and instancers (in this example we are doing the same for the geometries and instancers)
template<class OBJECT_INFO>
void raytrace_objects(const GMathRay& ray, const CoreArray<OBJECT_INFO>& infos, const SpherixResourceIndex& resources_index, double &closest_hit_t, GMathVec3d &closest_hit_normal, MaterialData& closest_hit_material)
{
    // Very simple linear raytracer, only supporting spheres
    for (auto object_info : infos) {
        const SpherixResourceInfo *resource_info = resources_index.is_key_exists(object_info.resource);
        // Transform ray to object space
        GMathMatrix4x4d transform = object_info.transform;
        transform.translate_right(resource_info->sphere.get_center());
        GMathMatrix4x4d inverse_transform;
        GMathRay transformed_ray;
        GMathMatrix4x4d::get_inverse(transform, inverse_transform);
        transformed_ray.transform(ray, inverse_transform);

        // Test intersection with sphere
        double t;
        GMathVec3d normal;
        if (resource_info->sphere.intersect(transformed_ray, t, normal)) {
            // If hit closer than closest, record hit infos
            if (t < closest_hit_t) {
                GMathMatrix4x4d inverse_transpose_transform;
                GMathMatrix4x4d::transpose(inverse_transform, inverse_transpose_transform);
                GMathVec3d transformed_normal;
                GMathMatrix4x4d::multiply(transformed_normal, normal, inverse_transpose_transform);

                closest_hit_t = t;
                closest_hit_normal = transformed_normal;
                closest_hit_material = object_info.material;
            }
        }
    }
}

// Multithread task to render a region of the image
class RenderRegionTask : public SysThreadTask {
public :
    RenderRegionTask(): progress(nullptr) {}

    void
    render_region(RenderData& render_data, const unsigned int& thread_id)
    {
        // Used to display a green box around the rendered region
        render_data.render_buffer->notify_start_render_region(render_data.region, true, thread_id);

        // Browse our image and for each pixel we compute a ray and raytrace the scene
        for (unsigned int pixel_y = 0; pixel_y < render_data.region.height; ++pixel_y) {
            for (unsigned int pixel_x = 0; pixel_x < render_data.region.width; ++pixel_x) {
                // Compute ray for the pixel [X, Y]
                GMathRay ray = render_data.camera->generate_ray(render_data.width,
                                                                render_data.height,
                                                                pixel_x + render_data.region.offset_x,
                                                                pixel_y + render_data.region.offset_y);

                GMathVec3f final_color = render_data.background_color;

                // Use this ray to raytrace the scene
                // If we hit something we take the color from the intersected material Sphere and multiply it per all the lights contribution
                // If nothing is hit we return the background renderer color
                double closest_hit_t = gmath_infinity;
                GMathVec3d closest_hit_normal;
                MaterialData closest_hit_material;

                // For simplicity, we handle instancers and geometries the same way
                raytrace_objects(ray, *geometries, *resources_index, closest_hit_t, closest_hit_normal, closest_hit_material);
                raytrace_objects(ray, *instancers, *resources_index, closest_hit_t, closest_hit_normal, closest_hit_material);

                if (closest_hit_t != gmath_infinity) {
                    // If the object doesn't have an assigned material, use default color
                    if (closest_hit_material.material) {
                        final_color = closest_hit_material.material->evaluate(ray.get_direction().get_data(), closest_hit_normal.get_data()) * render_data.light_contribution;
                    } else {
                        final_color = GMathVec3f(1.0f, 0.0f, 1.0f) * render_data.light_contribution;
                    }
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

    virtual void execution_entry(const unsigned int& id) {
        render_region(data, id);
        if (progress)
            progress->add_float(progress_increment);
    }


    RenderData data;
    const SpherixRenderDelegate *spherix_render_delegate;

    // To show the overall render progress
    CoreAtomic32 *progress;
    float progress_increment;

    // Objects
    const CoreArray<SpherixGeometryInfo> *geometries;
    const CoreArray<SpherixInstancerInfo> *instancers;
    const SpherixResourceIndex *resources_index;
};

/**
 * @brief The ExternalRenderer class used to declared a static method to raytrace the scene and render it into a render buffer
 */
class ExternalRenderer {
public :
    static void render(OfApp *application, const SpherixCamera& camera, const unsigned int image_width, const unsigned int image_height,
                const CoreArray<SpherixGeometryInfo>& geometries,
                const CoreArray<SpherixInstancerInfo>& instancers,
                const SpherixResourceIndex& resources_index,
                const CoreArray<SpherixLightInfo>& lights,
                const GMathVec3f& background_color,
                CoreAtomic32& progress,
                R2cRenderBuffer *render_buffer)
     {
        // Browse all the light in the scene and compute the light contribution (very simple lighting)
        GMathVec3f light_contribution = GMathVec3f(0.0f, 0.0f, 0.0f);
        for (const SpherixLightInfo& light_index : lights) {
            light_contribution += light_index.light_data.shader_light->evaluate();
        }

        // Creating render tasks
        // The bucket size needs to be 64x64 (this will be fix in the futur)
        const unsigned int task_w = gmath_min(64u, image_width);
        const unsigned int task_h = gmath_min(64u, image_height);
        const unsigned int bucket_count_x = (unsigned int)gmath_ceil((float)image_width / task_w);
        const unsigned int bucket_count_y = (unsigned int)gmath_ceil((float)image_height / task_h);
        const unsigned int task_count = bucket_count_x * bucket_count_y;
        const float progress_increment = 1.0f / task_count;

        // To use Clarisse's multi threading capabilities, we create a list of tasks
        // and feed them to the task manager
        // Our tasks only consists of a set of data, and a execution_entry() method.
        SysThreadTaskManager task_manager(&application->get_thread_manager());
        CoreVector<RenderRegionTask> tasks(task_count);

        // This should be created the least amount of times (when the image size is updated for example)
        float* image_buffer = new float[image_width * image_height * 4];
        float *next_buffer_entry = image_buffer;

        unsigned int task_id = 0;
        for(unsigned int j = 0; j < bucket_count_y; ++j) {
            const unsigned int offset_y = j * task_h;
            const unsigned int bucket_height = gmath_min(task_h, image_height - offset_y);
            for(unsigned int i = 0; i < bucket_count_x; ++i) {
                const unsigned int offset_x = i * task_w;
                const unsigned int bucket_width = gmath_min(task_w, image_width - offset_x);

                // Fill task data
                tasks[task_id].data.width = image_width;
                tasks[task_id].data.height = image_height;
                tasks[task_id].data.region = R2cRenderBuffer::Region(offset_x, offset_y, bucket_width, bucket_height);
                tasks[task_id].data.light_contribution = light_contribution;
                tasks[task_id].data.background_color = background_color;
                tasks[task_id].data.render_buffer = render_buffer;
                tasks[task_id].data.buffer_ptr = next_buffer_entry;
                tasks[task_id].data.camera = &camera;

                tasks[task_id].geometries = &geometries;
                tasks[task_id].instancers = &instancers;
                tasks[task_id].resources_index = &resources_index;

                tasks[task_id].progress = &progress;
                tasks[task_id].progress_increment = progress_increment;

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
};
