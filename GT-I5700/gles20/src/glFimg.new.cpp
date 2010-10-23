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

* \file   glFimg.cpp

* \author Prashant Singh (prashant@samsung.com)

* \brief  Hardware interface

*

*//*---------------------------------------------------------------------------

* NOTES:

*

*//*---------------------------------------------------------------------------

* HISTORY:

*

* 31.07.2006  Prashant Singh  Crude functions for firmware demo

*

* 21.08.2006  Prashant Singh  Shaders, host interface, draw modes,

*               transfer arrays and float constants completed

*               and tested on firmware

*

* 07.12.2006  Prashant Singh  Partial update management

*

* 30.01.2006  Prashant Singh  Draw elements support and documentation added

*

*******************************************************************************

*/

/*

*******************************************************************************

* Includes

*******************************************************************************

*/

#include <string.h>

#include "gl.h"
#include "glState.h"
#include "platform.h"
#include "register.h"


#include "fgl.h"


//#include "fimg_sysutil.h"

#include "glFimg.h"
#include "buffers.h"
#include "pixel.h"
#include "math.h"

#include "dma.h"


#include <sys/time.h>

///////////////////////////////////////////////////////////
//#include <sys/time.h>
#include <cutils/log.h>
//struct timeval __start;
//struct timeval __end;
/*
static inline double  __test_tval(struct timeval _tstart,struct timeval _tend)
{
	double t1 = 0.;
	double t2 = 0.;

	t1 = ((double)_tstart.tv_sec * 1000 + (double)_tstart.tv_usec/1000.0) ;
	t2 = ((double)_tend.tv_sec * 1000 + (double)_tend.tv_usec/1000.0) ;

	return t2-t1;
}
*/
///////////////////////////////////////////////////////////

/*

*******************************************************************************

* Macro definitions and enumerations

*******************************************************************************

*/
//#define NEWSFRSETTING


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

/*

*******************************************************************************

* Local Function Declarations

*******************************************************************************

*/

//-----------------------------------------------------------------------------

// Translation functions from GLenum to FimG constants

//-----------------------------------------------------------------------------


// Host interface


static FGL_AttribDataType AttribType  (GLenum type, GLboolean normalized);
inline int maxValidVerts(GLenum primType, int count);

// Per-Fragment unit


static FGL_StencilAct StencilAction (GLenum action);
static FGL_CompareFunc  CompareFunc   (GLenum func);
static FGL_BlendFunc  BlendFunc   (GLenum func);
static FGL_LogicalOp    LogicOp         (GLenum op);
// Texture unit


extern "C" void burst8copy_one_dst(int *source,int *dst, int num); //cglee

extern "C" void burst8copy(int *source,int *dst, int num); //cglee

extern "C" void __fglSendToFIFO(unsigned int bytes,void *buffer,unsigned int fifo);

int *bgImage;

/*

*******************************************************************************

* Function Definitions

*******************************************************************************

*/

/*

*******************************************************************************

* Interface Functions

*******************************************************************************

*/

/*-----------------------------------------------------------------------*//*!

* Initialize the FIMG hardware.

*

* There is not much required to be done right now. Maybe when we do a software

* reset then we need to initialize certain things. Checkout the function

* InitFimg() where some initialization is being done.

*

* \param [in] pState(OGLState)    Library context

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfInit(OGLState* pState)
{
	// *Pra* We are not clearing any buffer at initialization. Is it required?


	// *Pra* Need to soft reset the hardware here


	// Set all update bits to start with

	pState->updateBits.hi = UB_HI_ALL;
	pState->updateBits.vs = UB_VS_ALL;
	pState->updateBits.pe = UB_PE_ALL;
	pState->updateBits.re = UB_RE_ALL;
	pState->updateBits.fs = UB_FS_ALL;
	pState->updateBits.tu = UB_TU_ALL;
	pState->updateBits.pf = UB_PF_ALL;
	pState->updateBits.pfbuf = UB_PF_BUF_ALL;
	return GLF_ERR_SUCCESS;
}

/*-----------------------------------------------------------------------*//*!

* Finish hardware processing.

*

* Wait for hardware to finish work and then flush caches. This ensure all the

* commands already sent to hardware are completed and that hardware is idle

* now.

*

* \param [in] pState(OGLState)    Library context

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfFinish(OGLState* pState)
{
	GET_GL_STATE(ctx);
	//glfFlush is already called in glCore.cpp


	fglFlush(FGL_PIPESTATE_ALL);  // Pipeline status


	// Cache flush

	FGL_Blend blendParam;

	const unsigned int curBlendConfig = READREG(FGPF_BLEND); //save blend config

	memset(&blendParam,0,sizeof(FGL_Blend));
	fglSetBlend(FGL_TRUE, &blendParam);
	fglClearCache(FGL_CACHECTL_INIT_ALL);
	WRITEREG(FGPF_BLEND, curBlendConfig);  //restore blend config

	//  fglSysDelay(50);

	return GLF_ERR_SUCCESS;
}

static void memcpy_long_fimg(unsigned int * dst, unsigned int* src, unsigned int len)
{
	while (len >0 )
	{
		*dst++ = *src++;
		len--;
	}
}


/*-----------------------------------------------------------------------*//*!

* Flush the library state to FIMG hardware.

*

* \param [in] pState(OGLState)    Library context

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfFlush(OGLState* pState)
{
//	struct timeval __start;
//	struct timeval __end;
//  gettimeofday(&__start,NULL);/////////////////////////////

	fglFlush(FGL_PIPESTATE_ALL);
	
	FramebufferData fbData = getFBData();

	PxFmt colorFmt = fbData.nativeColorFormat;

	bool dither =  false; //pState->is_enabled_dither & (pixelSize(colorFmt)==2); //

	/* sanvd */
	FGL_FBCtrlParam fbctrlParam = {
		FGL_FALSE,
		127,
		0,
		dither,
		translateToFglPixelFormat(colorFmt)
	};

	fglSetFrameBufParams( (pFGL_FBCtrlParam)&fbctrlParam );

	fglSetFrameBufWidth((int)fbData.width);

	fglSetColorBufBaseAddr((int)fbData.colorAddr.paddr);
	
	fglSetZBufBaseAddr((int)fbData.depthStencilAddr.paddr);
	
	glfSetState(pState);

//	gettimeofday(&__end,NULL);///////////////////////////
//	LOGE("%s completed in: %5.5f ms",__FUNCTION__,__test_tval(__start,__end));
	
	return GLF_ERR_SUCCESS;
}


static glfError glfHardwareClear(OGLState* pState, GLbitfield mask)
{
	// TODO: move to initialize(InitFimg?)


	FramebufferData fbData = getFBData();


	//Wait till FIMG is free

	fglFlush(FGL_PIPESTATE_ALL);
	PxFmt colorFmt = fbData.nativeColorFormat;
	bool dither =  false;
	FGL_FBCtrlParam fbctrlParam = {FGL_FALSE, 127, 0, dither, translateToFglPixelFormat(colorFmt)};
	fglSetFrameBufParams( (pFGL_FBCtrlParam)&fbctrlParam );

	fglSetFrameBufWidth(fbData.width);
	fglSetColorBufBaseAddr((unsigned int)fbData.colorAddr.paddr);
	fglSetZBufBaseAddr((unsigned int)fbData.depthStencilAddr.paddr);


	// It can be implemented by vertex buffer

	unsigned int Clear_VS[] =
	{
		0x00000000, 0x00000000, 0x00f800e4, 0x00000000,
	};

	unsigned int Clear_FS[] =
	{
		0x00000000, 0x02ff0000, 0x00f810e4, 0x00000000,
	};

	FGL_BOOL colormask_r, colormask_g, colormask_b, colormask_a;
	unsigned int *pClearVSInstAddr = (unsigned int *)(FIMG_BASE + 0x11ff0);
	unsigned int *pClearPSInstAddr = (unsigned int *)(FIMG_BASE + 0x41ff0);
	unsigned int *pClearColorAddr = (unsigned int *)(FIMG_BASE + 0x44ff0);

	// TODO : If we gather SFRs setting HW for clearing, it can be set by burst

	fglFlush(FGL_PIPESTATE_ALL);

	// Load into last instruction memory

	memcpy_long_fimg(pClearVSInstAddr,Clear_VS,4);
	memcpy_long_fimg(pClearPSInstAddr, Clear_FS,4);
	memcpy_long_fimg(pClearColorAddr, (unsigned int*) &pState->clear_color_data.r,4);

	fglEnableDepthOffset(FGL_FALSE);
	fglSetFaceCullControl(FGL_FALSE, FGL_FALSE, FGL_FACE_BACK);
	fglSetLODControl(0);

	FGL_Blend blendParam;
	const unsigned int curBlendConfig = READREG(FGPF_BLEND); //save blend config

	memset(&blendParam,0,sizeof(FGL_Blend));

	fglSetBlend(FGL_FALSE, &blendParam); // disable blend always


	//    if(pState->scissor_test_data.is_enabled)

	const ScissorTestData& scissorData = pState->scissor_test_data;
	//    FramebufferData fbData = getFBData();

	unsigned int xmin,xmax,ymax,ymin;

	xmin = Plat::min(Plat::max(scissorData.x,(GLint)0),(GLint)fbData.width);
	ymin = Plat::min(Plat::max(scissorData.y,(GLint)0),(GLint)fbData.height);
	xmax = Plat::min(Plat::max(scissorData.x + scissorData.w,(GLint)0), (GLint)fbData.width);
	ymax = Plat::min(Plat::max(scissorData.y + scissorData.h,(GLint)0), (GLint)fbData.height);

	if(!fbData.flipped)
	{
		unsigned int tmp = fbData.height - ymin;
		ymin = fbData.height - ymax;
		ymax = tmp;
	}

	if (pState->scissor_test_data.is_enabled) // imbumoh

	{
		fglSetXClip(xmin,xmax);
		fglSetYClip(ymin,ymax);
	}
	else
	{
		fglSetXClip(0,fbData.width);
		fglSetYClip(0,fbData.height);
	}

	fglSetAlpha(FGL_FALSE, 0, FGL_COMP_NEVER); // always diabled


	FGL_Stencil stencilParam = {
		FGL_ACT_REPLACE,
		FGL_ACT_REPLACE,
		FGL_ACT_REPLACE,
		0, //mask

		pState->clear_stencil,
		FGL_COMP_ALWAYS
	};

	if(mask & GL_STENCIL_BUFFER_BIT) {
		fglSetFrontStencil(FGL_TRUE, &stencilParam);
	}
	else {
		fglSetFrontStencil(FGL_FALSE, &stencilParam);
	}

	// TODO : Color clear À¨ ÎÖÄâ ÀöÏü

	fglSetZBufBaseAddr((int)fbData.depthStencilAddr.paddr);

	if(mask & GL_DEPTH_BUFFER_BIT) {
		fglSetDepth(FGL_TRUE, FGL_COMP_ALWAYS);
	}
	else {
		fglSetDepth(FGL_FALSE, FGL_COMP_NEVER);
	}

	// just masking

	fglSetLogicalOp(FGL_FALSE, FGL_OP_COPY, FGL_OP_COPY);
	if(mask & GL_COLOR_BUFFER_BIT) {
		colormask_r = !(pState->color_mask_data.r); // imbumoh 20090711

		colormask_g = !(pState->color_mask_data.g);
		colormask_b = !(pState->color_mask_data.b);
		colormask_a = !(pState->color_mask_data.a);
		fglSetColorBufWriteMask(colormask_r, colormask_g, colormask_b, colormask_a);
	}
	else {
		fglSetColorBufWriteMask(FGL_TRUE, FGL_TRUE, FGL_TRUE, FGL_TRUE);
	}

	fglSetDepthStencilWriteMask(0, 0, FGL_FALSE);

	fglVSSetAttribNum(1);
	fglVSSetPCRangeEXT(511, 511, FGL_FALSE);
	fglSetPSParams(1, 511, 511, FGL_FALSE);
	//    fglPSSetAttribNum(1);

	//    fglPSSetPCRangeEXT(511, 511, FGL_FALSE);


	fglSetPointWidth(2048.0f);

	FGL_VertexCtx Vtx =
	{
		FGL_PRIM_POINTS,
		FGL_FALSE,
		0,
		0
	};

	fglSetVertexCtx(&Vtx);

	// Host Interface SFR Set

	FGL_HInterface HInterface =
	{
		FGL_FALSE,
		FGL_INDEX_DATA_UINT,
		FGL_TRUE,
		FGL_FALSE,
		1
	};
	fglSetHInterface(&HInterface);

	// swizzle and just transfer z value.

	FGL_Attribute HIAttr =
	{
		1,
		FGL_ATTRIB_ORDER_3RD,
		FGL_ATTRIB_ORDER_2ND,
		FGL_ATTRIB_ORDER_1ST,
		FGL_ATTRIB_ORDER_4TH,
		FGL_ATTRIB_DATA_FLOAT,
		FGL_TRUE
	};
	fglSetAttribute(0, &HIAttr);


	unsigned int clearObjData[] = {
		1,
		0xffffffff,
		*(unsigned int *)&pState->clear_depth
	};
	memcpy_long_fimg((unsigned int *)FGHI_FIFO_ENTRY, (unsigned int*) &clearObjData[0], 3);

	fglFlush(FGL_PIPESTATE_ALL);
	//fglClearCache(FGL_CACHECTL_FLUSH_ZCACHE | FGL_CACHECTL_FLUSH_CCACHE);

	//glfFinish(pState);

	fglSetBlend(FGL_TRUE, &blendParam);
	fglClearCache(FGL_CACHECTL_INIT_ALL);


	WRITEREG(FGPF_BLEND, curBlendConfig);  //restore blend config


	// enable blend if it was disabled

	//  if (pState->blend_data.is_enabled)

	//  {

	//      fglSetBlend(FGL_TRUE, 0);

	//  }


	return GLF_ERR_SUCCESS;

}

