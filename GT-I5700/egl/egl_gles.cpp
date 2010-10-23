/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : egl_gles.c                                              *
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
#include "egl_gles.h"

void eglGLESFreeBuffer( void *buffer )
{
    extern void GLESFreeBuffer( void *buffer );

    GLESFreeBuffer( buffer );
}

void *eglGLESGetBuffer( int size )
{
    extern void *GLESGetBuffer( int size );

    return GLESGetBuffer( size );
}

void eglGLESGetBufferAddress( void *buffer, void **physicalAddr, void **virtualAddr )
{
    extern void GLESGetBufferAddress( void *buffer, void **physicalAddr, void **virtualAddr );

    GLESGetBufferAddress( buffer, physicalAddr, virtualAddr );
}

static int isInit;

void eglGLESSetColorBuffer( iEGLSurface *surface, EGLint readdraw )
{
    extern void GLESSetColorBuffer( iEGLSurface *surface, EGLint readdraw );

    GLESSetColorBuffer( surface, readdraw );
}

EGLBoolean eglGLESCreateContext( iEGLContext *context )
{
    extern void* GLESCreateContext( EGLContext share_context, EGLint version );

    if( context )
    {
        if( context->shareContext )
            context->clientContext = GLESCreateContext( ((iEGLContext *)(context->shareContext))->clientContext, context->clientVersion );
        else
            context->clientContext = GLESCreateContext( EGL_NO_CONTEXT, context->clientVersion );
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

EGLBoolean eglGLESSetContext( iEGLContext *context )
{
    extern EGLBoolean GLESSetContext( void* context, EGLint version );

    if( context )
    {
        if( !GLESSetContext( context->clientContext, context->clientVersion ) )
            return EGL_FALSE;
    }
    else
    {
        if( !GLESSetContext( EGL_NO_CONTEXT, 0 ) )
            return EGL_FALSE;
    }

    return EGL_TRUE;
}

void eglGLESDestroyContext( iEGLContext *context )
{
    extern void  GLESDestroyContext( void* clientContext, EGLint version );

    if( context && context->clientContext )
        GLESDestroyContext( context->clientContext, context->clientVersion );
}

void eglGLESBindTexImage( iEGLSurface *surface )
{
    extern void GLESBindTexImage( iEGLSurface *surface );

    GLESBindTexImage( surface );
}

void eglGLESReleaseTexImage( iEGLSurface *surface )
{
    extern void GLESReleaseTexImage( iEGLSurface *surface );

    GLESReleaseTexImage( surface );
}


void eglGLESFlush( void )
{
    extern void GLESFlush( void );

    GLESFlush();
}

void eglGLESFinish( void )
{
    extern void GLESFinish( void );

    GLESFinish();
}

void eglGLESViewport( EGLint x, EGLint y, EGLint width, EGLint height )
{
    extern void GLESViewport( int x, int y, int width, int height );

    GLESViewport( x, y, width, height );
}

void eglGLESScissor( EGLint x, EGLint y, EGLint width, EGLint height )
{
    extern void GLESScissor( int x, int y, int width, int height );

    GLESScissor( x, y, width, height );
}

void eglGLESInitDriver( void )
{
    extern void GLESInitDriver( void );

    GLESInitDriver();
}

void eglGLESDeinitDriver( void )
{
    extern void GLESDeinitDriver( void );

    GLESDeinitDriver();
}
