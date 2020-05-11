//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef MODULE_RENDERER_REDSHIFT_H
#define MODULE_RENDERER_REDSHIFT_H

#include <module_project_item.h>

class OfObject;

/*! \class ModuleRendererRedshift
    \brief This class implements a small subset of Redshift render settings in Clarisse.
           This module a C++ interface to the RendererRedshift OfClass which exposes
           Redshift renderer specific attributes/properties. The role of the module
           is to implement what happends when users edit attributes of the renderer
           item in Clarisse. */
class ModuleRendererRedshift : public ModuleProjectItem { // FIXME: CLARISSEAPI should inherit from an abstract ModuleRenderer
public:

    ModuleRendererRedshift();
    virtual ~ModuleRendererRedshift() override;

    /*! \brief Synchronize Redshift renderer to the attributes of the actual renderer item
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

#endif
