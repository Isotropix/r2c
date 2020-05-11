//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef __ModuleLightRedshift_H__
#define __ModuleLightRedshift_H__

#include <module_light.h>

class RSLight;

class OfObject;

/*! \class ModuleLightRedshift
    \brief This class implements the Redshift Light abstract class in Clarisse. */
class ModuleLightRedshift : public ModuleLight {
public:

    ModuleLightRedshift();
    virtual ~ModuleLightRedshift() override;

    /*! \brief return a Clarisse UI style name from the specified Redshift shader class name.
     * \param class_name Redshift shader class name. */
    static CoreString mangle_class(const CoreString& class_name);

private:

    ModuleLightRedshift(const ModuleLightRedshift&) = delete;
    ModuleLightRedshift& operator=(const ModuleLightRedshift&) = delete;

    DECLARE_CLASS
};

#endif
