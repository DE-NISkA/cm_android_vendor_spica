/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : platform_eclair.c                                       *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/android_pmem.h>
#include <private/ui/android_natives_priv.h>
#include <hardware/hardware.h>
#include <cutils/log.h>
#include <ui/PixelFormat.h>

#include "gralloc_priv.h"

#include <KD/kd.h>

#include "region.h"

#include <interface/platform.h>

#define FB_DEVICE_NAME "/dev/graphics/fb0"

//#define kdAssert(c) ((void)( (c) ? 0 : (__assert(#c, __FILE__, __LINE__), 0)))
namespace
{
	struct EclairSurfaceDescription
	{
		int magic;
	android_native_buffer_t* buffer;
	android_native_buffer_t* previousBuffer;
	gralloc_module_t const* module;
	void* bits;
	Rect dirtyRegion;
	Rect oldDirtyRegion;

		static const int MAGIC = 'ESFD';
	};
}

static void* GetFramebufferAddress()
{
	static void* address = 0;

	if (address == 0)
	{
	int fb_fd = open(FB_DEVICE_NAME, O_RDWR, 0);

	if (fb_fd == -1)
	{
		LOGE("EGL: GetFramebufferAddress: cannot open fb");
		return 0;
	}

	fb_fix_screeninfo finfo;
	if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo) < 0)
	{
		LOGE("EGL: Failed to get framebuffer address");
		close(fb_fd);
		return 0;
	}
	close(fb_fd);

	address = reinterpret_cast<void*>(finfo.smem_start);
	}

	return address;
}

static void* GetPhysicalAdressByHandle(android_native_buffer_t* buffer)
{
	const private_handle_t* hnd = static_cast<const private_handle_t*>(buffer->handle);

	// this pointer came from framebuffer
	if (hnd->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER)
	{
	return static_cast<unsigned char*>(GetFramebufferAddress()) + hnd->offset;
	}

	// this pointer came from pmem domain
    pmem_region region;
    if (ioctl(hnd->fd, PMEM_GET_PHYS, &region) < 0)
    {
	LOGE("EGL: PMEM_GET_PHYS failed");
	return 0;
    }

    return reinterpret_cast<void*>(region.offset + hnd->offset);
}

static void DequeueBuffer(NativeWindowType window, EclairSurfaceDescription* surface)
{
	window->common.incRef(&window->common);
	if (window->dequeueBuffer(window, &surface->buffer) < 0)
	{
	LOGE("EGL: Failed to dequeue buffer");
	}

	surface->buffer->common.incRef(&surface->buffer->common);
	window->lockBuffer(window, surface->buffer);
	if (surface->module->lock(surface->module, surface->buffer->handle, GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN, 0, 0, surface->buffer->width, surface->buffer->height, &surface->bits) < 0)
	{
	LOGE("EGL: Failed to lock buffer");
	}
}

static void QueueBuffer(NativeWindowType window, EclairSurfaceDescription* surface)
{
	if (surface->module->unlock(surface->module, surface->buffer->handle) < 0)
	{
	LOGE("EGL: Failed to unlock buffer");
	}


	if (window->queueBuffer(window, surface->buffer) < 0)
	{
	LOGE("EGL: Failed to queue buffer");
	}
}

static void CopyDirtyRegions(EclairSurfaceDescription* surface)
{
	if (!surface->dirtyRegion.isEmpty())
	{
		surface->dirtyRegion.andSelf(Rect(surface->buffer->width, surface->buffer->height));

		if (surface->previousBuffer != 0)
		{
			void* prevBits;
	        if (surface->module->lock(surface->module, surface->previousBuffer->handle, GRALLOC_USAGE_SW_READ_OFTEN, 0, 0, surface->previousBuffer->width, surface->previousBuffer->height, &prevBits) == 0)
	        {
				// get pixel format information
				android::PixelFormatInfo formatInfo;
				if (android::getPixelFormatInfo(surface->buffer->format, &formatInfo) != android::NO_ERROR)
				{
					LOGE("EGL: Failed to get pixel color information");
					return;
				}

		        // copy scanlines one by one, if any
		        Region copyBack = Region::subtract(surface->oldDirtyRegion, surface->dirtyRegion);
		        if (!copyBack.isEmpty())
		        {
				copyBlt(surface->bits, prevBits, copyBack, surface->buffer->stride, surface->previousBuffer->stride, formatInfo.bytesPerPixel);
		        }

		        surface->oldDirtyRegion = surface->dirtyRegion;
		        surface->module->unlock(surface->module, surface->previousBuffer->handle);
	        }
	        else
	        {
			LOGE("EGL: Failed to lock buffer");
	        }
	    }
		//surface->dirtyRegion = surface->oldDirtyRegion;
	}

	if (surface->previousBuffer)
	{
		surface->previousBuffer->common.decRef(&surface->previousBuffer->common);
	}

	surface->previousBuffer = surface->buffer;
}

