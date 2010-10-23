/************************************************************************
*                                                                      *
*  Implementation of EGL, version 1.3                                  *
*                                                                      *
*  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
*                                                                      *
************************************************************************/

/************************************************************************
*                                                                      *
* FileName   : egl.c                                                   *
* Descrition :                                                         *
* Notes      :                                                         *
*                                                                      *
* Log                                                                  *
*      -                                                               *
*                                                                      *
************************************************************************/

#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gles11Context.h>
#include "common/egl_config.h"
#include "common/egl_memory.h"
#include "egl_vg.h"
#include "egl_gles.h"

#include <KD/kd.h>
#include <KD/KHR_formatted.h>

#include <interface/platform.h>

#include <sys/ioctl.h>
#include <fcntl.h>

///////////////////////////////////////////////////////////
#include <sys/time.h>
#include <cutils/log.h>


static inline double  __test_tval(struct timeval _tstart,struct timeval _tend)
{
	double t1 = 0.;
	double t2 = 0.;

	t1 = ((double)_tstart.tv_sec * 1000 + (double)_tstart.tv_usec/1000.0) ;
	t2 = ((double)_tend.tv_sec * 1000 + (double)_tend.tv_usec/1000.0) ;

	return t2-t1;
}

///////////////////////////////////////////////////////////




const char *lib_version = "1.3 (lib version 13.02.22)";

#define EGL_MAX_DISPLAY 100
#define EGL_MAX_THREAD  100

#define EGL_MAX_ASSOCIATED_WINDOW 100
#define EGL_MAX_ASSOCIATED_PIXMAP 100


#define   EGL_MIN( X, Y )          ( ( (X) < (Y) ) ?  (X) : (Y) )

/* add Define by Jung*/
#define FB0 0x44ba2000
#define FB1 0x44bed000

/*------------------------------------------------------------------------
* Internal EGL Types.
*----------------------------------------------------------------------*/
#if defined(WIN32)
typedef DWORD ThreadID;
#else
typedef pthread_t ThreadID;
#endif

typedef struct _EGLThreadState
{
	ThreadID       threadID;
	EGLDisplay     display;
	iEGLContext   *context;
	iEGLSurface   *read;
	iEGLSurface   *draw;
	EGLint         error;
	EGLint         boundAPI;
} EGLThreadState;

typedef struct _EGLConfigAtt
{
	EGLint      eglConfigID;

	EGLint      eglBufferSize;
	EGLint      eglRedSize;
	EGLint      eglGreenSize;
	EGLint      eglBlueSize;
	EGLint      eglLuminanceSize;
	EGLint      eglAlphaSize;

	EGLenum     eglColorBufferType;
	EGLenum     eglConfigCaveat;
	EGLint      eglMaxPbufferWidth;
	EGLint      eglMaxPbufferHeight;
	EGLint      eglMaxPbufferPixels;
	EGLint      eglMaxSwapInterval;
	EGLint      eglMinSwapInterval;
	EGLBoolean  eglNativeRenderable;
	EGLint      eglNativeVisualID;
	EGLint      eglNativeVisualType;
	EGLint      eglRenderableType;
	EGLint      eglSurfaceType;
	EGLenum   eglTransparentType;
	EGLint      eglTransparentRedValue;
	EGLint      eglTransparentGreenValue;
	EGLint      eglTransparentBlueValue;

	EGLint      eglConformant;        /* for EGL 1.3 */
	EGLint      eglMatchedPixmap;     /* for EGL 1.3 */

	EGLint      eglAlphaMaskSize;     /* used only by OpenVG */
	EGLint      eglLevel;         /* used only by OpenGLES */
	EGLBoolean  eglBindToTextureRGB;    /* used only by OpenGLES */
	EGLBoolean  eglBindToTextureRGBA;   /* used only by OpenGLES */
	EGLint      eglDepthSize;       /* used only by OpenGLES */
	EGLint      eglSampleBuffers;     /* used only by OpenGLES */
	EGLint      eglSamples;         /* used only by OpenGLES */
	EGLint      eglStencilSize;       /* used only by OpenGLES */

	EGLNativeWindowType associatedWindow[EGL_MAX_ASSOCIATED_WINDOW];
	EGLNativePixmapType associatedPixmap[EGL_MAX_ASSOCIATED_PIXMAP];
} EGLConfigAtt;

typedef struct _EGLState
{
	EGLBoolean      init;
	EGLDisplay      displays[EGL_MAX_DISPLAY];
	EGLint          num_display;
	EGLThreadState  threads[EGL_MAX_THREAD];
	EGLint          currentThreadID;
	EGLint          num_thread;
	EGLConfig      *configs;
	EGLConfigAtt   *defaultConfig;
	EGLint          num_config;
	EGLint          major;
	EGLint          minor;
} EGLState;

/*------------------------------------------------------------------------
* Global Variables.
*----------------------------------------------------------------------*/
static EGLState _egl;

/*------------------------------------------------------------------------
* Internal EGL Functions regarding Thread
*----------------------------------------------------------------------*/

#if defined(WIN32)

#include <windows.h>


static ThreadID getCurrentThreadID(void)
{
	return GetCurrentThreadId();
}

static HANDLE mutex = NULL;
static int mutexRefCount = 0;
static void initMutex(void)
{
	mutex = CreateMutex(NULL, FALSE, NULL);
	mutexRefCount = 0;
	assert(mutex != NULL);
}

static void deinitMutex(void)
{
	EGLBoolean ret;

	assert(mutex);
	assert(mutexRefCount == 0);
	ret = CloseHandle(mutex);
	assert(ret);
}

static void acquireMutex(void)
{
	DWORD ret;

	if(!mutex)
	initMutex();

	assert(mutex);
	ret = WaitForSingleObject(mutex, INFINITE);
	assert(ret != WAIT_FAILED);
	mutexRefCount++;
}

static void releaseMutex(void)
{
	EGLBoolean ret;

	assert(mutex);
	mutexRefCount--;
	assert(mutexRefCount >= 0);
	ret = ReleaseMutex(mutex);
	assert(ret);
}

#else



static ThreadID getCurrentThreadID(void)
{
	return pthread_self();
}

static pthread_mutex_t mutex;
static EGLint mutexRefCount = 0;
static EGLBoolean mutexInitialized = EGL_FALSE;
static pthread_once_t once_mutex_init = PTHREAD_ONCE_INIT;
static void initMutex(void)
{
	EGLint ret;
	pthread_mutexattr_t attr;

	ret = pthread_mutexattr_init(&attr);
	assert(!ret);
	ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	assert(!ret);
	ret = pthread_mutex_init(&mutex, &attr);
	pthread_mutexattr_destroy(&attr);
	assert(!ret);
	mutexInitialized = EGL_TRUE;
}

static void deinitMutex(void)
{
	EGLint ret;

	assert(mutexInitialized);
	assert(mutexRefCount == 0);
	ret = pthread_mutex_destroy(&mutex);
	assert(ret != EINVAL);
	assert(ret != EAGAIN);
	assert(!ret);
}

static void acquireMutex(void)
{
	EGLint ret;

	if(!mutexInitialized)
	pthread_once(&once_mutex_init, initMutex);

	assert(mutexInitialized);
	ret = pthread_mutex_lock(&mutex);
	assert(ret != EINVAL);
	assert(ret != EAGAIN);
	assert(ret != EDEADLK);
	assert(!ret);
	mutexRefCount++;
}

static void releaseMutex(void)
{
	EGLint ret;

	assert(mutexInitialized);
	mutexRefCount--;
	assert(mutexRefCount >= 0);
	ret = pthread_mutex_unlock(&mutex);
	assert(ret != EPERM);
	assert(!ret);
}

#endif

static EGLThreadState *getCurrentThread()
{
	EGLint i, num_thread;
	ThreadID currentThreadID = getCurrentThreadID();
	EGLThreadState *thread;

	num_thread = _egl.num_thread;

	for( i = 0; i < num_thread; i++ )
	{
		if(currentThreadID == _egl.threads[i].threadID)
		return &_egl.threads[i];
	}

	thread = &_egl.threads[num_thread];

	thread->threadID        = currentThreadID;
	thread->context     = (iEGLContext*)EGL_NO_CONTEXT;
	thread->read      = (iEGLSurface*)EGL_NO_SURFACE;
	thread->draw        = (iEGLSurface*)EGL_NO_SURFACE;
	_egl.num_thread         = num_thread + 1;
	thread->boundAPI        = EGL_OPENGL_ES_API;

	return thread;
}

/*------------------------------------------------------------------------
* Internal EGL Functions.
*----------------------------------------------------------------------*/
#define EGL_ERROR( error, retval ) { setError( error ); if(error != EGL_SUCCESS) fprintf(stderr,"egl ERROR: %d line: %d\n",error, __LINE__); releaseMutex(); return retval; }

static void setError(EGLint error)
{
	EGLThreadState *thread;

	if (!_egl.init)
	return;

	thread = getCurrentThread();
	if(!thread)
	return;

	thread->error = error;
}

#define ORIGIN_SWITCH 0
static void copyBuffer( void *dstBuffer, void *srcBuffer, EGLint dstWidth, EGLint dstHeight, EGLint srcWidth, EGLint srcHeight, EGLint bufferBytes )
{
	unsigned short *dstBuffer16 = (unsigned short *)dstBuffer;
	unsigned short *srcBuffer16 = (unsigned short *)srcBuffer;
	unsigned int   *dstBuffer32 = (unsigned int *)dstBuffer;
	unsigned int   *srcBuffer32 = (unsigned int *)srcBuffer;
	EGLint i;
	EGLint minWidth, minHeight;

	minWidth  = EGL_MIN( dstWidth, srcWidth );
	minHeight = EGL_MIN(dstHeight, srcHeight);

	if( bufferBytes == 2 )
	{
#if defined(ORIGIN_SWITCH)
		dstBuffer16+= dstWidth*(dstHeight-1);
		srcBuffer16+= srcWidth*(srcHeight-1);
#endif
		for( i = 0; i < minHeight; i++ )
		{
			memcpy( dstBuffer16, srcBuffer16, 2 * minWidth );

#if !defined(ORIGIN_SWITCH)
			dstBuffer16 += dstWidth;
			srcBuffer16 += srcWidth;
#else
			dstBuffer16 -= dstWidth;
			srcBuffer16 -= srcWidth;
#endif
		}
	}
	else if( bufferBytes == 4 )
	{
		for( i = 0; i < minHeight; i++ )
		{
			memcpy( dstBuffer32, srcBuffer32, 4 * minWidth );

			dstBuffer32 += dstWidth;
			srcBuffer32 += srcWidth;
		}
	}
}

static void resizeWindowSurface(EGLSurface surface, EGLint width, EGLint height)
{
	iEGLSurface  *iSurface = (iEGLSurface *)surface;
	EGLint        size;

#if !defined(DIRECT_WINDOW_DRAWING)
	if( iSurface->bufferBytes == 4 )
	{
		void *backBuffer1Handle, *backBuffer2Handle, *backBuffer3Handle;
		void *curColorBuffer;

		size = width * height * iSurface->bufferBytes;

#if defined(DOUBLE_BUFFER)
		backBuffer1Handle = iSurface->backBuffer1Handle;
		backBuffer2Handle = iSurface->backBuffer2Handle;

		if( iSurface->curDrawingBuffer == 1 )
		curColorBuffer = iSurface->backBuffer1->vaddr;
		else if( iSurface->curDrawingBuffer == 2 )
		curColorBuffer = iSurface->backBuffer2->vaddr;

		iSurface->backBuffer1Handle = eglGLESGetBuffer( size );
		iSurface->backBuffer2Handle = eglGLESGetBuffer( size );

		eglGLESGetBufferAddress( iSurface->backBuffer1Handle, &iSurface->backBuffer1->paddr, &iSurface->backBuffer1->vaddr );
		eglGLESGetBufferAddress( iSurface->backBuffer2Handle, &iSurface->backBuffer2->paddr, &iSurface->backBuffer2->vaddr );

		if( iSurface->curDrawingBuffer == 1 )
		copyBuffer( iSurface->backBuffer1->vaddr, curColorBuffer, width, height, iSurface->width, iSurface->height, iSurface->bufferBytes );
		else if( iSurface->curDrawingBuffer == 2 )
		copyBuffer( iSurface->backBuffer2->vaddr, curColorBuffer, width, height, iSurface->width, iSurface->height, iSurface->bufferBytes );

		eglGLESFreeBuffer( backBuffer1Handle );
		eglGLESFreeBuffer( backBuffer2Handle );

#elif defined(TRIPLE_BUFFER)
		backBuffer1Handle = iSurface->backBuffer1Handle;
		backBuffer2Handle = iSurface->backBuffer2Handle;

		if( iSurface->curDrawingBuffer == 1 )
		curColorBuffer = iSurface->backBuffer1->vaddr;
		else if( iSurface->curDrawingBuffer == 2 )
		curColorBuffer = iSurface->backBuffer2->vaddr;
		else if( iSurface->curDrawingBuffer == 3 )
		curColorBuffer = iSurface->backBuffer3->vaddr;

		iSurface->backBuffer1Handle = eglGLESGetBuffer( size );
		iSurface->backBuffer2Handle = eglGLESGetBuffer( size );
		iSurface->backBuffer3Handle = eglGLESGetBuffer( size );

		eglGLESGetBufferAddress( iSurface->backBuffer1Handle, &iSurface->backBuffer1->paddr, &iSurface->backBuffer1->vaddr );
		eglGLESGetBufferAddress( iSurface->backBuffer2Handle, &iSurface->backBuffer2->paddr, &iSurface->backBuffer2->vaddr );
		eglGLESGetBufferAddress( iSurface->backBuffer3Handle, &iSurface->backBuffer3->paddr, &iSurface->backBuffer3->vaddr );

		if( iSurface->curDrawingBuffer == 1 )
		copyBuffer( iSurface->backBuffer1->vaddr, curColorBuffer, width, height, iSurface->width, iSurface->height, iSurface->bufferBytes );
		else if( iSurface->curDrawingBuffer == 2 )
		copyBuffer( iSurface->backBuffer2->vaddr, curColorBuffer, width, height, iSurface->width, iSurface->height, iSurface->bufferBytes );
		else if( iSurface->curDrawingBuffer == 2 )
		copyBuffer( iSurface->backBuffer3->vaddr, curColorBuffer, width, height, iSurface->width, iSurface->height, iSurface->bufferBytes );

		eglGLESFreeBuffer( backBuffer1Handle );
		eglGLESFreeBuffer( backBuffer2Handle );
		eglGLESFreeBuffer( backBuffer3Handle );

#else
		backBuffer1Handle = iSurface->backBuffer1Handle;
		curColorBuffer = iSurface->backBuffer1->vaddr;
		iSurface->backBuffer1Handle = eglGLESGetBuffer( size );
		eglGLESGetBufferAddress( iSurface->backBuffer1Handle, &iSurface->backBuffer1->paddr, &iSurface->backBuffer1->vaddr );
		copyBuffer( iSurface->backBuffer1->vaddr, curColorBuffer, width, height, iSurface->width, iSurface->height, iSurface->bufferBytes );
		eglGLESFreeBuffer( backBuffer1Handle );

#endif
	}
	else if( iSurface->bufferBytes == 2 )
	{
		void *backBuffer1Handle, *backBuffer2Handle, *backBuffer3Handle;
		void *curColorBuffer;

		size = width * height * iSurface->bufferBytes;

#if defined(DOUBLE_BUFFER)
		backBuffer1Handle = iSurface->backBuffer1Handle;
		backBuffer2Handle = iSurface->backBuffer2Handle;

		if( iSurface->curDrawingBuffer == 1 )
		curColorBuffer = iSurface->backBuffer1->vaddr;
		else if( iSurface->curDrawingBuffer == 2 )
		curColorBuffer = iSurface->backBuffer2->vaddr;

		iSurface->backBuffer1Handle = eglGLESGetBuffer( size );
		iSurface->backBuffer2Handle = eglGLESGetBuffer( size );

		eglGLESGetBufferAddress( iSurface->backBuffer1Handle, &iSurface->backBuffer1->paddr, &iSurface->backBuffer1->vaddr );
		eglGLESGetBufferAddress( iSurface->backBuffer2Handle, &iSurface->backBuffer2->paddr, &iSurface->backBuffer2->vaddr );

		if( iSurface->curDrawingBuffer == 1 )
		copyBuffer( iSurface->backBuffer1->vaddr, curColorBuffer, width, height, iSurface->width, iSurface->height, iSurface->bufferBytes );
		else if( iSurface->curDrawingBuffer == 2 )
		copyBuffer( iSurface->backBuffer2->vaddr, curColorBuffer, width, height, iSurface->width, iSurface->height, iSurface->bufferBytes );

		eglGLESFreeBuffer( backBuffer1Handle );
		eglGLESFreeBuffer( backBuffer2Handle );

#elif defined(TRIPLE_BUFFER)
		backBuffer1Handle = iSurface->backBuffer1Handle;
		backBuffer2Handle = iSurface->backBuffer2Handle;

		if( iSurface->curDrawingBuffer == 1 )
		curColorBuffer = iSurface->backBuffer1->vaddr;
		else if( iSurface->curDrawingBuffer == 2 )
		curColorBuffer = iSurface->backBuffer2->vaddr;
		else if( iSurface->curDrawingBuffer == 3 )
		curColorBuffer = iSurface->backBuffer3->vaddr;

		iSurface->backBuffer1Handle = eglGLESGetBuffer( size );
		iSurface->backBuffer2Handle = eglGLESGetBuffer( size );
		iSurface->backBuffer3Handle = eglGLESGetBuffer( size );

		eglGLESGetBufferAddress( iSurface->backBuffer1Handle, &iSurface->backBuffer1->paddr, &iSurface->backBuffer1->vaddr );
		eglGLESGetBufferAddress( iSurface->backBuffer2Handle, &iSurface->backBuffer2->paddr, &iSurface->backBuffer2->vaddr );
		eglGLESGetBufferAddress( iSurface->backBuffer3Handle, &iSurface->backBuffer3->paddr, &iSurface->backBuffer3->vaddr );

		if( iSurface->curDrawingBuffer == 1 )
		copyBuffer( iSurface->backBuffer1->vaddr, curColorBuffer, width, height, iSurface->width, iSurface->height, iSurface->bufferBytes );
		else if( iSurface->curDrawingBuffer == 2 )
		copyBuffer( iSurface->backBuffer2->vaddr, curColorBuffer, width, height, iSurface->width, iSurface->height, iSurface->bufferBytes );
		else if( iSurface->curDrawingBuffer == 2 )
		copyBuffer( iSurface->backBuffer3->vaddr, curColorBuffer, width, height, iSurface->width, iSurface->height, iSurface->bufferBytes );

		eglGLESFreeBuffer( backBuffer1Handle );
		eglGLESFreeBuffer( backBuffer2Handle );
		eglGLESFreeBuffer( backBuffer3Handle );

#else
		backBuffer1Handle = iSurface->backBuffer1Handle;
		curColorBuffer = iSurface->backBuffer1->vaddr;
		iSurface->backBuffer1Handle = eglGLESGetBuffer( size );
		eglGLESGetBufferAddress( iSurface->backBuffer1Handle, &iSurface->backBuffer1->paddr, &iSurface->backBuffer1->vaddr );
		copyBuffer( iSurface->backBuffer1->vaddr, curColorBuffer, width, height, iSurface->width, iSurface->height, iSurface->bufferBytes );
		eglGLESFreeBuffer( backBuffer1Handle );

#endif
	}
#endif

	size = width * height * sizeof(unsigned int);

	if( iSurface->depthBufferHandle )
	eglGLESFreeBuffer( iSurface->depthBufferHandle );

	iSurface->depthBufferHandle = eglGLESGetBuffer( size );
	eglGLESGetBufferAddress( iSurface->depthBufferHandle, &iSurface->depthBuffer->paddr, &iSurface->depthBuffer->vaddr );

	iSurface->stencilBuffer = iSurface->depthBuffer;

	iSurface->width = width;
	iSurface->height = height;
}

