//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//
#pragma once

#include <module_texture_operator.h>



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

    ModuleTextureDummy();
    virtual ~ModuleTextureDummy() override;

    GMathVec3f evaluate(const GMathVec3f& ray_direction) {
        return get_callbacks<ModuleTextureDummyCallbacks>()->cb_evaluate(*get_object(), ray_direction);
    }

    /*! \brief return a Clarisse UI style name from the specified Dummy shader class name.
     * \param class_name Dummy shader class name. */
    static CoreString mangle_class(const CoreString& class_name);

protected:

    void module_constructor(OfObject& object) override;

    /*! \brief Synchronizing Dummy texture shader to the new Clarisse attribute value when changed */
    void on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags) override;

private:

    DECLARE_CLASS
};
