//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#pragma once

#include <module_light.h>

// Forward declaration
class OfObject;

/*! \class ModuleLightDummyCallbacks
    \brief declares custom callbacks for our own light representation */
class ModuleLightDummyCallbacks : public ModuleLightCallbacks  {
public :
    ModuleLightDummyCallbacks();

    virtual void init_callbacks(OfClassCallbacks& callbacks)
    {
        ModuleSceneItemCallbacks::init_callbacks(callbacks);
        ModuleLightDummyCallbacks& cb = (ModuleLightDummyCallbacks&)callbacks;
        cb.cb_evaluate = cb_evaluate;
    }

    typedef GMathVec3f (*EvaluateLightCallback) (OfObject& object);
    EvaluateLightCallback cb_evaluate;
};

/*! \class ModuleLightDummy
    \brief This class implements the Dummy Light abstract class in Clarisse. The role
           of the module is to implement what happends when users edit attributes of the
           light item in Clarisse. */
class ModuleLightDummy : public ModuleLight {
public:
    GMathVec3f evaluate()
    {
        return get_callbacks<ModuleLightDummyCallbacks>()->cb_evaluate(*get_object());
    }

private:
    ModuleLightDummy(const ModuleLightDummy&) = delete;
    ModuleLightDummy& operator=(const ModuleLightDummy&) = delete;

    DECLARE_CLASS
};

