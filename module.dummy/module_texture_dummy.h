//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

// Clarisse includes
#include <module_texture_operator.h>

/*! \class ModuleTextureDummyCallbacks
    \brief declares custom callbacks for our own texture representation.*/
class ModuleTextureDummyCallbacks : public ModuleGlObjectCallbacks  {
public :

    ModuleTextureDummyCallbacks();

    virtual void init_callbacks(OfClassCallbacks& callbacks)
    {
        ModuleGlObjectCallbacks::init_callbacks(callbacks);
        ModuleTextureDummyCallbacks& cb = (ModuleTextureDummyCallbacks&)callbacks;
        cb.cb_evaluate = cb_evaluate;
    }

    typedef GMathVec3f (*EvaluateTextureCallback) (OfObject& object, const GMathVec3f& ray_direction);
    EvaluateTextureCallback cb_evaluate;
};

/*! \class ModuleTextureDummy
    \brief This class implements the Texture abstract class in Clarisse. */
class ModuleTextureDummy : public ModuleTextureOperator {
public:
    GMathVec3f evaluate(const GMathVec3f& ray_direction)
    {
        return get_callbacks<ModuleTextureDummyCallbacks>()->cb_evaluate(*get_object(), ray_direction);
    }

private:
    DECLARE_CLASS
};
