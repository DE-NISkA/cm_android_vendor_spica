/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2009 Samsung Inc. All Rights Reserved.                 *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : gpi_memalloc_linux.c                                            *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      * 
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#include <KD/kd.h>
#include <KD/KHR_formatted.h>

#include <GPI/gpi.h>

#include <errno.h>
#include <sys/ioctl.h> 
#include <memory.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

struct s3c_3d_mem_alloc
{
        int                      size;
        unsigned int vir_addr;
        unsigned int phy_addr;
};

#define S3C_3D_MEM_ALLOC                _IOWR('S', 310, struct s3c_3d_mem_alloc)
#define S3C_3D_MEM_FREE                 _IOWR('S', 311, struct s3c_3d_mem_alloc)
#define S3C_3D_CACHE_INVALID  _IOWR('S', 316, struct s3c_3d_mem_alloc)
#define S3C_3D_CACHE_CLEAN    _IOWR('S', 317, struct s3c_3d_mem_alloc)
#define S3C_3D_CACHE_CLEAN_INVALID    _IOWR('S', 318, struct s3c_3d_mem_alloc)

extern GPI_DrvHandle g_g3d_fd;

/* GPI_AllocateMemory: allocate physical contiguos memory */
GPI_Handle GPI_AllocateMemory(unsigned int szBlock, unsigned int *pszAlloced)
{

        *pszAlloced = 0;
        if (g_g3d_fd < 0)
        {
                if(GPI_OpenDriver() == GPI_FAIL)
                {
                        GPILOG("Fail to open g3d driver: %s\n", strerror(errno));
                        return GPI_INVALID_HANDLE;
                }
        }

        if(szBlock == 0) 
        {
                GPILOG("Received an alloc request of size 0, 4 bytes will be allocated\n");
                szBlock = 4;
        }
    
        s3c_3d_mem_alloc *desc = (s3c_3d_mem_alloc*)kdMalloc(sizeof(s3c_3d_mem_alloc));
        desc->size = szBlock;
        desc->vir_addr = 0;
        desc->phy_addr = 0;

        if (ioctl(g_g3d_fd, S3C_3D_MEM_ALLOC, desc))
        {
                GPILOG("Unable to allocate %d bytes\n", szBlock);
                kdFree(desc);
                return GPI_INVALID_HANDLE;
        }

        if (desc->vir_addr == 0)
        {
                kdFree(desc);
                return GPI_INVALID_HANDLE;
        }

        *pszAlloced = desc->size;
        return desc;
}

/* GPI_FreeMemory: freeing of memory allocated by GPI_AllocateMemory */
GPI_Error GPI_FreeMemory(GPI_Handle handle)
{
        s3c_3d_mem_alloc *desc = static_cast<s3c_3d_mem_alloc*>(handle);
        if (ioctl(g_g3d_fd, S3C_3D_MEM_FREE, desc)) 
        {
                kdFree(desc);
                return GPI_FAIL;
        }

        kdFree(desc);

        return GPI_SUCCESS;
}

/* GPI_GetMemoryAddress: obtaining virtual and physical addresses of memory allocated by GPI_AllocateMemory */
GPI_Error GPI_GetMemoryAddress(GPI_Handle handle, void **virtualAddr, void **physicalAddr)
{
        s3c_3d_mem_alloc *desc = static_cast<s3c_3d_mem_alloc*>(handle);

        if (desc == 0)
        {
            return GPI_FAIL;
        }

        *virtualAddr = reinterpret_cast<void*>(desc->vir_addr);
        *physicalAddr = reinterpret_cast<void*>(desc->phy_addr);        

        return GPI_SUCCESS;
}

GPI_Error GPI_cache_invalid(void *addr, unsigned int size)
{
    struct s3c_3d_mem_alloc alloc_desc; 
    alloc_desc.size = size;
    alloc_desc.vir_addr = (unsigned int)addr;
    alloc_desc.phy_addr = 0;

    if( ioctl(g_g3d_fd, S3C_3D_CACHE_INVALID, &alloc_desc) ) {
        GPILOG("BufferManager unable to invaild cache addr = 0x%x, size = %d\n", (unsigned int)addr, size);
        return GPI_FAIL;
    }

        return GPI_SUCCESS;
}

GPI_Error GPI_cache_clean(void *addr, unsigned int size)
{
    struct s3c_3d_mem_alloc alloc_desc; 
    alloc_desc.size = size;
    alloc_desc.vir_addr = (unsigned int)addr;
    alloc_desc.phy_addr = 0;
    
    if( ioctl(g_g3d_fd, S3C_3D_CACHE_CLEAN, &alloc_desc) ) {
        GPILOG("BufferManager unable to clean cache addr = 0x%x, size = %d\n", (unsigned int)addr, size);
        return GPI_FAIL;
    }

        return GPI_SUCCESS;
}

GPI_Error GPI_cache_clean_invalid(void *addr, unsigned int size)
{
    struct s3c_3d_mem_alloc alloc_desc; 
    alloc_desc.size = size;
    alloc_desc.vir_addr = (unsigned int)addr;
    alloc_desc.phy_addr = 0;
    
    if( ioctl(g_g3d_fd, S3C_3D_CACHE_CLEAN_INVALID, &alloc_desc) ) {
        GPILOG("BufferManager unable to clean and invalid cache addr = 0x%x, size = %d\n", (unsigned int)addr, size);
        return GPI_FAIL;
    }

        return GPI_SUCCESS;
}

