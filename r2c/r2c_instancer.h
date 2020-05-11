//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef R2C_INSTANCER_H
#define R2C_INSTANCER_H

#include <gmath_matrix4x4.h>
#include <r2c_common.h>

class R2cSceneDelegate;
class R2cInstancerImpl;

/*! \class R2cInstancer
 *  \brief This class implements an instancer which wraps an SceneObjectTree.
 *  \note The internal representation of the instancer is flatten down for now.
 *        It is created using R2cSceneDelegate::create_instancer_description and
 *        destroyed with R2cSceneDelegate::destroy_instancer_description.
 *        Please also note that the flattened version of a instancer can allocate
 *        a large amount of memory when representing deep nested hierarchy of instances. */
class R2C_EXPORT R2cInstancer {
public:

    /*! \brief Returns the array of prototypes used by the instancer. */
    const CoreArray<R2cItemId>& get_prototypes() const;
    /*! \brief Returns an array of index where the index is indexing a resource of the instancer.*/
    const CoreArray<unsigned int>& get_indices() const;
    /*! \brief Returns an array of global matrices defining the transformation of each instance.*/
    const CoreArray<GMathMatrix4x4d>& get_matrices() const;

    /*! \brief Return the id of the instancer.*/
    R2cItemId get_id() const;

private:

    friend class R2cSceneDelegate;
    R2cInstancer(const R2cSceneDelegate& desc, R2cItemId id, const CoreArray<OfObject *>& prototypes);
    ~R2cInstancer();
    R2cInstancer(const R2cInstancer&) = delete;
    R2cInstancer& operator=(const R2cInstancer&) = delete;
    R2cInstancerImpl *m;
};

#endif
