/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : sec_gles_1_1.h                                          *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#ifndef __sec_gles_1_1_h__
#define __sec_gles_1_1_h__

extern void *GLESGetBuffer( int size );
extern void GLESFreeBuffer( void *buffer );
extern void GLESGetBufferAddress( void *buffer, void **physicalAddr, void **virtualAddr );

extern void GLESSetColorBuffer( iEGLSurface *surface, EGLint readdraw );

extern void* GLESCreateContext( EGLContext share_context, EGLint version );
extern EGLBoolean GLESSetContext( void* context, EGLint version );
extern void  GLESDestroyContext( void* clientContext, EGLint version );

extern void GLESBindTexImage( iEGLSurface *surface );
extern void GLESReleaseTexImage( iEGLSurface *surface );

extern void GLESFlush( void );
extern void GLESFinish( void );

extern void GLESViewport( EGLint x, EGLint y, EGLint width, EGLint height );
extern void GLESScissor( EGLint x, EGLint y, EGLint width, EGLint height );

extern void GLESDeinitDriver();
extern void GLESInitDriver();

#endif
