/**
 * Samsung Project
 * Copyright (c) 2007 Mobile XG, Samsung Electronics, Inc.
 * All right reserved.
 *
 * This software is the confidential and proprietary information
 * of Samsung Electronics Inc. ("Confidential Information"). You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung Electronics.
 */

/**
 * @file        global.c
 * @brief       This is the global-register-setting file.
 * @author      Inho Lee
 * @version     1.5
 */

#include "fgl.h"
#include "register.h"
#include "glConfig.h"

#include <GPI/gpi.h>

#include <cutils/log.h>

#ifdef __DEBUG
#define _DEBUG printf
#else
#define _DEBUG (void)
#endif

/***************************************************************************** 
 * FUNCTIONS: fglGetPipelineStatus
 * SYNOPSIS: This function obtains status of the pipeline
 * PARAMETERS: [OUT] status: the pointer of FGL_PipelineStatus
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglGetPipelineStatus(FGL_PipelineStatus *status)
{
        unsigned int bits = 0;

        //bits = READREG(FGGB_PIPESTATE);
        READREGP(FGGB_PIPESTATE, bits);

        // [0]: host fifo 
        status->isNotEmptyHOSTFIFO = !(bits & FGL_PIPESTATE_HOSTFIFO);
        // [1]: host interface 
        status->isNotEmptyHI = !(bits & FGL_PIPESTATE_HI);
        // [2]: FIFO between host interface and vertex shader
        status->isNotEmptyHI2VS = !(bits & FGL_PIPESTATE_HI2VS);
        // [3]: vertex cache
        status->isNotEmptyVC = !(bits & FGL_PIPESTATE_VC);
        // [4]: vertex shader
        status->isNotEmptyVS = !(bits & FGL_PIPESTATE_VS);
        // [8]: primitive engine
        status->isNotEmptyPE = !(bits & FGL_PIPESTATE_PE);
        // [9]: triangle setup engine
        status->isNotEmptyTSE = !(bits & FGL_PIPESTATE_TSE);
        // [10]: raster engine
        status->isNotEmptyRA = !(bits & FGL_PIPESTATE_RA);
        // [12:13]: pixel shader
        status->isNotEmptyPS = !(bits & FGL_PIPESTATE_PS);
        // [16:17]: per-fragment
        status->isNotEmptyPF = !(bits & FGL_PIPESTATE_PF);

        return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglFlush
 * SYNOPSIS: This function flushes the fimg3d pipeline
 * PARAMETERS: [IN] pipelineFlags: Specified pipeline states are flushed
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/
#include <cutils/log.h>

extern int fimg_wait_for_flush(unsigned int pipelineFlags);

FGL_Error
fglFlush(unsigned int pipelineFlags)
{
	unsigned int data;
//	int ret;

//	#if 1

//	READREGP(FGGB_PIPESTATE, data);

//	if(data & pipelineFlags)
//	{

//		_DEBUG("interrupt mode !!!!!\n");
//		ret = fimg_wait_for_flush(pipelineFlags);
//		if(ret) LOGE("g3d Wait For Flush Interrupt err");
//	}

//	#else
	do
	{
		data = READREG(FGGB_PIPESTATE);
		READREGP(FGGB_PIPESTATE, data);
	}
	while(data & pipelineFlags);
//	#endif

	return FGL_ERR_NO_ERROR; 
}


/***************************************************************************** 
 * FUNCTIONS: fglClearCache
 * SYNOPSIS: This function clears the caches
 * PARAMETERS: [IN] clearFlags: Specified caches are cleared
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglClearCache(unsigned int clearFlags)
{
	unsigned int data;
	int waitCount = 0x10000000;
	WRITEREG(FGGB_CACHECTL, clearFlags); // start clearing the cache

	do
	{
		READREGP(FGGB_CACHECTL, data);
		//for preventing lock up
		if(waitCount--<0)
		return FGL_ERR_STATUS_BUSY;        
	}
	while((data & clearFlags) != 0);

	return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSoftReset
 * SYNOPSIS: This function resets FIMG-3DSE, but the SFR values are not affected
 * PARAMETERS:
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglSoftReset(void)
{
        unsigned int i;
        
        WRITEREG(FGGB_RST, FGL_TRUE);
        
        /* delay */
        for(i = 0; i < 50; i++)
    {
#ifndef __GNUC__
    #ifndef WIN32
        __asm ("nop");
    #else
        __asm {nop}
        #endif
#else
        __asm__ __volatile__ ("nop");
#endif
    }

        WRITEREG(FGGB_RST, FGL_FALSE);

        return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglGetVersion
 * SYNOPSIS: This function gets FIMG-3DSE version.
 * PARAMETERS: [OUT] ver : FIMG3DSE version code
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglGetVersion(unsigned int* ver)
{
        READREGP(FGGB_VERSION, *ver);

        return FGL_ERR_NO_ERROR;
}


