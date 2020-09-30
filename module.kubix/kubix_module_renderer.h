//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#pragma once

#include <module_renderer.h>

class OfObject;

/*! \class ModuleRendererKubix
    \brief This class implements a small subset of Kubix render settings in Clarisse.
           This module a C++ interface to the KubixRenderer OfClass which exposes
           Kubix renderer specific attributes/properties. The role of the module
           is to implement what happends when users edit attributes of the renderer
           item in Clarisse. */
class ModuleRendererKubix : public ModuleRenderer {
public:
    ModuleRendererKubix();
    const GMathVec3f get_background_color() { return m_background_color; }

protected:
    /*! \brief Event method called when a user modifies an attribute of the item
     *  \param attr The attribute that has been modified.
     *  \param dirtiness dirtiness type sent from the attribute.
     *  \param  dirtiness_flags additional dirtiness flags. */
    void on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags) override;

private:

    GMathVec3f m_background_color;
    DECLARE_CLASS
};
