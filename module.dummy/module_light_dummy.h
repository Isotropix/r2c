//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#pragma once

#include <module_light.h>

class BboxLight;

class OfObject;

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
    \brief This class implements the Bbox Light abstract class in Clarisse. */
class ModuleLightDummy : public ModuleLight {
public:

    ModuleLightDummy();
    virtual ~ModuleLightDummy() override;

    GMathVec3f evaluate()
    {
        return get_callbacks<ModuleLightDummyCallbacks>()->cb_evaluate(*get_object());
    }

    /*! \brief return a Clarisse UI style name from the specified Bbox shader class name.
     * \param class_name Bbox shader class name. */
    static CoreString mangle_class(const CoreString& class_name);

private:

    ModuleLightDummy(const ModuleLightDummy&) = delete;
    ModuleLightDummy& operator=(const ModuleLightDummy&) = delete;

    DECLARE_CLASS
};