static glfError glfSoftwareClear(OGLState* pState, GLbitfield mask)
{
	FramebufferData fbData = getFBData();
	PxFmt colorFmt = fbData.nativeColorFormat;
	unsigned int colorMask = getColorMask(colorFmt, pState->color_mask_data.r, pState->color_mask_data.g, pState->color_mask_data.b, pState->color_mask_data.a);
	unsigned int pixSize = pixelSize(colorFmt);

	//size_t numPixels = fbData.width*fbData.height;


	const ScissorTestData& scissorData = pState->scissor_test_data;

	int x,y,h,w;
	if(pState->scissor_test_data.is_enabled)
	{
		x = Plat::min(Plat::max(scissorData.x,(GLint)0),(GLint)fbData.width);
		y = Plat::min(Plat::max(scissorData.y,(GLint)0),(GLint)fbData.height);
		w = Plat::min(Plat::max(scissorData.x + scissorData.w,(GLint)0), (GLint)fbData.width) -x;
		h = Plat::min(Plat::max(scissorData.y + scissorData.h,(GLint)0), (GLint)fbData.height) -y;
	}
	else
	{
		x = 0;
		y = 0;
		w = fbData.width;
		h = fbData.height;
	}

	int pixelStride = fbData.width;

	if(!fbData.flipped)
	{
		y = fbData.height-1 - y;
		pixelStride*= -1;
	}

	unsigned int color = 0;

	//Wait till FIMG has finished rendering and all FB data is flushed from FIMG caches to RAM.

	glfFinish(pState);

	bool resultDepthMask = (mask & GL_DEPTH_BUFFER_BIT) && pState->depth_write_mask;
	unsigned char resultStencilMask = (mask & GL_STENCIL_BUFFER_BIT) ? (pState->front_stencil_writemask & 0xFF ) : 0; //(pState->stencil_test_data.front_face_mask & 0xFF ) : 0;


	if(((mask& GL_DEPTH_BUFFER_BIT) || (mask& GL_STENCIL_BUFFER_BIT)) && fbData.depthStencilAddr.vaddr != 0 && fbData.nativeDepthStencilFormat != E_INVALID_PIXEL_FORMAT)
	{
		//TODO: remove hardcoding

		unsigned int depthStencilClear = convertFromDepthStencil(fbData.nativeDepthStencilFormat, pState->clear_depth, pState->clear_stencil);
		unsigned int depthStencilMask = getDepthStencilMask(fbData.nativeDepthStencilFormat, resultDepthMask, resultStencilMask);

		unsigned char* dst = (unsigned char*)fbData.depthStencilAddr.vaddr;
		dst += (y*fbData.width + x)*4;

		unsigned char* dstp = (unsigned char*)fbData.depthStencilAddr.paddr;
		dstp += (y*fbData.width + x)*4;

		if (w == fbData.width && h > 0)
		{
			if (pixelStride > 0)
			{
				Plat::memset_long(dst, depthStencilClear, w*h, depthStencilMask, dstp);
			}
			else if (pixelStride < 0)
			{
				Plat::memset_long(dst + (h - 1) * (pixelStride * 4), depthStencilClear, w*h, depthStencilMask, dstp + (h - 1) * (pixelStride * 4));
			}
		}
		else
		{
			for(int i=0; i<h; i++)
			{
				Plat::memset_long(dst, depthStencilClear, w, depthStencilMask, dstp);
				dst +=pixelStride*4;
				dstp +=pixelStride*4;
			}
		}
	}
	if((mask & GL_COLOR_BUFFER_BIT) && fbData.colorAddr.vaddr)
	{


		if(colorFmt == E_INVALID_PIXEL_FORMAT)
		{
			////LOGMSG("ERROR: unknown pixel format! color buffer clear unsuccessful\n");
		}
		else
		{
			color = convertFromRGBA(colorFmt, pState->clear_color_data.r, pState->clear_color_data.g, pState->clear_color_data.b, pState->clear_color_data.a);

			unsigned char* dst = (unsigned char*)fbData.colorAddr.vaddr;
			dst += (y*fbData.width + x)*pixSize;

			unsigned char* dstp = (unsigned char*)fbData.colorAddr.paddr;
			dstp += (y*fbData.width + x)*pixSize;

			if (w == fbData.width && h > 0)
			{
				if (pixelStride > 0)
				{
					if (pixSize == 4)
					{
						Plat::memset_long(dst, color, w*h, colorMask, dstp);
					}
					else if (pixSize == 2)
					{
						Plat::memset_short(dst, color, w*h, colorMask, dstp);
					}
				}
				else if (pixelStride < 0)
				{
					if (pixSize == 4)
					{
						Plat::memset_long(dst + (h - 1) * (pixelStride * pixSize), color, w*h, colorMask, dstp + (h - 1) * (pixelStride * pixSize));
					}
					else if (pixSize == 2)
					{
						Plat::memset_short(dst + (h - 1) * (pixelStride * pixSize), color, w*h, colorMask, dstp + (h - 1) * (pixelStride * pixSize));
					}
				}
			}
			else
			{
				if(pixSize == 4)
				{
					for(int i=0; i<h; i++)
					{
						Plat::memset_long(dst, color, w,colorMask, dstp);
						dst += pixelStride*pixSize;
						dstp += pixelStride*pixSize;
					}
				}
				else if(pixSize == 2)
				{
					for(int i=0; i<h; i++)
					{
						Plat::memset_short(dst, color, w,colorMask, dstp);
						dst += pixelStride*pixSize;
						dstp += pixelStride*pixSize;
					}
				}
				else
				{
					////LOGMSG("ERROR: unexpected bit depth! color buffer clear unsuccessful\n");
				}
			}
		}
	}

	return GLF_ERR_SUCCESS;
}

#define HWCLEAR_ONLY

