/*
*******************************************************************************
*
*                        SAMSUNG INDIA SOFTWARE OPERATIONS
*                               Copyright(C) 2006
*                              ALL RIGHTS RESERVED
*
* This program is proprietary  to Samsung India  Software Operations Pvt. Ltd.,
* and is protected under International Copyright Act as an unpublished work.Its
* use and disclosure is limited by the terms and conditions of a license agree-
* -ment. It may not be  copied or otherwise  reproduced or disclosed to persons
* outside the licensee's  organization except in accordance  with the terms and
* conditions of  such an agreement. All copies and  reproductions  shall be the
* property of  Samsung  India Software Operations Pvt. Ltd.  and must bear this
* notice in its entirety.
*
*******************************************************************************
*/

/*
***************************************************************************//*!
*
* \file   platform.cpp
* \author Prashant Singh (prashant@samsung.com),
*     Sandeep Kakarlapudi (s.kakarla@samsung.com)
* \brief  Platform specific functionality
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
* 22.08.2006  Prashant Singh      Draft implementation
*       Sandeep kakarlapudi
*
*
*******************************************************************************
*/

/*
*******************************************************************************
* Includes
*******************************************************************************
*/


#include "platform.h"
#include "string.h"
#include "stdlib.h"
#include "register.h"
#include "glState.h"
#include <cutils/log.h>

/*
*******************************************************************************
* Macro definitions and enumerations
*******************************************************************************
*/

/*
*******************************************************************************
* Type, Structure & Class Definitions
*******************************************************************************
*/

/*
*******************************************************************************
* Global Variables
*******************************************************************************
*/

mutex_t  gles20_fimgsfr_mutex;
mutex_t  gles20_chunkalloc_mutex;


/*
*******************************************************************************
* Local Function Declarations
*******************************************************************************
*/

/*
*******************************************************************************
* Function Definitions
*******************************************************************************
*/

void Plat::initMutex(  mutex_t*  mutex  ,  const char * func_name)
{

#ifdef MULTI_CONTEXT
	////LOGMSG("trying to lock fimg mutex in func: %s\n", func_name);
	pthread_mutex_init( mutex , NULL );
//	if(!pthread_mutex_init( mutex , NULL )) {
		//if(thread_debug)
		//LOGMSG("%s is init, thread id %ld",func_name,(long int)pthread_self());
//	}
//	else {
		//LOGMSG("func:  %s fimg mutex init failed\n", func_name);
//	}

#endif

}

void Plat::DeinitMutex(  mutex_t*  mutex )
{

#ifdef MULTI_CONTEXT
	////LOGMSG("trying to lock fimg mutex in func: %s\n", func_name);
	//  kdThreadMutexFree(*mutex);

#endif

}



void Plat::lock( mutex_t*  mutex , const char * func_name)
{
#ifdef MULTI_CONTEXT


	////LOGMSG("trying to lock fimg mutex in func: %s\n", func_name);
	if(!pthread_mutex_lock( mutex )) {
		if(thread_debug)
		fprintf(stderr,"%s is locked, thread id %ld \n",func_name,(long int)pthread_self());
	}
	else {
		fprintf(stderr,"func:  %s mutex locking failed\n", func_name);
	}
#endif

}

void Plat::unlock( mutex_t*  mutex , const char * func_name)
{
#ifdef MULTI_CONTEXT

	//Plat::printf("trying to unlock fimg mutex in func: %s\n", func_name);
	pthread_mutex_unlock( mutex );
//	if(!pthread_mutex_unlock( mutex )) {
//		if(thread_debug)
		//LOGMSG("%s is unlocked, thread id %ld",func_name,(long int)kdThreadSelf());

//	}else
//	{
		//LOGMSG(" func: %s  mutex unlocking failed\n", func_name);
//	}

#endif
}

#define S3C_FB_START_MEMSET _IOWR('M', 319, struct s3c_mem_dma_param)

struct s3c_mem_dma_param {
	int size;
	unsigned int src_addr;
	unsigned int dst_addr;
	int cfg;
};

#define FB0_NAME    "/dev/s3c-mem"

#include <fcntl.h>

