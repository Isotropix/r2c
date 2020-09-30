//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#pragma once

#include <module_light.h>

// Forward declaration
class OfObject;
class ExternalLightShader;

/*! \class ModuleLightSpherix
    \brief This class implements the Light Material abstract class in Clarisse. The role
           of the module is to implement what happends when users edit attributes of the
           light item in Clarisse. */
class ModuleLightSpherix : public ModuleLight {
public:

    ModuleLightSpherix();
    virtual ~ModuleLightSpherix() override;

    /*! \brief return the External shader attached to the Clarisse material item.
     * \note The material is automatically synched. */
    inline ExternalLightShader *get_light() { return m_light; }

protected:

    /*! \brief Create the external shader m_material associated to the material class in Clarisse
     *  Here we could so some connections to perform different task : See the Redshift example for more informations.
     * \param object Clarisse material instance. */
    void module_constructor(OfObject& object) override;
    /*! \brief Synchronizing External shader to the new Clarisse attribute value when changed */
    void on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags) override;

private:
    // Disable the = and copy operator
    ModuleLightSpherix(const ModuleLightSpherix&) = delete;
    ModuleLightSpherix& operator=(const ModuleLightSpherix&) = delete;

    // The actual External Shader
    ExternalLightShader *m_light;

    DECLARE_CLASS
};
