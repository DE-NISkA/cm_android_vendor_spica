/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : sec_gles_2_0.h                                          *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#ifndef __egl_h_
#define __egl_h_

/* All platform-dependent types and macro boilerplate (such as EGLAPI
 * and EGLAPIENTRY) should go in eglplatform.h.
 */
#include <ui/egl/android_natives.h>
#include <EGL/eglplatform.h>
#include <GLES/gles11Context.h>
/*****************************************************************************/

//struct egl_native_window_t;
//struct egl_native_pixmap_t;


/*typedef egl_native_window_t*     NativeWindowType;
typedef egl_native_pixmap_t*     NativePixmapType;
typedef void*                    NativeDisplayType;

typedef NativeDisplayType EGLNativeDisplayType;
typedef NativePixmapType  EGLNativePixmapType;
typedef NativeWindowType  EGLNativeWindowType;*/
/*
 * This a conveniance function to create a NativeWindowType surface
 * that maps to the whole screen
 * This function is actually implemented in libui.so
 */

NativeWindowType android_createDisplaySurface();

/* flags returned from swapBuffer */
#define EGL_NATIVES_FLAG_SIZE_CHANGED       0x00000001

/* surface flags */
#define EGL_NATIVES_FLAG_DESTROY_BACKBUFFER 0x00000001

#if 0
enum native_pixel_format_t
{
    NATIVE_PIXEL_FORMAT_RGBA_8888   = 1,
    NATIVE_PIXEL_FORMAT_RGB_565     = 4,
    NATIVE_PIXEL_FORMAT_RGBA_5551   = 6,
    NATIVE_PIXEL_FORMAT_RGBA_4444   = 7,
    NATIVE_PIXEL_FORMAT_YCbCr_422_SP= 0x10,
    NATIVE_PIXEL_FORMAT_YCbCr_420_SP= 0x11,
};

enum native_memory_type_t
{
    NATIVE_MEMORY_TYPE_PMEM         = 0,
    NATIVE_MEMORY_TYPE_GPU          = 1,
    NATIVE_MEMORY_TYPE_FB           = 2,
    NATIVE_MEMORY_TYPE_HEAP         = 128
};


struct egl_native_window_t
{
    /*
     * magic must be set to 0x600913
     */
    uint32_t    magic;

    /*
     * must be sizeof(egl_native_window_t)
     */
    uint32_t    version;

    /*
     * ident is reserved for the Android platform
     */
    uint32_t    ident;

    /*
     * width, height and stride of the window in pixels
     * Any of these value can be nul in which case GL commands are
     * accepted and processed as usual, but not rendering occurs.
     */
    int         width;      // w=h=0 is legal
    int         height;
    int         stride;

    /*
     * format of the native window (see ui/PixelFormat.h)
     */
    int         format;

    /*
     * Offset of the bits in the VRAM
     */
    intptr_t    offset;

    /*
     * flags describing some attributes of this surface
     * EGL_NATIVES_FLAG_DESTROY_BACKBUFFER: backbuffer not preserved after
     * eglSwapBuffers
     */
    uint32_t    flags;

    /*
     * horizontal and vertical resolution in DPI
     */
    float       xdpi;
    float       ydpi;

    /*
     * refresh rate in frames per second (Hz)
     */
    float       fps;


    /*
     *  Base memory virtual address of the surface in the CPU side
     */
    intptr_t    base;

    /*
     *  Heap the offset above is based from
     */
    int         fd;

    /*
     *  Memory type the surface resides into
     */
    uint8_t     memory_type;

    /*
     * Reserved for future use. MUST BE ZERO.
     */
    uint8_t     reserved_pad[3];
    int         reserved[8];

    /*
     * Vertical stride (only relevant with planar formats)
     */

    int         vstride;

    /*
     * Hook called by EGL to hold a reference on this structure
     */
    void        (*incRef)(NativeWindowType window);

    /*
     * Hook called by EGL to release a reference on this structure
     */
    void        (*decRef)(NativeWindowType window);

