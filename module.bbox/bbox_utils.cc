//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include "bbox_utils.h"

#include <core_log.h>
#include <gmath_matrix3x3.h>
#include <image_canvas.h>
#include <image_map_channel.h>
#include <module_camera.h>
#include <module_geometry.h>
#include <module_layer.h>
#include <module_light_bbox.h>
#include <module_material_bbox.h>
#include <module_scene_object_tree.h>
#include <of_context.h>
#include <poly_mesh_smoothed.h>
#include <poly_mesh.h>
#include <r2c_instancer.h>
#include <r2c_render_buffer.h>
#include <sys_globals.h>
#include <sys_thread_lock.h>


void BboxUtils::create_light(const R2cSceneDelegate &render_delegate, R2cItemId item_id, BboxLightInfo &light_info)
{
    // Get the OfObject of the light
    R2cItemDescriptor idesc = render_delegate.get_render_item(item_id);
    OfObject *item = idesc.get_item();

    // Fill the light data
    light_info.light_data.light_module = static_cast<ModuleLightBbox *>(item->get_module());
}
