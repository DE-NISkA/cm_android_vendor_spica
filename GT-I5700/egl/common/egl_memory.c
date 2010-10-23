/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : egl_memory.c                                            *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#include <interface/platform.h>
#include <sys/mman.h>      // mmap PROT_
#include <fcntl.h>         // O_RDWR
#include "egl_memory.h"

#define EGL_MEMORY_BASE                       0x57000000
#define EGL_MEMORY_SIZE                       0x01000000

#define EGL_MEMORY_OFFSET_COLORBUFFER1        0x00000000
//#define EGL_MEMORY_OFFSET_COLORBUFFER2        0x000BB800   // 800*480*2
#define EGL_MEMORY_OFFSET_COLORBUFFER2        0x00400000   // 800*480*2*2		// SYS.LSI

#define EGL_MEMORY_OFFSET_DEPTHBUFFER         0x00800000					// SYS.LSI
#define EGL_MEMORY_OFFSET_STENCILBUFFER       0x00000000
#define EGL_MEMORY_OFFSET_SAMPLEBUFFER        0x00000000

#define EGL_MEMORY_OFFSET_ALPHAMASKBUFFER     0x00000000



static unsigned int currentColorBuffer1    = EGL_MEMORY_BASE + EGL_MEMORY_OFFSET_COLORBUFFER1;
static unsigned int currentColorBuffer2    = EGL_MEMORY_BASE + EGL_MEMORY_OFFSET_COLORBUFFER2;
static unsigned int currentDepthBuffer     = EGL_MEMORY_BASE + EGL_MEMORY_OFFSET_DEPTHBUFFER;
static unsigned int currentStencilBuffer   = EGL_MEMORY_BASE + EGL_MEMORY_OFFSET_STENCILBUFFER;
static unsigned int currentSampleBuffer    = EGL_MEMORY_BASE + EGL_MEMORY_OFFSET_SAMPLEBUFFER;
static unsigned int currentAlphaMaskBuffer = EGL_MEMORY_BASE + EGL_MEMORY_OFFSET_ALPHAMASKBUFFER;

void PLATFORM_MALLOC_COLOR_BUFFER( void **colorBuffer1, void **colorBuffer2, int size )
{
#if defined(HUONE_MEMORY_MANAGEMENT)  // Temporary Define
    if( colorBuffer1 )
        (*colorBuffer1) = currentColorBuffer1;
    if( colorBuffer2 )
        (*colorBuffer2) = currentColorBuffer2;

	size = 0x00100000;	// SYS.LSI

    currentColorBuffer1 = *colorBuffer1 + size;
    currentColorBuffer2 = *colorBuffer2 + size;
#else
    if( colorBuffer1 )
        (*colorBuffer1) = PLATFORM_MALLOC( size );
    if( colorBuffer2 )
        (*colorBuffer2) = PLATFORM_MALLOC( size );
#endif

}

void PLATFORM_FREE_COLOR_BUFFER( void **colorBuffer1, void **colorBuffer2 )
{
#if defined(HUONE_MEMORY_MANAGEMENT)  // Temporary Define
    // nothing;
#else
    if( colorBuffer1 )
        PLATFORM_FREE( *colorBuffer1 );
    if( colorBuffer2 )
        PLATFORM_FREE( *colorBuffer2 );
#endif

    if( colorBuffer1 )
        (*colorBuffer1) = 0;
    if( colorBuffer2 )
        (*colorBuffer2) = 0;
}

void PLATFORM_MALLOC_DEPTH_BUFFER( void **depthBuffer, int size )
{
#if defined(HUONE_MEMORY_MANAGEMENT)  // Temporary Define
    if( depthBuffer )
        (*depthBuffer) = currentDepthBuffer;

    size = 0x00100000;	// SYS.LSI

    currentDepthBuffer = *depthBuffer + size;
#else
    if( depthBuffer )
        (*depthBuffer) = PLATFORM_MALLOC( size );
#endif

}