int memset_dma(unsigned long* dst, unsigned int c, size_t num, void* phy)
{
	// all bytes already aligned to 4 bytes boundary
	if ((num % 8) != 0)
	{
		*dst = c;
		dst++;
		num -= 4;
	}

	int dev_fp = open(FB0_NAME, O_RDWR);

	if (dev_fp < 0) {
		perror(FB0_NAME);
		return -1;
	}

	PtrChunkH mem = pCA->New(8);

	*((unsigned int*)mem->GetVirtAddr()) = c;
	*((unsigned int*)mem->GetVirtAddr()+1) = c;

	s3c_mem_dma_param dparam;
	dparam.size = num;
	dparam.src_addr = (unsigned int)mem->GetPhyAddr();
	dparam.dst_addr = (unsigned int)phy;

	ioctl(dev_fp, S3C_FB_START_MEMSET, &dparam);

	pCA->Free(mem);

	close(dev_fp);

	return 0;
}

void
Plat::memset_long(void* _dst, unsigned int c, size_t num, void* phy)
{
#ifdef PLATFORM_S5PC100

	if (phy != 0)
	{
		memset_dma((unsigned long*)_dst, c, 4 * num, phy);
		return;
	}

#endif // PLATFORM_S5PC100

#ifndef WIN32
	unsigned int *dst = (unsigned int*)(_dst);

	//  //LOGMSG("memset_long(%p, %x, %d)\n", dst, c, num);

	// Put dst on 8word (32byte) alignment
	while((((unsigned int)dst) % (4*8) != 0) && (num != 0)) {
		*dst = c;
		dst++; num--;
	}
#ifdef linux
	// Main set loop in assembly copies 8 words per cycle
	__asm__ __volatile__ (

#if 0
	"   ldm     %4, {r0-r7}     \n\t"
#else
	// Set a register set of 8 words
	" mov   r0, %4      \n"
	" mov   r1, r0      \n"
	" mov   r2, r0      \n"
	" mov   r3, r0      \n"
	" mov   r4, r0      \n"
	" mov   r5, r0      \n"
	" mov   r6, r0      \n"
	" mov   r7, r0      \n"
#endif


	// Main loop to set values
	" 2:        \n"
	" subs  %3, %3, #8    \n"
	" stmgeia %2!,{r0-r7}   \n"
	" bge   2b    \n"
	" add %3, %3, #8    \n"

	: "=r"(dst),  // %0
	"=r"(num)   // %1

	: "0"(dst),   // %2
	"1"(num),   // %3
#if 0
	"r"(&c)   // %4
#else
	"r"(c)    // %4
#endif

	: "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
	);

	// Copy rest of them
	while(num > 0){
		*dst = c;
		dst++; num--;
	}

#else
	int temp = num % 8;
	num = num - temp;

	//calling asm function for memset
	_gl_asm_memset(dst, c, num);

	// Copy rest of them
	if(temp != 0)
	{////LOGMSG("after _testcpy %d %x %x \n",num,dst,c);
		dst = dst+num;
		while(temp > 0){
			*dst = c;
			dst++; temp--;
		}
	}
#endif
#endif
}


void
Plat::memset_long(void* _dst, unsigned int _c, size_t num, unsigned int mask, void* phy)
{
	if(mask == 0)
	return;

	if(mask == 0xFFFFFFFF)
	{
		memset_long(_dst, _c, num, phy);
		return;
	}


	unsigned int *dst = (unsigned int*)(_dst);
	unsigned int c = _c & mask;

	while(num > 0)
	{
		*dst = c | (*dst & ~mask);
		dst++; num--;
	}
}

void *
Plat::memcpy_RTLSIM(void* dst, const void* src, size_t num)
{
	for(unsigned int j = 0; j < (num >> 2) ; j++)
	//for(unsigned int j = 0; j < num / 4 ; j++)
	{
		WRITEREG(((unsigned int *)dst + j),
		((unsigned int *)src + j));
	}

	return dst;
}

extern "C" int get_g3d_fd()
{
	static int g3d_fd = -1;

	if (g3d_fd < 0)
	{
		g3d_fd = open("/dev/s3c-g3d",O_RDWR);
		if(g3d_fd<0)
		{
			LOGE("fail to open g3d driver\n");
		}
	}

	return g3d_fd;
}

