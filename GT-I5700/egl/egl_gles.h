/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : egl_gles.h                                              *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#ifndef __egl_gles_h__
#define __egl_gles_h__

#ifdef __cplusplus
extern "C" {
#endif

extern void *eglGLESGetBuffer( int size );
extern void eglGLESFreeBuffer( void *buffer );
extern void eglGLESGetBufferAddress( void *buffer, void **physicalAddr, void **virtualAddr );
extern void eglGLESSetColorBuffer( iEGLSurface *surface, EGLint readdraw );
extern EGLBoolean eglGLESCreateContext( iEGLContext *context );
extern EGLBoolean eglGLESSetContext( iEGLContext *context );
extern void eglGLESDestroyContext( iEGLContext *context );

extern void eglGLESBindTexImage( iEGLSurface *surface );
extern void eglGLESReleaseTexImage( iEGLSurface *surface );

extern void eglGLESFlush( void );
extern void eglGLESFinish( void );

extern void eglGLESViewport( EGLint x, EGLint y, EGLint width, EGLint height );
extern void eglGLESScissor( EGLint x, EGLint y, EGLint width, EGLint height );

extern void eglGLESDeinitDriver();
extern void eglGLESInitDriver();


#ifdef __cplusplus
}
#endif

#endif
