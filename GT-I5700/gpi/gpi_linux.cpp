/**
 * Samsung Project
 * Copyright (c) 2008 Mobile Solution, Samsung Electronics, Inc.
 * All right reserved.
 *
 * This software is the confidential and proprietary information
 * of Samsung Electronics Inc. ("Confidential Information"). You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung Electronics.
 */

/**
 * @file        GPI_Wince.cpp
 * @brief       Graphic Platform Interface for Wince
 * @author      Imbum Oh
 * @version     1.0
 */

/* Includes */

#include <stdio.h>
#include <GPI/gpi.h>

#include <sys/ioctl.h>


/* Macros */
#define INTR_DEVFILE "/dev/s3c-g3d"

//#define PLATFORM_S3C6410

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

/* Type, Structure & Class Definitions */

#define S3C_3D_SFR_LOCK                 _IO('S', 312)
#define S3C_3D_SFR_UNLOCK               _IO('S', 313)

#define WAIT_FOR_FLUSH                  _IO('S', 100)
#define GET_CONFIG                      _IO('S', 101)

struct s3c_3d_mem_alloc {
    int      size;
    unsigned int vir_addr;
    unsigned int phy_addr;
};

typedef struct{
    unsigned int FIMGAddr;
    unsigned int FIMGSize;
} G3D_CONFIG_STRUCT;


/* Global Variables */
GPI_DrvHandle g_g3d_fd = -1;
static const char memdev_desc[] = "/dev/mem";
static int memdev;
static G3D_CONFIG_STRUCT g3d_config;
void    *g_FIMGvaddr;
void    *g_FIMGpaddr;

/* Local Function Declarations */

/* Function Definitions */
//============================================================================
// Driver initializer/Deinitializer
//============================================================================

//-------------------------------------------------------------------------
//  Function name - GPI_OpenDriver
//           Open Driver.
//  Parameter -
//-------------------------------------------------------------------------
GPI_Error GPI_OpenDriver()
{
    g_g3d_fd = open(INTR_DEVFILE,O_RDWR);
    if(g_g3d_fd<0)
    {
        GPILOG("fail to open g3d driver\n");
        return GPI_FAIL;
    }
    return GPI_SUCCESS;
}

//-------------------------------------------------------------------------
//  Function name - GPI_CloseDriver
//           Close Driver.
//  Parameter -
//-------------------------------------------------------------------------

GPI_Error GPI_CloseDriver()
{
    if (g_g3d_fd != -1)
    {
        if (close(g_g3d_fd) != 0)
          GPILOG("fail to close g3d driver\n");
        g_g3d_fd = -1;
     };
    return GPI_SUCCESS;
}

//-------------------------------------------------------------------------
//  Function name - GPI_InitDriver
//           Mapping FIMG to virtual address.
//  Parameter -
//    outFIMGVirt : /output/ virtual address of FIMG which is mapped.
//    outFIMGPhy  : /output/ physical address of FIMG
//-------------------------------------------------------------------------

GPI_Error GPI_InitDriver(void** outFIMGvirt, void** outFIMGphy)
{
    int retval;

    if(g_g3d_fd < 0)
    {
      if(GPI_OpenDriver() == GPI_FAIL)
        return GPI_FAIL;
    }

    g_FIMGpaddr = (void*)FIMG_PHY_BASE;//(void*)g3d_config.FIMGAddr;

    memdev = open(memdev_desc, O_RDWR | O_SYNC);
    if (memdev < 0) {
        GPILOG("Cannot open memory device (%s)\n", memdev_desc);
        return GPI_FAIL;
    }

    g_FIMGvaddr = mmap( NULL, FIMG_PHY_SIZE,
                    PROT_WRITE | PROT_READ, MAP_SHARED,
                    memdev,
                    (int) g_FIMGpaddr);

    if (g_FIMGvaddr < 0) {
        GPILOG("Unable to map fimg sfr\n");
        return GPI_FAIL;
    }

    *outFIMGvirt = g_FIMGvaddr;
    *outFIMGphy = g_FIMGpaddr;
    return GPI_SUCCESS;
}


//-------------------------------------------------------------------------
//  Function name - GPI_DeinitDriver
//           Close Driver and unmapping FIMG.
//  Parameter -
//
//-------------------------------------------------------------------------

GPI_Error GPI_DeinitDriver()
{
    munmap((void*)g_FIMGvaddr, FIMG_PHY_SIZE);
    close(memdev);
    GPI_CloseDriver();
    return GPI_SUCCESS;
}


//============================================================================
// Lock / Unlock API
//============================================================================

//-------------------------------------------------------------------------
//  Function name - GPI_LockGPUSFR
//           Lock SFR for GPU. This prevents from other processes and threads
//           Only this process can access SFR
//
//  Parameter -
//-------------------------------------------------------------------------
GPI_Error GPI_LockGPUSFR()
{
    if( ioctl(g_g3d_fd, S3C_3D_SFR_LOCK) ) {
        GPILOG("ERROR: unable to aquire GPU SFR lock\n");
        return GPI_FAIL;
    }
    return GPI_SUCCESS;
}

//-------------------------------------------------------------------------
//  Function name - GPI_UnlockGPUSFR
//           Unlock SFR for GPU.
//
//  Parameter -
//-------------------------------------------------------------------------
GPI_Error GPI_UnlockGPUSFR()
{
    if( ioctl(g_g3d_fd, S3C_3D_SFR_UNLOCK) ) {
        GPILOG("ERROR: unable to release GPU SFR lock\n");
        return GPI_FAIL;
    }
    return GPI_SUCCESS;
}

//============================================================================
// MISC API
//============================================================================

//-------------------------------------------------------------------------
//  Function name - GPI_WaitForFlushInterrupt
//           Wait for interrupt of pipeline status
//
//  Parameter -
//    pipelineflags : flags for checking status
//
//-------------------------------------------------------------------------
GPI_Error GPI_WaitForFlushInterrupt(unsigned int pipelineFlags)
{
    if( ioctl(g_g3d_fd, WAIT_FOR_FLUSH, pipelineFlags) )
    {
        GPILOG("ERROR: wait for interrupt error \n");
        return GPI_FAIL;
    }
    return GPI_SUCCESS;
}