static EGLBoolean isContextCurrentToOtherThread( EGLContext context )
{
	EGLint i, num_thread;
	ThreadID currentThreadID = getCurrentThreadID();
	iEGLContext *iContext = (iEGLContext *)context;

	num_thread = _egl.num_thread;

	for( i = 0; i < num_thread; i++ )
	{
		if(currentThreadID != _egl.threads[i].threadID)
		{
			if( _egl.threads[i].context )
			if( iContext == _egl.threads[i].context )
			return EGL_TRUE;
		}
	}

	return EGL_FALSE;
}

static EGLBoolean isSurfaceBoundToContextInOtherThread( EGLSurface surface )
{
	EGLint i, num_thread;
	ThreadID currentThreadID = getCurrentThreadID();
	iEGLSurface *iSurface = (iEGLSurface *)surface;

	num_thread = _egl.num_thread;

	for( i = 0; i < num_thread; i++ )
	{
		if(currentThreadID != _egl.threads[i].threadID)
		{
			if( _egl.threads[i].context && _egl.threads[i].context->drawSurface && _egl.threads[i].context->readSurface )
			if( iSurface == _egl.threads[i].context->drawSurface || iSurface == _egl.threads[i].context->readSurface )
			return EGL_TRUE;
		}
	}

	return EGL_FALSE;
}

static void sortingConfigs( EGLConfig** configs, EGLint num_config )
{
	EGLint i, j;
	EGLint tmpPriority;
	EGLConfigAtt** configAtt;
	EGLint* priority;
	EGLint caveatWeight          = 0;
	EGLint colorBufferTypeWeight = 0;
	EGLint colorBufferSizeWeight = 0;
	EGLint sampleBufferWeight    = 0;
	EGLint samplesWeight         = 0;
	EGLint depthSizeWeight       = 0;
	EGLint stencilSizeWeight     = 0;
	EGLint alphaMaskWeight       = 1;
	EGLint colorBufferTypeRange  = 2;
	EGLint colorBufferSizeRange  = 32;
	EGLint sampleBufferRange     = 1;
	EGLint samplesRange          = 32;
	EGLint depthSizeRange        = 32;
	EGLint stencilSizeRange      = 32;
	EGLint alphaMaskSizeRange    = 32;

	stencilSizeWeight     = alphaMaskWeight       * (alphaMaskSizeRange   + 1);
	depthSizeWeight       = stencilSizeWeight     * (stencilSizeRange     + 1);
	samplesWeight         = depthSizeWeight       * (depthSizeRange       + 1);
	sampleBufferWeight    = samplesWeight         * (samplesRange         + 1);
	colorBufferSizeWeight = sampleBufferWeight    * (sampleBufferRange    + 1);
	colorBufferTypeWeight = colorBufferSizeWeight * (colorBufferSizeRange + 1);
	caveatWeight          = colorBufferTypeWeight * (colorBufferTypeRange + 1);

	configAtt = (EGLConfigAtt **)malloc( sizeof(EGLConfigAtt *) * num_config );
	priority  = (EGLint*)malloc( sizeof(EGLint) * num_config );

	memset( configAtt, 0x00, sizeof(EGLConfigAtt *) * num_config );
	memset( priority,  0x00, sizeof(EGLint) * num_config );

	for( i = 0; i < num_config; i++ )
	configAtt[i] = (EGLConfigAtt*)(*configs)[i];

	for( i = 0; i < num_config; i++ )
	{
		if( configAtt[i]->eglConfigCaveat == EGL_NONE )
		priority[i] += caveatWeight * 3;
		else if( configAtt[i]->eglConfigCaveat == EGL_SLOW_CONFIG )
		priority[i] += caveatWeight * 2;
		else if( configAtt[i]->eglConfigCaveat == EGL_NON_CONFORMANT_CONFIG )
		priority[i] += caveatWeight * 1;
		else if( configAtt[i]->eglConfigCaveat == (EGLenum)EGL_DONT_CARE )
		priority[i] += caveatWeight * 0;

		if( configAtt[i]->eglColorBufferType == EGL_RGB_BUFFER )
		priority[i] += colorBufferTypeWeight * 2;
		else if( configAtt[i]->eglColorBufferType == EGL_LUMINANCE_BUFFER )
		priority[i] += colorBufferTypeWeight * 1;
		else if( configAtt[i]->eglColorBufferType == (EGLenum)EGL_DONT_CARE )
		priority[i] += colorBufferTypeWeight * 0;

		priority[i] += colorBufferSizeWeight * (colorBufferSizeRange - configAtt[i]->eglBufferSize);
		priority[i] += sampleBufferWeight    * (sampleBufferRange    - configAtt[i]->eglSampleBuffers);
		priority[i] += samplesWeight         * (samplesRange         - configAtt[i]->eglSamples);
		priority[i] += depthSizeWeight       * (depthSizeRange       - configAtt[i]->eglDepthSize);
		priority[i] += stencilSizeWeight     * (stencilSizeRange     - configAtt[i]->eglStencilSize);
		priority[i] += alphaMaskWeight       * (alphaMaskSizeRange   - configAtt[i]->eglAlphaMaskSize);
	}

	for( i = 0; i < num_config; i++ )
	{
		EGLConfigAtt* tmpConfigAtt;

		tmpPriority = priority[i];
		for( j = i; j < num_config; j++ )
		{
			if( tmpPriority < priority[j] )
			tmpPriority = priority[j];
		}

		for( j = i; j < num_config; j++ )
		{
			if( tmpPriority == priority[j] )
			break;
		}

		if( i != j )
		{
			priority[j] = priority[i];
			priority[i] = tmpPriority;

			tmpConfigAtt = configAtt[i];
			configAtt[i] = configAtt[j];
			configAtt[j] = tmpConfigAtt;
		}
	}

	for( i = 0; i < num_config; i++ )
	(*configs)[i] = configAtt[i];

	PLATFORM_FREE( configAtt );
	PLATFORM_FREE( priority );
}

/*------------------------------------------------------------------------
* EGL Functions.
*----------------------------------------------------------------------*/
EGLint eglGetError()
{
	EGLThreadState *thread;

	acquireMutex();

	if (!_egl.init)
	{
		releaseMutex();
		return EGL_SUCCESS;
	}

	thread = getCurrentThread();
	if(!thread)
	{
		releaseMutex();
		return EGL_SUCCESS;
	}

	releaseMutex();
	return thread->error;
}

static void InitializeEGL()
{

	EGLint i, j;
	EGLint (*eglConfigList)[33];
	EGLConfigAtt   *defaultConfig;

	_egl.num_display = 0;

	_egl.currentThreadID = 0;
	_egl.num_thread = 0;

	_egl.num_config = eglGetConfigNum();
	eglConfigList = eglGetConfigList();
	_egl.configs = (EGLConfig *)malloc( sizeof(EGLConfig) * _egl.num_config );

	_egl.defaultConfig = (EGLConfigAtt *)malloc( sizeof(EGLConfigAtt) * _egl.num_config );
	memset( _egl.defaultConfig, 0x00, sizeof(EGLConfigAtt) * _egl.num_config );

	defaultConfig = _egl.defaultConfig;

	for( i = 0; i < _egl.num_config; i++ )
	{
		defaultConfig[i].eglConfigID              = eglConfigList[i][0];
		defaultConfig[i].eglBufferSize            = eglConfigList[i][1];
		defaultConfig[i].eglRedSize               = eglConfigList[i][2];
		defaultConfig[i].eglGreenSize             = eglConfigList[i][3];
		defaultConfig[i].eglBlueSize              = eglConfigList[i][4];
		defaultConfig[i].eglAlphaSize             = eglConfigList[i][5];
		defaultConfig[i].eglLuminanceSize         = eglConfigList[i][6];
		defaultConfig[i].eglColorBufferType       = eglConfigList[i][7];
		defaultConfig[i].eglConfigCaveat          = eglConfigList[i][8];
		defaultConfig[i].eglMaxPbufferWidth       = eglConfigList[i][9];
		defaultConfig[i].eglMaxPbufferHeight      = eglConfigList[i][10];
		defaultConfig[i].eglMaxPbufferPixels      = eglConfigList[i][11];
		defaultConfig[i].eglMaxSwapInterval       = eglConfigList[i][12];
		defaultConfig[i].eglMinSwapInterval       = eglConfigList[i][13];
		defaultConfig[i].eglNativeRenderable      = eglConfigList[i][14];
		defaultConfig[i].eglNativeVisualID        = eglConfigList[i][15];
		defaultConfig[i].eglNativeVisualType      = eglConfigList[i][16];
		defaultConfig[i].eglRenderableType        = eglConfigList[i][17];
		defaultConfig[i].eglSurfaceType           = eglConfigList[i][18];
		defaultConfig[i].eglTransparentType       = eglConfigList[i][19];
		defaultConfig[i].eglTransparentRedValue   = eglConfigList[i][20];
		defaultConfig[i].eglTransparentGreenValue = eglConfigList[i][21];
		defaultConfig[i].eglTransparentBlueValue  = eglConfigList[i][22];
		defaultConfig[i].eglConformant            = eglConfigList[i][23];
		defaultConfig[i].eglMatchedPixmap         = eglConfigList[i][24];
		defaultConfig[i].eglAlphaMaskSize         = eglConfigList[i][25];
		defaultConfig[i].eglLevel                 = eglConfigList[i][26];
		defaultConfig[i].eglBindToTextureRGB      = eglConfigList[i][27];
		defaultConfig[i].eglBindToTextureRGBA     = eglConfigList[i][28];
		defaultConfig[i].eglDepthSize             = eglConfigList[i][29];
		defaultConfig[i].eglSampleBuffers         = eglConfigList[i][30];
		defaultConfig[i].eglSamples               = eglConfigList[i][31];
		defaultConfig[i].eglStencilSize           = eglConfigList[i][32];

		for( j = 0; j < EGL_MAX_ASSOCIATED_WINDOW; j++ )
		defaultConfig[i].associatedWindow[j] = ((EGLNativeWindowType)0);

		for( j = 0; j < EGL_MAX_ASSOCIATED_PIXMAP; j++ )
		defaultConfig[i].associatedPixmap[j] = ((EGLNativePixmapType)0);

		_egl.configs[i] = (EGLConfig)&defaultConfig[i];
	}

	_egl.major = 1;
	_egl.minor = 3;

	_egl.displays[0] = (EGLDisplay)1; //reseved for EGL_DEFAULT_DISPLAY
	_egl.num_display = 1;

	_egl.init = EGL_TRUE;

}

