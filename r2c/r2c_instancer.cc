//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <core_hash_table.h>
#include <sys_thread_lock.h>
#include <module_scene_object_tree.h>

#include "r2c_instancer.h"

class R2cInstancerImpl {
public:
    R2cInstancerImpl(const R2cSceneDelegate& delegate, R2cItemId item_id,
                          const CoreArray<OfObject *>& protos) : scene_delegate(&delegate), init(false), id(item_id), prototypes(protos) {}

    void initialize(R2cItemId id) {
        if (!init) {
            m_lock.lock();
            if (!init) {
                if (id != nullptr) {
                    OfObject *item = static_cast<OfObject *>(id);
                    CoreHashTable<OfObject *, unsigned int> indexmap;
                    // creating an index map
                    for (unsigned int i = 0; i < prototypes.get_count(); i++) indexmap.add(prototypes[i], i);
                    // populating the instancer
                    ModuleSceneObjectTree *sot = static_cast<ModuleSceneObjectTree *>(item->get_module());
                    CoreArray<ModuleSceneObjectGeometry> geometries;
                    // getting all instances flattened
                    sot->get_geometries(geometries, nullptr);
                    indices.resize(geometries.get_count());
                    matrices.resize(geometries.get_count());
                    // building internal array to abstract ModuleSceneObjectTree implementation
                    for (unsigned int i = 0; i < geometries.get_count(); i++) {
                        indices[i] = indexmap[geometries[i].get_scene_object()->get_object()];
                        matrices[i] = geometries[i].get_global_matrix();
                    }
                }
                init = true;
            }
            m_lock.unlock();
        }
    }

    const R2cSceneDelegate *scene_delegate;
    bool init;
    R2cItemId id;
    CoreArray<GMathMatrix4x4d> matrices;
    CoreArray<unsigned int> indices;
    const CoreArray<OfObject *>& prototypes;
    SysThreadLock m_lock;
};

R2cInstancer::R2cInstancer(const R2cSceneDelegate& delegate,
                                     R2cItemId id, const CoreArray<OfObject *>& prototypes) : m(new R2cInstancerImpl(delegate, id, prototypes))
{}

R2cInstancer::~R2cInstancer()
{
    delete m;
}

const CoreArray<R2cItemId>&
R2cInstancer::get_prototypes() const
{
    return *reinterpret_cast<const CoreArray<R2cItemId> *>(&m->prototypes);
}

const CoreArray<unsigned int>&
R2cInstancer::get_indices() const
{
    m->initialize(get_id());
    return m->indices;
}

const CoreArray<GMathMatrix4x4d>&
R2cInstancer::get_matrices() const
{
    m->initialize(get_id());
    return m->matrices;
}

R2cItemId
R2cInstancer::get_id() const
{
    return m->id;
}
