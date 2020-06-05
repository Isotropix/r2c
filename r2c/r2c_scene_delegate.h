//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef R2C_SCENE_DELEGATE_H
#define R2C_SCENE_DELEGATE_H

#include <event_object.h>
#include <gmath_matrix4x4.h>
#include <r2c_common.h>

class R2cInstancer;
class ModuleSceneObject;
class SceneObjectShading;

typedef CoreHashTable<OfObject *, bool> OfObjectIndex;
typedef CoreHashTable<R2cItemId, R2cItemDescriptor> RenderItemDependencyMap;
typedef CoreHashTable<OfObject *, CoreArray<OfObject *> *> InstancerPrototypesMap;
typedef CoreHashTable<ModuleSceneObject *, unsigned int> SceneObjectIndexMap;

/*! \class R2cSceneDelegate
    \brief This class implements a Clarisse Scene Delegate to work along a R2cRenderDelegate which must set using R2cSceneDelegate::set_render_delegate.
    \note Make sure to first call R2cSceneDelegate::initialize. */
class R2C_EXPORT R2cSceneDelegate : public EventObject {
public:

    //! Specifies which type of modifications is propagated through the dependency graph.
    enum Dirtiness {
        DIRTINESS_NONE                      = 0,        //!< No dirtiness
        DIRTINESS_KINEMATIC                 = 1 << 0,   //!< The transformation matrix has been modified.
        DIRTINESS_LIGHT                     = 1 << 1,   //!< The light shader has been modified.
        DIRTINESS_SHADING_GROUP             = 1 << 2,   //!< The shading groups of the geometry have been modified
        DIRTINESS_MATERIAL                  = 1 << 3,   //!< The material has been modified
        DIRTINESS_VISIBILITY                = 1 << 4,   //!< The item visibility has been modified
        DIRTINESS_GEOMETRY                  = 1 << 5,   //!< The geometry has been modified
        DIRTINESS_ALL                       = (1 << 6) - 1,
        DIRTINESS_COUNT                     = 8
    };
    static CoreString get_dirtiness_name(const Dirtiness& dirtiness);
    static void get_dirtiness_names(CoreVector<CoreString>& names, const int& dirtiness);

    /*! \brief Sets the render camera which must inherit from the class Camera
        \param camera defines the render viewpoint
        \note The input must inherit Camera otherwise the camera is set to nullptr */
    inline void set_camera(OfObject *camera) { set_input(&m_camera, camera, "Camera"); }

    /*! \brief Returns the render camera */
    inline R2cItemDescriptor get_camera() const { return get_item_descriptor(m_camera);}

    /*! \brief Sets the render settings which must inherit from the class Renderer
        \param render_settings defines the render_settins used in the scene. It should be a class known by the render delegate
        \note The input must inherit Renderer otherwise the renderer is set to nullptr */
    inline void set_render_settings(OfObject *render_settings) { set_input(&m_render_settings, render_settings, "Renderer"); }

    /*! \brief Returns the render settings */
    inline R2cItemDescriptor get_render_settings() const { return get_item_descriptor(m_render_settings);}

    /*! \brief Sets the geometries of the scene which should include SceneObjects
        \param geometries defines the clarisse group that sets the geometries of the scene
        \note The input must inherit from Group otherwise geometries is set to nullptr */
    inline void set_geometries(OfObject *geometries) { set_input(&m_geometries, geometries, "Group"); }

    /*! \brief Returns the group defining the scene. */
    inline R2cItemDescriptor get_geometries() const { return get_item_descriptor(m_geometries);}

    /*! \brief Sets the group of lights that are used in the scene.
        \param lights defines the clarisse group defining the lights of the scene.
        \note The input must inherit from Group otherwise lights is set to nullptr */
    inline void set_lights(OfObject *lights) { set_input(&m_lights, lights, "Group"); }

    /*! \brief Returns the group defining the lights of the scene. */
    inline R2cItemDescriptor get_lights() const { return get_item_descriptor(m_lights);}

    /*! \brief Sets the shading layer to define material association override
        \param shading_layer defines the shading layer that defines material association
        \note The input must inherit from ShadingLayer otherwise the shading layer is set to nullptr */
    inline void set_shading_layer(OfObject *shading_layer) { set_input(&m_shading_layer, shading_layer, "ShadingLayer"); }

    /*! \brief Returns the associated shading layer. */
    inline R2cItemDescriptor get_shading_layer() const { return get_item_descriptor(m_shading_layer);}

    /*! \brief Returns the render item which can be a light, geometry, instancer etc... */
    R2cItemDescriptor get_render_item(R2cItemId id) const;

    /*! \brief Returns the global transform of the specified item */
    const GMathMatrix4x4d& get_transform(R2cItemId id) const;

    /*! \brief Returns the if the render item is visible */
    bool get_visible(R2cItemId id) const;

    /*! \brief Generate an instancer description. Should call destroy_instancer when finished.
     *  \param id id of the instancer
     *  \param flattened set if the instancer should flatten the hierarchy of nested instancers.
     *  \note  Flattening a deep hierarchy of instances can use quite a lot of memory. Make sure
     * to flatten only when needed (case the render delegate doesn't support nested instancing).
     *  \note Make sure to call destroy_instancer_description. */
    R2cInstancer *create_instancer_description(R2cItemId id, bool flattened = true) const;
    /*! \brief Destroy an instancer description previously created using create_instancer_description.*/
    void destroy_instancer_description(R2cInstancer *instancer) const;