    /*
     * Hook called by EGL to perform a page flip. This function
     * may update the size attributes above, in which case it returns
     * the EGL_NATIVES_FLAG_SIZE_CHANGED bit set.
     */
    uint32_t    (*swapBuffers)(NativeWindowType window);

    /*
     * Hook called by EGL to set the swap rectangle. this hook can be
     * null (operation not supported)
     */
    void        (*setSwapRectangle)(NativeWindowType window, int l, int t, int w, int h);

    /*
     * Reserved for future use. MUST BE ZERO.
     */
    void        (*reserved_proc_0)(void);


    /*
     * Hook called by EGL to retrieve the next buffer to render into.
     * This call updates this structure.
     */
    uint32_t    (*nextBuffer)(NativeWindowType window);

    /*
     * Hook called by EGL when the native surface is associated to EGL
     * (eglCreateWindowSurface). Can be NULL.
     */
    void        (*connect)(NativeWindowType window);

    /*
     * Hook called by EGL when eglDestroySurface is called.  Can be NULL.
     */
    void        (*disconnect)(NativeWindowType window);

    /*
     * Reserved for future use. MUST BE ZERO.
     */
    void        (*reserved_proc[11])(void);

    /*
     *  Some storage reserved for the oem driver.
     */
    intptr_t    oem[4];
};


struct egl_native_pixmap_t
{
    int32_t     version;    /* must be 32 */
    int32_t     width;
    int32_t     height;
    int32_t     stride;
    uint8_t*    data;
    uint8_t     format;
    uint8_t     rfu[3];
    union {
        uint32_t    compressedFormat;
        int32_t     vstride;
    };
    int32_t     reserved;
};


/*****************************************************************************/

/*
 * OEM's egl's library (libhgl.so) must imlement these hooks to allocate
 * the GPU memory they need
 */


typedef struct
{
    // for internal use
    void*   user;
    // virtual address of this area
    void*   base;
    // size of this area in bytes
    size_t  size;
    // physical address of this area
    void*   phys;
    // offset in this area available to the GPU
    size_t  offset;
    // fd of this area
    int     fd;
} gpu_area_t;

typedef struct
{
    // area where GPU registers are mapped
    gpu_area_t regs;
    // number of extra areas (currently limited to 2)
    int32_t count;
    // extra GPU areas (currently limited to 2)
    gpu_area_t gpu[2];
} request_gpu_t;

void get_addressbase(AddressBase* base);

typedef request_gpu_t* (*OEM_EGL_acquire_gpu_t)(void* user);
typedef int (*OEM_EGL_release_gpu_t)(void* user, request_gpu_t* handle);
typedef void (*register_gpu_t)
        (void* user, OEM_EGL_acquire_gpu_t, OEM_EGL_release_gpu_t);

void oem_register_gpu(
        void* user,
        OEM_EGL_acquire_gpu_t acquire,
        OEM_EGL_release_gpu_t release);

#endif

/*****************************************************************************/





/* EGL Types */
#if defined(__ALEXVG_EGL_WIN32__)
#include <sys/types.h>
#endif

#define EGL_EGLEXT_PROTOTYPES

#if 1
typedef int                     EGLint;
typedef unsigned int            EGLBoolean;
typedef void*                   EGLConfig;
typedef void*                   EGLContext;
typedef void*                   EGLDisplay;
typedef void*                   EGLSurface;
typedef int                     EGLenum;
typedef unsigned int            EGLClientBuffer;
/* NativeDisplayType, NativeWindowType, NativePixmapType TBD */
//typedef void*                   NativeDisplayType;
//typedef void*                   NativeWindowType;
//typedef void*                   NativePixmapType;
//typedef void*                   EGLNativeDisplayType;
//typedef void*                   EGLNativeWindowType;
//typedef void*                   EGLNativePixmapType;


/*------------------------------------------------------------------------
 * EGL Enumerants.
 *----------------------------------------------------------------------*/

/* EGL Versioning */
#define EGL_VERSION_1_0			1
#define EGL_VERSION_1_1			1
#define EGL_VERSION_1_2			1
#define EGL_VERSION_1_3			1