EGLDisplay eglGetDisplay(EGLNativeDisplayType displayID)
{
	acquireMutex();

	if (displayID == EGL_DEFAULT_DISPLAY)
	{
		EGL_ERROR( EGL_SUCCESS, (void*)1 );
	}

	EGL_ERROR( EGL_SUCCESS, (EGLDisplay)displayID );
}

EGLBoolean eglInitialize(EGLDisplay dpy, EGLint* major, EGLint* minor)
{
	//LOGE("############################# eglInitialize ##########################");
	EGLint          i;
	EGLint          num_display;
	EGLint          num_thread;

	acquireMutex();

	if ( !_egl.init )
	{
		InitializeEGL();
		//EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );
	}

	num_display = _egl.num_display;

	for (i = 0; i < num_display; i++)
	{
		if (_egl.displays[i] == (EGLDisplay)dpy)
		break;
	}

	if( i == num_display )
	{
		_egl.num_display = num_display + 1;

		if ( _egl.num_display >= EGL_MAX_DISPLAY )
		EGL_ERROR( EGL_BAD_ALLOC, EGL_FALSE );

		_egl.displays[i] = (EGLDisplay)dpy;
	}

	if (major)
	*major = _egl.major;

	if (minor)
	*minor = _egl.minor;

	num_thread = _egl.num_thread;
	if ( num_thread >= EGL_MAX_THREAD )
	EGL_ERROR( EGL_BAD_ALLOC, EGL_FALSE );

	PLATFORM_INITIALIZE( dpy );

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglTerminate(EGLDisplay dpy)
{
	EGLint          i, j;
	EGLint          num_display;
	EGLint          num_thread;
	EGLThreadState *thread;
	iEGLContext    *context;
	iEGLSurface    *read, *draw;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_display = _egl.num_display;
	for (i = 0; i < num_display; i++)
	if (_egl.displays[i] == dpy)
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_thread = _egl.num_thread;
	for (i = 0; i < num_thread; i++)
	{
		thread = &_egl.threads[i];
		if (thread->display == dpy)
		{

			context = thread->context;

			if ( context )
			{
				context->referenceCount--;

				if ( context->referenceCount > 0 )
				context->deleteMark = EGL_TRUE;
				else
				eglDestroyContext( dpy, (EGLContext)context );
			}

			read = thread->read;
			draw = thread->draw;

			if ( read )
			{
				read->referenceCount--;

				if ( read->referenceCount > 0 )
				read->deleteMark = EGL_TRUE;
				else
				eglDestroySurface( dpy, (EGLSurface)read );
			}

			if ( draw && draw != read )
			{
				draw->referenceCount--;

				if ( draw->referenceCount > 0 )
				draw->deleteMark = EGL_TRUE;
				else
				eglDestroySurface( dpy, (EGLSurface)draw );
			}


			_egl.threads[i].threadID = 0;
			_egl.num_thread--;
		}
	}

	if ( _egl.num_thread )
	{
		for ( i = 0; i < EGL_MAX_THREAD; i++ )
		{
			thread = &_egl.threads[i];

			if ( !_egl.threads[i].threadID )
			{
				for ( j = i+1; j < EGL_MAX_THREAD; j++ )
				{
					if ( _egl.threads[j].threadID )
					{
						_egl.threads[i] = _egl.threads[j];
						_egl.threads[i].threadID = i + 1;
						_egl.threads[j].threadID = 0;
						break;
					}
				}
			}
		}

		thread = &_egl.threads[0];

	}
	else
	{
		_egl.init = EGL_FALSE;

		PLATFORM_FREE( _egl.configs );
		PLATFORM_FREE( _egl.defaultConfig );
	}

	PLATFORM_FINALIZE( dpy );

	// for driver
	eglGLESDeinitDriver();

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

const char* eglQueryString(EGLDisplay dpy, EGLint name)
{
	EGLint          i;
	EGLint          num_display;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, NULL );


	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, NULL );

	switch ( name )
	{
	case EGL_CLIENT_APIS:
		EGL_ERROR( EGL_SUCCESS, "OpenVG OpenGL_ES" );

	case EGL_VENDOR:
		EGL_ERROR( EGL_SUCCESS, "Samsung Electronics Co" );

	case EGL_VERSION:
		EGL_ERROR( EGL_SUCCESS, "1.3" );

	case EGL_EXTENSIONS:
		EGL_ERROR( EGL_SUCCESS, "" );

	default:
		EGL_ERROR( EGL_BAD_PARAMETER, NULL );
	}
}

