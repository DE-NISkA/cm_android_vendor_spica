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
* \file         platform.h
* \author       Prashant Singh (prashant@samsung.com),
*                       Sandeep Kakarlapudi (s.kakarla@samsung.com)
* \brief        Platform specific functionality
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*******************************************************************************
*/

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

/*
*******************************************************************************
* Includes
*******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#ifdef linux
#include <stdarg.h>
#ifdef MULTI_CONTEXT
#include <pthread.h>
#endif

#include <cutils/log.h>
#include <KD/kd.h>
#include <KD/KHR_formatted.h>
#include <GPI/gpi.h>

#ifdef MULTI_PROCESS
#include <sys/ioctl.h>
#define G3D_IOCTL_MAGIC			        'S' 
#define S3C_3D_SFR_LOCK                 _IO(G3D_IOCTL_MAGIC, 0) 
#define S3C_3D_SFR_UNLOCK               _IO(G3D_IOCTL_MAGIC, 1) 
#endif

#endif

///////////////////////////////////////////////////////////
#include <sys/time.h>
//#include <cutils/log.h>
//struct timeval __start;
//struct timeval __end;

static inline double  __test_tval(struct timeval _tstart,struct timeval _tend)
{
	double t1 = 0.;
	double t2 = 0.;

	t1 = ((double)_tstart.tv_sec * 1000 + (double)_tstart.tv_usec/1000.0) ;
	t2 = ((double)_tend.tv_sec * 1000 + (double)_tend.tv_usec/1000.0) ;

	return t2-t1;
}

///////////////////////////////////////////////////////////

/*
*******************************************************************************
* Macro definitions and enumerations
*******************************************************************************
*/

//#define SHARED_CONTEXT_DEBUG

#define thread_debug 0

#ifdef MULTI_CONTEXT    
#ifdef linux
typedef    pthread_mutex_t  mutex_t;
#endif
#else
typedef int  mutex_t;
#endif

/*
*******************************************************************************
* Global Variable Declarations
*******************************************************************************
*/
extern mutex_t gles20_fimgsfr_mutex;
extern mutex_t gles20_chunkalloc_mutex;
//extern int g3d_fd;

extern "C" int get_g3d_fd();


/*
*******************************************************************************
* Type, Structure & Class Definitions
*******************************************************************************
*/

typedef unsigned int size_t;

#ifdef WIN32
#define gAssert assert
#else
#define gAssert( _expr ) (void)( (_expr) ? 1 : (Plat::printf(">>ASSERTION FAILED: %s \n",#_expr)))
#endif


#ifndef __DEBUG                 
#define LOGMSG(x, ...)          
#else
#define LOGMSG          Plat::printf
#endif

//! Platform class encapsulating all platform specific functions
class Plat {
public:
	template <typename T>
	inline static T min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	inline static T max(const T& a, const T&b)
	{
		return a > b ? a : b;
	}
	template <typename T>
	inline static T clamp(const T& x, const T& a, const T& b)
	{
		return Plat::min(Plat::max(a,x), b);
	}

	inline static void* malloc (size_t size )
	{
		return ::malloc(size);
	}

	inline static void* realloc (void* ptr , size_t size )
	{
		return ::realloc(ptr , size);
	}

	template <typename T>
	inline static void safe_free (T& ptr)
	{
		if(ptr) {
			::free((void*)(ptr));
			ptr = 0;
		}
	}

	inline static void* memset (void* dst, int c, size_t num)
	{
		return ::memset(dst, c, num);
	}


	inline static void* memcpy (void* dst, const void* src, size_t num)
	{
		return ::memcpy(dst, src, num);
	}

	inline static void* memcpy_FIMG (void* dst, const void* src, size_t num)
	{
#ifndef FIMG_VERA_SIM
		return ::memcpy(dst, src, num);
#else
		return memcpy_RTLSIM(dst, src, num);
#endif
	}

	inline static int memcmp(const void* buf1, const void* buf2, size_t size)
	{
		return ::memcmp(buf1, buf2, size);
	}


	
	inline static int printf(const char* format, ...)
	{		
		char buffer[256];
		va_list args;
		va_start(args, format);
		int res = 0;
#ifndef __DEBUG                 
		while(0)
#endif
		{
			res = vsprintf(buffer,format, args);
			LOGE(buffer);
		}

		va_end(args);   

		return res;
	}

	static void memset_long(void* dst, unsigned int c, size_t num, void* phy = 0);

	static void *memcpy_RTLSIM(void* dst, const void* src, size_t num);

	inline static void memset_short(void* dst, unsigned short c, size_t num, void* phy = 0)
	{
		unsigned int c32 = (c <<16) | c;
		unsigned short *dst_short = (unsigned short*)(dst);

		if((unsigned int)dst & 0x3){
			*dst_short = c;
			dst_short++; 
			num--;
		} 

		memset_long((void *)dst_short, c32, num/2, phy);

		if(num%2)
		{
			dst_short += (num - 1);
			*(dst_short) = c;
		}

	}
	
	inline static void memset_short(void* dst, unsigned int c, size_t num, unsigned int mask, void* phy = 0)
	{
		unsigned int c32 = (c <<16) | c;
		unsigned int mask32 = (mask<<16)|mask;
		unsigned short *dst_short = (unsigned short*)(dst);

		if((unsigned int)dst & 0x3){
			*dst_short = (c & mask) | (*(dst_short) & ~mask);
			dst_short++; 
			num--;
		} 

		memset_long((void *)dst_short, c32, num/2, mask32, phy);

		if(num%2)
		{
			dst_short += (num - 1);
			*(dst_short) = (c & mask) | (*(dst_short) & ~mask);
		}
	}

	static void memset_long(void* dst, unsigned int c, size_t num, unsigned int mask, void* phy = 0);

	static void lock( mutex_t* mutex , const char * func_name);

	static void unlock( mutex_t* mutex , const char * func_name);
	
	static inline void lockGPUSFR( const char* func_name)
	{
//		struct timeval __start;
//		struct timeval __end;
//		LOGE("lockGPUSFR called in %s ", func_name);
//		gettimeofday(&__start,NULL);/////////////////////////////

#ifdef MULTI_PROCESS        
		if( ioctl(get_g3d_fd(), S3C_3D_SFR_LOCK) ) {
			LOGMSG("ERROR: unable to aquire GPU SFR lock\n");
		}
#endif        
//		gettimeofday(&__end,NULL);///////////////////////////
//		LOGE("lockGPUSFR in %s completed in: %5.5f ms",func_name,__test_tval(__start,__end));
		lock(&gles20_fimgsfr_mutex, func_name);

	}
	
	static inline void unlockGPUSFR( const char* func_name)
	{

		unlock(&gles20_fimgsfr_mutex, func_name);
#ifdef MULTI_PROCESS        
		if( ioctl(get_g3d_fd(), S3C_3D_SFR_UNLOCK) ) {
			LOGMSG("ERROR: unable to release GPU SFR lock\n");
		}
#endif        
	}

	static void  initMutex(  mutex_t* mutex  ,  const char * func_name);
	static void  DeinitMutex(  mutex_t* mutex);
};


/*
*******************************************************************************
* Function Declarations
*******************************************************************************
*/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_3D_PM
	int fimgPowerStatus(void);
	int lock3DCriticalSection(void);
	int unlock3DCriticalSection(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /*__PLATFORM_H__*/
