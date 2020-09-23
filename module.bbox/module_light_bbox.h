//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#pragma once

#include <module_light.h>

class BboxLight;

class OfObject;

/*! \class ModuleLightBbox
    \brief This class implements the Bbox Light abstract class in Clarisse. */
class ModuleLightBbox : public ModuleLight {
public:

    ModuleLightBbox();
    virtual ~ModuleLightBbox() override;

    /*! \brief return a Clarisse UI style name from the specified Bbox shader class name.
     * \param class_name Bbox shader class name. */
    static CoreString mangle_class(const CoreString& class_name);

private:

    ModuleLightBbox(const ModuleLightBbox&) = delete;
    ModuleLightBbox& operator=(const ModuleLightBbox&) = delete;

    DECLARE_CLASS
};