EGLBoolean eglGetConfigs(EGLDisplay dpy, EGLConfig* configs, EGLint config_size, EGLint* num_config)
{
	EGLint          i, j = 0;
	EGLint          num_display;

	acquireMutex();


	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_display = _egl.num_display;
	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_FALSE );

	if ( !configs )
	{
		if ( !num_config )
		EGL_ERROR( EGL_BAD_PARAMETER, EGL_FALSE );

		*num_config = _egl.num_config;
		EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
	}

	if ( !num_config )
	EGL_ERROR( EGL_BAD_PARAMETER, EGL_FALSE );

	*num_config = EGL_MIN( config_size, _egl.num_config );
	for ( i = 0; i < *num_config; i++ )
	{
		configs[j] = _egl.configs[i];
		j++;
	}

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config)
{
	EGLBoolean   flag;
	EGLint     i, j, k;
	EGLint     num_display;
	EGLConfigAtt  *configAtt=0;
	EGLint         boundAPI;
	EGLint       start = 0, end = 0;
	EGLThreadState *thread;
	EGLConfig   *tmpConfigs;

	acquireMutex();

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	boundAPI = thread->boundAPI;

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_display = _egl.num_display;
	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_FALSE );

	if ( !num_config )
	EGL_ERROR( EGL_BAD_PARAMETER, EGL_FALSE );

	*num_config = _egl.num_config;

	tmpConfigs = (EGLConfig*)malloc( sizeof(EGLConfig) * eglGetConfigNum() );

	if ( !attrib_list )
	{
		j = 0;
		for ( i = 0; i < *num_config; i++ )
		{
			configAtt = (EGLConfigAtt*)_egl.configs[i];
			if ( configAtt->eglBufferSize < 0 )
			break;
			if ( configAtt->eglRedSize < 0 )
			break;
			if ( configAtt->eglGreenSize < 0 )
			break;
			if ( configAtt->eglBlueSize < 0 )
			break;
			if ( configAtt->eglLuminanceSize < 0 )
			break;
			if ( configAtt->eglAlphaSize < 0 )
			break;
			if ( configAtt->eglColorBufferType != EGL_RGB_BUFFER )
			break;

			if ( tmpConfigs )
			tmpConfigs[j] = (EGLConfig)configAtt;

			j++;
		}

		sortingConfigs( &tmpConfigs, j );

		if (configs)
		{

			*num_config = EGL_MIN( config_size, j );

			for( i = 0; i < *num_config; i++ )
			configs[i] = tmpConfigs[i];

		}
		else
		{
			*num_config = j;
		}


		/*if ( *num_config == 0 && configs )
		*configs = 0;*/
	}
	else
	{
		EGLint configID = 0;
		EGLBoolean isSurface = EGL_FALSE, isNotTransparent = EGL_FALSE, isMatchedPixmap = EGL_FALSE;

		for ( i = 0; attrib_list[i] != EGL_NONE; i += 2 )
		{
			switch( attrib_list[i] )
			{
			case EGL_CONFIG_ID:
				configID = attrib_list[i + 1];
				break;
			case EGL_TRANSPARENT_TYPE:
				if( attrib_list[i + 1] == EGL_NONE )
				isNotTransparent = EGL_TRUE;
				break;
			case EGL_SURFACE_TYPE:
				if( (attrib_list[i + 1] & EGL_WINDOW_BIT) != EGL_WINDOW_BIT )
				isSurface = EGL_TRUE;
				break;
			}
		}

		if( configID )
		{
			for ( i = 0; i < *num_config; i++ )
			{
				configAtt = (EGLConfigAtt*)_egl.configs[i];

				if ( configAtt->eglConfigID == configID )
				break;
			}

			if( i == *num_config )
			{
				*num_config = 0;
				//configs[0] = (EGLConfig)EGL_NULL;
			}
			else
			{
				*num_config = 1;
				configs[0] = configAtt;
			}

			PLATFORM_FREE(tmpConfigs);
			EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
		}


		j = 0;
		for ( i = 0; i < *num_config; i++ )
		{
			if( isMatchedPixmap )
			break;

			flag = EGL_TRUE;
			configAtt = (EGLConfigAtt*)_egl.configs[i];

			for ( k = 0; attrib_list[k] != EGL_NONE; k += 2 )
			{
				if( attrib_list[k] == EGL_MATCH_NATIVE_PIXMAP )
				{
					isMatchedPixmap = EGL_TRUE;
					configAtt->eglMatchedPixmap = attrib_list[k + 1];

					break;
				}

				if ( attrib_list[k + 1] == EGL_DONT_CARE )
				continue;

				switch( attrib_list[k] )
				{
				case EGL_BUFFER_SIZE:
					if ( configAtt->eglBufferSize < attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_RED_SIZE:
					if ( configAtt->eglRedSize < attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_GREEN_SIZE:
					if ( configAtt->eglGreenSize < attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_BLUE_SIZE:
					if ( configAtt->eglBlueSize < attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_LUMINANCE_SIZE:
					if ( configAtt->eglLuminanceSize < attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_ALPHA_SIZE:
					if ( configAtt->eglAlphaSize < attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_ALPHA_MASK_SIZE:
					if ( configAtt->eglAlphaMaskSize < attrib_list[k + 1] && boundAPI == EGL_OPENVG_API )
					flag = EGL_FALSE;
					break;
				case EGL_DEPTH_SIZE:
					if ( configAtt->eglDepthSize < attrib_list[k + 1] && boundAPI == EGL_OPENGL_ES_API )
					flag = EGL_FALSE;
					break;
				case EGL_SAMPLE_BUFFERS:
					if ( configAtt->eglSampleBuffers < attrib_list[k + 1] && boundAPI == EGL_OPENGL_ES_API )
					flag = EGL_FALSE;
					break;
				case EGL_SAMPLES:
					if ( configAtt->eglSamples < attrib_list[k + 1] && boundAPI == EGL_OPENGL_ES_API )
					flag = EGL_FALSE;
					break;
				case EGL_STENCIL_SIZE:
					if ( configAtt->eglStencilSize < attrib_list[k + 1] && boundAPI == EGL_OPENGL_ES_API )
					flag = EGL_FALSE;
					break;
				case EGL_LEVEL:
					if ( configAtt->eglLevel != attrib_list[k + 1] && boundAPI == EGL_OPENGL_ES_API )
					flag = EGL_FALSE;
					break;
				case EGL_BIND_TO_TEXTURE_RGB:
					if ( configAtt->eglBindToTextureRGB != (EGLBoolean)attrib_list[k + 1] && boundAPI == EGL_OPENGL_ES_API )
					flag = EGL_FALSE;
					break;
				case EGL_BIND_TO_TEXTURE_RGBA:
					if ( configAtt->eglBindToTextureRGBA != (EGLBoolean)attrib_list[k + 1] && boundAPI == EGL_OPENGL_ES_API )
					flag = EGL_FALSE;
					break;
				case EGL_COLOR_BUFFER_TYPE:
					if ( configAtt->eglColorBufferType != (EGLenum)attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_CONFIG_CAVEAT:
					if ( configAtt->eglConfigCaveat != (EGLenum)attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_CONFIG_ID:
					if ( configAtt->eglConfigID != attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_NATIVE_RENDERABLE:
					if ( configAtt->eglNativeRenderable != (EGLBoolean)attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_NATIVE_VISUAL_TYPE:
					if ( (!isSurface && configAtt->eglNativeVisualType) && configAtt->eglNativeVisualType != attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_RENDERABLE_TYPE:
					if ( !(configAtt->eglRenderableType & attrib_list[k + 1]) )
					flag = EGL_FALSE;
					break;
				case EGL_SURFACE_TYPE:
					if ( !(configAtt->eglSurfaceType & attrib_list[k + 1]) )
					flag = EGL_FALSE;
					break;
				case EGL_TRANSPARENT_TYPE:
					if ( configAtt->eglTransparentType != (EGLenum)attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_TRANSPARENT_RED_VALUE:
					if ( !isNotTransparent && configAtt->eglTransparentRedValue != attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_TRANSPARENT_GREEN_VALUE:
					if ( !isNotTransparent && configAtt->eglTransparentGreenValue != attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_TRANSPARENT_BLUE_VALUE:
					if ( !isNotTransparent && configAtt->eglTransparentBlueValue != attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_MAX_SWAP_INTERVAL:
					if ( configAtt->eglMaxSwapInterval != attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_MIN_SWAP_INTERVAL:
					if ( configAtt->eglMinSwapInterval != attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;

				case EGL_CONFORMANT:
					if ( configAtt->eglConformant != attrib_list[k + 1] )
					flag = EGL_FALSE;
					break;
				case EGL_NATIVE_VISUAL_ID:
				case EGL_MAX_PBUFFER_WIDTH:
				case EGL_MAX_PBUFFER_HEIGHT:
				case EGL_MAX_PBUFFER_PIXELS:
					break;
				default:
					PLATFORM_FREE(tmpConfigs);
					EGL_ERROR( EGL_BAD_ATTRIBUTE, EGL_FALSE );
				}
			}

			if ( flag && tmpConfigs )
			{
				tmpConfigs[j] = (EGLConfig)configAtt;
				j++;
			}
		}

		sortingConfigs( &tmpConfigs, j );

		if (configs)
		{

			*num_config = EGL_MIN( config_size, j );

			for( i = 0; i < *num_config; i++ )
			configs[i] = tmpConfigs[i];

		}
		else
		{
			*num_config = j;
		}

		/*if ( *num_config == 0 && configs )
		*configs = 0;*/
	}

	PLATFORM_FREE(tmpConfigs);
	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint* value)
{
	EGLint         i;
	EGLint         num_display;
	EGLConfigAtt  *configAtt;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_FALSE );

	if( !config )
	EGL_ERROR( EGL_BAD_PARAMETER, EGL_FALSE );
	configAtt = (EGLConfigAtt*)config;

	switch( attribute )
	{
	case EGL_BUFFER_SIZE:
		*value = configAtt->eglBufferSize;
		break;
	case EGL_RED_SIZE:
		*value = configAtt->eglRedSize;
		break;
	case EGL_GREEN_SIZE:
		*value = configAtt->eglGreenSize;
		break;
	case EGL_BLUE_SIZE:
		*value = configAtt->eglBlueSize;
		break;
	case EGL_LUMINANCE_SIZE:
		*value = configAtt->eglLuminanceSize;
		break;
	case EGL_ALPHA_SIZE:
		*value = configAtt->eglAlphaSize;
		break;
	case EGL_COLOR_BUFFER_TYPE:
		*value = configAtt->eglColorBufferType;
		break;
	case EGL_CONFIG_CAVEAT:
		*value = configAtt->eglConfigCaveat;
		break;
	case EGL_CONFIG_ID:
		*value = configAtt->eglConfigID;
		break;
	case EGL_NATIVE_RENDERABLE:
		*value = configAtt->eglNativeRenderable;
		break;
	case EGL_RENDERABLE_TYPE:
		*value = configAtt->eglRenderableType;
		break;
	case EGL_SURFACE_TYPE:
		*value = configAtt->eglSurfaceType;
		break;
	case EGL_TRANSPARENT_TYPE:
		*value = configAtt->eglTransparentType;
		break;
	case EGL_CONFORMANT:
		*value = configAtt->eglConformant;
		break;
	case EGL_ALPHA_MASK_SIZE:
		*value = configAtt->eglAlphaMaskSize;
		break;
	case EGL_BIND_TO_TEXTURE_RGB:
		*value = configAtt->eglBindToTextureRGB;
		break;
	case EGL_BIND_TO_TEXTURE_RGBA:
		*value = configAtt->eglBindToTextureRGBA;
		break;
	case EGL_DEPTH_SIZE:
		*value = configAtt->eglDepthSize;
		break;
	case EGL_LEVEL:
		*value = configAtt->eglLevel;
		break;
	case EGL_MAX_PBUFFER_WIDTH:
		*value = configAtt->eglMaxPbufferWidth;
		break;
	case EGL_MAX_PBUFFER_HEIGHT:
		*value = configAtt->eglMaxPbufferHeight;
		break;
	case EGL_MAX_PBUFFER_PIXELS:
		*value = configAtt->eglMaxPbufferPixels;
		break;
	case EGL_MAX_SWAP_INTERVAL:
		*value = configAtt->eglMaxSwapInterval;
		break;
	case EGL_MIN_SWAP_INTERVAL:
		*value = configAtt->eglMinSwapInterval;
		break;
	case EGL_NATIVE_VISUAL_ID:
		*value = configAtt->eglNativeVisualID;
		break;
	case EGL_NATIVE_VISUAL_TYPE:
		*value = configAtt->eglNativeVisualType;
		break;
	case EGL_SAMPLE_BUFFERS:
		*value = configAtt->eglSampleBuffers;
		break;
	case EGL_SAMPLES:
		*value = configAtt->eglSamples;
		break;
	case EGL_STENCIL_SIZE:
		*value = configAtt->eglStencilSize;
		break;
	case EGL_TRANSPARENT_RED_VALUE:
		*value = configAtt->eglTransparentRedValue;
		break;
	case EGL_TRANSPARENT_GREEN_VALUE:
		*value = configAtt->eglTransparentGreenValue;
		break;
	case EGL_TRANSPARENT_BLUE_VALUE:
		*value = configAtt->eglTransparentBlueValue;
		break;
	default:
		EGL_ERROR( EGL_BAD_ATTRIBUTE, EGL_FALSE );
	}

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, NativeWindowType win, const EGLint* attrib_list)
{
	EGLint        i;
	//    EGLint        width = 0, height = 0;
	EGLint        num_display;
	EGLConfigAtt *configAtt = (EGLConfigAtt *)config;
	iEGLSurface  *surface;
	EGLint        renderBuffer = EGL_BACK_BUFFER;
	EGLint        colorSpace = EGL_VG_COLORSPACE_sRGB;
	EGLint        alphaFormat = EGL_VG_ALPHA_FORMAT_NONPRE;
	EGLint        size = 0;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NO_SURFACE );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_NO_SURFACE );

	if ( !config )
	EGL_ERROR( EGL_BAD_CONFIG, EGL_NO_SURFACE );

	if ( !(configAtt->eglSurfaceType & EGL_WINDOW_BIT) )
	EGL_ERROR( EGL_BAD_MATCH, EGL_NO_SURFACE );

	//if( !win )
	if( !PLATFORM_IS_WINDOW_VALID( dpy, win ) )
	EGL_ERROR( EGL_BAD_NATIVE_WINDOW, EGL_NO_SURFACE );

	for( i = 0; configAtt->associatedWindow[i]; i++ )
	{
		if( configAtt->associatedWindow[i] == win )
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}

	if ( attrib_list )
	{
		for ( i = 0; attrib_list[i] != EGL_NONE; i+=2 )
		{
			switch( attrib_list[i] )
			{
			case EGL_RENDER_BUFFER:
				renderBuffer = attrib_list[i+1];
				break;

			case EGL_VG_COLORSPACE:
				if( (attrib_list[i+1] == EGL_VG_COLORSPACE_LINEAR) && !(configAtt->eglSurfaceType & EGL_VG_COLORSPACE_LINEAR_BIT) )
				EGL_ERROR( EGL_BAD_MATCH, EGL_NO_SURFACE );
				colorSpace = attrib_list[i+1];
				break;
			case EGL_VG_ALPHA_FORMAT:
				if( (attrib_list[i+1] == EGL_VG_ALPHA_FORMAT_PRE) && !(configAtt->eglSurfaceType & EGL_VG_ALPHA_FORMAT_PRE_BIT) )
				EGL_ERROR( EGL_BAD_MATCH, EGL_NO_SURFACE );
				alphaFormat = attrib_list[i+1];
				break;
			}
		}
	}

	if ( !(surface = (iEGLSurface *)malloc( sizeof(iEGLSurface) )) )
	EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );


	memset( surface, 0x00, sizeof(iEGLSurface) );

	if( !(surface->backBuffer1 = (AddressBase *)malloc( sizeof(AddressBase) )) )
	{
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}

#if defined(DOUBLE_BUFFER)
	if( !(surface->backBuffer2 = (AddressBase *)malloc( sizeof(AddressBase) )) )
	{
		PLATFORM_FREE( surface->backBuffer1 );
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}
#elif defined(TRIPLE_BUFFER)
	if( !(surface->backBuffer2 = (AddressBase *)malloc( sizeof(AddressBase) )) )
	{
		PLATFORM_FREE( surface->backBuffer1 );
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}

	if( !(surface->backBuffer3 = (AddressBase *)malloc( sizeof(AddressBase) )) )
	{
		PLATFORM_FREE( surface->backBuffer1 );
		PLATFORM_FREE( surface->backBuffer2 );
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}
#endif

	if( !(surface->depthBuffer = (AddressBase *)malloc( sizeof(AddressBase) )) )
	{
		PLATFORM_FREE( surface->backBuffer1 );
#if defined(DOUBLE_BUFFER)
		PLATFORM_FREE( surface->backBuffer2 );
#elif defined(TRIPLE_BUFFER)
		PLATFORM_FREE( surface->backBuffer2 );
		PLATFORM_FREE( surface->backBuffer3 );
#endif
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}

	memset( surface->backBuffer1, 0x00, sizeof(AddressBase) );
#if defined(DOUBLE_BUFFER)

	memset( surface->backBuffer2, 0x00, sizeof(AddressBase) );

#elif defined(TRIPLE_BUFFER)
	memset( surface->backBuffer2, 0x00, sizeof(AddressBase) );
	memset( surface->backBuffer3, 0x00, sizeof(AddressBase) );
#endif
	memset( surface->depthBuffer, 0x00, sizeof(AddressBase) );

	PLATFORM_WINDOW_SURFACE_INITIALIZE( dpy, win, &surface->userData );

	PLATFORM_GET_WINDOW_SIZE(dpy, win, &surface->width, &surface->height, surface->userData);

	surface->renderBuffer = renderBuffer;

	surface->referenceCount = 0;
	surface->deleteMark = EGL_FALSE;
	surface->config = config;
	surface->window = win;
	surface->surfaceType = EGL_WINDOW_BIT;
	surface->isClientBuffer = EGL_FALSE;

	surface->display = dpy;

	surface->colorSpace = ( colorSpace == EGL_VG_COLORSPACE_sRGB ? EGL_TRUE : EGL_FALSE );
	surface->alphaFormat = ( alphaFormat == EGL_VG_ALPHA_FORMAT_NONPRE ? EGL_TRUE : EGL_FALSE );

	surface->redSize     = configAtt->eglRedSize;
	surface->greenSize   = configAtt->eglGreenSize;
	surface->blueSize    = configAtt->eglBlueSize;
	surface->alphaSize   = configAtt->eglAlphaSize;
	surface->depthSize   = configAtt->eglDepthSize;
	surface->stencilSize = configAtt->eglStencilSize;
	surface->sampleSize  = configAtt->eglSamples;


	if( configAtt->eglBufferSize > 16 && configAtt->eglBufferSize <= 32 )
	{
		surface->bufferBytes = 4;
		size = surface->width * surface->height * surface->bufferBytes;
	}
	else if( configAtt->eglBufferSize > 8 && configAtt->eglBufferSize <= 16 )
	{
		surface->bufferBytes = 2;
		size = surface->width * surface->height * surface->bufferBytes;
	}
	else EGL_ERROR( EGL_BAD_CONFIG, EGL_NO_SURFACE );


	// Alex: 2009.10.22
	/*if(win->memory_type == 2)
{
	PLATFORM_GET_DIPLAY_ADDR(surface->backBuffer1,surface->backBuffer2,win->base);
	surface->direct_fb = 1;
}
else if(win->memory_type == 1)
{
	get_addressbase(&Surface_addr);

	if(!Surface_addr.paddr || win->width != win->stride)
	{
#if !defined(DIRECT_WINDOW_DRAWING)
#if defined(DOUBLE_BUFFER)
	surface->backBuffer1Handle = eglGLESGetBuffer( size );
	surface->backBuffer2Handle = eglGLESGetBuffer( size );

		eglGLESGetBufferAddress( surface->backBuffer1Handle, &surface->backBuffer1->paddr, &surface->backBuffer1->vaddr );
		eglGLESGetBufferAddress( surface->backBuffer2Handle, &surface->backBuffer2->paddr, &surface->backBuffer2->vaddr );
#elif defined(TRIPLE_BUFFER)
		surface->backBuffer1Handle = eglGLESGetBuffer( size );
		surface->backBuffer2Handle = eglGLESGetBuffer( size );
		surface->backBuffer3Handle = eglGLESGetBuffer( size );

		eglGLESGetBufferAddress( surface->backBuffer1Handle, &surface->backBuffer1->paddr, &surface->backBuffer1->vaddr );
		eglGLESGetBufferAddress( surface->backBuffer2Handle, &surface->backBuffer2->paddr, &surface->backBuffer2->vaddr );
		eglGLESGetBufferAddress( surface->backBuffer3Handle, &surface->backBuffer3->paddr, &surface->backBuffer3->vaddr );
#else
	surface->backBuffer1Handle = eglGLESGetBuffer( size );
	eglGLESGetBufferAddress( surface->backBuffer1Handle, &surface->backBuffer1->paddr, &surface->backBuffer1->vaddr );
#endif
#endif
	surface->direct_fb = 0;
	}
	else
	{
#if defined(DOUBLE_BUFFER)
	surface->backBuffer1->paddr = Surface_addr.paddr + win->offset;
	surface->backBuffer1->vaddr = Surface_addr.vaddr + win->offset;
//      LOGE("####################backbuffer2 vaddr = 0x%x, paddr = 0x%x ###########", surface->backBuffer2->vaddr, surface->backBuffer2->paddr);
//      win->nextBuffer(win);
	surface->backBuffer2->paddr = Surface_addr.paddr + win->offset;
	surface->backBuffer2->vaddr = Surface_addr.vaddr + win->offset;
//      LOGE("####################backbuffer1 vaddr = 0x%x, paddr = 0x%x ###########", surface->backBuffer1->vaddr, surface->backBuffer1->paddr);
#else
	surface->backBuffer1->paddr = Surface_addr.paddr + win->offset;
	surface->backBuffer1->vaddr = Surface_addr.vaddr + win->offset;
#endif
	surface->direct_fb = 1;
	}
}
else*/
	{
#if !defined(DIRECT_WINDOW_DRAWING)
#if defined(DOUBLE_BUFFER)
		surface->backBuffer1Handle = eglGLESGetBuffer( size );
		surface->backBuffer2Handle = eglGLESGetBuffer( size );

		eglGLESGetBufferAddress( surface->backBuffer1Handle, &surface->backBuffer1->paddr, &surface->backBuffer1->vaddr );
		eglGLESGetBufferAddress( surface->backBuffer2Handle, &surface->backBuffer2->paddr, &surface->backBuffer2->vaddr );
#elif defined(TRIPLE_BUFFER)
		surface->backBuffer1Handle = eglGLESGetBuffer( size );
		surface->backBuffer2Handle = eglGLESGetBuffer( size );
		surface->backBuffer3Handle = eglGLESGetBuffer( size );

		eglGLESGetBufferAddress( surface->backBuffer1Handle, &surface->backBuffer1->paddr, &surface->backBuffer1->vaddr );
		eglGLESGetBufferAddress( surface->backBuffer2Handle, &surface->backBuffer2->paddr, &surface->backBuffer2->vaddr );
		eglGLESGetBufferAddress( surface->backBuffer3Handle, &surface->backBuffer3->paddr, &surface->backBuffer3->vaddr );
#else
		surface->backBuffer1Handle = eglGLESGetBuffer( size );
		eglGLESGetBufferAddress( surface->backBuffer1Handle, &surface->backBuffer1->paddr, &surface->backBuffer1->vaddr );
#endif
#endif
		//surface->direct_fb = 1;
	}

	if( surface->depthSize == 24 )
	{
		size = surface->width * surface->height * sizeof(unsigned int);

		surface->depthBufferHandle = eglGLESGetBuffer( size );
		eglGLESGetBufferAddress( surface->depthBufferHandle, &surface->depthBuffer->paddr, &surface->depthBuffer->vaddr );
	}

	surface->stencilBuffer = surface->depthBuffer;

	//  if(win->offset) {
	surface->curDrawingBuffer = 2;
	surface->frontBuffer = surface->backBuffer2;
	//  }
	//  else {
	//      surface->curDrawingBuffer = 1;
	//      surface->frontBuffer = surface->backBuffer1;
	//  }

	for( i = 0; configAtt->associatedWindow[i]; i++ );
	configAtt->associatedWindow[i] = win;

	if( surface->redSize == 8 && surface->greenSize == 8 && surface->blueSize == 8 && surface->alphaSize == 8 )
	surface->colorFormat = E_ARGB8;
	else if( surface->redSize == 8 && surface->greenSize == 8 && surface->blueSize == 8 && surface->alphaFormat == 0 )
	surface->colorFormat = E_ARGB0888;
	else if( surface->redSize == 5 && surface->greenSize == 6 && surface->blueSize == 5 )
	surface->colorFormat = E_RGB565;
	else if( surface->redSize == 5 && surface->greenSize == 5 && surface->blueSize == 5 && surface->alphaSize == 1 )
	surface->colorFormat = E_RGBA5551;
	else if( surface->redSize == 4 && surface->greenSize == 4 && surface->blueSize == 4 && surface->alphaSize == 4 )
	surface->colorFormat = E_ARGB4;

	if( surface->depthSize == 24 )
	{
		if( surface->stencilSize == 8 )
		surface->depthStencilFormat = E_Stencil8Depth24;
		else if( surface->stencilSize == 0 )
		surface->depthStencilFormat = E_Depth24;
	}
	else if( surface->depthSize == 0 )
	{
		if( surface->stencilSize == 8 )
		surface->depthStencilFormat = E_Stencil8;
		else if( surface->stencilSize == 0 )
		surface->depthStencilFormat = 0;
	}

	if( configAtt->eglSurfaceType & EGL_LOCK_SURFACE_BIT_KHR )
	surface->surfaceType |= EGL_LOCK_SURFACE_BIT_KHR;

	EGL_ERROR( EGL_SUCCESS, surface );
}

EGLSurface eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint* attrib_list)
{
	EGLint        i;
	EGLint        width = 0, height = 0;
	EGLint        num_display;
	EGLConfigAtt *configAtt = (EGLConfigAtt *)config;
	iEGLSurface  *surface;
	EGLint        colorSpace = EGL_VG_COLORSPACE_sRGB;
	EGLint        alphaFormat = EGL_VG_ALPHA_FORMAT_NONPRE;
	EGLBoolean    largestPbuffer = EGL_FALSE;
	EGLint        textureFormat = EGL_NO_TEXTURE;
	EGLint        textureTarget = EGL_NO_TEXTURE;
	EGLBoolean    mipmapTexture = EGL_FALSE;
	EGLint        size = 0;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NO_SURFACE );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_NO_SURFACE );

	if ( !config )
	EGL_ERROR( EGL_BAD_CONFIG, EGL_NO_SURFACE );


	if ( !(configAtt->eglSurfaceType & EGL_PIXMAP_BIT) )
	EGL_ERROR( EGL_BAD_MATCH, EGL_NO_SURFACE );

	if ( attrib_list )
	for ( i = 0; attrib_list[i] != EGL_NONE; i+=2 )
	{
		switch( attrib_list[i] )
		{
		case EGL_WIDTH:
			width = attrib_list[i+1];
			break;
		case EGL_HEIGHT:
			height = attrib_list[i+1];
			break;
		case EGL_VG_COLORSPACE:
			if( (attrib_list[i+1] == EGL_VG_COLORSPACE_LINEAR) && !(configAtt->eglSurfaceType & EGL_VG_COLORSPACE_LINEAR_BIT) )
			EGL_ERROR( EGL_BAD_MATCH, EGL_NO_SURFACE );

			colorSpace = attrib_list[i+1];
			break;
		case EGL_VG_ALPHA_FORMAT:
			if( (attrib_list[i+1] == EGL_VG_ALPHA_FORMAT_PRE) && !(configAtt->eglSurfaceType & EGL_VG_ALPHA_FORMAT_PRE_BIT) )
			EGL_ERROR( EGL_BAD_MATCH, EGL_NO_SURFACE );

			alphaFormat = attrib_list[i+1];
			break;
		case EGL_LARGEST_PBUFFER:
			largestPbuffer = attrib_list[i+1];
			break;
		case EGL_TEXTURE_FORMAT:
			if( !(configAtt->eglRenderableType & (EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT)) )
			EGL_ERROR( EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE );

			textureFormat = attrib_list[i+1];
			break;
		case EGL_TEXTURE_TARGET:
			if( !(configAtt->eglRenderableType & (EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT)) )
			EGL_ERROR( EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE );

			textureTarget = attrib_list[i+1];
			break;
		case EGL_MIPMAP_TEXTURE:
			if( !(configAtt->eglRenderableType & (EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT)) )
			EGL_ERROR( EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE );

			mipmapTexture = attrib_list[i+1];
			break;
		}
	}

	if ( !(surface = (iEGLSurface *)malloc( sizeof(iEGLSurface) )) )
	EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );

	memset( surface, 0x00, sizeof(iEGLSurface) );

	if( !(surface->backBuffer1 = (AddressBase *)malloc( sizeof(AddressBase) )) )
	{
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}

	/*
#if defined(DOUBLE_BUFFER)
	if( !(surface->backBuffer2 = (AddressBase *)PLATFORM_MALLOC( sizeof(AddressBase) )) )
	{
		PLATFORM_FREE( surface->backBuffer1 );
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}
#elif defined(TRIPLE_BUFFER)
	if( !(surface->backBuffer2 = (AddressBase *)PLATFORM_MALLOC( sizeof(AddressBase) )) )
	{
		PLATFORM_FREE( surface->backBuffer1 );
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}

	if( !(surface->backBuffer3 = (AddressBase *)PLATFORM_MALLOC( sizeof(AddressBase) )) )
	{
		PLATFORM_FREE( surface->backBuffer1 );
		PLATFORM_FREE( surface->backBuffer2 );
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}
#endif
*/

	if( !(surface->depthBuffer = (AddressBase *)malloc( sizeof(AddressBase) )) )
	{
		PLATFORM_FREE( surface->backBuffer1 );
		/*
#if defined(DOUBLE_BUFFER)
		PLATFORM_FREE( surface->backBuffer2 );
#elif defined(TRIPLE_BUFFER)
		PLATFORM_FREE( surface->backBuffer2 );
		PLATFORM_FREE( surface->backBuffer3 );
#endif
*/
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}

	memset( surface->backBuffer1, 0x00, sizeof(AddressBase) );
	/*
#if defined(DOUBLE_BUFFER)
	PLATFORM_MEMSET( surface->backBuffer2, 0x00, sizeof(AddressBase) );
#elif defined(TRIPLE_BUFFER)
	PLATFORM_MEMSET( surface->backBuffer2, 0x00, sizeof(AddressBase) );
	PLATFORM_MEMSET( surface->backBuffer3, 0x00, sizeof(AddressBase) );
#endif
*/
	memset( surface->depthBuffer, 0x00, sizeof(AddressBase) );

	if( width < 0 || height < 0 )
	{
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_PARAMETER, EGL_FALSE );
	}

	if( !(configAtt->eglSurfaceType & EGL_PBUFFER_BIT) )
	{
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_MATCH, EGL_NO_SURFACE );
	}

	if( (textureFormat == EGL_NO_TEXTURE && textureTarget != EGL_NO_TEXTURE) || (textureFormat != EGL_NO_TEXTURE && textureTarget == EGL_NO_TEXTURE) )
	{
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_MATCH, EGL_NO_SURFACE );
	}
	surface->width = width;
	surface->height = height;

	surface->renderBuffer = EGL_BACK_BUFFER;
	surface->referenceCount = 0;
	surface->deleteMark = EGL_FALSE;
	surface->config = config;
	surface->surfaceType = EGL_PBUFFER_BIT;
	surface->isClientBuffer = EGL_FALSE;

	surface->largestPbuffer = largestPbuffer;
	surface->textureFormat  = textureFormat;
	surface->textureTarget  = textureTarget;
	surface->mipmapTexture  = mipmapTexture;

	surface->redSize     = configAtt->eglRedSize;
	surface->greenSize   = configAtt->eglGreenSize;
	surface->blueSize    = configAtt->eglBlueSize;
	surface->alphaSize   = configAtt->eglAlphaSize;
	surface->depthSize   = configAtt->eglDepthSize;
	surface->stencilSize = configAtt->eglStencilSize;
	surface->sampleSize  = configAtt->eglSamples;

	if( configAtt->eglBufferSize > 16 && configAtt->eglBufferSize <= 32 )
	{
		surface->bufferBytes = 4;
		size = surface->width * surface->height * surface->bufferBytes;
		/*
#if defined(DOUBLE_BUFFER)
		surface->backBuffer1Handle = eglGLESGetBuffer( size );
		surface->backBuffer2Handle = eglGLESGetBuffer( size );

		eglGLESGetBufferAddress( surface->backBuffer1Handle, &surface->backBuffer1->paddr, &surface->backBuffer1->vaddr );
		eglGLESGetBufferAddress( surface->backBuffer2Handle, &surface->backBuffer2->paddr, &surface->backBuffer2->vaddr );
#elif defined(TRIPLE_BUFFER)
		surface->backBuffer1Handle = eglGLESGetBuffer( size );
		surface->backBuffer2Handle = eglGLESGetBuffer( size );
		surface->backBuffer3Handle = eglGLESGetBuffer( size );

		eglGLESGetBufferAddress( surface->backBuffer1Handle, &surface->backBuffer1->paddr, &surface->backBuffer1->vaddr );
		eglGLESGetBufferAddress( surface->backBuffer2Handle, &surface->backBuffer2->paddr, &surface->backBuffer2->vaddr );
		eglGLESGetBufferAddress( surface->backBuffer3Handle, &surface->backBuffer3->paddr, &surface->backBuffer3->vaddr );
#else
*/
		surface->backBuffer1Handle = eglGLESGetBuffer( size );
		eglGLESGetBufferAddress( surface->backBuffer1Handle, &surface->backBuffer1->paddr, &surface->backBuffer1->vaddr );
		//#endif
	}
	else if( configAtt->eglBufferSize > 8 && configAtt->eglBufferSize <= 16 )
	{
		surface->bufferBytes = 2;
		size = surface->width * surface->height * surface->bufferBytes;
		/*
#if defined(DOUBLE_BUFFER)
		surface->backBuffer1Handle = eglGLESGetBuffer( size );
		surface->backBuffer2Handle = eglGLESGetBuffer( size );

		eglGLESGetBufferAddress( surface->backBuffer1Handle, &surface->backBuffer1->paddr, &surface->backBuffer1->vaddr );
		eglGLESGetBufferAddress( surface->backBuffer2Handle, &surface->backBuffer2->paddr, &surface->backBuffer2->vaddr );
#elif defined(TRIPLE_BUFFER)
		surface->backBuffer1Handle = eglGLESGetBuffer( size );
		surface->backBuffer2Handle = eglGLESGetBuffer( size );
		surface->backBuffer3Handle = eglGLESGetBuffer( size );

		eglGLESGetBufferAddress( surface->backBuffer1Handle, &surface->backBuffer1->paddr, &surface->backBuffer1->vaddr );
		eglGLESGetBufferAddress( surface->backBuffer2Handle, &surface->backBuffer2->paddr, &surface->backBuffer2->vaddr );
		eglGLESGetBufferAddress( surface->backBuffer3Handle, &surface->backBuffer3->paddr, &surface->backBuffer3->vaddr );
#else
*/
		surface->backBuffer1Handle = eglGLESGetBuffer( size );
		eglGLESGetBufferAddress( surface->backBuffer1Handle, &surface->backBuffer1->paddr, &surface->backBuffer1->vaddr );
		//#endif
	}

	if( surface->depthSize == 24 )
	{
		size = width * height * sizeof(unsigned int);

		surface->depthBufferHandle = eglGLESGetBuffer( size );
		eglGLESGetBufferAddress( surface->depthBufferHandle, &surface->depthBuffer->paddr, &surface->depthBuffer->vaddr );
	}

	surface->stencilBuffer = surface->depthBuffer;

	surface->curDrawingBuffer = 1;
	surface->frontBuffer = surface->backBuffer1;

	if( surface->redSize == 8 && surface->greenSize == 8 && surface->blueSize == 8 && surface->alphaSize == 8 )
	surface->colorFormat = E_ARGB8;
	else if( surface->redSize == 8 && surface->greenSize == 8 && surface->blueSize == 8 && surface->alphaFormat == 0 )
	surface->colorFormat = E_ARGB0888;
	else if( surface->redSize == 5 && surface->greenSize == 6 && surface->blueSize == 5 )
	surface->colorFormat = E_RGB565;
	else if( surface->redSize == 5 && surface->greenSize == 5 && surface->blueSize == 5 && surface->alphaSize == 1 )
	surface->colorFormat = E_RGBA5551;
	else if( surface->redSize == 4 && surface->greenSize == 4 && surface->blueSize == 4 && surface->alphaSize == 4 )
	surface->colorFormat = E_ARGB4;

	if( surface->depthSize == 24 )
	{
		if( surface->stencilSize == 8 )
		surface->depthStencilFormat = E_Stencil8Depth24;
		else if( surface->stencilSize == 0 )
		surface->depthStencilFormat = E_Depth24;
	}
	else if( surface->depthSize == 0 )
	{
		if( surface->stencilSize == 8 )
		surface->depthStencilFormat = E_Stencil8;
		else if( surface->stencilSize == 0 )
		surface->depthStencilFormat = 0;
	}

	if( configAtt->eglSurfaceType & EGL_LOCK_SURFACE_BIT_KHR )
	surface->surfaceType |= EGL_LOCK_SURFACE_BIT_KHR;

	EGL_ERROR( EGL_SUCCESS, surface );
}

EGLSurface eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, NativePixmapType pixmap, const EGLint* attrib_list)
{
	EGLint        i;
	EGLint        width = 0, height = 0;
	EGLint        num_display;
	EGLConfigAtt *configAtt = (EGLConfigAtt *)config;
	iEGLSurface  *surface = 0;
	EGLint        colorSpace = EGL_VG_COLORSPACE_sRGB;
	EGLint        alphaFormat = EGL_VG_ALPHA_FORMAT_NONPRE;
	EGLint        size = 0;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NO_SURFACE );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_NO_SURFACE );

	if ( !config )
	EGL_ERROR( EGL_BAD_CONFIG, EGL_NO_SURFACE );

	if ( !(configAtt->eglSurfaceType & EGL_PIXMAP_BIT) )
	EGL_ERROR( EGL_BAD_MATCH, EGL_NO_SURFACE );

	//if ( !pixmap )
	if( !PLATFORM_IS_PIXMAP_VALID( dpy, pixmap ) )
	EGL_ERROR( EGL_BAD_NATIVE_PIXMAP, EGL_NO_SURFACE );

	for( i = 0; configAtt->associatedPixmap[i]; i++ )
	{
		if( configAtt->associatedPixmap[i] == pixmap )
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}

	if ( attrib_list )
	{
		for ( i = 0; attrib_list[i] != EGL_NONE; i+=2 )
		switch( attrib_list[i] )
		{
		case EGL_VG_COLORSPACE:
			if( (attrib_list[i+1] == EGL_VG_COLORSPACE_LINEAR) && !(configAtt->eglSurfaceType & EGL_VG_COLORSPACE_LINEAR_BIT) )
			EGL_ERROR( EGL_BAD_MATCH, EGL_NO_SURFACE );

			colorSpace = attrib_list[i+1];
			break;
		case EGL_VG_ALPHA_FORMAT:
			if( (attrib_list[i+1] == EGL_VG_ALPHA_FORMAT_PRE) && !(configAtt->eglSurfaceType & EGL_VG_ALPHA_FORMAT_PRE_BIT) )
			EGL_ERROR( EGL_BAD_MATCH, EGL_NO_SURFACE );

			alphaFormat = attrib_list[i+1];
			break;
		}
	}
	else
	//EGL_ERROR( EGL_SUCCESS, EGL_NO_SURFACE );

	if ( !(surface = (iEGLSurface *)malloc( sizeof(iEGLSurface) )) )
	EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );

	memset( surface, 0x00, sizeof(iEGLSurface) );

	if( !(surface->depthBuffer = (AddressBase *)malloc( sizeof(AddressBase) )) )
	{
		PLATFORM_FREE( surface );
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
	}

	memset( surface->depthBuffer, 0x00, sizeof(AddressBase) );

	PLATFORM_GET_PIXMAP_SIZE( dpy, pixmap, &width, &height );
	surface->width = width;
	surface->height = height;

	surface->renderBuffer = EGL_SINGLE_BUFFER;
	surface->referenceCount = 0;
	surface->deleteMark = EGL_FALSE;
	surface->config = config;
	surface->pixmap = pixmap;
	surface->surfaceType = EGL_PIXMAP_BIT;
	surface->isClientBuffer = EGL_FALSE;

	surface->colorSpace = ( colorSpace == EGL_VG_COLORSPACE_sRGB ? EGL_TRUE : EGL_FALSE );
	surface->alphaFormat = ( alphaFormat == EGL_VG_ALPHA_FORMAT_NONPRE ? EGL_TRUE : EGL_FALSE );

	surface->redSize     = configAtt->eglRedSize;
	surface->greenSize   = configAtt->eglGreenSize;
	surface->blueSize    = configAtt->eglBlueSize;
	surface->alphaSize   = configAtt->eglAlphaSize;
	surface->depthSize   = configAtt->eglDepthSize;
	surface->stencilSize = configAtt->eglStencilSize;
	surface->sampleSize  = configAtt->eglSamples;

	PLATFORM_GET_PIXMAP_COLORBUFFER( dpy, pixmap, &surface->backBuffer1->vaddr, &surface->backBuffer1->paddr );

	if( surface->depthSize == 24 )
	{
		size = width * height * sizeof(unsigned int);

		surface->depthBufferHandle = eglGLESGetBuffer( size );
		eglGLESGetBufferAddress( surface->depthBufferHandle, &surface->depthBuffer->paddr, &surface->depthBuffer->vaddr );
	}

	surface->stencilBuffer = surface->depthBuffer;

	surface->curDrawingBuffer = 1;

	for( i = 0; configAtt->associatedPixmap[i]; i++ );
	configAtt->associatedPixmap[i] = pixmap;

	if( surface->redSize == 8 && surface->greenSize == 8 && surface->blueSize == 8 && surface->alphaSize == 8 )
	surface->colorFormat = E_ARGB8;
	else if( surface->redSize == 8 && surface->greenSize == 8 && surface->blueSize == 8 && surface->alphaFormat == 0 )
	surface->colorFormat = E_ARGB0888;
	else if( surface->redSize == 5 && surface->greenSize == 6 && surface->blueSize == 5 )
	surface->colorFormat = E_RGB565;
	else if( surface->redSize == 5 && surface->greenSize == 5 && surface->blueSize == 5 && surface->alphaSize == 1 )
	surface->colorFormat = E_RGBA5551;
	else if( surface->redSize == 4 && surface->greenSize == 4 && surface->blueSize == 4 && surface->alphaSize == 4 )
	surface->colorFormat = E_ARGB4;

	if( surface->depthSize == 24 )
	{
		if( surface->stencilSize == 8 )
		surface->depthStencilFormat = E_Stencil8Depth24;
		else if( surface->stencilSize == 0 )
		surface->depthStencilFormat = E_Depth24;
	}
	else if( surface->depthSize == 0 )
	{
		if( surface->stencilSize == 8 )
		surface->depthStencilFormat = E_Stencil8;
		else if( surface->stencilSize == 0 )
		surface->depthStencilFormat = 0;
	}

	if( configAtt->eglSurfaceType & EGL_LOCK_SURFACE_BIT_KHR )
	surface->surfaceType |= EGL_LOCK_SURFACE_BIT_KHR;

	EGL_ERROR( EGL_SUCCESS, surface );
}

EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
	iEGLSurface    *iSurface = (iEGLSurface*)surface;
	NativeWindowType window = ((iEGLSurface*) iSurface)->window;

	acquireMutex();


	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	if ( !iSurface )
	EGL_ERROR( EGL_BAD_SURFACE, EGL_FALSE );

	if ( iSurface->referenceCount > 0 )
	{
		iSurface->deleteMark = EGL_TRUE;
		EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
	}

	if (iSurface->surfaceType & EGL_WINDOW_BIT)
	{
#if !defined(DIRECT_WINDOW_DRAWING)
#if defined(DOUBLE_BUFFER)
		eglGLESFreeBuffer( iSurface->backBuffer1Handle );
		eglGLESFreeBuffer( iSurface->backBuffer2Handle );
#elif defined(TRIPLE_BUFFER)
		eglGLESFreeBuffer( iSurface->backBuffer1Handle );
		eglGLESFreeBuffer( iSurface->backBuffer2Handle );
		eglGLESFreeBuffer( iSurface->backBuffer3Handle );
#else
		eglGLESFreeBuffer( iSurface->backBuffer1Handle );
#endif
#endif

		// Alex: 2009.10.22
		/*else if(window->memory_type == 1)
{
//    release_gpuaddress();
}*/


#if defined(DOUBLE_BUFFER)
		PLATFORM_FREE( iSurface->backBuffer1 );
		PLATFORM_FREE( iSurface->backBuffer2 );
#elif defined(TRIPLE_BUFFER)
		PLATFORM_FREE( iSurface->backBuffer1 );
		PLATFORM_FREE( iSurface->backBuffer2 );
		PLATFORM_FREE( iSurface->backBuffer3 );
#else
		PLATFORM_FREE( iSurface->backBuffer1 );
#endif

		PLATFORM_WINDOW_SURFACE_FINALIZE( dpy, iSurface->window, iSurface->userData);

	}
	else if (iSurface->surfaceType & EGL_PBUFFER_BIT)
	{
		if( !iSurface->isClientBuffer )
		{
			/*
#if defined(DOUBLE_BUFFER)
			eglGLESFreeBuffer( iSurface->backBuffer1Handle );
			eglGLESFreeBuffer( iSurface->backBuffer2Handle );
#elif defined(TRIPLE_BUFFER)
			eglGLESFreeBuffer( iSurface->backBuffer1Handle );
			eglGLESFreeBuffer( iSurface->backBuffer2Handle );
			eglGLESFreeBuffer( iSurface->backBuffer3Handle );
#else
*/
			eglGLESFreeBuffer( iSurface->backBuffer1Handle );
			//#endif

			/*
#if defined(DOUBLE_BUFFER)
			PLATFORM_FREE( iSurface->backBuffer1 );
			PLATFORM_FREE( iSurface->backBuffer2 );
#elif defined(TRIPLE_BUFFER)
			PLATFORM_FREE( iSurface->backBuffer1 );
			PLATFORM_FREE( iSurface->backBuffer2 );
			PLATFORM_FREE( iSurface->backBuffer3 );
#else
*/
			PLATFORM_FREE( iSurface->backBuffer1 );
			//#endif
		}

	}

	if( iSurface->depthBuffer )
	{
		if( iSurface->depthBufferHandle )
		eglGLESFreeBuffer( iSurface->depthBufferHandle );

		PLATFORM_FREE( iSurface->depthBuffer );
	}


	if (iSurface->surfaceType & EGL_WINDOW_BIT)
	{
		{
			EGLConfigAtt* configAtt = (EGLConfigAtt*)iSurface->config;
			EGLint i, j;

			for( i = 0; configAtt->associatedWindow[i]; i++ )
			{
				if( configAtt->associatedWindow[i] == iSurface->window )
				{
					for( j = i; configAtt->associatedWindow[j]; j++ )
					{
						if( (j + 1) != EGL_MAX_ASSOCIATED_WINDOW )
						configAtt->associatedWindow[j] = configAtt->associatedWindow[j+1];
						else
						configAtt->associatedWindow[j] = (EGLNativeWindowType)0;
					}

					break;
				}
			}
		}
	}
	else if (iSurface->surfaceType & EGL_PIXMAP_BIT)
	{
		{
			EGLConfigAtt* configAtt = (EGLConfigAtt*)iSurface->config;
			EGLint i, j;

			for( i = 0; configAtt->associatedPixmap[i]; i++ )
			{
				if( configAtt->associatedPixmap[i] == iSurface->pixmap)
				{
					for( j = i; configAtt->associatedPixmap[j]; j++ )
					{
						if( (j + 1) != EGL_MAX_ASSOCIATED_PIXMAP )
						configAtt->associatedPixmap[j] = configAtt->associatedPixmap[j+1];
						else
						configAtt->associatedPixmap[j] = (EGLNativePixmapType)0;
					}

					break;
				}
			}
		}
	}


	PLATFORM_FREE( iSurface );

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint* value)
{
	EGLint        i;
	EGLint        num_display;
	iEGLSurface  *iSurface = (iEGLSurface*)surface;
	EGLBoolean    isWindow = EGL_FALSE;
	EGLBoolean    isPbuffer = EGL_FALSE;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_display = _egl.num_display;
	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_FALSE );

	if ( !iSurface )
	EGL_ERROR( EGL_BAD_SURFACE, EGL_FALSE );

	if ( !value )
	EGL_ERROR( EGL_BAD_PARAMETER, EGL_FALSE );

	if( iSurface->surfaceType & EGL_WINDOW_BIT )
	isWindow = EGL_TRUE;

	if( iSurface->surfaceType & EGL_PBUFFER_BIT )
	isPbuffer = EGL_TRUE;

	switch( attribute )
	{
	case EGL_VG_ALPHA_FORMAT:
		*value = iSurface->alphaFormat;
		break;
	case EGL_VG_COLORSPACE:
		*value = iSurface->colorSpace;
		break;
	case EGL_CONFIG_ID:
		if ( iSurface->config )
		*value = ((EGLConfigAtt*)iSurface->config)->eglConfigID;
		break;
	case EGL_WIDTH:
		*value = iSurface->width;
		break;
	case EGL_HEIGHT:
		*value = iSurface->height;
		break;
	case EGL_RENDER_BUFFER:
		*value = iSurface->renderBuffer;
		break;
	case EGL_SWAP_BEHAVIOR:
		*value = EGL_BUFFER_PRESERVED;
		break;
	case EGL_MIPMAP_TEXTURE:
		if( isPbuffer )
		*value = iSurface->mipmapTexture;
		break;
	case EGL_MIPMAP_LEVEL:
		if( isPbuffer )
		*value = iSurface->mipmapLevel;
		break;
	case EGL_PIXEL_ASPECT_RATIO:
		if( isWindow )
		{
			if( PLATFORM_GET_DISPLAY_PIXEL_ASPECT_RATIO(iSurface->display) )
			*value = PLATFORM_GET_DISPLAY_PIXEL_ASPECT_RATIO(iSurface->display) * EGL_DISPLAY_SCALING;
			else
			*value = EGL_UNKNOWN;
		}
		break;
	case EGL_LARGEST_PBUFFER:
		if( isPbuffer )
		*value = iSurface->largestPbuffer;
		break;
	case EGL_HORIZONTAL_RESOLUTION:
		if( isWindow )
		{
			if( PLATFORM_GET_DISPLAY_HORIZONTAL_RESOLUTION(iSurface->display) )
			*value = PLATFORM_GET_DISPLAY_HORIZONTAL_RESOLUTION(iSurface->display) * EGL_DISPLAY_SCALING;
			else
			*value = EGL_UNKNOWN;
		}
		break;
	case EGL_VERTICAL_RESOLUTION:
		if( isWindow )
		{
			if( PLATFORM_GET_DISPLAY_VERTICAL_RESOLUTION(iSurface->display) )
			*value = PLATFORM_GET_DISPLAY_VERTICAL_RESOLUTION(iSurface->display) * EGL_DISPLAY_SCALING;
			else
			*value = EGL_UNKNOWN;
		}
		break;
	case EGL_TEXTURE_FORMAT:
		if( isPbuffer )
		*value = iSurface->textureFormat;
		break;
	case EGL_TEXTURE_TARGET:
		if( isPbuffer )
		*value = iSurface->textureTarget;
		break;
	case EGL_BITMAP_POINTER_KHR:
		if (!iSurface->isLocked)
		{
			EGL_ERROR( EGL_BAD_ACCESS, EGL_FALSE );
		}

		if (iSurface->preservePixels)
		{
			eglGLESFinish();
		}

		void *vaddr, *paddr;

		if (iSurface->surfaceType & EGL_PIXMAP_BIT)
		{
			PLATFORM_GET_PIXMAP_COLORBUFFER(iSurface->display, iSurface->pixmap, &vaddr, &paddr);
		}
		else if (iSurface->surfaceType & EGL_WINDOW_BIT)
		{
			PLATFORM_GET_WINDOW_ADDRESS(iSurface->display, iSurface->window, &vaddr, &paddr, iSurface->userData);
		}
		else
		{
			vaddr = iSurface->backBuffer1;
		}

		*value = (EGLint)vaddr;
		break;
	case EGL_BITMAP_PITCH_KHR:
		*value = iSurface->bitmapPitch;
		break;
	case EGL_BITMAP_ORIGIN_KHR:
		*value = iSurface->bitmapOrigin;
		break;
	case EGL_BITMAP_PIXEL_RED_OFFSET_KHR:
		*value = iSurface->bitmapPixelRedOffset;
		break;
	case EGL_BITMAP_PIXEL_GREEN_OFFSET_KHR:
		*value = iSurface->bitmapPixelGreenOffset;
		break;
	case EGL_BITMAP_PIXEL_BLUE_OFFSET_KHR:
		*value = iSurface->bitmapPixelBlueOffset;
		break;
	case EGL_BITMAP_PIXEL_ALPHA_OFFSET_KHR:
		*value = iSurface->bitmapPixelAlphaOffset;
		break;
	case EGL_BITMAP_PIXEL_LUMINANCE_OFFSET_KHR:
		*value = iSurface->bitmapPixelLuminanceOffset;
		break;

	default:
		EGL_ERROR( EGL_BAD_ATTRIBUTE, EGL_FALSE );
	}

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
	EGLint        i;
	EGLint        num_display;
	iEGLSurface  *iSurface = (iEGLSurface*)surface;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_display = _egl.num_display;
	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_FALSE );

	if ( !iSurface )
	EGL_ERROR( EGL_BAD_SURFACE, EGL_FALSE );

	switch( attribute )
	{
	case EGL_MIPMAP_LEVEL:
		iSurface->mipmapLevel = value;
		break;

	default:
		EGL_ERROR( EGL_BAD_PARAMETER, EGL_FALSE );
	}

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglBindTexImage( EGLDisplay dpy, EGLSurface surface, EGLint buffer )
{
	EGLint        i;
	EGLint        num_display;
	iEGLSurface *iSurface = (iEGLSurface*)surface;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_FALSE );

	if ( !iSurface )
	EGL_ERROR( EGL_BAD_SURFACE, EGL_FALSE );


	if (!(iSurface->surfaceType & EGL_PBUFFER_BIT))
	EGL_ERROR( EGL_BAD_SURFACE, EGL_FALSE );

	if( buffer != EGL_BACK_BUFFER )
	EGL_ERROR( EGL_BAD_PARAMETER, EGL_FALSE );

	eglGLESBindTexImage( iSurface );

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
	EGLint        i;
	EGLint        num_display;
	iEGLSurface  *iSurface = (iEGLSurface*)surface;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_FALSE );

	if ( !iSurface )
	EGL_ERROR( EGL_BAD_SURFACE, EGL_FALSE );

	if (!(iSurface->surfaceType & EGL_PBUFFER_BIT))
	EGL_ERROR( EGL_BAD_SURFACE, EGL_FALSE );

	if( buffer != EGL_BACK_BUFFER )
	EGL_ERROR( EGL_BAD_PARAMETER, EGL_FALSE );

	eglGLESReleaseTexImage( iSurface );


	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
	EGLint        i;
	EGLint        num_display;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_FALSE );

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLContext eglCreateContext( EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint* attrib_list )
{
	EGLint       i;
	EGLint       num_display;
	iEGLContext *context;
	EGLint     boundAPI;
	EGLint       clientVersion = 1;
	EGLThreadState *thread;
	iEGLContext *sharedContext = (iEGLContext*)share_list;
	//LOGE("**** eglCreateContext pid=0x%lx tid=0x%lx *******",getpid(),gettid());

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NO_CONTEXT );

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_NOT_INITIALIZED, NULL );

	boundAPI = thread->boundAPI;

	if( boundAPI == EGL_NONE )
	EGL_ERROR( EGL_BAD_MATCH, EGL_FALSE );

	if( !config )
	EGL_ERROR( EGL_BAD_CONFIG, EGL_FALSE );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_NO_CONTEXT );

	if( sharedContext && sharedContext->display != dpy )
	EGL_ERROR( EGL_BAD_MATCH, EGL_NO_CONTEXT );

	if ( !(context = (iEGLContext *)malloc(sizeof(iEGLContext))) )
	EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_CONTEXT );

	memset( context, 0x00, sizeof(iEGLContext) );

	if ( attrib_list )
	{
		for ( i = 0; attrib_list[i] != EGL_NONE; i+=2 )
		switch( attrib_list[i] )
		{
		case EGL_CONTEXT_CLIENT_VERSION:
			clientVersion = attrib_list[i+1];
			break;
		}
	}

	context->referenceCount = 0;
	context->deleteMark = EGL_FALSE;
	context->errorCode = EGL_SUCCESS;
	context->shareContext = share_list;

	context->drawSurface = EGL_NO_SURFACE;
	context->readSurface = EGL_NO_SURFACE;

	context->config = config;
	context->display = dpy;

	context->clientVersion = clientVersion;
	context->clientContext = EGL_NO_CONTEXT;

	context->boundAPI = boundAPI;

	switch( boundAPI )
	{
	case EGL_OPENVG_API:
		eglVGCreateContext( context );
		break;
	case EGL_OPENGL_ES_API:
		eglGLESInitDriver();
		eglGLESCreateContext( context );
		break;
	}

	EGL_ERROR( EGL_SUCCESS, context );
}

EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
	EGLint          i;
	EGLint          num_display;
	EGLThreadState *thread;
	EGLint        boundAPI;
	iEGLContext    *iContext = (iEGLContext*)ctx;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );


	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_FALSE );

	if ( !iContext )
	EGL_ERROR( EGL_BAD_CONTEXT, EGL_FALSE );

	if ( iContext->referenceCount > 0 )
	{
		iContext->deleteMark = EGL_TRUE;
		EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
	}

	thread = getCurrentThread();
	if(!thread)
	{
		EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );
	}
	else
	{
		if ( thread->context == ctx )
		{
			iContext->deleteMark = EGL_TRUE;
			EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
		}

		thread->context = (iEGLContext*)EGL_NO_CONTEXT;
	}

	boundAPI = thread->boundAPI;

	switch( boundAPI )
	{
	case EGL_OPENVG_API:
		eglVGDestroyContext( iContext );
		PLATFORM_FREE( iContext );
		break;
	case EGL_OPENGL_ES_API:
		eglGLESDestroyContext( iContext );
		//eglGLESDeinitDriver();
		PLATFORM_FREE( iContext );
		break;
	}

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
	EGLint          i;
	EGLint          num_display;
	EGLint          num_thread;
	EGLThreadState *thread;
	iEGLContext    *context;
	iEGLSurface    *drawSurface, *readSurface;
	EGLint      boundAPI;
	EGLint          width, height;

	acquireMutex();


	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_FALSE );

	if( ctx != EGL_NO_CONTEXT && isContextCurrentToOtherThread( ctx ) )
	EGL_ERROR( EGL_BAD_ACCESS, EGL_FALSE );

	if( draw != EGL_NO_SURFACE && isSurfaceBoundToContextInOtherThread( draw ) )
	EGL_ERROR( EGL_BAD_ACCESS, EGL_FALSE );

	if( read != EGL_NO_SURFACE && isSurfaceBoundToContextInOtherThread( read ) )
	EGL_ERROR( EGL_BAD_ACCESS, EGL_FALSE );

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	boundAPI = thread->boundAPI;

	if ( !ctx && draw )
	EGL_ERROR( EGL_BAD_CONTEXT, EGL_FALSE );

	if( ctx && (!draw || !read) )
	EGL_ERROR( EGL_BAD_MATCH, EGL_FALSE );

	if( (draw && (((iEGLSurface*)draw)->surfaceType & EGL_WINDOW_BIT) && !PLATFORM_IS_WINDOW_VALID(dpy,((iEGLSurface*)draw)->window)) ||
			(read && (((iEGLSurface*)read)->surfaceType & EGL_WINDOW_BIT) && !PLATFORM_IS_WINDOW_VALID(dpy,((iEGLSurface*)read)->window)) )
	EGL_ERROR( EGL_BAD_NATIVE_WINDOW, EGL_FALSE );

	if( boundAPI == EGL_OPENVG_API && draw != read )
	EGL_ERROR( EGL_BAD_MATCH, EGL_FALSE );

	context = thread->context;

	if ( context )
	{
		context->referenceCount--;

		if ( context->deleteMark && context != (iEGLContext *)ctx )
		{
			thread->context = (iEGLContext *)ctx;
			eglDestroyContext( dpy, context );
		}
	}

	readSurface = thread->read;

	if ( readSurface )
	{
		readSurface->referenceCount--;

		if ( readSurface->deleteMark && readSurface != (iEGLSurface *)read && readSurface != (iEGLSurface *)draw )
		{
			thread->read = (iEGLSurface *)read;
			eglDestroySurface( dpy, readSurface );
		}
	}

	drawSurface = thread->draw;

	if ( drawSurface && drawSurface != readSurface )
	{
		drawSurface->referenceCount--;

		if ( drawSurface->deleteMark && drawSurface != (iEGLSurface *)read && drawSurface != (iEGLSurface *)draw )
		{
			thread->draw = (iEGLSurface *)draw;
			eglDestroySurface( dpy, drawSurface );
		}
	}

	if ( !ctx && !read && !draw )
	{
		switch( boundAPI )
		{
		case EGL_OPENVG_API:
			eglVGSetContext( (iEGLContext*)EGL_NO_CONTEXT );
			break;
		case EGL_OPENGL_ES_API:
			eglGLESSetContext( (iEGLContext*)EGL_NO_CONTEXT );
			break;
		}

		thread->context = (iEGLContext*)EGL_NO_CONTEXT;
		thread->read = (iEGLSurface*)EGL_NO_SURFACE;
		thread->draw = (iEGLSurface*)EGL_NO_SURFACE;

		EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
	}

	drawSurface = (iEGLSurface *)draw;
	readSurface = (iEGLSurface *)read;

	//if( (drawSurface->surfaceType == EGL_WINDOW_BIT && !drawSurface->window) )
	if( (drawSurface->surfaceType & EGL_WINDOW_BIT) && !PLATFORM_IS_WINDOW_VALID(dpy,drawSurface->window) )
	EGL_ERROR( EGL_BAD_NATIVE_WINDOW, EGL_FALSE );

	drawSurface = (iEGLSurface*)draw;

	if ( drawSurface->isClientBuffer && drawSurface->referenceCount > 0 )
	EGL_ERROR( EGL_BAD_ACCESS, EGL_FALSE );

	thread->context = (iEGLContext*)ctx;
	thread->read = (iEGLSurface*)read;
	thread->draw = (iEGLSurface*)draw;
	thread->display = dpy;

	readSurface = (iEGLSurface*)read;

	if ( drawSurface )
	drawSurface->referenceCount++;

	if ( readSurface && readSurface != drawSurface )
	readSurface->referenceCount++;

	context = (iEGLContext *)ctx;

	context->drawSurface = draw;
	context->readSurface = read;
	context->display = dpy;
	context->referenceCount++;

	switch( boundAPI )
	{
	case EGL_OPENVG_API:
		if( !eglVGSetContext( (iEGLContext *)ctx ) )
		EGL_ERROR( EGL_BAD_ALLOC, EGL_FALSE );

		eglVGSetColorBuffer( drawSurface );

		break;
	case EGL_OPENGL_ES_API:
		if( !eglGLESSetContext( (iEGLContext *)ctx ) )
		EGL_ERROR( EGL_BAD_ALLOC, EGL_FALSE );

		//eglGLESSetColorBuffer( drawSurface );
		eglGLESSetColorBuffer( drawSurface, EGL_DRAW );
		eglGLESSetColorBuffer( readSurface, EGL_READ );

		if( !((iEGLContext *)ctx)->isActivated )
		{
			((iEGLContext *)ctx)->isActivated = EGL_TRUE;

			eglGLESViewport( 0, 0, drawSurface->width, drawSurface->height );
			eglGLESScissor( 0, 0, drawSurface->width, drawSurface->height );
		}
		break;
		default :
		EGL_ERROR( EGL_BAD_ALLOC, EGL_FALSE );
		break;
	}


	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLContext eglGetCurrentContext(void)
{
	EGLThreadState *thread;
	EGLContext      context;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NO_CONTEXT );

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NO_CONTEXT );

	if ( thread->boundAPI == EGL_NONE )
	EGL_ERROR( EGL_SUCCESS, EGL_NO_CONTEXT );

	context = thread->context;

	EGL_ERROR( EGL_SUCCESS, context );
}