/* EGL Enumerants. Bitmasks and other exceptional cases aside, most
 * enums are assigned unique values starting at 0x3000.
 */

/* EGL aliases */
#define EGL_FALSE			0
#define EGL_TRUE			1

/* Out-of-band handle values */
#define EGL_DEFAULT_DISPLAY		((void *)0)
#define EGL_NO_CONTEXT			((EGLContext)0)
#define EGL_NO_DISPLAY			((EGLDisplay)0)
#define EGL_NO_SURFACE			((EGLSurface)0)

/* Out-of-band attribute value */
#define EGL_DONT_CARE			((EGLint)-1)

/* Errors / GetError return values */
#define EGL_SUCCESS			0x3000
#define EGL_NOT_INITIALIZED		0x3001
#define EGL_BAD_ACCESS			0x3002
#define EGL_BAD_ALLOC			0x3003
#define EGL_BAD_ATTRIBUTE		0x3004
#define EGL_BAD_CONFIG			0x3005
#define EGL_BAD_CONTEXT			0x3006
#define EGL_BAD_CURRENT_SURFACE		0x3007
#define EGL_BAD_DISPLAY			0x3008
#define EGL_BAD_MATCH			0x3009
#define EGL_BAD_NATIVE_PIXMAP		0x300A
#define EGL_BAD_NATIVE_WINDOW		0x300B
#define EGL_BAD_PARAMETER		0x300C
#define EGL_BAD_SURFACE			0x300D
#define EGL_CONTEXT_LOST		0x300E	/* EGL 1.1 - IMG_power_management */

/* Reserved 0x300F-0x301F for additional errors */

/* Config attributes */
#define EGL_BUFFER_SIZE			0x3020
#define EGL_ALPHA_SIZE			0x3021
#define EGL_BLUE_SIZE			0x3022
#define EGL_GREEN_SIZE			0x3023
#define EGL_RED_SIZE			0x3024
#define EGL_DEPTH_SIZE			0x3025
#define EGL_STENCIL_SIZE		0x3026
#define EGL_CONFIG_CAVEAT		0x3027
#define EGL_CONFIG_ID			0x3028
#define EGL_LEVEL			0x3029
#define EGL_MAX_PBUFFER_HEIGHT		0x302A
#define EGL_MAX_PBUFFER_PIXELS		0x302B
#define EGL_MAX_PBUFFER_WIDTH		0x302C
#define EGL_NATIVE_RENDERABLE		0x302D
#define EGL_NATIVE_VISUAL_ID		0x302E
#define EGL_NATIVE_VISUAL_TYPE		0x302F
#define EGL_PRESERVED_RESOURCES		0x3030
#define EGL_SAMPLES			0x3031
#define EGL_SAMPLE_BUFFERS		0x3032
#define EGL_SURFACE_TYPE		0x3033
#define EGL_TRANSPARENT_TYPE		0x3034
#define EGL_TRANSPARENT_BLUE_VALUE	0x3035
#define EGL_TRANSPARENT_GREEN_VALUE	0x3036
#define EGL_TRANSPARENT_RED_VALUE	0x3037
#define EGL_NONE			0x3038	/* Attrib list terminator */
#define EGL_BIND_TO_TEXTURE_RGB		0x3039
#define EGL_BIND_TO_TEXTURE_RGBA	0x303A
#define EGL_MIN_SWAP_INTERVAL		0x303B
#define EGL_MAX_SWAP_INTERVAL		0x303C
#define EGL_LUMINANCE_SIZE		0x303D
#define EGL_ALPHA_MASK_SIZE		0x303E
#define EGL_COLOR_BUFFER_TYPE		0x303F
#define EGL_RENDERABLE_TYPE		0x3040
#define EGL_MATCH_NATIVE_PIXMAP		0x3041	/* Pseudo-attribute (not queryable) */
#define EGL_CONFORMANT			0x3042

/* Reserved 0x3041-0x304F for additional config attributes */