/***************************************************************************** 
 * FUNCTIONS: fglGetInterrupt
 * SYNOPSIS: This function returns a bool which interrupt is generated or not
 *           In later implementation, programmer can connect this register
 *           to some conditions except pipeline status.
 *           
 * PARAMETERS: [OUT] isGenInterrupt: interrupt bit
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglGetInterrupt(FG_BOOL* isGenInterrupt)
{
        //*isGenInterrupt = READREG(FGGB_INTPENDING);
        READREGP(FGGB_INTPENDING, *isGenInterrupt);
#ifdef _FGL_VALUE_CHECK
        *isGenInterrupt &= FGL_TRUE; // return first bit only
#endif

        return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglPendInterrupt
 * SYNOPSIS: This function makes currunt interrupt to pend
 * PARAMETERS:
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglPendInterrupt(void)
{
        WRITEREG(FGGB_INTPENDING, FGL_TRUE);

        return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglEnableInterrupt
 * SYNOPSIS: This function enables the FIMG-3DSE interrupt
 * PARAMETERS:
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglEnableInterrupt(void)
{
        WRITEREG(FGGB_INTMASK, FGL_TRUE);
        return FGL_ERR_NO_ERROR;
}
        
/***************************************************************************** 
 * FUNCTIONS: fglDisableInterrupt
 * SYNOPSIS: This function disables the FIMG-3DSE interrupt
 * PARAMETERS:
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglDisableInterrupt(void)
{
        WRITEREG(FGGB_INTMASK, FGL_FALSE);
        return FGL_ERR_NO_ERROR;
}


/***************************************************************************** 
 * FUNCTIONS: fglSetInterruptBlock
 * SYNOPSIS: This function sets pipeline blocks to generate interrupt.
 * PARAMETERS: [IN] pipeMask: Oring PIPESTATE_XXXX block of generating interrupt
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglSetInterruptBlock(unsigned int pipeMask)
{
        WRITEREG(FGGB_PIPEMASK, pipeMask);
        return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetInterruptState
 * SYNOPSIS: This function sets an interrupt generated state of each block
 * PARAMETERS: [IN] status: each block state for interrupt to occur
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglSetInterruptState(FGL_PipelineStatus status)
{
        unsigned int bitPipeState;

        bitPipeState = status.isNotEmptyHOSTFIFO << FGL_PIPESTATE_HOSTFIFO_BIT;
        bitPipeState |= status.isNotEmptyHI << FGL_PIPESTATE_HI_BIT;
        bitPipeState |= status.isNotEmptyHI2VS << FGL_PIPESTATE_HI2VS_BIT;
        bitPipeState |= status.isNotEmptyVS << FGL_PIPESTATE_VS_BIT;
        bitPipeState |= status.isNotEmptyVC << FGL_PIPESTATE_VC_BIT;
        bitPipeState |= status.isNotEmptyPS << FGL_PIPESTATE_PS0_BIT;
        bitPipeState |= status.isNotEmptyPF << FGL_PIPESTATE_PF0_BIT;
        
#ifndef _FIMG_PIPELINE_SINGLE
        bitPipeState |= status.isNotEmptyPS << FGL_PIPESTATE_PS1_BIT;
        bitPipeState |= status.isNotEmptyPF << FGL_PIPESTATE_PF1_BIT;
#endif // _FIMG_PIPELINE_SINGLE

        WRITEREG(FGGB_PIPETGTSTATE, bitPipeState);

        return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglGetInterruptState
 * SYNOPSIS: This function returns the value of pipeline-state when interrupt
 *           is to occur
 * PARAMETERS: [OUT] status: Each block state of pipeline
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglGetInterruptState(FGL_PipelineStatus* status)
{
        unsigned int bitPipeState;

         //bitPipeState = READREG(FGGB_PIPEINTSTATE);
         READREGP(FGGB_PIPEINTSTATE, bitPipeState);

        status->isNotEmptyHOSTFIFO = bitPipeState & FGL_PIPESTATE_HOSTFIFO;
        status->isNotEmptyHI = bitPipeState & FGL_PIPESTATE_HI;
        status->isNotEmptyHI2VS = bitPipeState & FGL_PIPESTATE_HI2VS;
        status->isNotEmptyVS = bitPipeState & FGL_PIPESTATE_VS;
        status->isNotEmptyVC = bitPipeState & FGL_PIPESTATE_VC;
        status->isNotEmptyPS = bitPipeState & FGL_PIPESTATE_PS0;
        status->isNotEmptyPF = bitPipeState & FGL_PIPESTATE_PF0;
        
#ifndef _FIMG_PIPELINE_SINGLE
        status->isNotEmptyPS |= (bitPipeState & FGL_PIPESTATE_PS1);
        status->isNotEmptyPF |= (bitPipeState & FGL_PIPESTATE_PF1);
#endif //_FIMG_PIPELINE_SINGLE

        return FGL_ERR_NO_ERROR;
}

#if TARGET_FIMG_VERSION == _FIMG3DSE_VER_2_0

/***************************************************************************** 
 * FUNCTIONS: fglGetExeState
 * SYNOPSIS: This function gets a state of FIMG execution controller's
 *           state machine
 * PARAMETERS: [OUT] status: State of FIMG execution controller
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglGetExeState(FGL_ExeState status)
{
        unsigned int bitExeState;

    READREGP(FGGB_EXESTATE, bitExeState);

    status = (FGL_ExeState)bitExeState;

        return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetNextStep
 * SYNOPSIS: This function make FIMG go to next step
 *           when the execution controller is in FGL_EXESTATE_INTR
 *           (Useful to debug)
 * RETURNS: FGL_ERR_NO_ERROR - always.
 * ERRNO:
 *****************************************************************************/

FGL_Error
fglSetNextStep(void)
{
    WRITEREG(FGGB_NEXTSTEP, 0x1);

        return FGL_ERR_NO_ERROR;
}


#endif
