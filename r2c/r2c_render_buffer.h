//
// Copyright 2020 - present Isotropix SAS. See License.txt for license information
//

#ifndef R2C_RENDER_BUFFER_H
#define R2C_RENDER_BUFFER_H

#include <r2c_export.h>

class ModuleLayerR2cScene;

/*! \class R2cRenderBuffer
    \brief Manages an abstract render buffer that is passed to a R2cRenderDelegate */
class R2C_EXPORT R2cRenderBuffer {
public:

    struct Region {
        Region(unsigned int x, unsigned int y, unsigned int w, unsigned int h) : offset_x(x), offset_y(y), width(w), height(h) {}
        unsigned int offset_x, offset_y, width, height;
    };

    enum AOVId {
        AOV_ID_RGBA = 0
    };

    R2cRenderBuffer() {}
    virtual ~R2cRenderBuffer() {}

    //! Return the width of the buffer
    virtual unsigned int get_width() const = 0;
    //! Return the height of the buffer
    virtual unsigned int get_height() const = 0;

    /*! \brief Helper to fill the RGBA render buffer
        \note Please refer to R2cRenderBuffer::fill_region for more information. */
    inline void fill_rgba_region(const float *rgba, const unsigned int& src_stride, const Region& region, const bool& lock) {
        fill_region(AOV_ID_RGBA, rgba, src_stride, region, lock);
    }

    /*! \brief Fill the specified region of the render buffer with the specified buffer
     *  \param aov_id ID of the buffer that can be used for AOVs
     *  \param src_data input buffer
     *  \param src_stride size of the stride in bytes
     *  \param region region to fill in buffer coordinates
     *  \param lock if true, the implementation of this method must garantee thread safety since there will be concurrent calls.
     *  \note  If you need to use a lock please look at SysThreadLock since it provides a thread safe locking mechanism. */
    virtual void fill_region(const unsigned int& aov_id, const float *src_data, const unsigned int& src_stride, const Region& region, const bool& lock) = 0;

    /*! \brief Notify the display to display the current region being rendered
     *  \param region region that is being rendered
     *  \param lock if true, the implementation of this method must garantee thread safety since there will be concurrent calls. */
    virtual void notify_start_render_region(const Region& region, const bool& lock) const {}

private:

    R2cRenderBuffer(const R2cRenderBuffer&) = delete;
    R2cRenderBuffer& operator=(const R2cRenderBuffer&) = delete;
};

class ImageCanvas;
class ModuleLayer;
class ClarisseLayerRenderBufferImpl;

/*! \class ClarisseLayerRenderBuffer
    \brief Implementation of R2cRenderBuffer specialized for a ModuleLayer */
class R2C_EXPORT ClarisseLayerRenderBuffer : public R2cRenderBuffer {
public:

    ClarisseLayerRenderBuffer(ModuleLayerR2cScene& layer, ImageCanvas& canvas);
    virtual ~ClarisseLayerRenderBuffer() override;
    unsigned int get_width() const override;
    unsigned int get_height() const override;

    void fill_region(const unsigned int& layer_id, const float *src_data, const unsigned int& src_stride, const R2cRenderBuffer::Region& region, const bool& lock) override;
    void notify_start_render_region(const Region& region, const bool& lock) const override;

private:

    ClarisseLayerRenderBufferImpl *m;
};

#endif
