//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

#include <module_material.h>

class OfObject;

class ModuleMaterialDummyCallbacks : public ModuleMaterialCallbacks  {
public :
    ModuleMaterialDummyCallbacks();

    virtual void init_callbacks(OfClassCallbacks& callbacks)
    {
        ModuleMaterialCallbacks::init_callbacks(callbacks);
        ModuleMaterialDummyCallbacks& cb = (ModuleMaterialDummyCallbacks&)callbacks;
        cb.cb_shade = cb_shade;
    }

    typedef GMathVec3f (*ShadeCallback) (OfObject& object, const GMathVec3f& ray_dir, const GMathVec3f& normal);
    ShadeCallback cb_shade;
};

/*! \class ModuleMaterialDummy
    \brief This class implements the Bbox Material abstract class in Clarisse. */
class ModuleMaterialDummy : public ModuleMaterial {
public:

    ModuleMaterialDummy();
    virtual ~ModuleMaterialDummy() override;

    inline GMathVec3f shade(const GMathVec3f& ray_dir, const GMathVec3f& normal)
    {
        return get_callbacks<ModuleMaterialDummyCallbacks>()->cb_shade(*get_object(), ray_dir, normal);
    }

private:
    DECLARE_CLASS
};
