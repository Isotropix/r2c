//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

// Clarisse includes
#include <of_app.h>
#include <of_class.h>
#include <of_class_factory.h>
#include <of_object_factory.h>

// Local includes
#include "./spherix_external_renderer.h"
#include "./spherix_module_texture.h"


/**
 * @brief The SpherixRegisterShaders class Is an helper that will allow to register the external shader to Clarisse
 */
class SpherixRegisterShaders {
public :

    /**
     * @brief register_shaders Register the external shaders to Clarisse
     * The registeration is performed when launching Clarisse (in our case when loading the module.spherix)
     * This is a crucial step and without it nothing will work
     * @param application
     */
    static void register_shaders(OfApp& application) {

        // Create a shader arrays that will be registered
        CoreVector<ExternalShader *> shaders;
        shaders.add(new SpherixMaterialDiffuse());
        shaders.add(new SpherixMaterialReflection());
        shaders.add(new SpherixLightDistant());
        shaders.add(new SpherixTextureColor());

        // Create the classes and attributes associated to the shaders
        for (const ExternalShader *shader : shaders)
        {
            // Get the base class, this is the Clarisse class from which the shader inherits.
            // It is important to have this class to create the callbacks
            OfClass *base_class = application.get_factory().get_classes().get(CoreString(shader->ext_class_base_name.data()));

            // Create the actual class
            OfClass *class_shader = application.get_factory().get_classes().add(CoreString(shader->class_name.data()), CoreString(shader->ext_class_base_name.data()));

            // Set the callbacks (important, not setting the callbacks will lead to a crash)
            class_shader->set_callbacks(base_class->get_callbacks());

            // Parse the shader attributes and add them the the Clarisse class
            for (const Parameter *param : shader->parameters) {
                // Create the corresponding Clarisse attributes
                create_attribute_from_definition(param, class_shader);
            }

            delete shader;
        }

    }

    static void create_attribute_from_definition(const Parameter *param, OfClass *class_shader)
    {
        switch (param->type) {
            // Basic types
            case ExternalRendererType::EXTR_TYPE_DOUBLE :
            {
                const ParameterDouble *param_double = static_cast<const ParameterDouble *>(param);

                OfAttr *attr = class_shader->add_attribute(CoreString(param->name.data()), OfAttr::TYPE_DOUBLE, OfAttr::CONTAINER_SINGLE, OfAttr::VISUAL_HINT_SCALE, CoreString(param->group_name.data()));
                attr->set_value_count(1);

                // Set the default value
                const double default_value = param_double->get_default_value();
                attr->set_double(default_value);

                // Set the numeric range (the attribute value will be clamped between [param_double->min_numeric_range, param_double->max_numeric_range])
                if (param_double->enable_numeric_range) {
                    attr->enable_range(true);
                    attr->set_numeric_range(param_double->min_numeric_range, param_double->max_numeric_range);
                }

                // Set the ui numeric range (the slider will be clamped between [param_double->min_numeric_ui_range, param_double->max_numeric_ui_range])
                if (param_double->enable_numeric_ui_range) {
                    attr->enable_ui_range(true);
                    attr->set_numeric_ui_range(param_double->min_numeric_ui_range, param_double->max_numeric_ui_range);
                }

                // Set the attribut texturable (it allows to set a texture using the little checker next to the attribute)
                attr->set_texturable(param_double->is_texturable);

            }
                break;
            case ExternalRendererType::EXTR_TYPE_COLOR :
            {
                const ParameterColor *param_color = static_cast<const ParameterColor *>(param);

                OfAttr *attr = class_shader->add_attribute(CoreString(param->name.data()), OfAttr::TYPE_DOUBLE, OfAttr::CONTAINER_ARRAY, OfAttr::VISUAL_HINT_RGB, CoreString(param->group_name.data()));
                attr->set_value_count(3);

                // Set the default value
                const double *default_value = param_color->get_default_value();
                attr->set_double(default_value[0], 0);
                attr->set_double(default_value[1], 1);
                attr->set_double(default_value[2], 2);

                attr->set_texturable(param_color->is_texturable);

            }
                break;
            case ExternalRendererType::EXTR_TYPE_BOOL :
            {
                OfAttr *attr = class_shader->add_attribute(CoreString(param->name.data()), OfAttr::TYPE_BOOL, OfAttr::CONTAINER_SINGLE, OfAttr::VISUAL_HINT_DEFAULT, CoreString(param->group_name.data()));
                attr->set_value_count(1);
            }
                break;
        default :
            CORE_ASSERT(false);
        }
    }
};