/*-----------------------------------------------------------------------*//*!

* Clear buffers.

*

* Clear Z-buffer or color buffer or both depending on the mask value.

*

* \param [in] pState(OGLState)    Library context

* \param [in] mask(GLbitfield)    Clear mask

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/
glfError glfClear ( OGLState* pState, GLbitfield mask )
{
#ifdef MIXED_HWSWCLEAR

	PxFmt colorFmt = fbData.nativeColorFormat;
	unsigned int colorMask = getColorMask(colorFmt, pState->color_mask_data.r, pState->color_mask_data.g, pState->color_mask_data.b, pState->color_mask_data.a);
	unsigned int pixSize = pixelSize(colorFmt);

	// clearing of depth and stencil buffers is usually faster if they are needed

	// to be cleared at the same time and scissor test is currently not enabled

	if ((mask & (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) == (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT))
	{

		const ScissorTestData& scissorData = pState->scissor_test_data;
		if (!pState->scissor_test_data.is_enabled || (scissorData.x == 0 && scissorData.y == 0 && scissorData.w == fbData.width && scissorData.h == fbData.height))
		{
			glfSoftwareClear(pState, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			mask &= ~(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			if (mask == 0) return GLF_ERR_SUCCESS;
		}

	}

	// hardware clearing of color buffer with active color write mask is not supported

	if ((mask & GL_COLOR_BUFFER_BIT) && (colorMask != (pixSize == 2 ? 0xFFFF : 0xFFFFFFFF)))
	{
		glfSoftwareClear(pState, GL_COLOR_BUFFER_BIT);
		mask &= ~GL_COLOR_BUFFER_BIT;
		if (mask == 0) return GLF_ERR_SUCCESS;
	}

	// do rest of clearing using harwdare clearing

	return glfHardwareClear(pState, mask);

#else

#ifdef HWCLEAR_ONLY

	if (mask == 0) return GLF_ERR_SUCCESS;

	FramebufferData fbData = getFBData();

	//Wait till FIMG is free

	fglFlush(FGL_PIPESTATE_ALL);

	// imbumoh 20090718

	PxFmt colorFmt = fbData.nativeColorFormat;
	bool dither =  false;
	FGL_FBCtrlParam fbctrlParam = {FGL_FALSE, 127, 0, dither, translateToFglPixelFormat(colorFmt)};
	fglSetFrameBufParams( (pFGL_FBCtrlParam)&fbctrlParam );

	fglSetFrameBufWidth(fbData.width);
	fglSetColorBufBaseAddr((unsigned int)fbData.colorAddr.paddr);
	fglSetZBufBaseAddr((unsigned int)fbData.depthStencilAddr.paddr);

	return glfHardwareClear(pState, mask);
#else

	FramebufferData fbData = getFBData();
	PxFmt colorFmt = fbData.nativeColorFormat;
	unsigned int colorMask = getColorMask(colorFmt, pState->color_mask_data.r, pState->color_mask_data.g, pState->color_mask_data.b, pState->color_mask_data.a);
	unsigned int pixSize = pixelSize(colorFmt);
	GLbitfield mask2 ;

	if (mask == 0) return GLF_ERR_SUCCESS;

	if( (mask & (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT))== GL_STENCIL_BUFFER_BIT )
	{
		glfHardwareClear(pState, GL_STENCIL_BUFFER_BIT);
		mask &= ~(GL_STENCIL_BUFFER_BIT);
	}

	//if( mask & (GL_COLOR_BUFFER_BIT) ) : color mask is not working in case of hardware clear

	if ((mask & GL_COLOR_BUFFER_BIT) && (colorMask == (pixSize == 2 ? 0xFFFF : 0xFFFFFFFF)))
	{
		glfHardwareClear(pState, GL_COLOR_BUFFER_BIT);
		mask &= ~(GL_COLOR_BUFFER_BIT);
	}

	if (mask != 0)
	glfSoftwareClear(pState, mask);

#endif

#endif

	return GLF_ERR_SUCCESS;
}

/*-----------------------------------------------------------------------*//*!

* Set the library state to FIMG hardware.

*

* \param [in] pState(OGLState)    Library context

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfSetState(OGLState* pState)
{
	//temporarily disable dirty state tracking

	//till FBO dirty state tracking is implemented

	pState->curFBData = getFBData();

	#if 0
	pState->updateBits.hi = UB_HI_ALL;
	pState->updateBits.vs = UB_VS_ALL;
	pState->updateBits.pe = UB_PE_ALL;
	pState->updateBits.re = UB_RE_ALL;
	pState->updateBits.fs = UB_FS_ALL;
	pState->updateBits.tu = UB_TU_ALL;
	pState->updateBits.pf = UB_PF_ALL;
	pState->updateBits.pfbuf = UB_PF_BUF_ALL;
	#else
	//Set dirty bit if rendertarget info has changed since the previous draw call.

	if( pState->prevFBData.width != pState->curFBData.width
			|| pState->prevFBData.height != pState->curFBData.height
			|| pState->prevFBData.flipped != pState->curFBData.flipped ) {
		pState->updateBits.pe = UB_PE_ALL; //tbd narrow down after reviewing setpe

		pState->updateBits.re = UB_RE_ALL; //tbd narrow down after reviewing setra

		pState->updateBits.pf = UB_PF_ALL; //tbd narrow down afrer reviewing setpf

	}

	if( pState->prevFBData.nativeColorFormat != pState->curFBData.nativeColorFormat
			|| pState->prevFBData.nativeDepthStencilFormat != pState->curFBData.nativeDepthStencilFormat ) {
		pState->updateBits.pf = UB_PF_ALL; //tbd narrow down afrer reviewing setpf

	}

	pState->prevFBData = pState->curFBData;

	#endif
	glfSetVertexShader(pState);
	glfSetPrimEngine(pState);
	glfSetFragShader(pState);
	glfSetRasterEngine(pState);
	glfSetPerFragUnit(pState);
	glfSetTextureUnit(pState);
	
	return GLF_ERR_SUCCESS;
}

/*-----------------------------------------------------------------------*//*!

* Set the Host interface hardware.

*

* \param [in] pState(OGLState)    Library context

* \param [in] attribInfo(AttribInfo*) attribute info

* \param [in] reorder(bool)         true if the attribs are to be transferred

*                                   in memory order rather than the attribute

*                                   register order

* \param [in] isBuf(GLboolean)    If we want buffered vertex attribute transfer

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfSetHostIf ( OGLState* pState, AttribInfo* attribInfo, bool reorder, GLboolean isBuf )
{
	Executable *pExe = (pState->current_executable);
	FGL_HInterface hostIf;
	FGL_Attribute fglAttrib;
	VertexAttribArrayData *pAttribData;
	int i;
	unsigned int POINT_SPRITE;
	if( pExe->pointCoordIndex != -1){
		POINT_SPRITE = GL_TRUE;
	}
	else{
		POINT_SPRITE = GL_FALSE;
	}
	if(isBuf == GL_TRUE) {

		//---------------------------------------------------------------------

		// Buffered transfer

		//---------------------------------------------------------------------


		hostIf.enableVtxBuffer = FGL_TRUE;  //Enable vertex buffer

		hostIf.idxType = FGL_INDEX_DATA_UINT; //Don't care in autoinc buffered mode

		hostIf.enableAutoInc = FGL_TRUE;       //Enable auto inc

		hostIf.enableVtxCache = FGL_FALSE;     //diabled in auto inc?


		hostIf.numVSOut = (POINT_SPRITE) ? pExe->numVSVarying + 1 : pExe->numVSVarying ;

		if(pState->VPPSEnable)
		{
			hostIf.numVSOut++;
		}

#ifdef FIMG_WRONG_INTERPOLATION_WORKAROUND
		if(pExe->numFSVarying > 4){
			hostIf.numVSOut = 9;
		}
#endif

		fglSetHInterface(& hostIf);
		//gAssert(0&&"Not implemented");

		//return GLF_ERR_ERROR;

	} else {

		//---------------------------------------------------------------------

		// Un-buffered transfer

		//---------------------------------------------------------------------


		hostIf.enableVtxBuffer = FGL_FALSE;   // Enable vertex buffer

		hostIf.idxType = FGL_INDEX_DATA_UINT; // Don't care for unbuffered mode

		hostIf.enableAutoInc = FGL_TRUE;    // Non-indexed mode

		hostIf.enableVtxCache = FGL_FALSE;    // Unbuffered


		hostIf.numVSOut = (POINT_SPRITE) ? pExe->numVSVarying + 1 : pExe->numVSVarying ;

		if(pState->VPPSEnable)


		{
			hostIf.numVSOut++;
		}
#ifdef FIMG_WRONG_INTERPOLATION_WORKAROUND
		if(pExe->numFSVarying > 4){
			hostIf.numVSOut = 9;
		}
#endif

		fglSetHInterface(& hostIf);
	}

	//-------------------------------------------------------------------------

	// Configure attributes

	//-------------------------------------------------------------------------


	for(i = 0; i < pExe->numAttribs; i++) {
		int indx = pExe->attribMap[i];

		pAttribData = &(pState->vertex_attrib_array_data[indx]);

		fglAttrib.numComp = pAttribData->size;  // Size

		fglAttrib.srcX = FGL_ATTRIB_ORDER_1ST;  // Order -> X, Y, Z, W

		fglAttrib.srcY = FGL_ATTRIB_ORDER_2ND;
		fglAttrib.srcZ = FGL_ATTRIB_ORDER_3RD;
		fglAttrib.srcW = FGL_ATTRIB_ORDER_4TH;

		if(pAttribData->enabled)
		{
			fglAttrib.numComp = pAttribData->size;  // Size


			// Convert GL to FGL type

			fglAttrib.type = AttribType(pAttribData->type, pAttribData->normalized);
		}
		else
		{
			fglAttrib.numComp = 4;

			// Convert GL to FGL type

			fglAttrib.type = AttribType(GL_FLOAT, GL_FALSE);
		}

		//get location of ith attribute

		int loc = (attribInfo->attribOrder[i>>2] >> (8*(i&0x3))  )&0xFF;
		//if reorder==1, the attribs are transferred in memory location order

		int k = reorder ?  loc : i;

		if(k == pExe->numAttribs - 1) {     // Set end flag if this is the last entry

			fglAttrib.bEndFlag = FGL_TRUE;
		} else {
			fglAttrib.bEndFlag = FGL_FALSE;
		}

		fglSetAttribute(k, &fglAttrib);     // Set hw register

	}

	return GLF_ERR_SUCCESS;
}


/*-----------------------------------------------------------------------*//*!

* Set the vertex shader hardware.

*

* \param [in] pState(OGLState)    Library context

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfSetVertexShader ( OGLState* pState )
{
	Executable *pExe = (pState->current_executable);
	ShaderExecutable *pShader = &(pState->current_executable->vs);
	//unsigned int *ubits = &(pState->updateBits.vs);


	unsigned int indices[3];
	int i=1; //1st is anyway set to 0

	const unsigned int UNUSED = 0xB;


	// Set VS fields ------------------------------------------------------


	fglVSSetAttribNum(/*pExe->numVSVarying,*/pExe->numAttribs);

	// -2 is for "pc end" optimization

	fglVSSetPCRangeEXT(pExe->vs.pcStart, (pExe->vs.binaryCodeSize >> 4)-2,
	FGL_FALSE);

	// Set varying mappings -----------------------------------------------


	indices[0] = indices[1] = indices[2]= 0;
#if 1
	const bool VPPS = (pState->VPPSEnable);
#else
	const bool VPPS = (pExe->pointSizeIndex != -1);
