/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : alexvg_engine.c                                         *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#include <EGL/egl.h>
#include <GLES/pixelFmts.h>
#include <interface/platform.h>
#include <GLES/gles11Context.h>
#include "../../common/egl_config.h"
#include "alexvg_engine.h"

/**********************************************************************

 Function   : VGSetColorBuffer

 Parameter  :

 Notes      : The specified colorBuffer is color buffer of current surface.
              It will be called by eglMakeCurrent.
              OpenVG uses the specifed colorBuffer as rendering buffer.

 **********************************************************************/
void VGSetColorBuffer( iEGLSurface *surface )
{
    return;
}

/**********************************************************************

 Function   : VGCreatePbufferFromClientBuffer

 Parameter  : unsigned int clientBuffer

 Notes      : 1. create a pbuffer from client buffer of client APIs(OpenVG)
              2. return that buffer

 **********************************************************************/
void* VGCreatePbufferFromClientBuffer( unsigned int clientBuffer, unsigned int * width, unsigned int * height )
{
    return (void *)0;
}

/**********************************************************************

 Function   : VGCreateContext

 Parameter  : EGLContext share_context

 Notes      : 1. create a context of client APIs(OpenVG)
              2. return that context

 **********************************************************************/
void* VGCreateContext( EGLContext share_context )
{
    return (void *)0;
}

/**********************************************************************

 Function   : VGSetContext

 Parameter  : void* context
              int width
              int height

 Notes      : Attach or detach a context to the driver.
              It will be called by eglMakeCurrent.

 **********************************************************************/
EGLBoolean VGSetContext( void* context, int width, int height )
{
    return 0;
}

/**********************************************************************

 Function   : VGDestroyContext

 Parameter  : void * clientContext

 Notes      : destory the specified clientContext

 **********************************************************************/
void VGDestroyContext( void* context )
{
    return;
}

/**********************************************************************

 Function   : VGFlush

 Parameter  :

 Notes      : ensures that all outstanding requests on the current context will complete in finite time

 **********************************************************************/
void VGFlush( void )
{
    return;
}

/**********************************************************************

 Function   : VGFinish

 Parameter  :

 Notes      : forces all outstanding requests on the current context to complete
              return only when the last request has completed

 **********************************************************************/
void VGFinish( void )
{
    return;
}
