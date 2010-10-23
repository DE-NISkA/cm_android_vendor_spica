/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : sec_gles_1_1.c                                          *
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

#include "../gles20/src/ChunkAlloc.h"
#include <EGL/egl.h>

#include "../include/EGL/egl_config.h"
#include "sec_gles11.h"
#include "sec_gles20.h"
#include "sec_gles.h"

static int s_Version = 0;

/**********************************************************************

 Function   : GLESGetBuffer

 Parameter  :

 Notes      :

 **********************************************************************/
void* GLESGetBuffer(int size)
{
//	LOGE("EGL: creating new buffer");
	ChunkHandle* ch=new ChunkHandle(size);
//	LOGE("EGL: creating new buffer OK");
	return ch;
}

/**********************************************************************

 Function   : GLESFreeBuffer

 Parameter  :

 Notes      :

 **********************************************************************/
void GLESFreeBuffer(void* buffer)
{
//  LOGE("EGL: trying free buffer");
  if(buffer)
  {
  ChunkHandle *ch=(ChunkHandle*)buffer;
  delete ch;
  }
//  LOGE("EGL: free buffer OK");
}

/**********************************************************************

 Function   : GLESGetBufferAddress

 Parameter  :

 Notes      :

 **********************************************************************/
void GLESGetBufferAddress( void *buffer, void **physicalAddr, void **virtualAddr )
{
	if(buffer)
	{
	ChunkHandle* res = (ChunkHandle*)buffer;
	*physicalAddr = res->GetPhyAddr();
	*virtualAddr  = res->GetVirtAddr();
	}
}

/**********************************************************************

 Function   : GLESSetColorBuffer

 Parameter  : void *colorBuffer
              int bufferBytes
              void *depthBuffer
              void *stencilBuffer
              int depthSize
              int stencilSize
              int width
              int height

 Notes      : The specified colorBuffer and depthStencilBuffer is color, depth, stencil buffer of current surface.
              It will be called by eglMakeCurrent and eglSwapBuffers.
              OpenGLES uses the specifed colorBuffer as rendering buffer.

 **********************************************************************/
void GLESSetColorBuffer( iEGLSurface *surface, EGLint readdraw )
{
  if (s_Version == 1)
  {
    GLESSetColorBuffer11(surface, readdraw);
  }
  else if (s_Version == 2)
  {
    GLESSetColorBuffer20(surface, readdraw);
  }
  else
  {
    LOGE("EGL: null context in %s:%d", __FILE__, __LINE__);
  }
}

/**********************************************************************

 Function   : GLESCreateContext

 Parameter  : EGLContext share_context

 Notes      : 1. create a context of client APIs(OpenGLES)
              2. return that context

 **********************************************************************/
void* GLESCreateContext( EGLContext share_context, EGLint version )
{
  if (version == 1)
  {
    return GLESCreateContext11(share_context);
  }
  else if (version == 2)
  {
    return GLESCreateContext20(share_context);
  }

  return 0;
}

/**********************************************************************

 Function   : GLESSetContext

 Parameter  : void* context
              int width
              int height

 Notes      : Attach or detach a context to the driver.
              It will be called by eglMakeCurrent.

 **********************************************************************/
EGLBoolean GLESSetContext( void* context, EGLint version )
{
  if (version == 1 || (context == 0 && s_Version == 1))
  {
    if (!GLESSetContext11(context))
    {
      return EGL_FALSE;
    }
  }
  else if (version == 2 || (context == 0 && s_Version == 2))
  {
    if (!GLESSetContext20(context))
    {
      return EGL_FALSE;
    }
  }

  s_Version = version;

  return EGL_TRUE;
}

/**********************************************************************

 Function   : GLESDestroyContext

 Parameter  : void * clientContext

 Notes      : destory the specified clientContext

 **********************************************************************/
void GLESDestroyContext( void* clientContext, EGLint version )
{
  if (version == 1)
  {
    GLESDestroyContext11(clientContext);
  }
  else if (version == 2)
  {
    GLESDestroyContext20(clientContext);
  }
}

/**********************************************************************

 Function   : GLESBindTexImage

 Parameter  : unsigned int *colorBuffer
              int width
              int height
              int buffer

 Notes      : bind specified colorBuffer to texture iamge
              Specifed buffer is the name of that texture.

 **********************************************************************/
void GLESBindTexImage( iEGLSurface *surface )
{
  if (s_Version == 1)
  {
    GLESBindTexImage11(surface);
  }
  else if (s_Version == 2)
  {
    GLESBindTexImage20(surface);
  }
  else
  {
    LOGE("EGL: null context in %s:%d", __FILE__, __LINE__);
  }
}

/**********************************************************************

 Function   : GLESReleaseTexImage

 Parameter  : int buffer

 Notes      : release specified buffer

 **********************************************************************/
void GLESReleaseTexImage( iEGLSurface *surface )
{
  if (s_Version == 1)
  {
    GLESReleaseTexImage11(surface);
  }
  else if (s_Version == 2)
  {
    GLESReleaseTexImage20(surface);
  }
  else
  {
    LOGE("EGL: null context in %s:%d", __FILE__, __LINE__);
  }
}

/**********************************************************************

 Function   : GLESFlush

 Parameter  :

 Notes      : ensures that all outstanding requests on the current context will complete in finite time

 **********************************************************************/
void GLESFlush( void )
{
  if (s_Version == 1)
  {
    GLESFlush11();
  }
  else if (s_Version == 2)
  {
    GLESFlush20();
  }
  else
  {
    LOGE("EGL: null context in %s:%d", __FILE__, __LINE__);
  }
}

/**********************************************************************

 Function   : GLESFinish

 Parameter  :

 Notes      : forces all outstanding requests on the current context to complete
              return only when the last request has completed

 **********************************************************************/
void GLESFinish( void )
{
  if (s_Version == 1)
  {
    GLESFinish11();
  }
  else if (s_Version == 2)
  {
    GLESFinish20();
  }
  else
  {
    LOGE("EGL: null context in %s:%d", __FILE__, __LINE__);
  }
}

/**********************************************************************

 Function   : GLESViewport

 Parameter  :

 Notes      :

 **********************************************************************/
void GLESViewport( EGLint x, EGLint y, EGLint width, EGLint height )
{
  if (s_Version == 1)
  {
    GLESViewport11(x, y, width, height);
  }
  else if (s_Version == 2)
  {
    GLESViewport20(x, y, width, height);
  }
  else
  {
    LOGE("EGL: null context in %s:%d", __FILE__, __LINE__);
  }
}

/**********************************************************************

 Function   : GLESScissor

 Parameter  :

 Notes      :

 **********************************************************************/
void GLESScissor( EGLint x, EGLint y, EGLint width, EGLint height )
{
  if (s_Version == 1)
  {
    GLESScissor11(x, y, width, height);
  }
  else if (s_Version == 2)
  {
    GLESScissor20(x, y, width, height);
  }
  else
  {
    LOGE("EGL: null context in %s:%d", __FILE__, __LINE__);
  }
}

void GLESInitDriver()
{
  // all initialization will be done on first creation of a context
}

void GLESDeinitDriver()
{
  // shutdown GLES1 if any
  GLESDeinitDriver11();

  // shutdown GLES2 if any
  GLESDeinitDriver20();
}