#endif
	const unsigned int shiftValue = VPPS ? 1 : 0;
	for( ; i < pExe->numVSVarying; i++) {
		if(pExe->varyingMap[i] != -1) {
			indices[i / 4] |= ((pExe->varyingMap[i] + shiftValue) & 0xF) << (8*(i & 0x3));
		} else {
			indices[i / 4] |= (UNUSED) << (8*(i & 0x3));
		}
	}

	for( ; i < (MAX_VARYING_MAPPINGS+1); i++) {
		indices[i / 4] |= (UNUSED) << (8*(i & 0x3));  // Set all unused to 0xB

	}

	//for setting point size index to 1

	if(VPPS){
		int ind = (pExe->pointSizeIndex)/4;
		int shift = pExe->pointSizeIndex - 4 * ind;
		indices[ind] = (indices[ind] &( ~(0xf<<(shift * 8)))) | (0x1<<(shift * 8));
		//indices[ind] = (indices[ind] & 0xfffff0ff) |(0x100);

	}

	WRITEREG(FGVS_OUT_ATTRIB_IDX0, indices[0]);
	WRITEREG(FGVS_OUT_ATTRIB_IDX1, indices[1]);
	WRITEREG(FGVS_OUT_ATTRIB_IDX2, indices[2]);

	// ////LOGMSG("OutAttrib: 0x%x, 0x%x, 0x%x\n", indices[0], indices[1], indices[2]);


	// Copy vertex shader instructions ------------------------------------


	Plat::memcpy_FIMG((void*) FGVS_INSTMEM_SADDR,
	(const void*) pShader->binaryCode,
	pShader->binaryCodeSize);
	//}


	// Copy const registers ---------------------------------------------------


	// Float constants

	Plat::memcpy_FIMG((void*) FGVS_CFLOAT_SADDR,
	(const void*) pShader->constFloatMemBlock,
	pShader->fmbSize);


	// Integer constants

	Plat::memcpy_FIMG((void*) FGVS_CINT_SADDR,
	(const void*) pShader->constIntMemBlock,
	pShader->imbSize);


	// Boolean constants


	Plat::memcpy_FIMG((void*) FGVS_CBOOL_SADDR,
	(const void*) pShader->constBoolMemBlock,
	pShader->bmbSize);

	return GLF_ERR_SUCCESS;
}

/*-----------------------------------------------------------------------*//*!

* Helper function to set fragment shader execution mode.

*//*------------------------------------------------------------------------*/

FGL_Error
_PSExecuteMode (
FGL_ExecuteMode exeMode
)
{
	FGL_Error ret = FGL_ERR_UNKNOWN;

	if(exeMode == (FGL_ExecuteMode)READREG(FGPS_EXE_MODE))
	{
		return FGL_ERR_NO_ERROR;
	}

	switch(exeMode)
	{
	case FGL_HOST_ACCESS_MODE:

		if((READREG(FG_PIPELINE_STATUS) & PS_ISNOT_EMPTY_MASK) != FGL_ZERO)
		{
			//////LOGMSG((DBG_ERROR, "Pipeline status is not empty"));

			return FGL_ERR_STATUS_BUSY;
		}

		WRITEREG(FGPS_EXE_MODE, HOST_ACCESS_MODE);
		ret = FGL_ERR_NO_ERROR;
		break;

	case FGL_PS_EXECUTE_MODE:

		if((READREG(FGPS_INBUF_STATUS) & PS_INBUF_STATUS_MASK) != FGL_ZERO)
		{
			//////LOGMSG((DBG_ERROR, "The input buffer of pixel shader is not ready);

			return FGL_ERR_STATUS_BUSY;
		}

		WRITEREG(FGPS_EXE_MODE, PS_EXECUTION_MODE);
		ret = FGL_ERR_NO_ERROR;
		break;

	default:
		//////LOGMSG((DBG_ERROR, "Cannot set PS execute mode - invalid exeMode"));

		ret = FGL_ERR_INVALID_PARAMETER;
		break;
	}

	return ret;
}

/*-----------------------------------------------------------------------*//*!

* Set the fragment shader hardware.

*

* \param [in] pState(OGLState)    Library context

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfSetFragShader( OGLState* pState )
{
	Executable *pExe = (pState->current_executable);
	ShaderExecutable *pShader = &(pState->current_executable->fs);
	//unsigned int inBufStat;

	//unsigned int *ubits = &(pState->updateBits.fs);



	// Set fragment shader registers --------------------------------------


#if 0
	_PSExecuteMode(FGL_HOST_ACCESS_MODE);
	WRITEREG(FGPS_ATTRIB_NUM, pExe->numFSVarying);

	do {
		fglPSGetInBufferStatus(&inBufStat);

	} while(INPUT_BUFFER_READY != inBufStat);

	unsigned int programLength = ((pExe->fs.binaryCodeSize >> 4) - 1);
	WRITEREG(FGPS_PC_START, pExe->fs.pcStart);
	WRITEREG(FGPS_PC_END, programLength | (1<<9));  // 1<<9 is to ignore pc end


	WRITEREG(FGPS_PC_COPY, PROGRAM_COUNT_COPYOUT);

	_PSExecuteMode(FGL_PS_EXECUTE_MODE);
#else
	//#ifndef NEWSFRSETTING


	unsigned int attribNum = (pExe->numFSVarying ? pExe->numFSVarying : 1);
#ifdef FIMG_WRONG_INTERPOLATION_WORKAROUND
	if(pExe->numFSVarying > 4)
	{
		attribNum = 8;
	}
#endif

	// -2 is for "pc end" optimization

	fglSetPSParams(attribNum,
	pExe->fs.pcStart,
	(pExe->fs.binaryCodeSize >> 4) - 2,
	FGL_FALSE);

	//#else

	//  fglPSSetAttributeNum((pExe->numFSVarying) ? pExe->numFSVarying  : 1);


	//#endif


#endif

	// Copy fragment shader to instruction memory -------------------------


	Plat::memcpy_FIMG((void*) FGPS_INSTMEM_SADDR,
	(const void*) pShader->binaryCode,
	pShader->binaryCodeSize);

	// Copy constants ---------------------------------------------------------


	// Float consts

	Plat::memcpy_FIMG((void*) FGPS_CFLOAT_SADDR,
	(const void*) pShader->constFloatMemBlock,
	pShader->fmbSize);


	// Integer constants

	Plat::memcpy_FIMG((void*) FGPS_CINT_SADDR,
	(const void*) pShader->constIntMemBlock,
	pShader->imbSize);


	// Boolean constants

	Plat::memcpy_FIMG((void*) FGPS_CBOOL_SADDR,
	(const void*) pShader->constBoolMemBlock,
	pShader->bmbSize);

	return GLF_ERR_SUCCESS;
}

/*-----------------------------------------------------------------------*//*!

* Set the draw mode to primitive engine.

*

* \param [in] pState(OGLState)    Library context

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfSetDrawMode(OGLState *pState, GLenum mode)
{
	Executable *pExe = pState->current_executable;
	bool pointSprite = (pExe->pointCoordIndex != -1);
	FGL_VertexCtx fglVtx;

	FGL_BOOL VPPS;

	VPPS = fglVtx.enablePointSize = pState->VPPSEnable;

	switch(mode){

	case GL_POINTS:
		if(pointSprite && pState->is_point_sprite_enable)
		{
			fglVtx.prim = FGL_PRIM_POINT_SPRITE;
		}
		else
		{
			fglVtx.prim = FGL_PRIM_POINTS;
		}
		break;

	case GL_LINES:
		fglVtx.prim = FGL_PRIM_LINES;
		break;
	case GL_LINE_STRIP:
		fglVtx.prim = FGL_PRIM_LINE_STRIP;
		break;

	case GL_LINE_LOOP:
		fglVtx.prim = FGL_PRIM_LINE_LOOP;
		break;

	case GL_TRIANGLES:
		fglVtx.prim = FGL_PRIM_TRIANGLES;
		break;

	case GL_TRIANGLE_STRIP:
		fglVtx.prim = FGL_PRIM_TRIANGLE_STRIP;
		break;

	case GL_TRIANGLE_FAN:
		fglVtx.prim = FGL_PRIM_TRIANGLE_FAN;
		break;

	default:
		return GLF_ERR_ERROR;
	}

#ifdef EN_EXT_VARYING_INTERPOLATION_EXP
	fglVtx.varyingInterpolation = VPPS ? (pExe->varyingInterpolation & ~1) : pExe->varyingInterpolation>>1; //since varyingInterpolation uses lsb 0 for gl_position

#else
	fglVtx.varyingInterpolation = 0;    // Smooth shading

	// Not configurable in OpenGL ES 2.0

#endif //EN_EXT_VARYING_INTERPOLATION_EXP


	//fglVtx.numVSOut = pExe->numVSVarying - 1;   // Exclude position


	fglVtx.numVSOut = (VPPS) ? pExe->numVSVarying : pExe->numVSVarying -1   ;
	if(pointSprite) //Sandeep K

	{
		fglVtx.numVSOut++;
	}

	//fglVtx.numVSOut += 1;

	//if(fglVtx.numVSOut ==0){

	//  fglVtx.numVSOut = 1;

	//} //this alone is not woking

#ifdef FIMG_WRONG_INTERPOLATION_WORKAROUND
	if(pExe->numFSVarying > 4){
		fglVtx.numVSOut = 8;
	}
#endif

	fglSetVertexCtx(&fglVtx);

	//Config rasterizer //TODO: move me out?

	if(fglVtx.prim == FGL_PRIM_POINT_SPRITE)
	{
		WRITEREG(FGRA_COORD_REPLACE, (1<<(pExe->pointCoordIndex)));
	}
	else
	{
		WRITEREG(FGRA_COORD_REPLACE,0);
	}

	return GLF_ERR_SUCCESS;
}

/*-----------------------------------------------------------------------*//*!

* Set the primitive engine hardware.

*

* \param [in] pState(OGLState)    Library context

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfSetPrimEngine(OGLState *pState)
{

	unsigned int *ubits = &(pState->updateBits.pe);
#if  TRACE_DIRTY_STATE == ENABLE
	if(*ubits == GL_FALSE){
		return GLF_ERR_SUCCESS;
	}
#endif

	FramebufferData& fbData = pState->curFBData;//getFBData();


	// Depth range ------------------------------------------------------------


	if(fglSetDepthRange(pState->depth_range_data.near_depth,
				pState->depth_range_data.far_depth) != FGL_ERR_NO_ERROR){
		goto error_exit;
	}


	int x,y,width, height;
	x = pState->viewport_data.x;
	y = pState->viewport_data.y;
	width = pState->viewport_data.w;
	height = pState->viewport_data.h;

	//vieport specify the the window coordinate in which to map from the device coordinate space.Hence the below code is commented.

	/*

	if( x < 0) x = 0;

	if( y < 0) y = 0;



	if((unsigned int)(x + width) > fbData.width )

	{

		width = fbData.width - x;

	}



	if((unsigned int)(y + height) > fbData.height )

	{

		height = fbData.height - y;

	}



*/
	// Viewport

	if(fglSetViewportParams( fbData.flipped ? FGL_FALSE : FGL_TRUE,
				(float)x, (float)y, (float)width, (float)height,
				(float)fbData.height) != FGL_ERR_NO_ERROR) {
		goto error_exit;
	}


	// OLD Viewport Settings

	// Viewport ---------------------------------------------------------------

	/*  if(fglSetViewportParams(FGL_TRUE,

		(float)pState->viewport_data.x, (float)pState->viewport_data.y,

		(float)pState->viewport_data.w, (float)pState->viewport_data.h,

		(float)pState->viewport_data.h) != FGL_ERR_NO_ERROR) {

	goto error_exit;

	}



*/

	*ubits = GL_FALSE;
	return GLF_ERR_SUCCESS;

	error_exit:
	return GLF_ERR_ERROR;
}

