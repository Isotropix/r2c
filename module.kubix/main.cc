//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Clarisse includes
#include <dso_export.h>
#include <module_object.h>

// Forward declarations of module registration functions
namespace KubixMaterial { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace KubixLight    { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace KubixRenderer { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace KubixLayer    { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace KubixTexture  { void on_register(OfApp &, CoreVector<OfClass *> &); }

IX_BEGIN_EXTERN_C

    DSO_EXPORT void
    on_register_module(OfApp& app, CoreVector<OfClass *>& new_classes) {
        // register module classes
        KubixMaterial::on_register(app, new_classes);
        KubixLight::on_register(app, new_classes);
        KubixRenderer::on_register(app, new_classes);
        KubixLayer::on_register(app, new_classes);
        KubixTexture::on_register(app, new_classes);
    }

IX_END_EXTERN_C
