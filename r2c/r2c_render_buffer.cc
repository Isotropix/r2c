//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#include <r2c_render_buffer.h>

#include <image_canvas.h>
#include <image_map_channel.h>
#include <image_map_tile.h>
#include <module_layer.h>

class ClarisseLayerRenderBufferImpl {
public:

    ClarisseLayerRenderBufferImpl(ModuleLayer& ilayer, ImageCanvas& icanvas) : layer(&ilayer), canvas(&icanvas) {}

    ModuleLayer *layer;
    ImageCanvas *canvas;
};

ClarisseLayerRenderBuffer::ClarisseLayerRenderBuffer(ModuleLayer& layer, ImageCanvas& canvas) : R2cRenderBuffer(), m(new ClarisseLayerRenderBufferImpl(layer, canvas))
{
}

ClarisseLayerRenderBuffer::~ClarisseLayerRenderBuffer()
{
    delete m;
}

unsigned int
ClarisseLayerRenderBuffer::get_width() const
{
    return static_cast<unsigned int>(m->canvas->get_width());
}

unsigned int
ClarisseLayerRenderBuffer::get_height() const
{
    return static_cast<unsigned int>(m->canvas->get_height());
}

void
ClarisseLayerRenderBuffer::fill_region(const unsigned int& layer_id, const float *src_data, const unsigned int& src_stride, const Region& region, const bool& lock)
{
    if (layer_id == AOV_ID_RGBA) {
        const unsigned int src_channel_count = 4;
        float **dest = new float*[src_channel_count];
        for (unsigned int i = 0; i < src_channel_count; i++) {
            dest[i] = new float[region.width * region.height];
        }

        for (unsigned int y = 0; y < region.height; y++) {
            const unsigned int offset = y * src_stride;
            const unsigned int doffset = y * region.width;
            for (unsigned int x = 0; x < region.width; x++) {
                const unsigned int didx = doffset + x;
                const unsigned int idx = (offset + x) * src_channel_count;
                dest[0][didx] = src_data[idx];
                dest[1][didx] = src_data[idx + 1];
                dest[2][didx] = src_data[idx + 2];
                dest[3][didx] = src_data[idx + 3];
            }
        }

        GMathVec4i cregion(static_cast<int>(region.offset_x), static_cast<int>(region.offset_y), static_cast<int>(region.width), static_cast<int>(region.height));
        GMathVec4i progress_region(static_cast<int>(region.offset_x),
                                   static_cast<int>(region.offset_y),
                                   static_cast<int>(gmath_minui(m->canvas->get_tile_size(), region.width)),
                                   static_cast<int>(gmath_minui(region.height, m->canvas->get_tile_size())));



        ImageMap *image = m->canvas->get_image();
        ImageMapChannel *red = image->get_red_channel();
        ImageMapChannel *green = image->get_green_channel();
        ImageMapChannel *blue = image->get_blue_channel();
        ImageMapChannel *alpha = image->get_alpha_channel();

        if (lock) m->canvas->lock();

        red->fill_tiles(dest[0], cregion);
        green->fill_tiles(dest[1], cregion);
        blue->fill_tiles(dest[2], cregion);
        alpha->fill_tiles(dest[3], cregion);

        if (lock) m->canvas->unlock();

        // FIXME: CLARISSEAPI We shouldn't have to do all that. It should be handled by the ModuleLayer

        // getting the tiles intersecting the region we updated
        CoreVector<ImageMapTileHandle> tiles;
        red->get_tiles(tiles, cregion);
        for (auto tile_handle : tiles) {
            const ImageMapTile& tile = *tile_handle.get_object();
            int x = gmath_maxi(tile.get_x(), cregion[0]);
            int y = gmath_maxi(tile.get_y(), cregion[1]);

            int tx_end = tile.get_x() + static_cast<int>(tile.get_width());
            int ty_end = tile.get_y() + static_cast<int>(tile.get_height());
            int rx_end = static_cast<int>(region.offset_x + region.width);
            int ry_end = static_cast<int>(region.offset_y + region.height);

            // clamping the region to the tile region since we don't want to
            // notify we updated outside of our region
            progress_region[0] = x;
            progress_region[1] = y;
            progress_region[2] = tx_end > rx_end ? rx_end - tile.get_x() : static_cast<int>(tile.get_width());
            progress_region[3] = ty_end > ry_end ? ry_end - tile.get_y() : static_cast<int>(tile.get_height());
            // notifying the subregion in the corresponding tile
            m->layer->update_region(progress_region, true);
        }
        // Do we really have to call that? It messes up with the bucket display...
        //m->layer->bucket_render_end(cregion, 0);

        for (unsigned int i = 0; i < src_channel_count; i++) delete dest[i];
        delete [] dest;
    } else {
        LOG_WARNING("ClarisseLayerRenderBuffer::fill_buffer layer_id (" << layer_id << ") is not implemented.\n");
    }
}

void
ClarisseLayerRenderBuffer::notify_start_render_region(const Region& region, const bool& lock) const
{
    GMathVec4i cregion(static_cast<int>(region.offset_x), static_cast<int>(region.offset_y), static_cast<int>(region.width), static_cast<int>(region.height));
    // the method is already thread safe
    m->layer->bucket_render_start(cregion, 0);
}
