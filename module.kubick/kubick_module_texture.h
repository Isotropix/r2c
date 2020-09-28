//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

// Clarisse includes
#include <module_texture_operator.h>

/*! \class ModuleTextureKubickCallbacks
    \brief declares custom callbacks for our own texture representation.*/
class ModuleTextureKubickCallbacks : public ModuleGlObjectCallbacks  {
public :

    ModuleTextureKubickCallbacks();

    virtual void init_callbacks(OfClassCallbacks& callbacks)
    {
        ModuleGlObjectCallbacks::init_callbacks(callbacks);
        ModuleTextureKubickCallbacks& cb = (ModuleTextureKubickCallbacks&)callbacks;
        cb.cb_evaluate = cb_evaluate;
    }

    typedef GMathVec3f (*EvaluateTextureCallback) (OfObject& object);
    EvaluateTextureCallback cb_evaluate;
};

/*! \class ModuleTextureKubick
    \brief This class implements the Texture abstract class in Clarisse. */
class ModuleTextureKubick : public ModuleTextureOperator {
public:
    GMathVec3f evaluate()
    {
        return get_callbacks<ModuleTextureKubickCallbacks>()->cb_evaluate(*get_object());
    }

private:
    DECLARE_CLASS
};
