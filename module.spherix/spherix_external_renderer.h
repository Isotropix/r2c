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

    virtual GMathVec3f evaluate(const double *ray_direction, const double *normal) {return GMathVec3f(0.0f);}
};

class ExternalLightShader : public ExternalShader {
public :
    ExternalLightShader() : ExternalShader() {}
    ExternalLightShader(std::string name, unsigned int parameter_count) : ExternalShader("LightSpherix", name, parameter_count) {}

    virtual GMathVec3f evaluate() { return GMathVec3f(0.0f); }
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
    GMathVec3f evaluate(const double *ray_direction, const double *normal)
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

    virtual GMathVec3f evaluate(const double *ray_direction, const double *normal) final
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

    virtual GMathVec3f evaluate() final
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
