//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#pragma once

#include <module_renderer.h>

class OfObject;

/*! \class ModuleRendererBbox
    \brief This class implements a small subset of Bbox render settings in Clarisse.
           This module a C++ interface to the RendererBbox OfClass which exposes
           Bbox renderer specific attributes/properties. The role of the module
           is to implement what happends when users edit attributes of the renderer
           item in Clarisse. */
class ModuleRendererBbox : public ModuleRenderer {
public:

    ModuleRendererBbox();
    virtual ~ModuleRendererBbox() override;

    /*! \brief Synchronize Bbox renderer to the attributes of the actual renderer item
     *  \param sampling_quality global sampling multiplier.
     *  \note  The multiplier should affect all sampling values so that a sampling_quality of 0.0 should result to 1 spp. */
    void sync(const float& sampling_quality);

protected:

    /*! \brief Event method called when a user modifies an attribute of the item
     *  \param attr The attribute that has been modified.
     *  \param dirtiness dirtiness type sent from the attribute.
     *  \param  dirtiness_flags additional dirtiness flags. */
    void on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags) override;

private:

    DECLARE_CLASS
};
