

#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


#include "gl.h"
#include "ChunkAlloc.h"
#include "glState.h"
#include "register.h"
#include "macros.h"
#include "fgl.h"
#include "glprof.h"

#include "platform.h"
#include <linux/android_pmem.h>

#include <cutils/log.h>


#ifdef PLATFORM_S3C6410
#define FIMG_PHY_BASE           0x72000000
#define FIMG_PHY_SIZE           0x90000
#else
#ifdef PLATFORM_S5PC100
#define FIMG_PHY_BASE           0xEF000000
#define FIMG_PHY_SIZE           0x90000
#else

#ifdef PLATFORM_S5P6442

#define FIMG_PHY_BASE           0xD8000000
#define FIMG_PHY_SIZE           0x90000

#else

#error Unknown hardware platform
#endif // PLATFORM_S5PC100
#endif // PLATFORM_S3C6410
#endif // PLATFORM_S5P6442

// Pool memory
AddressBase gPoolMem = {
	NULL, NULL
};
size_t gPoolMem_size = 0;

// FIMG registers
AddressBase gFimgBase = {
	(void*)FIMG_PHY_BASE, NULL
};


BufferManager *pCA;

const char memdev_desc[] = "/dev/mem";
int memdev;

int *dma_buffer_virtual;
int dummyIntrFd=-1;

#ifdef MULTI_CONTEXT
pthread_once_t gles2InitControl = PTHREAD_ONCE_INIT;
#ifndef FIMG_GLES_11
pthread_key_t tls_context_key20 = 0;
#endif
#endif

static int GLES2InitdriverResult = GL_TRUE;


extern "C" void gfFlipColorBuffers(GLES2Context _ctx);

static bool isGLInitialized;

static pthread_mutex_t mutex;
static pthread_once_t once_mutex_init = PTHREAD_ONCE_INIT;

static void initMutex(void)
{
	EGLint ret;
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&mutex, &attr);
	pthread_mutexattr_destroy(&attr);
}

static void acquireMutex(void)
{
	pthread_once(&once_mutex_init, initMutex);
	pthread_mutex_lock(&mutex);
}

static void releaseMutex(void)
{
	pthread_mutex_unlock(&mutex);
}

#ifdef USE_3D_PM
void PM_InitFimg (void);

extern "C" void InitFimg (void)
{
	lock3DCriticalSection();
	PM_InitFimg();
	unlock3DCriticalSection();
}

#include <cutils/log.h>

void PM_InitFimg (void)
#else
extern "C" void InitFimg (void)
#endif
{
	unsigned int GPU_version = 0;

	//fglGetVersion(&GPU_version);
	////LOGMSG("\n\n\n  *** FIMG VERSION : 0x%x ***   \n\n\n",GPU_version);

	// wait for pipeline flush . added by JJG
	fglFlush(FGL_PIPESTATE_ALL);

	fglGetVersion(&GPU_version);


	//LOGMSG("\n\n\n  *** FIMG VERSION : 0x%x ***   \n\n\n",GPU_version);

	fglSoftReset();

	WRITEREG(FGPF_STENCIL_DEPTH_MASK, 0);

	WRITEREG(FGRA_PIXEL_SAMPOS ,FGL_SAMPLE_CENTER);

}
static void __GLES2DeInitdriver(void);

static void __GLES2Initdriver(void){

	int retval;

	//-------------------------------------------------------------------------
	// Map FIMG SFRs
	//-------------------------------------------------------------------------

	memdev = open(memdev_desc, O_RDWR | O_SYNC);
	if (memdev < 0) {
		Plat::printf("Cannot open memory device (%s)\n", memdev_desc);
		GLES2InitdriverResult = GL_FALSE;
		return;
	}

	gFimgBase.vaddr = mmap( NULL, FIMG_PHY_SIZE,
	PROT_WRITE | PROT_READ, MAP_SHARED,
	memdev,
	(int) gFimgBase.paddr);

	if (gFimgBase.vaddr < 0) {
		Plat::printf("Unable to map fimg sfr\n");
		GLES2InitdriverResult = GL_FALSE;
		return;
	}

	Plat::printf("FIMG SFRs\n   fd\t%d\n   mem\t%p -> %p\n   size\t0x%x\n",
	memdev, gFimgBase.vaddr, gFimgBase.paddr, FIMG_PHY_SIZE);

//	pCA = BufferManager::GetInstance();
	pCA = new BufferManager();

	//-------------------------------------------------------------------------
	// Graphics hardware (FIMG) initialization
	//-------------------------------------------------------------------------

	Plat::lockGPUSFR(__FUNCTION__);
	//LOGMSG("InitFimg()\n");

	InitFimg();

	Plat::unlockGPUSFR(__FUNCTION__);

#ifdef MULTI_CONTEXT
	#ifndef FIMG_GLES_11
	pthread_key_create(&tls_context_key20, 0);
	#endif
#endif

	//atexit(__GLES2DeInitdriver);
}