/*-----------------------------------------------------------------------*//*!

* Set the raster engine hardware.

*

* \param [in] pState(OGLState)    Library context

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfSetRasterEngine(OGLState* pState)
{
	FG_BOOL enable, bCW;
	FGL_Face face;


	unsigned int *ubits = &(pState->updateBits.re);
#if  TRACE_DIRTY_STATE == ENABLE
	if(*ubits == GL_FALSE)
	{
		return GLF_ERR_SUCCESS;
	}
#endif
	const FramebufferData& fbData = pState->curFBData;//getFBData();

	float lineWidth = (float)floor(pState->line_width+0.5);
	if(lineWidth == 0.0)
	lineWidth = 1.0;

	int xmin,xmax,tymax,tymin,ymax,ymin; //actual viewport extents that will be used


	xmin = Plat::clamp(pState->viewport_data.x, (GLint)0, (GLint)fbData.width);
	tymin = Plat::clamp(pState->viewport_data.y,(GLint)0, (GLint)fbData.height);
	xmax =  Plat::clamp(pState->viewport_data.x + pState->viewport_data.w,  (GLint)0, (GLint)fbData.width);
	tymax = Plat::clamp(pState->viewport_data.y + pState->viewport_data.h,(GLint)0, (GLint)fbData.height);

	//Window X/Y-Clipping region control register

#if GLF_SCISSOR_IN_RA == ENABLE

	if(pState->scissor_test_data.is_enabled == GL_TRUE){

		const ScissorTestData& scissorData = pState->scissor_test_data;

		xmin = Plat::clamp(scissorData.x, xmin, xmax);
		tymin = Plat::clamp(scissorData.y,tymin, tymax);
		xmax = Plat::clamp(scissorData.x + scissorData.w, xmin, xmax);
		tymax = Plat::clamp(scissorData.y + scissorData.h, tymin, tymax);

		if(!fbData.flipped)
		{

			ymax = fbData.height - tymin ;  //-1 added then fail in some conditions as unsigned int.

			ymin = fbData.height - tymax ; //-1 added failed in glScissor(120,160,240, 320);

		}
		else
		{
			ymax = tymax;
			ymin = tymin;
		}

		fglSetXClip(xmin,xmax);
		fglSetYClip(ymin,ymax);
	}
	else{

		if(!fbData.flipped)
		{

			ymax = fbData.height - tymin ;  //-1 added then fail in some conditions as unsigned int.

			ymin = fbData.height - tymax ; //-1 added failed in glScissor(120,160,240, 320);

		}
		else
		{
			ymax = tymax;
			ymin = tymin;
		}

		fglSetXClip(xmin,xmax);
		fglSetYClip(ymin,ymax);



	}
#endif


	// polygon depth offset

	fglEnableDepthOffset(pState->polygon_offset_data.is_enabled_fill);
	if(pState->polygon_offset_data.is_enabled_fill)
	{
		fglSetDepthOffsetParamf(FGL_DEPTH_OFFSET_FACTOR, pState->polygon_offset_data.factor);
		fglSetDepthOffsetParamf(FGL_DEPTH_OFFSET_UNITS, pState->polygon_offset_data.units);
	}

	// moved to InitFimg() in libmain.cpp because the are hard coded.

#if 0

	// Pixel sampler ----------------------------------------------------------


	if(fglSetPixelSamplePos(FGL_SAMPLE_CENTER) != FGL_ERR_NO_ERROR)
	goto error_exit;
#endif
	// Front face and face culling --------------------------------------------

	{
		if(pState->front_face_mode == GL_CW)
		bCW = FGL_TRUE;
		else
		bCW = FGL_FALSE;

		if (pState->cull_face_data.is_enabled == GL_TRUE)
		enable = FGL_TRUE;
		else
		enable = FGL_FALSE;

		if(pState->cull_face_data.mode == GL_FRONT)
		face = FGL_FACE_FRONT;
		else if(pState->cull_face_data.mode == GL_BACK)
		face = FGL_FACE_BACK;
		else
		face = FGL_FACE_FRONT_AND_BACK;

		if(fglSetFaceCullControl(enable, bCW, face) != FGL_ERR_NO_ERROR)
		goto error_exit;
	}

	// LOD control ------------------------------------------------------------



	//if(fglSetLODControl(0x7) != FGL_ERR_NO_ERROR)

	if(fglSetLODControl((unsigned int)((1<<24)-1)) != FGL_ERR_NO_ERROR)
	goto error_exit;

	// line & point width

	WRITEREGF(FGRA_LWIDTH, lineWidth);

	WRITEREGF(FGRA_PWIDTH, (pState->point_size));


	*ubits = GL_FALSE;
	return GLF_ERR_SUCCESS;

	error_exit:
	return GLF_ERR_ERROR;
}




FGL_Error pickfglSetBlend (bool noAlphaBits, FGL_BOOL enable, pFGL_Blend blendParam )
{
	//FramebufferData fbData = getFBData();


	if (noAlphaBits)
	{
		return( fglSetBlendWorkAround(enable, blendParam));
	}
	else
	{
		return (fglSetBlend(enable, blendParam));
	}
}











/*-----------------------------------------------------------------------*//*!

* Set the per-fragment unit.

*

* \param [in] pState(OGLState)    Library context

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfSetPerFragUnit (OGLState *pState)
{
#if TRACE_DIRTY_STATE == ENABLE
	if(pState->updateBits.pf == GL_FALSE){
		return GLF_ERR_SUCCESS;
	}
#endif


	FGL_Stencil frontStencilParam, backStencilParam;


	//unsigned int *ubits = &(pState->updateBits.pf);


	const FramebufferData& fbData = pState->curFBData;
	const bool alphaExists = hasAlpha(fbData.nativeColorFormat);
	const bool noAlphaBits = !alphaExists;
	const bool colorExists = fbData.colorAddr.vaddr != 0;
	const bool depthExists = fbData.depthStencilAddr.vaddr != 0 && hasDepth(fbData.nativeDepthStencilFormat);
	const bool stencilExists = fbData.depthStencilAddr.vaddr != 0 && hasStencil(fbData.nativeDepthStencilFormat);

	#if GLF_SCISSOR_IN_RA == DISABLE

	const ScissorTestData& scissorData = pState->scissor_test_data;

	int xmin,xmax,tymax,tymin,ymax,ymin;

	xmin = Plat::min(Plat::max(scissorData.x,(GLint)0),(GLint)fbData.width);
	tymin = Plat::min(Plat::max(scissorData.y,(GLint)0),(GLint)fbData.height);
	xmax = Plat::min(Plat::max(scissorData.x + scissorData.w,(GLint)0), (GLint)fbData.width);
	tymax = Plat::min(Plat::max(scissorData.y + scissorData.h,(GLint)0), (GLint)fbData.height);

	if(!fbData.flipped)
	{

		ymax = fbData.height - tymin;
		ymin = fbData.height - tymax;
	}
	else
	{
		ymax = tymax;
		ymin = tymin;
	}
	// Scissor ---------------------------------------------------------------

	{

		// TODO : using fglSetXClip and fglSetYClip

		fglSetScissor((FGL_BOOL)pState->scissor_test_data.is_enabled,
		xmax, xmin, ymax, ymin);



	}
#endif

	// Alpha test ------------------------------------------------------------


	{
#ifdef EN_EXT_ALPHA_TEST_EXP

		if(pState->alpha_test_data.is_enabled == GL_TRUE) {
			fglSetAlpha(FGL_TRUE,
			static_cast<unsigned int>(255*clamp(pState->alpha_test_data.refValue)+0.5),
			CompareFunc(pState->alpha_test_data.func));
		}
		else
		fglSetAlpha(FGL_FALSE, 0, FGL_COMP_NEVER);

#else

		fglSetAlpha(FGL_FALSE, 0, FGL_COMP_NEVER);


#endif
	}

	// Stencil ---------------------------------------------------------------


	{


		if(pState->stencil_test_data.is_enabled == GL_TRUE && stencilExists) {
			frontStencilParam.zpass =
			StencilAction(pState->stencil_test_data.front_face_zpass);
			frontStencilParam.zfail =
			StencilAction(pState->stencil_test_data.front_face_zfail);
			frontStencilParam.sfail =
			StencilAction(pState->stencil_test_data.front_face_fail);
			frontStencilParam.mask =
			pState->stencil_test_data.front_face_mask & 0xFF; //mask

			frontStencilParam.ref =
			pState->stencil_test_data.front_face_ref & 0xFF;
			frontStencilParam.mode =
			CompareFunc(pState->stencil_test_data.front_face_func);

			backStencilParam.zpass =
			StencilAction(pState->stencil_test_data.back_face_zpass);
			backStencilParam.zfail =
			StencilAction(pState->stencil_test_data.back_face_zfail);
			backStencilParam.sfail =
			StencilAction(pState->stencil_test_data.back_face_fail);
			backStencilParam.mask =
			pState->stencil_test_data.back_face_mask  & 0xFF; //mask

			backStencilParam.ref =
			pState->stencil_test_data.back_face_ref  & 0xFF;
			backStencilParam.mode =
			CompareFunc(pState->stencil_test_data.back_face_func);
			fglSetFrontStencil(FGL_TRUE, &frontStencilParam);
			fglSetBackStencil(&backStencilParam);
		}
		else
		// don't care the garbage value

		fglSetFrontStencil(FGL_FALSE, &frontStencilParam);

	}

	// Depth test ------------------------------------------------------------

	{


		if(pState->is_enabled_depth == GL_TRUE && depthExists) {
			fglSetDepth(FGL_TRUE, CompareFunc(pState->depth_test_func));
		} else {
			fglSetDepth(FGL_FALSE, FGL_COMP_NEVER);
		}


	}

#ifdef EN_EXT_LOGIC_OP_EXP


	if(pState->is_logicOp_enabled == GL_TRUE && colorExists)
	{
		FGL_LogicalOp logicOp = LogicOp(pState->logicOp);
		FGL_Blend blendParam;

		fglSetLogicalOp(FGL_TRUE, logicOp, logicOp);
		// garbage blendParam value

		pickfglSetBlend(noAlphaBits,FGL_FALSE, &blendParam);
	}


	else
#endif
	{
		// Blend  ------------------------------------------------------------

		{


			FGL_Blend blendParam;
			if(pState->blend_data.is_enabled == GL_TRUE && colorExists) {

				unsigned int constColor =
				(((unsigned int)pState->blend_data.blnd_clr_red && 0xff)
				<< 24)
				| (((unsigned int)pState->blend_data.blnd_clr_green && 0xff)
				<< 16)
				| (((unsigned int)pState->blend_data.blnd_clr_blue && 0xff)
				<< 8)
				| ((unsigned int)pState->blend_data.blnd_clr_alpha && 0xff);

				// *Pra* Using the RGB mode equation as hardware supports only one

				switch(pState->blend_data.eqn_modeRGB){
				case GL_FUNC_ADD:
					blendParam.colorEqua = FGL_EQ_ADD;
					break;

				case GL_FUNC_SUBTRACT:
					blendParam.colorEqua = FGL_EQ_SUBTRACT;
					break;

				case GL_FUNC_REVERSE_SUBTRACT:
					blendParam.colorEqua = FGL_EQ_REV_SUBTRACT;
					break;
				}

				switch(pState->blend_data.eqn_modeAlpha){
				case GL_FUNC_ADD:
					blendParam.alphaEqua = FGL_EQ_ADD;
					break;

				case GL_FUNC_SUBTRACT:
					blendParam.alphaEqua = FGL_EQ_SUBTRACT;
					break;

				case GL_FUNC_REVERSE_SUBTRACT:
					blendParam.alphaEqua = FGL_EQ_REV_SUBTRACT;
					break;
				}

				blendParam.dstAlpha = BlendFunc(pState->blend_data.fn_dstAlpha);
				blendParam.dstColor = BlendFunc(pState->blend_data.fn_dstRGB);
				blendParam.srcAlpha = BlendFunc(pState->blend_data.fn_srcAlpha);
				blendParam.srcColor = BlendFunc(pState->blend_data.fn_srcRGB);

				pickfglSetBlend(noAlphaBits,FGL_TRUE, &blendParam);
				fglSetBlendColor(constColor);
			} else {
				pickfglSetBlend(noAlphaBits,FGL_FALSE, &blendParam);
			}


		}

		// Logical Operation -----------------------------------------------------

		{

			fglSetLogicalOp(FGL_FALSE, FGL_OP_COPY, FGL_OP_COPY);

		}
	}

	{
		unsigned int mask = 0;
		if(colorExists){
			if(!(pState->color_mask_data.a)) {
				mask |= 0x1;
			}
			if(!(pState->color_mask_data.b)) {
				mask |= 0x2;
			}
			if(!(pState->color_mask_data.g)) {
				mask |= 0x4;
			}
			if(!(pState->color_mask_data.r)) {
				mask |= 0x8;
			}
		}else {
			mask = 0x15;    //disable color writes

		}

		#ifdef FIMG_MASKING_WORKAROUND
		// FIMG requires that blending be enabled when mask is used

		if(mask && pState->blend_data.is_enabled == GL_FALSE)
		{

			// we don't neet to set const color

			FGL_Blend blendParam;
			blendParam.colorEqua = FGL_EQ_ADD;
			blendParam.alphaEqua = FGL_EQ_ADD;
			blendParam.dstAlpha = FGL_BLEND_ZERO;
			blendParam.dstColor = FGL_BLEND_ZERO;
			blendParam.srcAlpha = FGL_BLEND_ONE;
			blendParam.srcColor = FGL_BLEND_ONE;

			pickfglSetBlend(noAlphaBits,FGL_TRUE, &blendParam);

		}
#endif

		WRITEREG(FGPF_COLOR_MASK, mask);
	}


	fglSetDepthStencilWriteMask(~(pState->back_stencil_writemask) & 0xff, ~(pState->front_stencil_writemask) & 0xff,
	pState->depth_write_mask ? 0 : 1 );



	pState->updateBits.pf = GL_FALSE; //now the state updated so no more dirty bit

	return GLF_ERR_SUCCESS;

	//error: Commented to silence compiler

	//  return GLF_ERR_ERROR;

}

/*-----------------------------------------------------------------------*//*!

* Set the texture unit.

*

* \param [in] pState(OGLState)    Library context

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfSetTextureUnit (OGLState* pState)
{
	TextureState*   pTexState = &(pState->texState);
	//TextureObject*  pTexObj;

	TexStateToConfigFimg*        pTexStateToConfig;

	Executable *pExe = (pState->current_executable);
	const int  diffTexUnitRegSize = 0x50;
	int noOftexUnitsUsed = 0;
	for(int i = 0; i < MAX_TEXTURE_UNITS; i++) {

		if(pExe->fsSamplerMappings[i].isUsed == false){

#if RESET_UNUSED_TEXTURE_UNITS == ENABLE
			unsigned int* texRegAddr = (unsigned int  *)(FGTU_TEX0_CTRL + diffTexUnitRegSize * i);
			WRITEREG(texRegAddr++, 0x08000000 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
			WRITEREG(texRegAddr++, 0 );
#endif
			continue;
		}
		noOftexUnitsUsed++;

		pTexStateToConfig  = &(pState->texState.pTexObjectToConfigFimg[i]);

		//to check whether the register need to be updated or not (dirty state tracking)

#if TRACE_DIRTY_STATE == ENABLE
		if((pTexState->prevTexUnitBinding[i] == pTexStateToConfig->id)
				&& pTexStateToConfig->pTexFGLState->fglStateDirty == GL_FALSE)
		{
			continue;
		}
		else
		{
			pTexState->prevTexUnitBinding[i] = pTexStateToConfig->id;
		}
#endif


		//Plat::memcpy_FIMG((void*)(FGTU_TEX0_CTRL +(FGTU_TEX1_CTRL - FGTU_TEX0_CTRL) * i ) , pTexObj->texFGLState, sizeof(pTexObj->texFGLState) );

		unsigned int * texFGLData = pTexStateToConfig->pTexFGLState->texSFRtate ;
		unsigned int* texRegAddr = (unsigned int  *)(FGTU_TEX0_CTRL + diffTexUnitRegSize * i);
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );
		WRITEREG(texRegAddr++, *texFGLData++ );


		if(pTexStateToConfig->pTexFGLState->palEntryFGLState != NULL)
		{
			GLuint* tempPlaetetFGLState = pTexStateToConfig->pTexFGLState->palEntryFGLState;
			for(unsigned int x=0; x < pTexStateToConfig->pTexFGLState->noOfpalette ; x++)
			{

				WRITEREG(FGTU_PALETTE_ADDR, x);       // index

				WRITEREG(FGTU_PALETTE_ENTRY, *tempPlaetetFGLState++); // paletted color

			}
		}
	}


	//resetting the fgl dirty state for the used texture objects

	for( int j = 0, i = 0 ; j < noOftexUnitsUsed; i++) {
		if(pExe->fsSamplerMappings[i].isUsed == true){
			//pTexStateToConfig  = pTexStateToConfig];  // check

			pState->texState.pTexObjectToConfigFimg[i].pTexFGLState->fglStateDirty = GL_FALSE;
			j++;
		}
	}


	return GLF_ERR_SUCCESS;
}



/*-----------------------------------------------------------------------*//*!

* Get Attrib Info

*

* This function is to be called from before transfering the vertex attrib data

* It write the attrib information into attribInfo. This info is used to

* determine the optimal method to transfer the data to the GPU.

*

* \param [in] pState(OGLState)            Library context

* \param [out] attribInfo(AttribInfo*)      first as in glDrawArrays

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfGetAttribInfo(OGLState* pState, AttribInfo* attribInfo, bool analyzeAttribLayout)
{
	const char numAttribs = pState->current_executable->numAttribs;
	attribInfo->nonDwordAttribUsed = false;
	attribInfo->vboAttribSources = true; //denotes that all sources are from VBO.

	attribInfo->numAttribs = numAttribs;
	attribInfo->packedVertex = false;
	attribInfo->packedArray = false;
	attribInfo->vertexSize = 0;
	attribInfo->attribOrder[0] = 0x3020100;
	attribInfo->attribOrder[1] = 0x7060504;
	attribInfo->attribOrder[2] = 0xb0a0908;
	attribInfo->firstAttribSrc = 0;


	//-------------------------------------------------------------------------
	// Read attribute mappings
	//-------------------------------------------------------------------------


	for(int i = 0; i < numAttribs; i++) {

		VertexAttribArrayData *pAttribData = &(pState->vertex_attrib_array_data[pState->current_executable->attribMap[i]]);

		if(pAttribData->enabled) {
			// Check attrib data pointer

			if(pAttribData == NULL || pAttribData->ptr == NULL) {
				////LOGMSG("WARNING: null data \n");
				return GLF_ERR_ERROR;
			}

			if(pAttribData->vboID == 0){
				attribInfo->vboAttribSources = false;
			}
			else {
				if(pAttribData->vertex_attrib_ptr_binding != NULL)
				{
					if( (pAttribData->vertex_attrib_ptr_binding->in_use == GL_TRUE) &&
							(pAttribData->vertex_attrib_ptr_binding->id == pAttribData->vboID)&&
							(pAttribData->vertex_attrib_ptr_binding->mapped == GL_TRUE))
					{
						//Can't source data from a currently mapped buffer object

						set_err(GL_INVALID_OPERATION);
						return GLF_ERR_ERROR;
					}
				}
			}

			attribInfo->attribData[i].dataTranslation = DATA_TRANSLATION_NONE;

			switch(pAttribData->type){

			case GL_BYTE:
			case GL_UNSIGNED_BYTE:
				attribInfo->attribData[i].dataType = pAttribData->type;
				#ifdef FIMG_1_2_SHORT_VERTEX_DATA_WORKAROUND
				attribInfo->attribData[i].dataTranslation = DATA_TRANSLATION_BYTE_TO_FLOAT;
				attribInfo->attribData[i].dataType = GL_FLOAT;
				#endif
				attribInfo->attribData[i].dataSize = pAttribData->size * sizeof(GLbyte);
				//attribInfo->nonDwordAttribUsed = true;

				//////LOGMSG("( Byte %d ) ",pAttribData->size);

				break;

			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
				attribInfo->attribData[i].dataType = pAttribData->type;
				#ifdef FIMG_1_2_SHORT_VERTEX_DATA_WORKAROUND
				attribInfo->attribData[i].dataTranslation = DATA_TRANSLATION_SHORT_TO_FLOAT;
				attribInfo->attribData[i].dataType = GL_FLOAT;
				#endif
				attribInfo->attribData[i].dataSize = pAttribData->size * sizeof(GLshort);
				//attribInfo->nonDwordAttribUsed = true;

				//////LOGMSG("( Short %d ) ",pAttribData->size);

				break;

			case GL_INT:
			case GL_UNSIGNED_INT:
			case GL_FLOAT:
			case GL_FIXED:
				attribInfo->attribData[i].dataType = pAttribData->type;
				attribInfo->attribData[i].dataSize = pAttribData->size * sizeof(GLint);
				break;
			}

			//        if(attribInfo->attribData[i].dataSize % 4 != 0) {

			//Sandeep K. even if an attrib is DWord size, if it needs data translation, we set nonDwordAttribUsed to true so that optimized

			//    transfer routines are not used.

			if(attribInfo->attribData[i].dataSize % 4 != 0   || attribInfo->attribData[i].dataTranslation != DATA_TRANSLATION_NONE) {
				attribInfo->nonDwordAttribUsed = true;
			}

			if(pAttribData->stride != 0)
			attribInfo->attribData[i].stride = pAttribData->stride;
			else
			attribInfo->attribData[i].stride = attribInfo->attribData[i].dataSize;

			attribInfo->attribData[i].pData = (const GLubyte*)pAttribData->ptr;
		}
		else
		{
			attribInfo->vboAttribSources = false;
			attribInfo->attribData[i].dataSize = 4*sizeof(GLfloat);
			attribInfo->attribData[i].stride = 0;
			attribInfo->attribData[i].dataTranslation = DATA_TRANSLATION_NONE;
			attribInfo->attribData[i].pData = (const GLubyte*)&(pState->vertex_attribs[pState->current_executable->attribMap[i]]);
			attribInfo->attribData[i].dataType = GL_FLOAT;
		}
	}

	return GLF_ERR_SUCCESS;
}

enum { MIN_VERTS_FOR_ATTRIB_ANALYSIS = 64 }; //The right value depends on platform and needs to be measured..


inline void
glfTransfer_GenericVertex(const AttribInfo& attribInfo, GLint first, GLint count)
{
	for(int i = first; i < count+first; i++) {
		for(int j = 0; j < attribInfo.numAttribs; j++) {
			__fglSendToFIFO( attribInfo.attribData[j].dataSize,
			(void *)(attribInfo.attribData[j].pData + attribInfo.attribData[j].stride * i ),FGHI_FIFO_ENTRY);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
inline void
glfTransfer_IndependentPrimitives(OGLState* pState, const AttribInfo& attribInfo, GLenum primitive, GLint first, GLsizei count)
{
	const unsigned int pipemask = FGL_PIPESTATE_HI_TILL_PS;
	const GLint indices[2][3] = { {0,1,2},{1,0,2} };

	//Configure default input attrib order

	WRITEREG(FGVS_IN_ATTRIB_IDX0, 0x3020100);
	WRITEREG(FGVS_IN_ATTRIB_IDX1, 0x7060504);
	WRITEREG(FGVS_IN_ATTRIB_IDX2, 0xb0a0908);

	//Handle triangle primitives by converting tri strips, tri fans into tri lists

	if(primitive == GL_TRIANGLES)
	{

		for(GLint i=first; i < (first + count);  i+=3 /*jump to next tri*/)
		{
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)3); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);
			glfTransfer_GenericVertex(attribInfo, i, 3);
		}
	} else if(primitive == GL_TRIANGLE_STRIP)
	{
		glfSetDrawMode(pState, GL_TRIANGLES);

		for(GLint i=first; i < (first+count-2);  i++)
		{
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)3); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);

			glfTransfer_GenericVertex(attribInfo, i+indices[i&0x1][0],1);
			glfTransfer_GenericVertex(attribInfo, i+indices[i&0x1][1],1);
			glfTransfer_GenericVertex(attribInfo, i+indices[i&0x1][2],1);

		}
	} else if(primitive == GL_TRIANGLE_FAN)
	{
		glfSetDrawMode(pState, GL_TRIANGLES);

		for(GLint i=first+1 /*2nd vertex*/; i < (first+count-1); i++)
		{
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)3); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);
			glfTransfer_GenericVertex(attribInfo, first, 1);
			glfTransfer_GenericVertex(attribInfo,     i, 2);
		}
	} //Handle line primitives by convertings line strips, loops into line segments

	else if(primitive == GL_LINES)
	{

		for(GLint i=first; i < (first+count);  i+=2 /*jump to next line*/)
		{
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)2); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);
			glfTransfer_GenericVertex(attribInfo, i, 2);
		}
	} else if(primitive == GL_LINE_STRIP || primitive == GL_LINE_LOOP)
	{
		glfSetDrawMode(pState, GL_LINES);

		for(GLint i=first; i< (first+count-1); i++)
		{
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)2); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);
			glfTransfer_GenericVertex(attribInfo, i, 2);
		}
		if(primitive == GL_LINE_LOOP)
		{ // transfer the last segment too

			//fglFlush(pipemask);
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)2); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);
			glfTransfer_GenericVertex(attribInfo, first+count-1, 1);
			glfTransfer_GenericVertex(attribInfo,         first, 1);
		}
	} else if(primitive == GL_POINTS)
	{

		for(GLint i=first; i< (first + count); i++)
		{
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)1); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);

			glfTransfer_GenericVertex(attribInfo, i, 1);
		}
	}
}

