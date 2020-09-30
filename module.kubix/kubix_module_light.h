//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#pragma once

#include <module_light.h>

// Forward declaration
class OfObject;

/*! \class ModuleLightKubixCallbacks
    \brief declares custom callbacks for our own light representation */
class ModuleLightKubixCallbacks : public ModuleLightCallbacks  {
public :
    ModuleLightKubixCallbacks();

    virtual void init_callbacks(OfClassCallbacks& callbacks)
    {
        ModuleSceneItemCallbacks::init_callbacks(callbacks);
        ModuleLightKubixCallbacks& cb = (ModuleLightKubixCallbacks&)callbacks;
        cb.cb_evaluate = cb_evaluate;
    }

    typedef GMathVec3f (*EvaluateLightCallback) (OfObject& object);
    EvaluateLightCallback cb_evaluate;
};

/*! \class ModuleLightKubix
    \brief This class implements the Kubix Light abstract class in Clarisse. The role
           of the module is to implement what happends when users edit attributes of the
           light item in Clarisse. */
class ModuleLightKubix : public ModuleLight {
public:
    GMathVec3f evaluate()
    {
        return get_callbacks<ModuleLightKubixCallbacks>()->cb_evaluate(*get_object());
    }

private:
    ModuleLightKubix(const ModuleLightKubix&) = delete;
    ModuleLightKubix& operator=(const ModuleLightKubix&) = delete;

    DECLARE_CLASS
};

