//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#pragma once

#include <module_light.h>

// Forward declaration
class OfObject;

/*! \class ModuleLightKubickCallbacks
    \brief declares custom callbacks for our own light representation */
class ModuleLightKubickCallbacks : public ModuleLightCallbacks  {
public :
    ModuleLightKubickCallbacks();

    virtual void init_callbacks(OfClassCallbacks& callbacks)
    {
        ModuleSceneItemCallbacks::init_callbacks(callbacks);
        ModuleLightKubickCallbacks& cb = (ModuleLightKubickCallbacks&)callbacks;
        cb.cb_evaluate = cb_evaluate;
    }

    typedef GMathVec3f (*EvaluateLightCallback) (OfObject& object);
    EvaluateLightCallback cb_evaluate;
};

/*! \class ModuleLightKubick
    \brief This class implements the Dummy Light abstract class in Clarisse. The role
           of the module is to implement what happends when users edit attributes of the
           light item in Clarisse. */
class ModuleLightKubick : public ModuleLight {
public:
    GMathVec3f evaluate()
    {
        return get_callbacks<ModuleLightKubickCallbacks>()->cb_evaluate(*get_object());
    }

private:
    ModuleLightKubick(const ModuleLightKubick&) = delete;
    ModuleLightKubick& operator=(const ModuleLightKubick&) = delete;

    DECLARE_CLASS
};