template<typename T> inline void
glfTransfer_IndependentIndexedPrimitives(OGLState* pState, const AttribInfo& attribInfo, GLenum primitive, const T* indices, GLsizei count)
{
	const unsigned int pipemask = FGL_PIPESTATE_HI_TILL_PS;
	const GLint idx[2][3] = { {0,1,2},{1,0,2} };

	//Configure default input attrib order

	WRITEREG(FGVS_IN_ATTRIB_IDX0, 0x3020100);
	WRITEREG(FGVS_IN_ATTRIB_IDX1, 0x7060504);
	WRITEREG(FGVS_IN_ATTRIB_IDX2, 0xb0a0908);

	const T first = 0;

	//Handle triangle primitives by converting tri strips, tri fans into tri lists

	if(primitive == GL_TRIANGLES) {

		for(GLint i=first; i < count;  i+=3 /*jump to next tri*/) {
			//fglFlush(pipemask);
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)3); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);
			glfTransfer_GenericVertex(attribInfo, indices[i]  , 1);
			glfTransfer_GenericVertex(attribInfo, indices[i+1], 1);
			glfTransfer_GenericVertex(attribInfo, indices[i+2], 1);
		}
	} else if(primitive == GL_TRIANGLE_STRIP) {
		glfSetDrawMode(pState, GL_TRIANGLES);
		for(GLint i=first; i < count-2;  i++) {
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)3); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);

			glfTransfer_GenericVertex(attribInfo, indices[i+idx[i&0x1][0]],1);
			glfTransfer_GenericVertex(attribInfo, indices[i+idx[i&0x1][1]],1);
			glfTransfer_GenericVertex(attribInfo, indices[i+idx[i&0x1][2]],1);

		}
	} else if(primitive == GL_TRIANGLE_FAN) {
		glfSetDrawMode(pState, GL_TRIANGLES);
		for(GLint i=first+1 /*2nd vertex*/; i < count-1; i++) {
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)3); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);
			glfTransfer_GenericVertex(attribInfo, indices[first], 1);
			glfTransfer_GenericVertex(attribInfo,     indices[i], 1);
			glfTransfer_GenericVertex(attribInfo,   indices[i+1], 1);
		}
	} //Handle line primitives by convertings line strips, loops into line segments

	else if(primitive == GL_LINES) {

		for(GLint i=first; i < count;  i+=2 /*jump to next line*/) {
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)2); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);
			glfTransfer_GenericVertex(attribInfo,   indices[i], 1);
			glfTransfer_GenericVertex(attribInfo, indices[i+1], 1);
		}
	} else if(primitive == GL_LINE_STRIP || primitive == GL_LINE_LOOP) {
		glfSetDrawMode(pState, GL_LINES);

		for(GLint i=first; i< count-1; i++) {
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)2); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);
			glfTransfer_GenericVertex(attribInfo,   indices[i], 1);
			glfTransfer_GenericVertex(attribInfo, indices[i+1], 1);
		}
		if(primitive == GL_LINE_LOOP) { // transfer the last segment too
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)2); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);
			glfTransfer_GenericVertex(attribInfo, indices[first+count-1], 1);
			glfTransfer_GenericVertex(attribInfo,         indices[first], 1);
		}
	} else if(primitive == GL_POINTS){

		for(GLint i=first; i< count; i++) {
			WRITEREG(FGHI_FIFO_ENTRY, (unsigned int)1); WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);
			glfTransfer_GenericVertex(attribInfo,indices[i], 1);
		}
	}
}

