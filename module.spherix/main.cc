//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

// Clarisse includes
#include <dso_export.h>
#include <module_object.h>

#include <spherix_register_shaders.h>

// Forward declarations of module registration functions
namespace SpherixRenderer     { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace SpherixLayer        { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace SpherixMaterial     { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace SpherixLight        { void on_register(OfApp &, CoreVector<OfClass *> &); }
namespace SpherixTexture      { void on_register(OfApp &, CoreVector<OfClass *> &); }

IX_BEGIN_EXTERN_C

    DSO_EXPORT void
    on_register_module(OfApp& app, CoreVector<OfClass *>& new_classes) {
        // register module classes
        SpherixRenderer::on_register(app, new_classes);
        SpherixLayer::on_register(app, new_classes);

        SpherixLight::on_register(app, new_classes);
        SpherixTexture::on_register(app, new_classes);
        SpherixMaterial::on_register(app, new_classes);

        SpherixRegisterShaders::register_shaders(app);
    }

IX_END_EXTERN_C