/* Config attribute values */
#define EGL_SLOW_CONFIG			0x3050	/* EGL_CONFIG_CAVEAT value */
#define EGL_NON_CONFORMANT_CONFIG	0x3051	/* EGL_CONFIG_CAVEAT value */
#define EGL_TRANSPARENT_RGB		0x3052	/* EGL_TRANSPARENT_TYPE value */
#define EGL_RGB_BUFFER			0x308E	/* EGL_COLOR_BUFFER_TYPE value */
#define EGL_LUMINANCE_BUFFER		0x308F	/* EGL_COLOR_BUFFER_TYPE value */

/* More config attribute values, for EGL_TEXTURE_FORMAT */
#define EGL_NO_TEXTURE			0x305C
#define EGL_TEXTURE_RGB			0x305D
#define EGL_TEXTURE_RGBA		0x305E
#define EGL_TEXTURE_2D			0x305F

/* Config attribute mask bits */
#define EGL_PBUFFER_BIT			0x0001	/* EGL_SURFACE_TYPE mask bits */
#define EGL_PIXMAP_BIT			0x0002	/* EGL_SURFACE_TYPE mask bits */
#define EGL_WINDOW_BIT			0x0004	/* EGL_SURFACE_TYPE mask bits */
#define EGL_VG_COLORSPACE_LINEAR_BIT	0x0020	/* EGL_SURFACE_TYPE mask bits */
#define EGL_VG_ALPHA_FORMAT_PRE_BIT	0x0040	/* EGL_SURFACE_TYPE mask bits */

#define EGL_OPENGL_ES_BIT		0x0001	/* EGL_RENDERABLE_TYPE mask bits */
#define EGL_OPENVG_BIT			0x0002	/* EGL_RENDERABLE_TYPE mask bits */
#define EGL_OPENGL_ES2_BIT		0x0004	/* EGL_RENDERABLE_TYPE mask bits */

/* QueryString targets */
#define EGL_VENDOR			0x3053
#define EGL_VERSION			0x3054
#define EGL_EXTENSIONS			0x3055
#define EGL_CLIENT_APIS			0x308D

/* QuerySurface / CreatePbufferSurface targets */
#define EGL_HEIGHT			0x3056
#define EGL_WIDTH			0x3057
#define EGL_LARGEST_PBUFFER		0x3058
#define EGL_TEXTURE_FORMAT		0x3080
#define EGL_TEXTURE_TARGET		0x3081
#define EGL_MIPMAP_TEXTURE		0x3082
#define EGL_MIPMAP_LEVEL		0x3083
#define EGL_RENDER_BUFFER		0x3086
#define EGL_VG_COLORSPACE		0x3087
#define EGL_VG_ALPHA_FORMAT		0x3088
#define EGL_HORIZONTAL_RESOLUTION	0x3090
#define EGL_VERTICAL_RESOLUTION		0x3091
#define EGL_PIXEL_ASPECT_RATIO		0x3092
#define EGL_SWAP_BEHAVIOR		0x3093

/* EGL_RENDER_BUFFER values / BindTexImage / ReleaseTexImage buffer targets */
#define EGL_BACK_BUFFER			0x3084
#define EGL_SINGLE_BUFFER		0x3085

/* OpenVG color spaces */
#define EGL_VG_COLORSPACE_sRGB		0x3089	/* EGL_VG_COLORSPACE value */
#define EGL_VG_COLORSPACE_LINEAR	0x308A	/* EGL_VG_COLORSPACE value */

/* OpenVG alpha formats */
#define EGL_VG_ALPHA_FORMAT_NONPRE	0x308B	/* EGL_ALPHA_FORMAT value */
#define EGL_VG_ALPHA_FORMAT_PRE		0x308C	/* EGL_ALPHA_FORMAT value */

/* Constant scale factor by which fractional display resolutions &
 * aspect ratio are scaled when queried as integer values.
 */
#define EGL_DISPLAY_SCALING		10000

/* Unknown display resolution/aspect ratio */
#define EGL_UNKNOWN			((EGLint)-1)

