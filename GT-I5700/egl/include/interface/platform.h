/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : platform.h                                              *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#ifndef __platform_h_
#define __platform_h_

#ifdef __cplusplus
extern "C" {
#endif

void  PLATFORM_INITIALIZE(NativeDisplayType display);
void  PLATFORM_FINALIZE(NativeDisplayType display);
int   PLATFORM_GET_DISPLAY_HORIZONTAL_RESOLUTION(NativeDisplayType display);
int   PLATFORM_GET_DISPLAY_VERTICAL_RESOLUTION(NativeDisplayType display);
int   PLATFORM_GET_DISPLAY_PIXEL_ASPECT_RATIO(NativeDisplayType display);

void  PLATFORM_WINDOW_SURFACE_INITIALIZE(NativeDisplayType display, NativeWindowType window, void** userData);
void  PLATFORM_WINDOW_SURFACE_FINALIZE(NativeDisplayType display, NativeWindowType window, void* userData);
void* PLATFORM_WINDOW_SURFACE_GET_BUFFER(NativeDisplayType display, NativeWindowType window, void* userData);
void  PLATFORM_WINDOW_SURFACE_SET_SWAP_RECTANGLE(NativeDisplayType display, NativeWindowType window, int x, int y, int w, int h, void* userData);
int   PLATFORM_IS_WINDOW_VALID(NativeDisplayType display, NativeWindowType window);
void  PLATFORM_GET_WINDOW_ADDRESS(NativeDisplayType display, NativeWindowType window, void **virtualAddr, void **physicalAddr, void* userData);
void  PLATFORM_GET_WINDOW_SIZE(NativeDisplayType display, NativeWindowType window, int *width, int *height, void* userData);

int   PLATFORM_IS_PIXMAP_VALID(NativeDisplayType display, NativePixmapType pixmap);
void  PLATFORM_GET_PIXMAP_SIZE(NativeDisplayType display, NativePixmapType pixmap, int *width, int *height);
void  PLATFORM_GET_PIXMAP_COLORBUFFER(NativeDisplayType display, NativePixmapType pixmap, void **virtualAddr, void **physicalAddr);

void  PLATFORM_DISPLAY_BUFFER(NativeDisplayType display, NativeWindowType window, void *buffer, int width, int height, int bufferBytes, void* userData);
void  PLATFORM_COPY_BUFFER(NativeDisplayType display, void *buffer, NativePixmapType pixmap, int width, int height, int bufferBytes);

void *PLATFORM_MALLOC( int size );

void PLATFORM_FREE( void *buffer );

void PLATFORM_MEMSET( void *buffer, int value, int size );

void PLATFORM_MEMCPY( void *dst, void *src, int size );

int PLATFORM_STRCMP( const char *str1, char *str2, int length );

#ifdef __cplusplus
}
#endif

#endif
