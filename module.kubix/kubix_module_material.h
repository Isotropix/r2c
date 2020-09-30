//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

#include <module_material.h>

class OfObject;

/*! \class ModuleMaterialKubixCallbacks
    \brief declares custom callbacks for our own material representation.*/
class ModuleMaterialKubixCallbacks : public ModuleMaterialCallbacks  {
public :
    ModuleMaterialKubixCallbacks();

    virtual void init_callbacks(OfClassCallbacks& callbacks)
    {
        ModuleMaterialCallbacks::init_callbacks(callbacks);
        ModuleMaterialKubixCallbacks& cb = (ModuleMaterialKubixCallbacks&)callbacks;
        cb.cb_shade = cb_shade;
    }

    typedef GMathVec3f (*ShadeCallback) (OfObject& object, const GMathVec3f& ray_dir, const GMathVec3f& normal);
    ShadeCallback cb_shade;
};

/*! \class ModuleMaterialKubix
    \brief This class implements the Kubix Material abstract class in Clarisse. The role
           of the module is to implement what happends when users edit attributes of the
           material item in Clarisse. */
class ModuleMaterialKubix : public ModuleMaterial {
public:
    inline GMathVec3f shade(const GMathVec3f& ray_dir, const GMathVec3f& normal)
    {
        return get_callbacks<ModuleMaterialKubixCallbacks>()->cb_shade(*get_object(), ray_dir, normal);
    }

private:
    ModuleMaterialKubix(const ModuleMaterialKubix&) = delete;
    ModuleMaterialKubix& operator=(const ModuleMaterialKubix&) = delete;

    DECLARE_CLASS
};