    /*! \brief Return the prototypes associoted with the specified instancer.
     *  \note return an empty array if the specified id is invalid. */
    const CoreArray<R2cItemId>& get_prototypes(R2cItemId id, bool flattened = true) const;

    /*! \brief Returns the geometry resource associated with the item. */
    R2cGeometryResource get_geometry_resource(R2cItemId id) const;

    /*! \brief Returns the shading group info associated to the specified scene object.
     * The shading group info gathers all information such as which material, displacement or clip map
     * texture are bound to the shading group of the geometry.
     * The number of shading group is defined by the R2cGeometryResource. However,
     * in the event of an instancer the number of shading groups is the sum of the
     * shading groups of the prototypes. The index matches the order of the prototypes as they appear
     * in the array returned by R2cSceneDelegate::get_prototypes such as if
     * prototypes[0] defines 5 shading groups, index between [0, 4] are the ones related
     * to prototypes[0]. The first shading group index of prototypes[1] will then be 5.
     *  \param id id of the geometry/instancer
     *  \param shading_group_id Index to the shading group defined by the geometry. */
    R2cShadingGroupInfo get_shading_group_info(R2cItemId id, const unsigned int& shading_group_id) const;

    /*! \brief Returns the render delegate associated with the scene.
     *  \note Can be nullptr if no render delegate was set. */
    R2cRenderDelegate *get_render_delegate() const { return m_render_delegate; }

    /*! \brief Associate the specified delegate with the scene delegate.
     *  \param render_delegate Render delegate to attach the scene to.
     *  \note If a previous render delegate was associated, it gets automatically cleared before being unattached. */
    void set_render_delegate(R2cRenderDelegate *render_delegate);

    /*! \brief Return the current application */
    OfApp& get_application() const;

    /*! \brief Synchronize the scene descriptor with the Clarisse render scene.
     * \note Must be called before calling R2cRenderDelegate::render */
    void sync();

    /*! \brief Returns true is initialize has been properly called */
    static bool is_initialized();
    /*! \brief Performs some internal initializations
     * \note Must be called before creating any instances of the class */
    static void initialize(OfApp& application);
    /*! \brief Returns a new Clarisse Scene Delegate
     * \note You must have called R2cSceneDelegate::initialize otherwise it will return 0 */
    static R2cSceneDelegate *create();
    /*! \brief Destroy the given R2cSceneDelegate*/
    static void destroy(R2cSceneDelegate *delegate);
    /*! \brief Returns a new Clarisse Scene Delegate
     * \note You must have called R2cSceneDelegate::initialize otherwise it will return 0 */
    static const R2cSceneDelegate *get_null();

private:

    R2cSceneDelegate();
    R2cSceneDelegate(const R2cSceneDelegate&) = delete;
    R2cSceneDelegate& operator=(const R2cSceneDelegate&) = delete;
    virtual ~R2cSceneDelegate();

    void clear(); // called when setting Render Delegate

    // helpers

    void remove_geometry(R2cItemId id);
    void insert_geometry(OfObject *geometry);

    void remove_light(R2cItemId id);
    void insert_light(OfObject *light);

    void set_input(OfObject **m_input, OfObject *new_input, const CoreString& class_name);
    void sync_index(OfObject *item, OfObjectIndex& index, const CoreVector<const OfClass *>& supported_classes, CoreVector<OfObject *>& added_items, CoreVector<OfObject *>& removed_items);

    /*!\brief Events method to react to scene changes */
    void on_input_destroyed(EventObject& sender, const EventInfo& evtid, void *data);
    void on_geometries_group_update(EventObject& sender, const EventInfo& evtid, void *data);
    void on_lights_group_update(EventObject& sender, const EventInfo& evtid, void *data);
    void on_dependency_destroyed(EventObject& sender, const EventInfo& evtid, void *data);

    void dirty_geometry_index();
    void dirty_light_index();
    void dirty_shading();
    void dirty_shading_table();
    void dirty_all();

    /*!\brief Dispatching Clarisse dirtiness to the render delegate */
    void dispatch_scene_object_dirtiness(EventObject& sender, const EventInfo& evtid, void *data);
    void dispatch_light_dirtiness(EventObject& sender, const EventInfo& evtid, void *data);
    void dispatch_shading_layer_dirtiness(EventObject& sender, const EventInfo& evtid, void *data);

    R2cRenderDelegate *m_render_delegate;

    // helper to return the descriptor from the given member;
    R2cItemDescriptor get_item_descriptor(OfObject *item) const;
    OfObject *m_render_settings;
    OfObject *m_camera;
    OfObject *m_geometries;
    OfObject *m_lights;
    OfObject *m_shading_layer;

    // define all items dependencies used by the renderer (geometries, lights, prototypes)
    // whereas they are directly visible or not
    RenderItemDependencyMap m_render_item_dependencies;

    // define which high level geometries are rendered
    OfObjectIndex m_geometry_index;
    bool m_geometry_index_dirty;

    // define which lights are rendered
    OfObjectIndex m_light_index;
    bool m_light_index_dirty;

    // list of propotypes per instancer
    InstancerPrototypesMap m_instancer_prototypes;

    SceneObjectIndexMap m_scene_object_index;
    const SceneObjectShading *m_shading_table;
    bool m_shading_table_dirty;

    // Clarisse classes supported by the render delegate
    struct {
        CoreVector<const OfClass *> cameras;
        CoreVector<const OfClass *> lights;
        CoreVector<const OfClass *> geometries;
        CoreVector<const OfClass *> materials;
    } m_supported_classes;

    DECLARE_CLASS
};

#endif
