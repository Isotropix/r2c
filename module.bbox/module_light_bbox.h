//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#pragma once

#include <module_light.h>

class BboxLight;

class OfObject;

class ModuleLightBboxCallbacks : public ModuleLightCallbacks  {
public :

    ModuleLightBboxCallbacks();

    virtual void init_callbacks(OfClassCallbacks& callbacks)
    {
        ModuleSceneItemCallbacks::init_callbacks(callbacks);
        ModuleLightBboxCallbacks& cb = (ModuleLightBboxCallbacks&)callbacks;
        cb.cb_evaluate = cb_evaluate;
    }

    typedef GMathVec3f (*EvaluateLightCallback) (OfObject& object);
    EvaluateLightCallback cb_evaluate;
};

/*! \class ModuleLightBbox
    \brief This class implements the Bbox Light abstract class in Clarisse. */
class ModuleLightBbox : public ModuleLight {
public:

    ModuleLightBbox();
    virtual ~ModuleLightBbox() override;

    GMathVec3f evaluate()
    {
        return get_callbacks<ModuleLightBboxCallbacks>()->cb_evaluate(*get_object());
    }

    /*! \brief return a Clarisse UI style name from the specified Bbox shader class name.
     * \param class_name Bbox shader class name. */
    static CoreString mangle_class(const CoreString& class_name);

private:

    ModuleLightBbox(const ModuleLightBbox&) = delete;
    ModuleLightBbox& operator=(const ModuleLightBbox&) = delete;

    DECLARE_CLASS
};

