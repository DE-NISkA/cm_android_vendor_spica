/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : sec_gles20.c                                            *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#include <fcntl.h>
#include <dlfcn.h>

#include <cutils/log.h>
#include <private/ui/android_natives_priv.h>

#include <KD/kd.h>
#include <EGL/egl.h>
#include <GLES/glContext.h>

#include <interface/platform.h>

#include "../../common/egl_config.h"

#include "sec_gles20.h"

namespace
{
  void* s_GLESLibraryHandle;

  struct GLESFunctionTable
  {
      GLboolean (*initDriver)();
      void* (*createContext)(void*);
      void* (*setContext)(void*);
      GLboolean (*destroyContext)(void*);
      GLboolean (*deinitDriver)(void);
      GLboolean (*setDrawSurface)(GLES2SurfaceData*);
      GLboolean (*setReadSurface)(GLES2SurfaceData*);
      void (*flush)();
      void (*finish)();
      void (*viewport)(EGLint,EGLint,EGLint,EGLint);
      void (*scissor)(EGLint,EGLint,EGLint,EGLint);
      void (*bindTexImage)(const GLES2SurfaceData*, GLenum, GLint, GLuint);
      void (*releaseTexImage)(const GLES2SurfaceData*);
  } s_GLESFunctionTable;
}

#define GET_FUNCTION_ADDRESS(v, handle, name) v = reinterpret_cast<typeof(v)>(dlsym(handle, name)); \
                                              if (v == 0) \
                                              { \
                                                 LOGE("Cannot map %s function from libGLESv2_fimg.so", name); \
                                                 dlclose(handle); \
                                                 handle =0; \
                                                 return 0; \
                                              }

static int MapGLESFunctions()
{
  memset(&s_GLESFunctionTable, 0, sizeof(s_GLESFunctionTable));

    s_GLESLibraryHandle = dlopen("libGLESv2_fimg.so", RTLD_LAZY);

    if (s_GLESLibraryHandle == 0)
    {
        LOGE("Cannot load libGLESv2_fimg.so");
        return 0;
    }

    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.flush, s_GLESLibraryHandle, "glFlush");
    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.finish, s_GLESLibraryHandle, "glFinish");
    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.viewport, s_GLESLibraryHandle, "glViewport");
    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.scissor, s_GLESLibraryHandle, "glScissor");
    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.initDriver, s_GLESLibraryHandle, "GLES2Initdriver");
    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.createContext, s_GLESLibraryHandle, "GLES2CreateContext");
    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.setContext, s_GLESLibraryHandle, "GLES2SetContext");
    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.destroyContext, s_GLESLibraryHandle, "GLES2DestroyContext");
    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.deinitDriver, s_GLESLibraryHandle, "GLES2DeInitdriver");
    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.setDrawSurface, s_GLESLibraryHandle, "GLES2SetDrawSurface");
    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.setReadSurface, s_GLESLibraryHandle, "GLES2SetReadSurface");
    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.bindTexImage, s_GLESLibraryHandle, "GLES2BindTexImage");
    GET_FUNCTION_ADDRESS(s_GLESFunctionTable.releaseTexImage, s_GLESLibraryHandle, "GLES2ReleaseTexImage");

    LOGI("libGLESv2_fimg.so was loaded and GLES 2.0 API was successfully mapped");

    return 1;
}

static void EGLSurfaceToGLESSurface(iEGLSurface* surface, GLES2SurfaceData* surf)
{
  if (surface->surfaceType & EGL_WINDOW_BIT)
  {
    PLATFORM_GET_WINDOW_ADDRESS(surface->display, surface->window, &surf->colorAddr.vaddr, &surf->colorAddr.paddr, surface->userData);
  }
  else
  {
    surf->colorAddr.vaddr = surface->backBuffer1->vaddr;
    surf->colorAddr.paddr = surface->backBuffer1->paddr;
  }

  surf->depthStencilAddr.paddr = surface->depthBuffer->paddr;
  surf->depthStencilAddr.vaddr = surface->depthBuffer->vaddr;
  surf->flipped = 0;
  surf->height = surface->height;
  surf->width = surface->width;
  surf->nativeColorFormat = static_cast<PxFmt>(surface->colorFormat);
  surf->nativeDepthStencilFormat = static_cast<PxFmt>(surface->depthStencilFormat);
}

