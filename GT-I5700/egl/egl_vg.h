/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : egl_vg.h                                                *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#ifndef __egl_vg_h__
#define __egl_vg_h__

#ifdef __cplusplus
extern "C" {
#endif

extern void eglVGSetColorBuffer( iEGLSurface *surface );
extern EGLBoolean eglVGCreatePbufferFromClientBuffer( iEGLSurface *surface, EGLClientBuffer buffer );
extern EGLBoolean eglVGCreateContext( iEGLContext *context );
extern EGLBoolean eglVGSetContext( iEGLContext *context );
extern void eglVGDestroyContext( iEGLContext *context );

extern void eglVGFlush( void );
extern void eglVGFinish( void );

#ifdef __cplusplus
}
#endif

#endif
