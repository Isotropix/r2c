//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

#include <module_texture.h>

class OfObject;
class ExternalTextureShader;

/*! \class ModuleTextureSpherix
    \brief This class implements the Spherix Texture abstract class in Clarisse. The role
           of the module is to implement what happends when users edit attributes of the
           texture item in Clarisse. */
class ModuleTextureSpherix : public ModuleTexture {
public:

    ModuleTextureSpherix();
    virtual ~ModuleTextureSpherix() override;

    /*! \brief return the External shader attached to the Clarisse texture item.
     * \note The material is automatically synched. */
    inline ExternalTextureShader *get_texture() { return m_texture; }

protected:

    /*! \brief Create the external shader m_texture associated to the texture class in Clarisse
     *  Here we could so some connections to perform different task : See the Redshift example for more informations.
     * \param object Clarisse material instance. */
    void module_constructor(OfObject& object) override;
    /*! \brief Synchronizing External shader to the new Clarisse attribute value when changed */
    void on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags) override;

private:
    // Disable the = and copy operator
    ModuleTextureSpherix(const ModuleTextureSpherix&) = delete;
    ModuleTextureSpherix& operator=(const ModuleTextureSpherix&) = delete;

    // The actual External Shader
    ExternalTextureShader *m_texture;

    DECLARE_CLASS
};
