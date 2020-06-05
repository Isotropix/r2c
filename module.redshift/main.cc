//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <dso_export.h>
#include <module_object.h>
#include <sys_globals.h>

#include "redshift_utils.h"

// Forward declarations of module registration functions
namespace MaterialRedshift { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace TextureRedshift { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace LightRedshift { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace RendererRedshift { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace LayerRedshift { void on_register(OfApp &, CoreVector<OfClass *> &); }

IX_BEGIN_EXTERN_C

    DSO_EXPORT void
    on_register_module(OfApp& app, CoreVector<OfClass *>& new_classes) {
        // register module classes
        MaterialRedshift::on_register(app, new_classes);
        TextureRedshift::on_register(app, new_classes);
        LightRedshift::on_register(app, new_classes);
        RendererRedshift::on_register(app, new_classes);
        LayerRedshift::on_register(app, new_classes);
        // Initializing Redshift Engine
        // We must initialize early on to register all shaders/lights etc...
        if (!RedshiftUtils::is_initialized()) {
            RedshiftUtils::initialize(app);
        }
    }

IX_END_EXTERN_C
