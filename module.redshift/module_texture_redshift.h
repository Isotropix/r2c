//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef MODULE_TEXTURE_REDSHIFT_H
#define MODULE_TEXTURE_REDSHIFT_H

#include <module_texture_operator.h>

class RSShaderNode;

/*! \class ModuleTextureRedshift
    \brief This class implements the Redshift Texture abstract class in Clarisse. */
class ModuleTextureRedshift : public ModuleTextureOperator {
public:

	ModuleTextureRedshift();
    virtual ~ModuleTextureRedshift() override;

    /*! \brief return the Redshift shader attached to the Clarisse texture item.
     * \note The shader is automatically synched. */
    inline RSShaderNode *get_shader() { return m_shader; }

    /*! \brief return a Clarisse UI style name from the specified Redshift shader class name.
     * \param class_name Redshift shader class name. */
    static CoreString mangle_class(const CoreString& class_name);

protected:

    void module_constructor(OfObject& object) override;

    /*! \brief Synchronizing Redshift texture shader to the new Clarisse attribute value when changed */
    void on_attribute_change(const OfAttr& attr, int& dirtiness, const int& dirtiness_flags) override;

private:

	ModuleTextureRedshift(const ModuleTextureRedshift&) = delete;
	ModuleTextureRedshift& operator=(const ModuleTextureRedshift&) = delete;

    CoreString m_shader_class_name;
	RSShaderNode *m_shader;
    DECLARE_CLASS
};

#endif
