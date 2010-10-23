/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : egl_vg.c                                                *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#include <EGL/egl.h>
#include <GLES/pixelFmts.h>
#include <GLES/gles11Context.h>
#include "common/egl_config.h"
#include "common/egl_memory.h"
#include "egl_vg.h"

void eglVGSetColorBuffer( iEGLSurface *surface )
{
    extern void VGSetColorBuffer( iEGLSurface *surface );

    VGSetColorBuffer( surface );
}

EGLBoolean eglVGCreatePbufferFromClientBuffer( iEGLSurface *surface, EGLClientBuffer buffer )
{
    extern void* VGCreatePbufferFromClientBuffer( unsigned int clientBuffer, unsigned int * width, unsigned int * height );

    if( surface )
        surface->backBuffer1 = VGCreatePbufferFromClientBuffer( buffer, &surface->width, &surface->height );
    else
        return EGL_FALSE;

    if( surface->backBuffer1 )
        return EGL_TRUE;
    else
        return EGL_FALSE;
}

EGLBoolean eglVGCreateContext( iEGLContext *context )
{
    extern void* VGCreateContext( EGLContext share_context );

    if( context )
    {
        if( context->shareContext )
            context->clientContext = VGCreateContext( ((iEGLContext *)(context->shareContext))->clientContext );
        else
            context->clientContext = VGCreateContext( EGL_NO_CONTEXT );
    }
    else
    {
        return EGL_FALSE;
    }

    if( context->clientContext )
        return EGL_TRUE;
    else
        return EGL_FALSE;
}

EGLBoolean eglVGSetContext( iEGLContext *context )
{
    extern EGLBoolean VGSetContext( void* context, int width, int height );

    if( context )
    {
        if( !VGSetContext( context->clientContext, ((iEGLSurface*)context->drawSurface)->width, ((iEGLSurface*)context->drawSurface)->height ) )
            return EGL_FALSE;
    }
    else
    {
        if( !VGSetContext( EGL_NO_CONTEXT, 0, 0 ) )
            return EGL_FALSE;
    }

    return EGL_TRUE;
}

void eglVGDestroyContext( iEGLContext *context )
{
    extern void  VGDestroyContext( void* context );

    if( context && context->clientContext )
        VGDestroyContext( context->clientContext );
}

void eglVGFlush( void )
{
    extern void VGFlush( void );

    VGFlush();
}

void eglVGFinish( void )
{
    extern void VGFinish( void );

    VGFinish();
}
