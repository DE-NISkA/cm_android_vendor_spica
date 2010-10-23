/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : egl_config.h                                            *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#ifndef __egl_config_h_
#define __egl_config_h_

//#include <GLES/glContext.h>

typedef struct _iEGLContext
{
    EGLint              referenceCount;
    EGLBoolean          deleteMark;
    EGLContext          shareContext;
    EGLint              errorCode;
    EGLDisplay          display;
    EGLSurface          readSurface;
    EGLSurface          drawSurface;
    EGLConfig           config;
    EGLint              clientVersion;
    EGLBoolean          isActivated;
    EGLint              boundAPI;

    void*               clientContext;
} iEGLContext;

typedef struct _iEGLSurface
{
    EGLint              width;
    EGLint              height;

    EGLint              referenceCount;
    EGLBoolean          deleteMark;
    EGLConfig           config;
    EGLNativeWindowType window;
    EGLNativePixmapType pixmap;
    EGLint              surfaceType;
    EGLBoolean          isClientBuffer;

    EGLint              renderBuffer;

    EGLBoolean          colorSpace;
    EGLBoolean          alphaFormat;

    EGLBoolean          isBound;
    EGLint              boundTexture;

    EGLBoolean          largestPbuffer;
    EGLint              textureFormat;
    EGLint              textureTarget;
    EGLBoolean          mipmapTexture;
    EGLint              mipmapLevel;

    EGLDisplay          display;

    EGLint              bufferBytes;
    EGLint              redSize;
    EGLint              greenSize;
    EGLint              blueSize;
    EGLint              alphaSize;
    EGLint              depthSize;
    EGLint              stencilSize;
    EGLint              sampleSize;

    EGLint              colorFormat;
    EGLint              depthStencilFormat;

    EGLint              curDrawingBuffer;
    AddressBase        *backBuffer1;
    AddressBase        *backBuffer2;
    AddressBase        *backBuffer3;
    AddressBase        *frontBuffer;
    AddressBase        *alphaMaskBuffer;
    AddressBase        *depthBuffer;
    AddressBase        *stencilBuffer;
    AddressBase        *sampleBuffer;
    void               *backBuffer1Handle;
    void               *backBuffer2Handle;
    void               *backBuffer3Handle;
    void               *depthBufferHandle;

    /* attributes for extension */
    EGLBoolean          isLocked;
    void*               bitmapPointer;
    EGLint              bitmapPitch;
    EGLenum             bitmapOrigin;
    EGLint              bitmapPixelRedOffset;
    EGLint              bitmapPixelGreenOffset;
    EGLint              bitmapPixelBlueOffset;
    EGLint              bitmapPixelAlphaOffset;
    EGLint              bitmapPixelLuminanceOffset;
    EGLBoolean          preservePixels;
    EGLint              usageHint;

    void* userData;

} iEGLSurface;

typedef struct _iEGLImage
{
    EGLDisplay          display;
    EGLContext          context;
    EGLenum             target;
    EGLClientBuffer     buffer;

    void               *imageData;
} iEGLImage;

#ifdef __cplusplus
extern "C" {
#endif

extern EGLint eglGetConfigNum();
extern EGLint (*eglGetConfigList())[33];

#ifdef __cplusplus
}
#endif

#endif