EGLSurface eglGetCurrentSurface(EGLint readdraw)
{
	EGLThreadState *thread;
	EGLSurface      surface;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NO_SURFACE );


	if ( readdraw != EGL_READ && readdraw != EGL_DRAW )
	EGL_ERROR( EGL_BAD_PARAMETER, EGL_NO_SURFACE );

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NO_SURFACE );

	if ( !thread->context )
	EGL_ERROR( EGL_SUCCESS, EGL_NO_SURFACE );

	if ( readdraw == EGL_READ )
	surface = thread->context->readSurface;
	else
	surface = thread->context->drawSurface;

	EGL_ERROR( EGL_SUCCESS, surface );
}

EGLDisplay eglGetCurrentDisplay(void)
{
	EGLThreadState *thread;
	EGLDisplay      display;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NO_DISPLAY );

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NO_DISPLAY );

	if ( thread->boundAPI == EGL_NONE )
	EGL_ERROR( EGL_SUCCESS, EGL_NO_DISPLAY );

	display = thread->context->display;

	EGL_ERROR( EGL_SUCCESS, display );
}

EGLBoolean eglQueryContext(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint* value)
{
	EGLint          i;
	EGLint          num_display;
	iEGLSurface    *surface;
	iEGLContext    *context = (iEGLContext*)ctx;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_FALSE );

	if ( !ctx )
	EGL_ERROR( EGL_BAD_CONTEXT, EGL_FALSE );

	if ( !value )
	EGL_ERROR( EGL_BAD_PARAMETER, EGL_FALSE );

	switch( attribute )
	{
	case EGL_CONFIG_ID:
		if ( context->config )
		*value = ((EGLConfigAtt*)context->config)->eglConfigID;
		break;
	case EGL_CONTEXT_CLIENT_TYPE:
		*value = context->boundAPI;
		break;
	case EGL_RENDER_BUFFER:
		surface = (iEGLSurface *)context->drawSurface;
		if ( !surface )
		{
			*value = EGL_NONE;
			break;
		}

		*value = surface->renderBuffer;
		break;
	case EGL_CONTEXT_CLIENT_VERSION:
		*value = context->clientVersion;
		break;
	default:
		EGL_ERROR(EGL_BAD_ATTRIBUTE, EGL_FALSE);
	}

	EGL_ERROR(EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglWaitGL(void)
{
	EGLenum api = eglQueryAPI();

	acquireMutex();

	eglBindAPI( EGL_OPENGL_ES_API );
	eglWaitClient();
	eglBindAPI( api );

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglWaitNative(EGLint engine)
{
	EGLThreadState *thread;
	EGLint      boundAPI;
	iEGLContext    *context;

	acquireMutex();

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	boundAPI = thread->boundAPI;

	context = thread->context;

	if( !context )
	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );

	if( !context->drawSurface )
	EGL_ERROR( EGL_BAD_CURRENT_SURFACE, EGL_FALSE );

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
//	EGLint     boundAPI;
	iEGLSurface *iSurface = (iEGLSurface *)surface;
	EGLint       width, height;
	EGLThreadState *thread;

	NativeWindowType window = ((iEGLSurface*) iSurface)->window;
	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	if( !iSurface )
	EGL_ERROR( EGL_BAD_SURFACE, EGL_FALSE );

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	if( !thread->context )
	EGL_ERROR( EGL_BAD_SURFACE, EGL_FALSE );

	if( (thread->context->drawSurface != surface && thread->context->readSurface != surface) )
	EGL_ERROR( EGL_BAD_SURFACE, EGL_FALSE );
	
//	boundAPI = thread->boundAPI;
	

	// Alex: 2009.10.22
/*	if( iSurface->surfaceType & EGL_WINDOW_BIT)
	{
		switch( boundAPI )
		{
		case EGL_OPENVG_API:
			eglVGFinish();
			break;

		case EGL_OPENGL_ES_API:

			eglGLESFinish();

			break;
		}
	}*/
	
	PLATFORM_GET_WINDOW_SIZE(dpy, iSurface->window, &width, &height, iSurface->userData);

	if( iSurface->width != width || iSurface->height != height )
	{
//		switch( boundAPI )
//		{
//		case EGL_OPENVG_API:
//			eglVGSetColorBuffer( iSurface );
//			break;

//		case EGL_OPENGL_ES_API:
			if( (thread->context->drawSurface == surface) )
			eglGLESSetColorBuffer( iSurface, EGL_DRAW );
			else if( (thread->context->readSurface == surface) )
			eglGLESSetColorBuffer( iSurface, EGL_READ );

//			break;
//		}
	}

	if( iSurface->surfaceType & EGL_WINDOW_BIT )
	{
		if( iSurface->curDrawingBuffer == 1 )
		{
			iSurface->frontBuffer = iSurface->backBuffer1;
			iSurface->curDrawingBuffer = 2;
		}
		else
		{
			iSurface->frontBuffer = iSurface->backBuffer2;
			iSurface->curDrawingBuffer = 1;
		}
	}
	else
	{
		iSurface->curDrawingBuffer = 1;
	}
	
	PLATFORM_DISPLAY_BUFFER( dpy, iSurface->window, 0, iSurface->width, iSurface->height, iSurface->bufferBytes,iSurface->userData);
	
	PLATFORM_GET_WINDOW_SIZE(dpy, iSurface->window, &width, &height, iSurface->userData);

	if ((iSurface->width != width) || (iSurface->height != height))
	{
		//LOGE("Resizing from (%d,%d) to (%d,%d)",iSurface->width,iSurface->height,iSurface->buffer->width,iSurface->buffer->height);

		resizeWindowSurface( (EGLSurface)iSurface, width,height);

//		switch( boundAPI )
//		{
//		case EGL_OPENVG_API:
//			eglVGSetColorBuffer( iSurface );
//			break;

//		case EGL_OPENGL_ES_API:
			if( (thread->context->drawSurface == surface) )
			eglGLESSetColorBuffer( iSurface, EGL_DRAW );
			else if( (thread->context->readSurface == surface) )
			eglGLESSetColorBuffer( iSurface, EGL_READ );

//			break;
//		}
	}
	
//	switch( boundAPI )
//	{
//	case EGL_OPENVG_API :
//		eglVGSetColorBuffer( iSurface );
//		break;

//	case EGL_OPENGL_ES_API :
		if( (thread->context->drawSurface == surface) )
		eglGLESSetColorBuffer( iSurface, EGL_DRAW );
		else if( (thread->context->readSurface == surface) )
		eglGLESSetColorBuffer( iSurface, EGL_READ );

//		break;
//	}

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglCopyBuffers(EGLDisplay dpy, EGLSurface surface, NativePixmapType target)
{
	iEGLSurface  *iSurface = (iEGLSurface *)surface;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	if( !iSurface || !iSurface->backBuffer1 )
	EGL_ERROR( EGL_BAD_SURFACE, EGL_FALSE );

	if ( !target )
	EGL_ERROR( EGL_BAD_SURFACE, EGL_FALSE );

	PLATFORM_COPY_BUFFER( dpy, iSurface->frontBuffer->vaddr, target, iSurface->width, iSurface->height, iSurface->bufferBytes);

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglSetSwapRectangleANDROID(EGLDisplay eglDpy, EGLSurface surface, EGLint left, EGLint top, EGLint width, EGLint height)
{
	iEGLSurface *iSurface = (iEGLSurface *)surface;

	NativeWindowType window = ((iEGLSurface*) iSurface)->window;
	acquireMutex();

	if (!_egl.init)
	{
		EGL_ERROR(EGL_NOT_INITIALIZED, EGL_FALSE);
	}

	if (!iSurface)
	{
		EGL_ERROR(EGL_BAD_SURFACE, EGL_FALSE);
	}

	PLATFORM_WINDOW_SURFACE_SET_SWAP_RECTANGLE(eglDpy, iSurface->window, left, top, width, height, iSurface->userData);

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLClientBuffer eglGetRenderBufferANDROID(EGLDisplay eglDpy, EGLSurface surface)
{
	iEGLSurface *iSurface = (iEGLSurface *)surface;

	NativeWindowType window = ((iEGLSurface*) iSurface)->window;
	acquireMutex();

	if (!_egl.init)
	{
		EGL_ERROR(EGL_NOT_INITIALIZED, 0);
	}

	if (!iSurface)
	{
		EGL_ERROR(EGL_BAD_SURFACE, 0);
	}

	return (EGLClientBuffer)PLATFORM_WINDOW_SURFACE_GET_BUFFER(eglDpy, iSurface->window, iSurface->userData);
}

EGLSurface eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list)
{
	EGLint        i;
	EGLint        width = 0, height = 0;
	EGLint        num_display;
	EGLConfigAtt *configAtt = (EGLConfigAtt *)config;
	iEGLSurface  *surface;
	EGLint      boundAPI;
	EGLThreadState *thread;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NO_SURFACE );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_NO_SURFACE );

	if ( !config )
	EGL_ERROR( EGL_BAD_CONFIG, EGL_NO_SURFACE );


	if( buftype != EGL_OPENVG_IMAGE )
	EGL_ERROR( EGL_BAD_PARAMETER, EGL_NO_SURFACE );

	if ( !(configAtt->eglSurfaceType & EGL_PIXMAP_BIT) )
	EGL_ERROR( EGL_BAD_MATCH, EGL_NO_SURFACE );

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NO_SURFACE );

	boundAPI = thread->boundAPI;

	if ( attrib_list )
	for ( i = 0; attrib_list[i] != EGL_NONE; i+=2 )
	{
		switch ( attrib_list[i] )
		{
		}
	}

	switch( boundAPI )
	{
	case EGL_OPENVG_API:

		if ( !(surface = (iEGLSurface *)malloc( sizeof(iEGLSurface) )) )
		EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );

		memset( surface, 0x00, sizeof(iEGLSurface) );

		if( !eglVGCreatePbufferFromClientBuffer( surface, buffer ) )
		{
			PLATFORM_FREE( surface );
			EGL_ERROR( EGL_BAD_ALLOC, EGL_NO_SURFACE );
		}

		break;
	case EGL_OPENGL_ES_API:
		EGL_ERROR( EGL_BAD_ACCESS, EGL_NO_SURFACE );
		break;
		default :
		surface = (iEGLSurface*)EGL_NO_SURFACE;
		break;
	}

	surface->renderBuffer = EGL_BACK_BUFFER;

	surface->referenceCount = 0;
	surface->deleteMark = EGL_FALSE;
	surface->config = config;
	surface->surfaceType = EGL_PBUFFER_BIT;
	surface->isClientBuffer = EGL_TRUE;


	surface->redSize     = configAtt->eglRedSize;
	surface->greenSize   = configAtt->eglGreenSize;
	surface->blueSize    = configAtt->eglBlueSize;
	surface->alphaSize   = configAtt->eglAlphaSize;

	if( surface->redSize == 8 && surface->greenSize == 8 && surface->blueSize == 8 && surface->alphaSize == 8 )
	surface->colorFormat = E_ARGB8;
	else if( surface->redSize == 8 && surface->greenSize == 8 && surface->blueSize == 8 && surface->alphaFormat == 0 )
	surface->colorFormat = E_ARGB0888;
	else if( surface->redSize == 5 && surface->greenSize == 6 && surface->blueSize == 5 )
	surface->colorFormat = E_RGB565;
	else if( surface->redSize == 5 && surface->greenSize == 5 && surface->blueSize == 5 && surface->alphaSize == 1 )
	surface->colorFormat = E_RGBA5551;
	else if( surface->redSize == 4 && surface->greenSize == 4 && surface->blueSize == 4 && surface->alphaSize == 4 )
	surface->colorFormat = E_ARGB4;

	if( configAtt->eglSurfaceType & EGL_LOCK_SURFACE_BIT_KHR )
	surface->surfaceType |= EGL_LOCK_SURFACE_BIT_KHR;

	EGL_ERROR( EGL_SUCCESS, surface );
}