/* Back buffer swap behaviors */
#define EGL_BUFFER_PRESERVED		0x3094	/* EGL_SWAP_BEHAVIOR value */
#define EGL_BUFFER_DESTROYED		0x3095	/* EGL_SWAP_BEHAVIOR value */

/* CreatePbufferFromClientBuffer buffer types */
#define EGL_OPENVG_IMAGE		0x3096

/* QueryContext targets */
#define EGL_CONTEXT_CLIENT_TYPE		0x3097

/* CreateContext attributes */
#define EGL_CONTEXT_CLIENT_VERSION	0x3098

/* BindAPI/QueryAPI targets */
#define EGL_OPENGL_ES_API		0x30A0
#define EGL_OPENVG_API			0x30A1

/* GetCurrentSurface targets */
#define EGL_DRAW			0x3059
#define EGL_READ			0x305A

/* WaitNative engines */
#define EGL_CORE_NATIVE_ENGINE		0x305B

/* EGL 1.2 tokens renamed for consistency in EGL 1.3 */
#define EGL_COLORSPACE			EGL_VG_COLORSPACE
#define EGL_ALPHA_FORMAT		EGL_VG_ALPHA_FORMAT
#define EGL_COLORSPACE_sRGB		EGL_VG_COLORSPACE_sRGB
#define EGL_COLORSPACE_LINEAR		EGL_VG_COLORSPACE_LINEAR
#define EGL_ALPHA_FORMAT_NONPRE		EGL_VG_ALPHA_FORMAT_NONPRE
#define EGL_ALPHA_FORMAT_PRE		EGL_VG_ALPHA_FORMAT_PRE



/* EGL Functions */
#ifdef __cplusplus
extern "C" {
#endif

EGLint     eglGetError( void );

EGLDisplay eglGetDisplay(NativeDisplayType display);
EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor);
EGLBoolean eglTerminate(EGLDisplay dpy);

const char *eglQueryString(EGLDisplay dpy, EGLint name);

EGLBoolean eglGetConfigs(EGLDisplay dpy, EGLConfig *configs,
                         EGLint config_size, EGLint *num_config);
EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list,
                           EGLConfig *configs, EGLint config_size,
                           EGLint *num_config);
EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config,
                              EGLint attribute, EGLint *value);

EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,
                                  NativeWindowType win,
                                  const EGLint *attrib_list);
EGLSurface eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config,
                                   const EGLint *attrib_list);
EGLSurface eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config,
                                  NativePixmapType pixmap,
                                  const EGLint *attrib_list);
EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface);
EGLBoolean eglQuerySurface(EGLDisplay dpy, EGLSurface surface,
                           EGLint attribute, EGLint *value);

EGLBoolean eglBindAPI(EGLenum api);
EGLenum    eglQueryAPI(void);

EGLBoolean eglWaitClient(void);

EGLBoolean eglReleaseThread(void);

EGLSurface eglCreatePbufferFromClientBuffer(
                                        EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer,
                                        EGLConfig config, const EGLint *attrib_list);

EGLBoolean eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface,
                            EGLint attribute, EGLint value);
EGLBoolean eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
EGLBoolean eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer);

EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval);

EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config,
                            EGLContext share_context,
                            const EGLint *attrib_list);
EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx);
EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw,
                          EGLSurface read, EGLContext ctx);

EGLContext eglGetCurrentContext(void);
EGLSurface eglGetCurrentSurface(EGLint readdraw);
EGLDisplay eglGetCurrentDisplay(void);
EGLBoolean eglQueryContext(EGLDisplay dpy, EGLContext ctx,
                           EGLint attribute, EGLint *value);

EGLBoolean eglWaitGL(void);
EGLBoolean eglWaitNative(EGLint engine);
EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface);
EGLBoolean eglCopyBuffers(EGLDisplay dpy, EGLSurface surface,
                          NativePixmapType target);

/* This is a generic function pointer type, whose name indicates it must
 * be cast to the proper type *and calling convention* before use.
 */
typedef void (*__eglMustCastToProperFunctionPointerType)(void);
void       (*eglGetProcAddress(const char *procname))(void);

#endif
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __egl_h_ */