// Alex: 2009.10.23

extern "C" int fimg_wait_for_flush(unsigned int pipelineFlags)
{

	if( ioctl(get_g3d_fd(), WAIT_FOR_FLUSH, pipelineFlags) ) 
	{
		return GPI_FAIL;
	}

	return GPI_SUCCESS;
}

#ifdef USE_3D_PM
//#define INTR_DEVFILE            "/dev/s3c-g3d"
#define LOCK_CRITICAL_SECTION     1
#define UNLOCK_CRITICAL_SECTION   0
#define S3C_3D_POWER_INIT         _IOWR('S', 321, struct s3c_3d_pm_status)
#define S3C_3D_CRITICAL_SECTION   _IOWR('S', 322, struct s3c_3d_pm_status)
#define S3C_3D_POWER_STATUS       _IOWR('S', 323, struct s3c_3d_pm_status)

struct s3c_3d_pm_status
{
	unsigned int criticalSection;
	int powerStatus;
	int reserved;
};

/****************************************************************************
* Function : fimgPowerInit
*
* Inputs:
*
*
* Outputs:
*
* Description:
*
* Note : created 2009.07.06 by zepplin
****************************************************************************/
int fimgPowerInit(void)
{
	struct s3c_3d_pm_status status_desc;
	status_desc.criticalSection = 0;
	status_desc.powerStatus = 0;
	status_desc.reserved = 0;

	if( ioctl(get_g3d_fd(), S3C_3D_POWER_INIT, &status_desc) )
	{
		LOGE("[3D][ERR]fimgPowerInit ioctl was not opened\n");
		/*error handling here*/
		return -1;
	}

	return 0;
}

/****************************************************************************
* Function : fimgPowerStatus
*
* Inputs:
*
*
* Outputs:
*
* Description:
*
* Note : created 2009.06.26 by zepplin
****************************************************************************/
extern "C" int fimgPowerStatus(void)
{
	struct s3c_3d_pm_status status_desc;
	status_desc.criticalSection = 0;
	status_desc.powerStatus = 0;
	status_desc.reserved = 0;

	if( ioctl(get_g3d_fd(), S3C_3D_POWER_STATUS, &status_desc) )
	{
		LOGE("[3D][ERR]fimgPowerStatus ioctl was not opened\n");
		/*add error handling code here*/
		return -1;
	}

	if(status_desc.powerStatus==0)
	{
		/*power off
		add error handling core here & reset parameters
		*/
		return 0;
	}
	else
	{
		/*power on
		add extra parameter handling here
		*/
		return 1;
	}

	return 0;

}

/****************************************************************************
* Function : lock3DCriticalSection
*
* Inputs:
*
*
* Outputs:
*
* Description:
*
* Note : created 2009.06.26 by zepplin
****************************************************************************/
extern "C" int lock3DCriticalSection(void)
{
	struct s3c_3d_pm_status status_desc;
	status_desc.criticalSection = LOCK_CRITICAL_SECTION;
	status_desc.powerStatus = 0;
	status_desc.reserved = 0;

	if( ioctl(get_g3d_fd(), S3C_3D_CRITICAL_SECTION,&status_desc) )
	{
		LOGE("[3D][ERR]lock3DCriticalSection ioctl was not opened\n");
		/*add error handling code here*/
		return -1;
	}

	return 0;
}

/****************************************************************************
* Function : unlock3DCriticalSection
*
* Inputs:
*
*
* Outputs:
*
* Description:
*
* Note : created 2009.06.26 by zepplin
****************************************************************************/
extern "C" int unlock3DCriticalSection(void)
{
	struct s3c_3d_pm_status status_desc;
	status_desc.criticalSection = UNLOCK_CRITICAL_SECTION;
	status_desc.powerStatus = 0;
	status_desc.reserved = 0;

	if( ioctl(get_g3d_fd(), S3C_3D_CRITICAL_SECTION, &status_desc) )
	{
		LOGE("[3D][ERR]unlock3DCriticalSection ioctl was not opened\n");
		/*add error handling code here*/
		return -1;
	}

	return 0;
}

#endif