EGLBoolean eglWaitClient(void)
{
	EGLThreadState *thread;
	EGLint      boundAPI;
	iEGLContext    *context;

	acquireMutex();

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	boundAPI = thread->boundAPI;

	context = thread->context;

	if( !context )
	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );

	if( !context->drawSurface )
	EGL_ERROR( EGL_BAD_CURRENT_SURFACE, EGL_FALSE );

	switch( boundAPI )
	{
	case EGL_OPENVG_API:
		eglVGFinish();
		break;
	case EGL_OPENGL_ES_API:
		eglGLESFinish();
		break;
	}

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglBindAPI(EGLenum api)
{
	EGLThreadState *thread;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	switch ( api )
	{
	case EGL_OPENVG_API:
	case EGL_OPENGL_ES_API:
	case EGL_NONE:
		thread->boundAPI = api;
		break;
	default:
		EGL_ERROR( EGL_BAD_PARAMETER, EGL_FALSE );
	}

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLenum eglQueryAPI(void)
{
	EGLenum         api;
	EGLThreadState *thread;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NONE );

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_NONE );

	api = thread->boundAPI;

	EGL_ERROR( EGL_SUCCESS, api );
}

EGLBoolean eglReleaseThread(void)
{
	EGLThreadState *thread;
	iEGLContext    *context;
	iEGLSurface    *drawSurface, *readSurface;

	acquireMutex();

	if ( !_egl.init )

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );

	thread = getCurrentThread();
	if(!thread)
	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );



	context = thread->context;

	if ( context )
	{
		switch ( thread->boundAPI )
		{
		case EGL_OPENVG_API:
			context->referenceCount--;

			if ( context->deleteMark )
			eglDestroyContext( context->display, context );

			break;
		case EGL_OPENGL_ES_API:

			break;
		}
	}

	readSurface = thread->read;

	if ( readSurface )
	{
		readSurface->referenceCount--;

		if ( readSurface->deleteMark )
		eglDestroySurface( readSurface->display, readSurface );
	}

	drawSurface = thread->draw;

	if ( drawSurface && drawSurface != readSurface )
	{
		drawSurface->referenceCount--;

		if ( drawSurface->deleteMark )
		eglDestroySurface( drawSurface->display, drawSurface );
	}

	switch( thread->boundAPI )
	{
	case EGL_OPENVG_API:
		eglVGSetContext( (iEGLContext*)EGL_NO_CONTEXT );
		break;
	case EGL_OPENGL_ES_API:
		eglGLESSetContext( (iEGLContext*)EGL_NO_CONTEXT );
		break;
	}
	thread->context = (iEGLContext*)EGL_NO_CONTEXT;
	thread->read = (iEGLSurface*)EGL_NO_SURFACE;
	thread->draw = (iEGLSurface*)EGL_NO_SURFACE;

	thread->boundAPI = EGL_OPENGL_ES_API;

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}