/*-----------------------------------------------------------------------*//*!

* Draw (Unbuffered).

*

* This function is to be called from glDrawArrays(/glDrawElements?)

* while trasfering attributes

* to the host interface in un-buffered mode.

*

* \param [in] pState(OGLState)    Library context

* \param [in] first(GLint)      first as in glDrawArrays

* \param [in] count(GLint)      count as in glDrawArrays

*

* \retval glfError        Result code (GLF_ERR_ERROR or SUCCESS)

*//*------------------------------------------------------------------------*/

glfError
glfDraw(OGLState* pState, GLenum primitive, GLint first, GLsizei count)
{
	AttribInfo attribInfo;
	gAssert(first >= 0 && count >= 0);
	if(glfGetAttribInfo(pState, &attribInfo, 0) != GLF_ERR_SUCCESS){
		return GLF_ERR_ERROR;
	}

	fglFlush(FGL_PIPESTATE_ALL);

	const bool reorder = false;
	glfSetHostIf(pState,&attribInfo, reorder , GL_FALSE);

	glfTransfer_IndependentPrimitives(pState,  attribInfo, primitive, first, count);

	return GLF_ERR_SUCCESS;
}


template<typename T> glfError
glfDrawElements(OGLState* pState, GLenum primitive, GLsizei count, const T* indices)
{
//	gettimeofday(&__start,NULL);/////////////////////////////
	AttribInfo attribInfo;
	gAssert(count >= 0);

	if(indices == 0 || count == 0) {
		return GLF_ERR_SUCCESS;
	}

	if(glfGetAttribInfo(pState, &attribInfo, 0 ) != GLF_ERR_SUCCESS){
		return GLF_ERR_ERROR;
	}

	//Wait till FIMG is free

	fglFlush(FGL_PIPESTATE_ALL);
	const bool reorder=false;
	glfSetHostIf(pState,&attribInfo, reorder , GL_FALSE);

	glfTransfer_IndependentIndexedPrimitives(pState,  attribInfo, primitive, indices, count);

//	gettimeofday(&__end,NULL);///////////////////////////
//	LOGE("%s completed in: %5.5f ms",__FUNCTION__,__test_tval(__start,__end));
	return GLF_ERR_SUCCESS;
}

