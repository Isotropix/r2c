//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <dso_export.h>
#include <module_object.h>
#include <sys_globals.h>

// Forward declarations of module registration functions
namespace MaterialDummy { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace LightDummy { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace RendererDummy { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace LayerDummy { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace TextureDummy { void on_register(OfApp &, CoreVector<OfClass *> &); }

IX_BEGIN_EXTERN_C

    DSO_EXPORT void
    on_register_module(OfApp& app, CoreVector<OfClass *>& new_classes) {
        // register module classes
        MaterialDummy::on_register(app, new_classes);
        LightDummy::on_register(app, new_classes);
        RendererDummy::on_register(app, new_classes);
        LayerDummy::on_register(app, new_classes);
        TextureDummy::on_register(app, new_classes);
    }

IX_END_EXTERN_C
