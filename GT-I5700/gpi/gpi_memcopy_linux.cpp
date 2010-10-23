/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2009 Samsung Inc. All Rights Reserved.                 *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : gpi_memcopy_linux.c                                     *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      * 
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#include <KD/kd.h>

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h> 
#include <stdlib.h>
#include <unistd.h>
 
#include <GPI/gpi.h>

// hardware caps
typedef enum tagHardwareCaps
{
    HW_CAPS_DMA_IS_AVAILABLE = 0x1
} HardwareCaps;

// gets hardware caps for hardware
static int GetHardwareCaps()
{
    static int caps = 0;
    caps |= HW_CAPS_DMA_IS_AVAILABLE;
    return caps;
}

#define S3C_FB_START_MEMCPY     _IOWR('M', 318, struct s3c_mem_dma_param) 

struct s3c_mem_dma_param
{
        int size;
        unsigned int src_addr;
        unsigned int dst_addr;
        int cfg;
};

static int s_MemDriver = -1;

static void closeMemDriver()
{
    close(s_MemDriver);
}

static GPI_Error memcpy_dma(void* dstVirtualAddr, void* dstPhysicalAddr, void* srcVirtualAddr, void* srcPhysicalAddr, unsigned int size)
{
    if (s_MemDriver < 0)
    {
        s_MemDriver = open("/dev/s3c-mem", O_RDWR);
        if (s_MemDriver < 0)
        {
            kdLogMessage("Fail to open s3cmem driver\n");
            return GPI_FAIL;
        }

        atexit(closeMemDriver);
    }

    int bytesToCopyManually = size % 4;

        struct s3c_mem_dma_param dparam;
        dparam.size = size - bytesToCopyManually;
        dparam.dst_addr = (unsigned int)dstPhysicalAddr;    
        dparam.src_addr = (unsigned int)srcPhysicalAddr;

        if (ioctl(s_MemDriver, S3C_FB_START_MEMCPY, &dparam))
        {
        kdLogMessage("Copying of memory through DMA failed\n");
        return GPI_FAIL;
        }

    // copy the rest
    kdMemcpy(static_cast<char*>(dstVirtualAddr) + size - bytesToCopyManually, static_cast<char*>(srcVirtualAddr) + size - bytesToCopyManually, bytesToCopyManually);

    return GPI_SUCCESS;
}

/* GPI_MemCpy: copy a memory region by its physical address */
GPI_Error GPI_MemCpy(void* dstVirtualAddr, void* dstPhysicalAddr, void* srcVirtualAddr, void* srcPhysicalAddr, unsigned int size)
{
    if ((GetHardwareCaps() & HW_CAPS_DMA_IS_AVAILABLE) && dstPhysicalAddr != 0 && srcPhysicalAddr != 0)
    {
        return memcpy_dma(dstVirtualAddr, dstPhysicalAddr, srcVirtualAddr, srcPhysicalAddr, size);
    }
    
    kdMemcpy(dstVirtualAddr, srcVirtualAddr, size);
    return GPI_SUCCESS;
}