/* egl extension */
EGLBoolean eglLockSurfaceKHR( EGLDisplay display, EGLSurface surface, const EGLint *attrib_list)
{
	int          i;
	EGLBoolean   preservePixels = EGL_FALSE;
	EGLint       usageHint = EGL_READ_SURFACE_BIT_KHR | EGL_WRITE_SURFACE_BIT_KHR;
	iEGLSurface *iSurface = (iEGLSurface*)surface;

	acquireMutex();

	if( !(iSurface->surfaceType & EGL_LOCK_SURFACE_BIT_KHR) )
	EGL_ERROR( EGL_BAD_ACCESS, EGL_FALSE );

	if( iSurface->isLocked )
	EGL_ERROR( EGL_BAD_ACCESS, EGL_FALSE );

	if( eglGetCurrentSurface( EGL_DRAW ) == surface )
	EGL_ERROR( EGL_BAD_ACCESS, EGL_FALSE );

	if ( attrib_list )
	{
		for ( i = 0; attrib_list[i] != EGL_NONE; i+=2 )
		{
			switch( attrib_list[i] )
			{
			case EGL_MAP_PRESERVE_PIXELS_KHR:
				preservePixels = attrib_list[i+1];
				break;
			case EGL_LOCK_USAGE_HINT_KHR:
				usageHint = attrib_list[i+1];
				break;
			}
		}
	}

	iSurface->isLocked       = EGL_TRUE;
	iSurface->preservePixels = preservePixels;
	iSurface->usageHint      = usageHint;

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLBoolean eglUnlockSurfaceKHR( EGLDisplay display, EGLSurface surface)
{
	iEGLSurface *iSurface = (iEGLSurface*)surface;

	acquireMutex();

	if( !iSurface->isLocked )
	EGL_ERROR( EGL_BAD_ACCESS, EGL_FALSE );

	/* needed proper system framebuffer drawing */
	//PLATFORM_REFRECT_BUFFER( iSurface->bitmapPointer, iSurface->width, iSurface->height );

	iSurface->isLocked = EGL_FALSE;

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

EGLImageKHR eglCreateImageKHR( EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, EGLint* attr_list)
{
	iEGLImage    *image;
	EGLint        i;
	EGLint        num_display;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, 0 );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, 0 );


	switch( target )
	{
	case EGL_NATIVE_PIXMAP_KHR:
		if( !buffer )
		EGL_ERROR( EGL_BAD_PARAMETER, 0 );
		break;
	case EGL_VG_PARENT_IMAGE_KHR:
	case EGL_GL_TEXTURE_2D_KHR:
	case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR:
	case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR:
	case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR:
	case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR:
	case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR:
	case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR:
	case EGL_GL_TEXTURE_3D_KHR:
	case EGL_GL_RENDERBUFFER_KHR:
		if( !ctx )
		EGL_ERROR( EGL_BAD_CONTEXT, 0 );

		if( !buffer )
		EGL_ERROR( EGL_BAD_PARAMETER, 0 );
		break;

	default:
		EGL_ERROR( EGL_BAD_PARAMETER, 0 );
	}

	image = (iEGLImage *)malloc( sizeof(iEGLImage) );
	memset( image, 0xFF, sizeof(iEGLImage) );

	if ( !image )
	EGL_ERROR( EGL_BAD_ALLOC, 0 );

	image->display = dpy;
	image->context = ctx;
	image->target  = target;
	image->buffer  = buffer;

	switch( target )
	{
	case EGL_NATIVE_PIXMAP_KHR:
		//eglCreatePixampImage( image->imageData, buffer );
		break;
	case EGL_VG_PARENT_IMAGE_KHR:
		//eglVGCreateImageKHR( iamge->imageData, buffer );
		break;
	case EGL_GL_TEXTURE_2D_KHR:
		//eglGLESCreate2DTexImageKHR( iamge->imageData, buffer );
		break;
	case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR:
		//eglVGCreateCubePosiXTexImageKHR( iamge->imageData, buffer );
		break;
	case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR:
		//eglVGCreateCubeNegaXTexImageKHR( iamge->imageData, buffer );
		break;
	case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR:
		//eglVGCreateCubePosiYTexImageKHR( iamge->imageData, buffer );
		break;
	case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR:
		//eglVGCreateCubeNegaYTexImageKHR( iamge->imageData, buffer );
		break;
	case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR:
		//eglVGCreateCubePosiZTexImageKHR( iamge->imageData, buffer );
		break;
	case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR:
		//eglVGCreateCubeNegaZTexImageKHR( iamge->imageData, buffer );
		break;
	case EGL_GL_TEXTURE_3D_KHR:
		//eglVGCreate3DTexImageKHR( iamge->imageData, buffer );
		break;
	case EGL_GL_RENDERBUFFER_KHR:
		//eglVGCreateRenderBufferImageKHR( iamge->imageData, buffer );
		break;
	}

	EGL_ERROR( EGL_SUCCESS, image );
}

EGLBoolean eglDestroyImageKHR( EGLDisplay dpy, EGLImageKHR image)
{
	iEGLImage    *iImage = (iEGLImage*)image;
	EGLint        i;
	EGLint        num_display;

	acquireMutex();

	if ( !_egl.init )
	EGL_ERROR( EGL_NOT_INITIALIZED, EGL_FALSE );

	num_display = _egl.num_display;

	for ( i = 0; i < num_display; i++ )
	if ( _egl.displays[i] == dpy )
	break;

	if ( i == num_display )
	EGL_ERROR( EGL_BAD_DISPLAY, EGL_FALSE );

	if( iImage->display != dpy )
	EGL_ERROR( EGL_BAD_MATCH, EGL_FALSE );

	if( !iImage )
	EGL_ERROR( EGL_BAD_PARAMETER, EGL_FALSE );

	PLATFORM_FREE( iImage );

	EGL_ERROR( EGL_SUCCESS, EGL_TRUE );
}

void (*eglGetProcAddress(const char *procname)) (void)
{
	acquireMutex();

	if(!strcmp( procname, "eglLockSurfaceKHR"))
	EGL_ERROR( EGL_SUCCESS, (__eglMustCastToProperFunctionPointerType)eglLockSurfaceKHR );

	if(!strcmp( procname, "eglUnlockSurfaceKHR"))
	EGL_ERROR( EGL_SUCCESS, (__eglMustCastToProperFunctionPointerType)eglUnlockSurfaceKHR );

	if(!strcmp( procname, "eglCreateImageKHR"))
	EGL_ERROR( EGL_SUCCESS, (__eglMustCastToProperFunctionPointerType)eglCreateImageKHR );

	if(!strcmp( procname, "eglDestroyImageKHR"))
	EGL_ERROR( EGL_SUCCESS, (__eglMustCastToProperFunctionPointerType)eglDestroyImageKHR );

	if(!strcmp( procname, "eglSetSwapRectangleANDROID"))
	EGL_ERROR( EGL_SUCCESS, (__eglMustCastToProperFunctionPointerType)eglSetSwapRectangleANDROID);

	if(!strcmp( procname, "eglGetRenderBufferANDROID"))
	EGL_ERROR( EGL_SUCCESS, (__eglMustCastToProperFunctionPointerType)eglGetRenderBufferANDROID);

	EGL_ERROR( EGL_SUCCESS, (__eglMustCastToProperFunctionPointerType)EGL_FALSE );
}