void PLATFORM_INITIALIZE(NativeDisplayType display)
{
  // nothing to do here
}

void PLATFORM_FINALIZE(NativeDisplayType display)
{
  // nothing to do here
}

int PLATFORM_GET_DISPLAY_HORIZONTAL_RESOLUTION(NativeDisplayType display)
{
    return -1;
}

int PLATFORM_GET_DISPLAY_VERTICAL_RESOLUTION(NativeDisplayType display)
{
    return -1;
}

int PLATFORM_GET_DISPLAY_PIXEL_ASPECT_RATIO(NativeDisplayType display)
{
    return -1;
}

void PLATFORM_WINDOW_SURFACE_INITIALIZE(NativeDisplayType display, NativeWindowType window, void** userData)
{
	//kdAssert(PLATFORM_IS_WINDOW_VALID(display, window) && *userData != 0);

	EclairSurfaceDescription* surface = static_cast<EclairSurfaceDescription*>(malloc(sizeof(EclairSurfaceDescription)));
	memset(surface, 0, sizeof(EclairSurfaceDescription));
	surface->magic = EclairSurfaceDescription::MAGIC;

	// get a reference to gralloc module which is used for locking and unlocking of underlying buffers
	hw_module_t const* module;
	if (hw_get_module(GRALLOC_HARDWARE_MODULE_ID, &module) < 0)
	{
	LOGE("EGL: Failed to get reference on gralloc module");
	}

	surface->module = reinterpret_cast<gralloc_module_t const*>(module);

	// dequeue and prepare a buffer for the next frame
	DequeueBuffer(window, surface);

	*userData = surface;
}

void PLATFORM_WINDOW_SURFACE_FINALIZE(NativeDisplayType display, NativeWindowType window, void* userData)
{
	//kdAssert(PLATFORM_IS_WINDOW_VALID(display, window));

	EclairSurfaceDescription* surface = static_cast<EclairSurfaceDescription*>(userData);
	//kdAssert(surface != 0 && surface->magic == EclairSurfaceDescription::MAGIC);

	// send the last buffer for rendering
	QueueBuffer(window, surface);

	// relinquish references for the previous buffer, the current buffer and the window
	if (surface->previousBuffer)
	{
	surface->previousBuffer->common.decRef(&surface->previousBuffer->common);
	}

	surface->buffer->common.decRef(&surface->buffer->common);
	surface->magic = 0;
	PLATFORM_FREE(surface);

	window->common.decRef(&window->common);
}

void* PLATFORM_WINDOW_SURFACE_GET_BUFFER(NativeDisplayType display, NativeWindowType window, void* userData)
{
	//kdAssert(PLATFORM_IS_WINDOW_VALID(display, window));

	EclairSurfaceDescription* surface = static_cast<EclairSurfaceDescription*>(userData);
	//kdAssert(surface != 0 && surface->magic == EclairSurfaceDescription::MAGIC);

	return surface->buffer;
}

void PLATFORM_WINDOW_SURFACE_SET_SWAP_RECTANGLE(NativeDisplayType display, NativeWindowType window, int x, int y, int w, int h, void* userData)
{
	//kdAssert(PLATFORM_IS_WINDOW_VALID(display, window));

	EclairSurfaceDescription* surface = static_cast<EclairSurfaceDescription*>(userData);
	//kdAssert(surface != 0 && surface->magic == EclairSurfaceDescription::MAGIC);

	surface->dirtyRegion = Rect(x, y, x + w, y + h);
}

int PLATFORM_IS_WINDOW_VALID(NativeDisplayType display, NativeWindowType window)
{
    return window != 0;
}