glfError
glfDrawElements(OGLState* pState, GLenum primitive, GLsizei count, GLenum type, const void* indices)
{
	if(type == GL_UNSIGNED_INT) {
		return glfDrawElements<GLuint>(pState, primitive, count, static_cast<const GLuint*>(indices));
	}else if(type == GL_UNSIGNED_SHORT) {
		return glfDrawElements<GLushort>(pState, primitive, count, static_cast<const GLushort*>(indices));
	}else if(type == GL_UNSIGNED_BYTE) {
		return glfDrawElements<GLubyte>(pState, primitive, count, static_cast<const GLubyte*>(indices));
	}

	return GLF_ERR_SUCCESS;
}


/*

*******************************************************************************

* Local Functions

*******************************************************************************

*/

/*-----------------------------------------------------------------------*//*!

* Translate stencil action from GLenum to stencil action accepted by register

* level api

*

* \param  [in] action(GLenum) OpenGL stencil action

*

* \retval FGL_StencilAct    Stencil action accepted by register level api

*

*//*------------------------------------------------------------------------*/

static FGL_StencilAct
StencilAction (GLenum action)
{
	FGL_StencilAct result = (FGL_StencilAct) 0;

	switch (action){
	case GL_KEEP:
		result = FGL_ACT_KEEP;
		break;

	case GL_ZERO:
		result = FGL_ACT_ZERO;
		break;

	case GL_REPLACE:
		result = FGL_ACT_REPLACE;
		break;

	case GL_INCR:
		result = FGL_ACT_INCR;
		break;

	case GL_DECR:
		result = FGL_ACT_DECR;
		break;

	case GL_INVERT:
		result = FGL_ACT_INVERT;
		break;

	case GL_INCR_WRAP:
		result = FGL_ACT_INCRWRAP;
		break;

	case GL_DECR_WRAP:
		result = FGL_ACT_DECRWRAP;
		break;
	}

	return result;
}


/*-----------------------------------------------------------------------*//*!

* Translate compare function from GLenum to compare function accepted

* by register level api

*

* \param  [in] action(GLenum) OpenGL compare function

*

* \retval FGL_CompareFunc   FimG compare function

*

*//*------------------------------------------------------------------------*/

static FGL_CompareFunc
CompareFunc (GLenum func)
{
	FGL_CompareFunc result = (FGL_CompareFunc) 0;

	switch(func){
	case GL_NEVER:
		result = FGL_COMP_NEVER;
		break;

	case GL_ALWAYS:
		result = FGL_COMP_ALWAYS;
		break;

	case GL_LESS:
		result = FGL_COMP_LESS;
		break;

	case GL_LEQUAL:
		result = FGL_COMP_LEQUAL;
		break;

	case GL_EQUAL:
		result = FGL_COMP_EQUAL;
		break;

	case GL_GREATER:
		result = FGL_COMP_GREATER;
		break;

	case GL_GEQUAL:
		result = FGL_COMP_GEQUAL;
		break;

	case GL_NOTEQUAL:
		result = FGL_COMP_NOTEQUAL;
		break;

	}

	return result;
}


/*-----------------------------------------------------------------------*//*!

* Translate blend function from GLenum to blend function accepted

* by register level api

*

* \param  [in] action(GLenum) OpenGL blend function

*

* \retval FGL_CompareFunc   FimG blend function

*

*//*------------------------------------------------------------------------*/

static FGL_BlendFunc
BlendFunc(GLenum func)
{
	FGL_BlendFunc result = (FGL_BlendFunc) 0;

	switch(func) {
	case GL_ZERO:
		result = FGL_BLEND_ZERO;
		break;

	case GL_ONE:
		result = FGL_BLEND_ONE;
		break;

	case GL_SRC_COLOR:
		result = FGL_BLEND_SRC_COLOR;
		break;

	case GL_ONE_MINUS_SRC_COLOR:
		result = FGL_BLEND_ONE_MINUS_SRC_COLOR;
		break;

	case GL_DST_COLOR:
		result = FGL_BLEND_DST_COLOR;
		break;

	case GL_ONE_MINUS_DST_COLOR:
		result = FGL_BLEND_ONE_MINUS_DST_COLOR;
		break;

	case GL_SRC_ALPHA:
		result = FGL_BLEND_SRC_ALPHA;
		break;

	case GL_ONE_MINUS_SRC_ALPHA:
		result = FGL_BLEND_ONE_MINUS_SRC_ALPHA;
		break;

	case GL_DST_ALPHA:
		result = FGL_BLEND_DST_ALPHA;
		break;

	case GL_ONE_MINUS_DST_ALPHA:
		result = FGL_BLEND_ONE_MINUS_DST_ALPHA;
		break;

	case GL_CONSTANT_COLOR:
		result = FGL_BLEND_CONSTANT_COLOR;
		break;

	case GL_ONE_MINUS_CONSTANT_COLOR:
		result = FGL_BLEND_ONE_MINUS_CONSTANT_COLOR;
		break;

	case GL_CONSTANT_ALPHA:
		result = FGL_BLEND_CONSTANT_ALPHA;
		break;

	case GL_ONE_MINUS_CONSTANT_ALPHA:
		result = FGL_BLEND_ONE_MINUS_CONSTANT_ALPHA;
		break;

	case GL_SRC_ALPHA_SATURATE:
		result = FGL_BLEND_SRC_ALPHA_SATURATE;
		break;
	}

	return result;
}

/*-----------------------------------------------------------------------*//*!

* Translate logic op from GLenum to logic op accepted by register level api

*

* \param  [in] op(GLenum) OpenGL logic op

*

* \retval FGL_LogicalOp       FimG logic op

*

*//*------------------------------------------------------------------------*/

static FGL_LogicalOp
LogicOp(GLenum op)
{
	FGL_LogicalOp result = (FGL_LogicalOp)0;
	switch(op)
	{
	case GL_CLEAR_EXP:          result = FGL_OP_CLEAR; break;
	case GL_AND_EXP:            result = FGL_OP_AND; break;
	case GL_AND_REVERSE_EXP:    result = FGL_OP_AND_REVERSE; break;
	case GL_COPY_EXP:           result = FGL_OP_COPY; break;
	case GL_AND_INVERTED_EXP:   result = FGL_OP_AND_INVERTED; break;
	case GL_NOOP_EXP:           result = FGL_OP_NOOP; break;
	case GL_XOR_EXP:            result = FGL_OP_XOR; break;
	case GL_OR_EXP:             result = FGL_OP_OR; break;
	case GL_NOR_EXP:            result = FGL_OP_NOR; break;
	case GL_EQUIV_EXP:          result = FGL_OP_EQUIV; break;
	case GL_INVERT_EXP:         result = FGL_OP_INVERT; break;
	case GL_OR_REVERSE_EXP:     result = FGL_OP_OR_REVERSE; break;
	case GL_COPY_INVERTED_EXP:  result = FGL_OP_COPY_INVERTED; break;
	case GL_OR_INVERTED_EXP:    result = FGL_OP_OR_INVERTED; break;
	case GL_NAND_EXP:           result = FGL_OP_NAND; break;
	case GL_SET_EXP:            result = FGL_OP_SET; break;
	default:
		gAssert(0&&"Unexpected logicop!");
	}
	return result;
}



/*-----------------------------------------------------------------------*//*!

* Translate attribute type from OpenGL type to FGL enum

*//*------------------------------------------------------------------------*/

static FGL_AttribDataType
AttribType (GLenum type, GLboolean normalized)
{
	FGL_AttribDataType result = (FGL_AttribDataType) 0;

	if(normalized == GL_TRUE) {

		// Normalized


		switch(type) {
		case GL_BYTE:
			result = FGL_ATTRIB_DATA_NBYTE;
			break;

		case GL_UNSIGNED_BYTE:
			result = FGL_ATTRIB_DATA_NUBYTE;
			break;

		case GL_SHORT:
			result = FGL_ATTRIB_DATA_NSHORT;
			break;

		case GL_UNSIGNED_SHORT:
			result = FGL_ATTRIB_DATA_NUSHORT;
			break;

		case GL_INT:
			result = FGL_ATTRIB_DATA_NINT;
			break;

		case GL_UNSIGNED_INT:
			result = FGL_ATTRIB_DATA_NUINT;
			break;

		case GL_FLOAT:
			result = FGL_ATTRIB_DATA_FLOAT;
			break;

		case GL_FIXED:
			result = FGL_ATTRIB_DATA_NFIXED;
			break;
		}

	}else {

		// Not normalized


		switch(type) {
		case GL_BYTE:
			#ifdef FIMG_1_2_SHORT_VERTEX_DATA_WORKAROUND
			result = FGL_ATTRIB_DATA_FLOAT;
			#else
			result = FGL_ATTRIB_DATA_BYTE;
			#endif
			break;

		case GL_UNSIGNED_BYTE:
			#ifdef FIMG_1_2_SHORT_VERTEX_DATA_WORKAROUND
			result = FGL_ATTRIB_DATA_FLOAT;
			#else
			result = FGL_ATTRIB_DATA_UBYTE;
			#endif
			break;

		case GL_SHORT:
			#ifdef FIMG_1_2_SHORT_VERTEX_DATA_WORKAROUND
			result = FGL_ATTRIB_DATA_FLOAT;
			#else
			result = FGL_ATTRIB_DATA_SHORT;
			#endif
			break;

		case GL_UNSIGNED_SHORT:
			#ifdef FIMG_1_2_SHORT_VERTEX_DATA_WORKAROUND
			result = FGL_ATTRIB_DATA_FLOAT;
			#else
			result = FGL_ATTRIB_DATA_USHORT;
			#endif
			break;

		case GL_INT:
			result = FGL_ATTRIB_DATA_INT;
			break;

		case GL_UNSIGNED_INT:
			result = FGL_ATTRIB_DATA_UINT;
			break;

		case GL_FLOAT:
			result = FGL_ATTRIB_DATA_FLOAT;
			break;

		case GL_FIXED:
			result = FGL_ATTRIB_DATA_FIXED;
			break;
		}
	}

	return result;
}
