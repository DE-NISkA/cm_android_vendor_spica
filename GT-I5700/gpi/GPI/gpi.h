/**
 * Samsung Project
 * Copyright (c) 2009 Software Solution, Samsung Electronics, Inc.
 * All right reserved.
 *
 * This software is the confidential and proprietary information
 * of Samsung Electronics Inc. ("Confidential Information"). You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung Electronics.
 */

/**
 * @file        gpi.h
 * @brief       This is the header file for Graphic Platform Interface.
 * @author      Imbum Oh
 * @version     1.0
 */

#ifndef __GPI_H__
#define __GPI_H__
        

#if defined __cplusplus
extern "C" {
#endif

#include <KD/kd.h>
#include <KD/KHR_formatted.h>
#include <stdarg.h>
#include <fcntl.h> /* for O_RDWR */
#include <unistd.h> /* for open/close .. */
#include <sys/ioctl.h> /* for ioctl */
#include <sys/mman.h>   

#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fb.h>

#include <sys/time.h>
#include <signal.h>

/****************************************************************************
 *  Typedefs
 ****************************************************************************/
typedef int GPI_Error; 

/* handles */
typedef void* GPI_Handle;
typedef int GPI_DrvHandle;

/****************************************************************************
 *  Defines
 ****************************************************************************/
#define GPI_SUCCESS  0
#define GPI_FAIL    (-1)

#define GPI_NULL        ((void*)0)
#define GPI_INVALID_HANDLE ((void*)0)

/* null pointer */
#define GPI_NULL ((void*)0)
/****************************************************************************
 *  Macros
 ****************************************************************************/
#define         GPILOG          kdLogMessagefKHR

/****************************************************************************
 *  Enumerated types
 ****************************************************************************/

/****************************************************************************
 *  EXPORT VARIABLES
 ****************************************************************************/
extern GPI_DrvHandle g_g3d_fd;


/****************************************************************************
 *  EXPORT FUNCTIONS
 ****************************************************************************/

/***************************************************************************** 
 * 
 *  Driver Initializer/Deinitializer API
 * 
 *****************************************************************************/
GPI_Error GPI_OpenDriver();

GPI_Error GPI_CloseDriver();

GPI_Error GPI_InitDriver(void** outFIMGvirt, void** outFIMGphy);

GPI_Error GPI_DeinitDriver();

/***************************************************************************** 
 * 
 *  Memory Block Management API
 * 
 *****************************************************************************/
GPI_Error GPI_AllocMemBlock( unsigned int szBlock, void** pVirtaddr, void** pPhyaddr);

GPI_Error GPI_FreeMemBlock(void* virtaddr, void* phyaddr);

/* GPI_MemCpy: copy a memory region by its physical address */
GPI_Error GPI_MemCpy(void* srcVirtualAddr, void* srcPhysicalAddr, void* dstVirtualAddr, void* dstPhysicalAddr, unsigned int size);

/* GPI_AllocateMemory: allocate physical contiguos memory */
GPI_Handle GPI_AllocateMemory(unsigned int szBlock, unsigned int *pszAlloced);

/* GPI_GetMemoryAddress: obtaining virtual and physical addresses of memory allocated by GPI_AllocateMemory */
GPI_Error GPI_GetMemoryAddress(GPI_Handle handle, void **virtualAddr, void **physicalAddr);

/* GPI_FreeMemory: freeing of memory allocated by GPI_AllocateMemory */
GPI_Error GPI_FreeMemory(GPI_Handle handle);

/* GPI_LoadLibrary: load a shared library */
GPI_Handle GPI_LoadLibrary(const char* name);

/* GPI_LoadLibrary: obtaining the address of a symbol from given shared library */
GPI_Handle GPI_GetProcAddress(GPI_Handle handle, const char* name);

/* GPI_LoadLibrary: freeing a library loaded by GPI_LoadLibrary */
GPI_Error GPI_FreeLibrary(GPI_Handle handle);

GPI_Error GPI_cache_invalid(void *addr, unsigned int size);
GPI_Error GPI_cache_clean(void *addr, unsigned int size);
GPI_Error GPI_cache_clean_invalid(void *addr, unsigned int size);

/***************************************************************************** 
 * 
 *  Lock/Unlock API
 * 
 *****************************************************************************/
GPI_Error GPI_LockGPUSFR();

GPI_Error GPI_UnlockGPUSFR();

/***************************************************************************** 
 * 
 *  MISC API
 * 
 *****************************************************************************/
GPI_Error GPI_WaitForFlushInterrupt(unsigned int pipelineFlags);



#if defined __cplusplus
}
#endif

#endif  /* __GPI_H__ */

