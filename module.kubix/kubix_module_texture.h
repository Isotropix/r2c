//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

// Clarisse includes
#include <module_texture_operator.h>

/*! \class ModuleTextureKubixCallbacks
    \brief declares custom callbacks for our own texture representation.*/
class ModuleTextureKubixCallbacks : public ModuleGlObjectCallbacks  {
public :

    ModuleTextureKubixCallbacks();

    virtual void init_callbacks(OfClassCallbacks& callbacks)
    {
        ModuleGlObjectCallbacks::init_callbacks(callbacks);
        ModuleTextureKubixCallbacks& cb = (ModuleTextureKubixCallbacks&)callbacks;
        cb.cb_evaluate = cb_evaluate;
    }

    typedef GMathVec3f (*EvaluateTextureCallback) (OfObject& object);
    EvaluateTextureCallback cb_evaluate;
};

/*! \class ModuleTextureKubix
    \brief This class implements the Texture abstract class in Clarisse. */
class ModuleTextureKubix : public ModuleTextureOperator {
public:
    GMathVec3f evaluate()
    {
        return get_callbacks<ModuleTextureKubixCallbacks>()->cb_evaluate(*get_object());
    }

private:
    DECLARE_CLASS
};