void PLATFORM_FREE_DEPTH_BUFFER( void **depthBuffer )
{
#if defined(HUONE_MEMORY_MANAGEMENT)  // Temporary Define
    // nothing;
#else
    if( depthBuffer )
        PLATFORM_FREE( *depthBuffer );
#endif

    if( depthBuffer )
        (*depthBuffer) = 0;
}

void PLATFORM_MALLOC_STENCIL_BUFFER( void **stencilBuffer, int size )
{
#if defined(HUONE_MEMORY_MANAGEMENT)  // Temporary Define
    if( stencilBuffer )
        (*stencilBuffer) = currentStencilBuffer;

    size = 0x00100000;	// SYS.LSI

    currentStencilBuffer = *stencilBuffer + size;
#else
    if( stencilBuffer )
        (*stencilBuffer) = PLATFORM_MALLOC( size );
#endif

}


void PLATFORM_FREE_STENCIL_BUFFER( void **stencilBuffer )
{
#if defined(HUONE_MEMORY_MANAGEMENT)  // Temporary Define
    // nothing;
#else
    if( stencilBuffer )
        PLATFORM_FREE( *stencilBuffer );
#endif

    if( stencilBuffer )
        (*stencilBuffer) = 0;
}

void PLATFORM_MALLOC_SAMPLE_BUFFER( void **sampleBuffer, int size )
{
#if defined(HUONE_MEMORY_MANAGEMENT)  // Temporary Define
    if( sampleBuffer )
        (*sampleBuffer) = currentSampleBuffer;

    size = 0x00100000;	// SYS.LSI

    currentSampleBuffer = *sampleBuffer + size;
#else
    if( sampleBuffer )
        (*sampleBuffer) = PLATFORM_MALLOC( size );
#endif

}


void PLATFORM_FREE_SAMPLE_BUFFER( void **sampleBuffer )
{
#if defined(HUONE_MEMORY_MANAGEMENT)  // Temporary Define
    // nothing;
#else
    if( sampleBuffer )
        PLATFORM_FREE( *sampleBuffer );
#endif

    if( sampleBuffer )
        (*sampleBuffer) = 0;
}

void PLATFORM_MALLOC_ALPHA_MASK_BUFFER( void **alphaMaskBuffer, int size )
{
#if defined(HUONE_MEMORY_MANAGEMENT)  // Temporary Define
    if( alphaMaskBuffer )
        (*alphaMaskBuffer) = currentAlphaMaskBuffer;

    size = 0x00100000;	// SYS.LSI

    currentAlphaMaskBuffer = *alphaMaskBuffer + size;
#else
    if( alphaMaskBuffer )
        (*alphaMaskBuffer) = PLATFORM_MALLOC( size );
#endif

}


void PLATFORM_FREE_ALPHA_MASK_BUFFER( void **alphaMaskBuffer )
{
#if defined(HUONE_MEMORY_MANAGEMENT)  // Temporary Define
    // nothing;
#else
    if( alphaMaskBuffer )
        PLATFORM_FREE( *alphaMaskBuffer );
#endif

    if( alphaMaskBuffer )
        (*alphaMaskBuffer) = 0;
}


void *PLATFORM_GET_CONTIGUOUS_ADDRESS( void *address, int size )
{
    static int count;

#if defined(HUONE_MEMORY_MANAGEMENT)  // Temporary Define
    const char memdev_desc[] = "/dev/mem";
    static int memdev;
    void *virtualAddress = 0;

    if(count == 0)
    {
        memdev = open(memdev_desc, O_RDWR | O_SYNC);

    if (memdev < 0) {
     printf("Cannot open memory device (%s)\n", memdev_desc);
     return;
        }
    }
    virtualAddress = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, memdev, address);
    count++;
    return virtualAddress;
#else
    void *physicalAddress = 0;

    return physicalAddress;
#endif

}
