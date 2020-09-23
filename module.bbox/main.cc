//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <dso_export.h>
#include <module_object.h>
#include <sys_globals.h>

#include "bbox_utils.h"

// Forward declarations of module registration functions
namespace MaterialBbox { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace LightBbox { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace RendererBbox { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace LayerBbox { void on_register(OfApp &, CoreVector<OfClass *> &); }

IX_BEGIN_EXTERN_C

    DSO_EXPORT void
    on_register_module(OfApp& app, CoreVector<OfClass *>& new_classes) {
        // register module classes
        MaterialBbox::on_register(app, new_classes);
        LightBbox::on_register(app, new_classes);
        RendererBbox::on_register(app, new_classes);
        LayerBbox::on_register(app, new_classes);
    }

IX_END_EXTERN_C
