//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Clarisse includes
#include <dso_export.h>
#include <module_object.h>

// Forward declarations of module registration functions
namespace KubickMaterial { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace KubickLight    { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace KubickRenderer { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace KubickLayer    { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace KubickTexture  { void on_register(OfApp &, CoreVector<OfClass *> &); }

IX_BEGIN_EXTERN_C

    DSO_EXPORT void
    on_register_module(OfApp& app, CoreVector<OfClass *>& new_classes) {
        // register module classes
        KubickMaterial::on_register(app, new_classes);
        KubickLight::on_register(app, new_classes);
        KubickRenderer::on_register(app, new_classes);
        KubickLayer::on_register(app, new_classes);
        KubickTexture::on_register(app, new_classes);
    }

IX_END_EXTERN_C
