//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

#include <module_material.h>

class OfObject;

class ModuleMaterialBboxCallbacks : public ModuleMaterialCallbacks  {
public :
    ModuleMaterialBboxCallbacks();

    virtual void init_callbacks(OfClassCallbacks& callbacks)
    {
        ModuleMaterialCallbacks::init_callbacks(callbacks);
        ModuleMaterialBboxCallbacks& cb = (ModuleMaterialBboxCallbacks&)callbacks;
        cb.cb_shade = cb_shade;
    }

    typedef GMathVec3f (*ShadeCallback) (OfObject& object);
    ShadeCallback cb_shade;
};

/*! \class ModuleMaterialBbox
    \brief This class implements the Bbox Material abstract class in Clarisse. */
class ModuleMaterialBbox : public ModuleProjectItem {
public:

    ModuleMaterialBbox();
    virtual ~ModuleMaterialBbox() override;

    inline GMathVec3f shade()
    {
        return get_callbacks<ModuleMaterialBboxCallbacks>()->cb_shade(*get_object());
    }

private:
    DECLARE_CLASS
};
