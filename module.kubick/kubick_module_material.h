//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

#include <module_material.h>

class OfObject;

/*! \class ModuleMaterialKubickCallbacks
    \brief declares custom callbacks for our own material representation.*/
class ModuleMaterialKubickCallbacks : public ModuleMaterialCallbacks  {
public :
    ModuleMaterialKubickCallbacks();

    virtual void init_callbacks(OfClassCallbacks& callbacks)
    {
        ModuleMaterialCallbacks::init_callbacks(callbacks);
        ModuleMaterialKubickCallbacks& cb = (ModuleMaterialKubickCallbacks&)callbacks;
        cb.cb_shade = cb_shade;
    }

    typedef GMathVec3f (*ShadeCallback) (OfObject& object, const GMathVec3f& ray_dir, const GMathVec3f& normal);
    ShadeCallback cb_shade;
};

/*! \class ModuleMaterialKubick
    \brief This class implements the Dummy Material abstract class in Clarisse. The role
           of the module is to implement what happends when users edit attributes of the
           material item in Clarisse. */
class ModuleMaterialKubick : public ModuleMaterial {
public:
    inline GMathVec3f shade(const GMathVec3f& ray_dir, const GMathVec3f& normal)
    {
        return get_callbacks<ModuleMaterialKubickCallbacks>()->cb_shade(*get_object(), ray_dir, normal);
    }

private:
    ModuleMaterialKubick(const ModuleMaterialKubick&) = delete;
    ModuleMaterialKubick& operator=(const ModuleMaterialKubick&) = delete;

    DECLARE_CLASS
};
