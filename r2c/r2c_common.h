//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef R2C_COMMON_H
#define R2C_COMMON_H

#include <r2c_export.h>
#include <core_string.h>

class OfApp;
class OfClass;
class OfObject;
class ResourceData;
class GeometryObject;

class R2cRenderDelegate;
class R2cItemDescriptor;
class R2cSceneDelegate;


typedef void *R2cItemId; /**< Define an id to a Clarisse item */
typedef ResourceData *R2cResourceId; /**< Define an id to a Clarisse Resource */

/*! \class R2cGeometryResource
    \brief This class implements a geometry resource which wraps a GeometryObject. */
class R2C_EXPORT R2cGeometryResource {
public:

    R2cGeometryResource() : m_id(nullptr) {}
    /*! \brief Return the resource ID
     *  \note When the ID is null, the resource is null */
    inline R2cResourceId get_id() { return m_id; }
    /*! \brief Return true if the resource is null */
    inline bool is_null() { return m_id == nullptr; }
    /*! \brief Return the geometry associated with the resource
     *  \note May return nullptr if the resource is null */
    const GeometryObject *get_geometry() const;

private:

    friend class R2cSceneDelegate;
    friend class R2cInstancer;

    R2cGeometryResource(R2cResourceId id) : m_id(id) {}
    inline void set_id(R2cResourceId id) { m_id = id; }

    R2cResourceId m_id;
};

/*! \class R2cItemDescriptor
    \brief This class implements an item Descriptor which is a handle to an OfObject returned by a R2cSceneDelegate.
    \note  The life time of the descriptor can be pretty short. Their life cycle should be considered valid
           only within the call of a method in the render delegate. The good practice is to store instead R2cItemId
           which are ids identifying a unique item of the scene. They can be easily obtained using R2cItemDescriptor::get_id.
           These ids can be used in hash maps etc... Simply use R2cSceneDelegate::get_render_item to retreive a valid R2cItemDescriptor
           from the R2cItemId*/
class R2C_EXPORT R2cItemDescriptor {
public:

    //!\brief Specifies the type of the item.
    enum Type {
        TYPE_INSTANCER,     //!< \brief The item is an instancer (inherits from SceneObjectTree)
        TYPE_GEOMETRY,      //!< \brief The item is a renderable geometry (inherits from SceneObject)
        TYPE_LIGHT,         //!< \brief The item is a light (inherits from Light)
        TYPE_MATERIAL,      //!< \brief The item is a material (inherits from Material)
        TYPE_CAMERA,        //!< \brief The item is a camera (inherits from Camera)
        TYPE_GROUP,         //!< \brief The item is a group (collection of items)
        TYPE_UNKNOWN,       //!< \brief The item is of an unknown type
        TYPE_COUNT
    };

    //!\brief Return the specified Type as a string.
    static CoreString get_type_name(const Type& type);

    R2cItemDescriptor() : m_id(nullptr), m_refcount(-1), m_type(TYPE_UNKNOWN) {}

    /*! \brief Return the ID of the item
     *  \note While IDs can be used as hash, note that they may be recycled during a session of Clarisse
     * to describe a new item. Since they are not garanteed to be unique over a session, you need to be
     * careful when using them as hash. For example, during cleanup you should always treat removed
     * items that have been destroyed BEFORE treating inserted ones. You can check if the item is
     * destroyed using is_destroyed(). Indeed, when an item is destroyed there's a very small chance
     * that the id gets recycled for a new item. In the last case you can always use get_full_name()
     * which returns the unique name of the item even if it got destroyed. */
    inline R2cItemId get_id() const { return m_id; }

    /*! \brief Return true if the ID is null
     *  \note When the ID is null, the item is null */
    inline bool is_null() const { return m_id == nullptr; }
    /*! \brief Return true if the item has been destroyed
     *  \note When the item is destroyed it is no longer in Clarisse. */
    inline bool is_destroyed() const { return (m_refcount == -1 || is_null()); }
    /*! \brief Return the full name of the item whereas it is alive or destroyed */
    inline const CoreString& get_full_name() const { return m_full_name; }
    /*! \brief Return the type of the item */
    inline const Type& get_type() const { return m_type; }

    /*! \brief Return true if the item is an instancer */
    inline bool is_instancer() const { return m_type == TYPE_INSTANCER; }
    /*! \brief Return true if the item is a geometry */
    inline bool is_geometry() const { return m_type == TYPE_GEOMETRY; }
    /*! \brief Return true if the item is a light */
    inline bool is_light() const { return m_type == TYPE_LIGHT; }
    /*! \brief Return true if the item is a material */
    inline bool is_material() const { return m_type == TYPE_MATERIAL; }
    /*! \brief Return true if the item defines a transform ie (a scene item) */
    inline bool has_transform() const { return is_instancer() || is_geometry() || is_light(); }

    /*! \brief Return the Clarisse OfObject.
     *  \note Return nullptr if the item is destroyed. */
    OfObject *get_item() const;

private:

    friend class R2cSceneDelegate;
    inline void set_id(R2cItemId id) { m_id = id; }
    inline void set_full_name(const CoreString& str) { m_full_name = str; }
    inline void set_destroyed() { m_refcount = -1; }
    inline void set_type(Type type) { m_type = type; }
    // internal refcounting used for item dependency cleanup
    inline void set_refcount(const int& refcount) { m_refcount = refcount; }
    inline const int& get_refcount() const { return m_refcount; }

    R2cItemId m_id;
    CoreString m_full_name;
    int m_refcount;
    Type m_type;
};

#endif
