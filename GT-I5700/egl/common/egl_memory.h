/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : egl_memory.h                                            *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#ifndef __egl_memory_h_
#define __egl_memory_h_

#ifdef __cplusplus
extern "C" {
#endif

extern void PLATFORM_MALLOC_COLOR_BUFFER( void **colorBuffer1, void **colorBuffer2, int size );
extern void PLATFORM_MALLOC_DEPTH_BUFFER( void **depthBuffer, int size );
extern void PLATFORM_MALLOC_STENCIL_BUFFER( void **stencilBuffer, int size );
extern void PLATFORM_MALLOC_SAMPLE_BUFFER( void **sampleBuffer, int size );

extern void PLATFORM_FREE_COLOR_BUFFER( void **colorBuffer1, void **colorBuffer2 );
extern void PLATFORM_FREE_DEPTH_BUFFER( void **depthBuffer );
extern void PLATFORM_FREE_STENCIL_BUFFER( void **stencilBuffer );
extern void PLATFORM_FREE_SAMPLE_BUFFER( void **sampleBuffer );

extern void PLATFORM_MALLOC_TEXTURE_BUFFER( void **textureBuffer, int size );
extern void PLATFORM_FREE_TEXTURE_BUFFER( void **textureBuffer );

extern void PLATFORM_MALLOC_ALPHA_MASK_BUFFER( void **alphaMaskBuffer, int size );
extern void PLATFORM_FREE_ALPHA_MASK_BUFFER( void **alphaMaskBuffer );

extern void *PLATFORM_GET_CONTIGUOUS_ADDRESS( void *address, int size );

#ifdef __cplusplus
}
#endif

#endif
