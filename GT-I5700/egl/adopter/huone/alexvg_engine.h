/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : alexvg_engine.h                                         *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#ifndef __alexv_engine_h__
#define __alexv_engine_h__

#ifdef __cplusplus
extern "C" {
#endif

extern void VGSetColorBuffer( iEGLSurface *surface );
extern void* VGCreatePbufferFromClientBuffer( unsigned int clientBuffer, unsigned int * width, unsigned int * height );

extern void* VGCreateContext( EGLContext share_context );
extern EGLBoolean VGSetContext( void* context, int width, int height );
extern void  VGDestroyContext( void* context );

extern void VGFlush( void );
extern void VGFinish( void );

#ifdef __cplusplus
}
#endif

#endif
