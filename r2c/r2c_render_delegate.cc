//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include "r2c_scene_delegate.h"

#include "r2c_render_delegate.h"

IMPLEMENT_CLASS(R2cRenderDelegate, CoreBaseObject);


R2cRenderDelegate::R2cRenderDelegate()
{
    m_scene_delegate = R2cSceneDelegate::get_null();
}