static void __GLES2DeInitdriver(void)
{
	//    LOGE("__GLES2DeInitdriver");
	Plat::printf("Unmap FIMG SFRs, Frame buffer and Pool memory\n");
	
	LOGE("GLES: trying to delete allocator");
	if(pCA) delete pCA;
	pCA=NULL;
//	pCA->DestroyInstance();
	LOGE("GLES: allocator deleted OK");
	
	munmap((void*)gFimgBase.vaddr, FIMG_PHY_SIZE);

	Plat::printf("Close %s \n", memdev_desc);
	if(memdev)  close(memdev);
	memdev=0;

#ifdef MULTI_CONTEXT
	#ifndef FIMG_GLES_11
	pthread_key_delete(tls_context_key20);
	#endif
#endif

}

extern "C" GLboolean GLES2Initdriver(int g3dfd)
{
	// initialize file descriptor for g3d driver. added by JJG
	dummyIntrFd = g3dfd;
#ifdef MULTI_CONTEXT

	acquireMutex();

	if (!isGLInitialized)
	{
		__GLES2Initdriver();
	}

	isGLInitialized = true;

	releaseMutex();

#else
	static bool GLES2initialized = false;
	if(!GLES2initialized){
		__GLES2Initdriver();
		GLES2initialized = true;
	}
#endif

	return GLES2InitdriverResult;
}

extern "C" GLboolean GLES2DeInitdriver(void)
{
#ifdef MULTI_CONTEXT
	#ifndef FIMG_GLES_11
	pthread_setspecific(tls_context_key20, NULL);
	#else

	acquireMutex();

	if (isGLInitialized)
	{
		__GLES2DeInitdriver();
	}

	isGLInitialized = false;

	releaseMutex();

#endif
#endif

	return GL_TRUE;
}

#ifdef CACHE_MEM
/****************************************************************************
* Function : clean_invalidate_cached_buffer
*
* Inputs:
*
*
* Outputs:
*
* Description:
*
* Note : created 2009.06.19 by zepplin
****************************************************************************/
void clean_invalidate_cached_buffer(void)
{
	GET_GL_STATE(ctx);
	FramebufferData *fbData=NULL;
	unsigned int pixSize;

	fbData = &ctx->defFBData;
	pixSize = pixelSize(fbData->nativeColorFormat);

	pCA->cache_clean_invalid(fbData->colorAddr.vaddr, fbData->width * fbData->height * pixSize);

}

void clean_cached_buffer(void)
{
	GET_GL_STATE(ctx);
	FramebufferData *fbData=NULL;
	unsigned int pixSize;

	fbData = &ctx->defFBData;
	pixSize = pixelSize(fbData->nativeColorFormat);

	pCA->cache_clean(fbData->colorAddr.vaddr, fbData->width * fbData->height * pixSize);

}

void invalidate_cached_buffer(void)
{
	GET_GL_STATE(ctx);
	FramebufferData *fbData=NULL;
	unsigned int pixSize;

	fbData = &ctx->defFBData;
	pixSize = pixelSize(fbData->nativeColorFormat);

	pCA->cache_invalid(fbData->colorAddr.vaddr, fbData->width * fbData->height * pixSize);

}

#endif


extern "C"  BufferHandle BufferAlloc(GLsizei size)
{
	LOGE("GLES: BufferAlloc called");
	ChunkHandle* chunk = pCA->New(size);

	return (void *)chunk;
}

extern "C"  GLboolean BufferFree(BufferHandle handle)
{
	LOGE("GLES: BufferFree called");
	ChunkHandle* chunk = (ChunkHandle*)handle;
	pCA->Free(chunk);
	return GL_TRUE;
}

extern "C"  BufferAddress GetBufferAddress(BufferHandle handle)
{
	LOGE("GLES: GetBufferAddress start");
	ChunkHandle* chunk = (ChunkHandle*)handle;

	BufferAddress addr ; // (BufferAddress)Plat::malloc(sizeof(AddressBase));
	LOGE("GLES: BufferAddress addr");
	addr.paddr = chunk->GetPhyAddr();
	addr.vaddr = chunk->GetVirtAddr();
	LOGE("GLES: GetBufferAddress end");
	
	return addr;
}