void PLATFORM_GET_WINDOW_ADDRESS(NativeDisplayType display, NativeWindowType window, void **virtualAddr, void **physicalAddr, void* userData)
{
	//kdAssert(virtualAddr != 0 && physicalAddr != 0 && PLATFORM_IS_WINDOW_VALID(display, window));

	EclairSurfaceDescription* surface = static_cast<EclairSurfaceDescription*>(userData);
	//kdAssert(surface != 0 && surface->magic == EclairSurfaceDescription::MAGIC);

	*virtualAddr = surface->bits;
	*physicalAddr = GetPhysicalAdressByHandle(surface->buffer);
}

void PLATFORM_GET_WINDOW_SIZE(NativeDisplayType display, NativeWindowType window, int *width, int *height, void* userData)
{
	//kdAssert(width != 0 && height != 0 && PLATFORM_IS_WINDOW_VALID(display, window));

	EclairSurfaceDescription* surface = static_cast<EclairSurfaceDescription*>(userData);
	//kdAssert(surface != 0 && surface->magic == EclairSurfaceDescription::MAGIC);

    *width  = surface->buffer->width;
    *height = surface->buffer->height;
}

int PLATFORM_IS_PIXMAP_VALID(NativeDisplayType display, NativePixmapType pixmap)
{
    return pixmap != 0;
}

void PLATFORM_GET_PIXMAP_SIZE(NativeDisplayType display, NativePixmapType pixmap, int *width, int *height)
{
	//kdAssert(width != 0 && height != 0 && PLATFORM_IS_PIXMAP_VALID(display, pixmap));

	*width = pixmap->width;
	*height = pixmap->height;
}

void PLATFORM_GET_PIXMAP_COLORBUFFER(NativeDisplayType display, NativePixmapType pixmap, void **virtualAddr, void **physicalAddr)
{
	//kdAssert(virtualAddr != 0 && physicalAddr != 0 && PLATFORM_IS_PIXMAP_VALID(display, pixmap));

	*virtualAddr = pixmap->data;
	*physicalAddr = 0;
}

void PLATFORM_DISPLAY_BUFFER(NativeDisplayType display, NativeWindowType window, void *buffer, int width, int height, int bufferBytes, void* userData)
{


	//kdAssert(PLATFORM_IS_WINDOW_VALID(display, window));

	EclairSurfaceDescription* surface = static_cast<EclairSurfaceDescription*>(userData);
	//kdAssert(surface != 0 && surface->magic == EclairSurfaceDescription::MAGIC);
	
	// copy dirty regions from the last frame to the current frame
	CopyDirtyRegions(surface);

	// send the buffer for rendering
	QueueBuffer(window, surface);

	// dequeue and prepare a new buffer for the next frame
	DequeueBuffer(window, surface);

}

void PLATFORM_COPY_BUFFER(NativeDisplayType display, void *buffer, NativePixmapType pixmap, int width, int height, int bufferBytes)
{
	//kdAssert(PLATFORM_IS_PIXMAP_VALID(display, pixmap));

	// get pixel format information
	android::PixelFormatInfo formatInfo;
	if (android::getPixelFormatInfo(pixmap->format, &formatInfo) != android::NO_ERROR)
	{
		LOGE("EGL: Failed to get pixel color information");
		return;
	}

	// be paranoid
	if (pixmap->data == 0)
	{
	LOGE("EGL: Pixmap data pointer is NULL");
	return;
	}

	if (width != pixmap->width || height != pixmap->height || bufferBytes != (int)formatInfo.bytesPerPixel)
	{
	LOGE("EGL: Pixmap dimensions are differrent from surface dimensions");
	return;
	}

	memcpy(buffer, pixmap->data, pixmap->width * pixmap->height * formatInfo.bytesPerPixel);
}

void *PLATFORM_MALLOC( int size )
{
    return malloc( size );
}

void PLATFORM_FREE( void *buffer )
{
	if( buffer )
    free( buffer );
}

void PLATFORM_MEMSET( void *buffer, int value, int size )
{
    memset( buffer, value, size );
}

void PLATFORM_MEMCPY( void *dst, void *src, int size )
{
    memcpy( dst, src, size );
}

int PLATFORM_STRCMP( const char *str1, char *str2, int length )
{
    return strncmp(str1, str2, length);
}