void GLESSetColorBuffer20(iEGLSurface *surface, EGLint readdraw)
{
    GLES2SurfaceData surf;
    EGLSurfaceToGLESSurface(surface, &surf);

    if (readdraw == EGL_DRAW)
    {
    LOGE_IF(s_GLESFunctionTable.setDrawSurface == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
        s_GLESFunctionTable.setDrawSurface(&surf);
  }
    else if (readdraw == EGL_READ)
    {
      LOGE_IF(s_GLESFunctionTable.setReadSurface == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
        s_GLESFunctionTable.setReadSurface(&surf);
  }
}

void* GLESCreateContext20(EGLContext share_context)
{
  // check if GLES library needs to be loaded
  if (s_GLESLibraryHandle == 0)
  {
    // load the GLES20 library and map all neccessary functions
    if (!MapGLESFunctions())
    {
      return 0;
    }

    // need to initialize GLES after first loading of the library
    LOGE_IF(s_GLESFunctionTable.initDriver == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
    if (!s_GLESFunctionTable.initDriver())
    {
      LOGE("EGL: Cannot init OpenGL/ES 2.0 driver");
      return 0;
    }
  }

  LOGE_IF(s_GLESFunctionTable.createContext == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
  return s_GLESFunctionTable.createContext(share_context);
}

EGLBoolean GLESSetContext20(void* context)
{
  LOGE_IF(s_GLESFunctionTable.setContext == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
  s_GLESFunctionTable.setContext(context);
  return EGL_TRUE;
}

void GLESDestroyContext20(void* clientContext)
{
  LOGE_IF(s_GLESFunctionTable.destroyContext == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
  s_GLESFunctionTable.destroyContext(clientContext);
}

void GLESBindTexImage20(iEGLSurface *surface)
{
  GLES2SurfaceData surf;
  EGLSurfaceToGLESSurface(surface, &surf);

  LOGE_IF(s_GLESFunctionTable.bindTexImage == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
  s_GLESFunctionTable.bindTexImage(&surf, surface->textureTarget == EGL_TEXTURE_2D ? GL_TEXTURE_2D : 0, 0, surface->mipmapTexture);
}

void GLESReleaseTexImage20(iEGLSurface *surface)
{
    GLES2SurfaceData surf;
    EGLSurfaceToGLESSurface(surface, &surf);

  LOGE_IF(s_GLESFunctionTable.releaseTexImage == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
  s_GLESFunctionTable.releaseTexImage(&surf);
}

void GLESFlush20()
{
  LOGE_IF(s_GLESFunctionTable.flush == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
  s_GLESFunctionTable.flush();
}

void GLESFinish20()
{
  LOGE_IF(s_GLESFunctionTable.finish == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
  s_GLESFunctionTable.finish();
}

void GLESViewport20(EGLint x, EGLint y, EGLint width, EGLint height)
{
  LOGE_IF(s_GLESFunctionTable.viewport == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
  s_GLESFunctionTable.viewport(x, y, width, height);
}

void GLESScissor20(EGLint x, EGLint y, EGLint width, EGLint height)
{
  LOGE_IF(s_GLESFunctionTable.scissor == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
  s_GLESFunctionTable.scissor(x, y, width, height);
}

void GLESDeinitDriver20()
{
  // shutdown GLES2 if any
  if (s_GLESLibraryHandle != 0)
  {
    LOGE_IF(s_GLESFunctionTable.deinitDriver == 0, "EGL: null function pointer in %s:%d", __FILE__, __LINE__);
    s_GLESFunctionTable.deinitDriver();

    dlclose(s_GLESLibraryHandle);

    s_GLESLibraryHandle = 0;

    memset(&s_GLESFunctionTable, 0, sizeof(s_GLESFunctionTable));

    LOGI("libGLESv2_fimg.so was unloaded and GLES 2.0 API was successfully unmapped\n");
  }
}
