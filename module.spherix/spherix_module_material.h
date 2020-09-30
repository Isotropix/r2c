//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

#include <module_material.h>

class OfObject;
class ExternalMaterialShader;

/*! \class ModuleMaterialSpherix
    \brief This class implements the Spherix Material abstract class in Clarisse. The role
           of the module is to implement what happends when users edit attributes of the
           material item in Clarisse. */
class ModuleMaterialSpherix : public ModuleMaterial {
public:

    ModuleMaterialSpherix();
    virtual ~ModuleMaterialSpherix() override;

    /*! \brief return the External shader attached to the Clarisse material item.
     * \note The material is automatically synched. */
    inline ExternalMaterialShader *get_material() { return m_material; }

protected:

    /*! \brief Create the external shader m_material associated to the material class in Clarisse
     *  Here we could so some connections to perform different task : See the Redshift example for more informations.
     * \param object Clarisse material instance. */
    void module_constructor(OfObject& object) override;
    /*! \brief Synchronizing External shader to the new Clarisse attribute value when changed */
    void on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags) override;

private:
    // Disable the = and copy operator
    ModuleMaterialSpherix(const ModuleMaterialSpherix&) = delete;
    ModuleMaterialSpherix& operator=(const ModuleMaterialSpherix&) = delete;

    // The actual External Shader
    ExternalMaterialShader *m_material;

    DECLARE_CLASS
};
