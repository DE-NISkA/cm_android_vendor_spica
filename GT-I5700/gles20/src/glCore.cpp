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
* \file         glCore.cpp
* \author       Sandeep Kakarlapudi (s.kakarla@samsung.com)
*                       Anurag Ved (anuragv@samsung.com)
* \brief        Implementation of Core functionality of GL
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*       07.08.2006      Sandeep Kakarlapudi             Initial version from OGL2_glfimg.c
*
*******************************************************************************
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//TODO: modify the includes
#include "glState.h"
#include "glFimg.h"
//#include "egl.h"
#include "buffers.h"
#include "pixel.h"
#include "platform.h"

#include "glprof.h"

#include <cutils/log.h>
#include <time.h>

/*
*******************************************************************************
* Global Variable Definitions
*******************************************************************************
*/

#ifndef MULTI_CONTEXT
OGLState *pgles2State; 
#endif

static GLuint gContextIDCounter = 2000;

#ifdef CACHE_MEM 
extern void invalidate_cached_buffer(void);
extern void clean_cached_buffer(void);
extern void clean_invalidate_cached_buffer(void);
#endif

extern int fimgPowerInit(void);
extern int fimgPowerStatus(void);
extern int lock3DCriticalSection(void);
extern int unlock3DCriticalSection(void);



bool stateFlush( OGLState * ctx);

//TODO: Need a cleanup function

static void gfSetFrameBuffer(OGLState* ctx, GLint Width, GLint Height)
{
	//      GET_GL_STATE(ctx);

	ctx->viewport_data.x = 0;
	ctx->viewport_data.y = 0;
	ctx->viewport_data.w =  Width;
	ctx->viewport_data.h =  Height;

	ctx->scissor_test_data.w = Width;
	ctx->scissor_test_data.h = Height;
	ctx->scissor_test_data.x        = 0;
	ctx->scissor_test_data.y        = 0;
}

static void set_default_values(OGLState* ctx)
{
	ctx->version = 20;
	ctx->id      = gContextIDCounter++;
	ctx->init_dimensions = GL_TRUE;
	
	//   GLint W_WIDTH = ctx->defFBData.width;
	//   GLint W_HEIGHT = ctx->defFBData.height;

	ctx->sharedState = NULL;

	for(int i=0; i<MAX_VERTEX_ATTRIBS; i++)
	{
		ctx->vertex_attribs[i][0] = ctx->vertex_attribs[i][1] = ctx->vertex_attribs[i][2] = 0.0f;
		ctx->vertex_attribs[i][3] = 1.0f;
	}

	for(int i=0; i<MAX_VERTEX_ATTRIBS; i++)
	{

		ctx->vertex_attrib_array_data[i].size       = 4;
		ctx->vertex_attrib_array_data[i].type       = GL_FLOAT;
		ctx->vertex_attrib_array_data[i].normalized = GL_FALSE;
		ctx->vertex_attrib_array_data[i].stride     = 0;
		ctx->vertex_attrib_array_data[i].ptr         = NULL;
		ctx->vertex_attrib_array_data[i].enabled = GL_FALSE;
		ctx->vertex_attrib_array_data[i].vboID = 0;
		ctx->vertex_attrib_array_data[i].vertex_attrib_ptr_binding = NULL;
	}


	Plat::memset(&(ctx->buffer_object_data),0,sizeof(BufferObjectData));

	ctx->depth_range_data.near_depth =0.0f;
	ctx->depth_range_data.far_depth =1.0f;
	/*
	ctx->viewport_data.x = 0;
	ctx->viewport_data.y = 0;
	ctx->viewport_data.w =  W_WIDTH;
	ctx->viewport_data.h =  W_HEIGHT;
*/
	ctx->front_face_mode = GL_CCW;

	ctx->point_size = 1.0f;

	ctx->line_width  = 1.0f;

	ctx->cull_face_data.is_enabled = GL_FALSE;
	ctx->cull_face_data.mode        = GL_BACK;

	ctx->polygon_offset_data.factor = 0.0;
	ctx->polygon_offset_data.units = 0.0;
	ctx->polygon_offset_data.is_enabled_fill = GL_FALSE;

	ctx->pixel_store_data.pack_alignment = 4;
	ctx->pixel_store_data.unpack_alignment = 4;

	ctx->sample_coverage_data.is_enabled_alpha_to_coverage = GL_FALSE;
	ctx->sample_coverage_data.is_enabled_coverage = GL_FALSE;
	ctx->sample_coverage_data.value = 1.0f;
	ctx->sample_coverage_data.invert = GL_FALSE;

	ctx->stencil_test_data.is_enabled = GL_FALSE;

	ctx->stencil_test_data.front_face_func = GL_ALWAYS;
	ctx->stencil_test_data.front_face_mask = ~0;
	ctx->stencil_test_data.front_face_ref    = 0;
	ctx->stencil_test_data.front_face_fail    = GL_KEEP;
	ctx->stencil_test_data.front_face_zfail  = GL_KEEP;
	ctx->stencil_test_data.front_face_zpass = GL_KEEP;

	ctx->stencil_test_data.back_face_func = GL_ALWAYS;
	ctx->stencil_test_data.back_face_mask = ~0;
	ctx->stencil_test_data.back_face_ref    = 0;
	ctx->stencil_test_data.back_face_fail    = GL_KEEP;
	ctx->stencil_test_data.back_face_zfail  = GL_KEEP;
	ctx->stencil_test_data.back_face_zpass = GL_KEEP;


	ctx->blend_data.is_enabled = GL_FALSE;
	ctx->blend_data.fn_srcRGB = GL_ONE;
	ctx->blend_data.fn_srcAlpha = GL_ONE;
	ctx->blend_data.fn_dstRGB = GL_ZERO;
	ctx->blend_data.fn_dstAlpha = GL_ZERO;
	ctx->blend_data.eqn_modeRGB = GL_FUNC_ADD;
	ctx->blend_data.eqn_modeAlpha = GL_FUNC_ADD;

	ctx->blend_data.blnd_clr_alpha = 0;
	ctx->blend_data.blnd_clr_blue = 0;
	ctx->blend_data.blnd_clr_green = 0;
	ctx->blend_data.blnd_clr_red = 0;

	ctx->scissor_test_data.is_enabled = GL_FALSE;
	/*   ctx->scissor_test_data.w          = W_WIDTH;
	ctx->scissor_test_data.h            = W_HEIGHT;
	ctx->scissor_test_data.x        = 0;
	ctx->scissor_test_data.y        = 0;
*/

	ctx->depth_test_func = GL_LESS;
	ctx->is_enabled_depth = GL_FALSE;

	ctx->depth_write_mask = GL_TRUE;

	ctx->front_stencil_writemask = ~0;
	ctx->back_stencil_writemask = ~0;

	ctx->color_mask_data.r = GL_TRUE;
	ctx->color_mask_data.g = GL_TRUE;
	ctx->color_mask_data.b = GL_TRUE;
	ctx->color_mask_data.a = GL_TRUE;

	ctx->clear_color_data.r = 0;
	ctx->clear_color_data.g = 0;
	ctx->clear_color_data.b = 0;
	ctx->clear_color_data.a = 0;

	ctx->clear_depth = 1.0f;

	ctx->clear_stencil = 0;
	ctx->is_enabled_dither = GL_TRUE;
	ctx->is_enabled_vertex_program_point_size = GL_FALSE;

	ctx->hint_data.generate_mipmap_hint                 = GL_DONT_CARE;
	ctx->hint_data.fragment_shader_derivative_hint = GL_DONT_CARE;

	//  ctx->shader_and_program_data;  //TODO: verify this is to be set to zero
	ctx->current_program = 0;

	ctx->error = GL_NO_ERROR;

	ctx->str_data.version = "2.0"; //not used, instead GLES2_VERSION_STRING is used
	ctx->str_data.vendor = "Samsung Electronics";
	ctx->str_data.renderer = "FIMG";
	ctx->str_data.extensions = "OES_read_format OES_compressed_paletted_texture OES_framebuffer_object OES_stencil8 OES_texture_compression_S3TC OES_mapbuffer OES_texture_npot " ;
	ctx->str_data.shadingLangVersion = "1.00";

#ifdef EN_EXT_ALPHA_TEST_EXP

	ctx->alpha_test_data.func       = GL_ALWAYS;
	ctx->alpha_test_data.refValue   = 0.0f;
	ctx->alpha_test_data.is_enabled = false;

#endif

#ifdef EN_EXT_LOGIC_OP_EXP
	ctx->logicOp                     = GL_COPY_EXP;
	ctx->is_logicOp_enabled          = GL_FALSE;
#endif

#ifdef EN_EXT_POINT_SPRITE_EXP
	ctx->is_point_sprite_enable = GL_TRUE; //for default case this should be always true
#endif
	//clear out temporary data;
	Plat::memset(ctx->tempStrBuff,0,GLF_TEMP_STRING_BUFFER_LENGTH);
	ctx->tempInt4[0] = ctx->tempInt4[1] = ctx->tempInt4[2] = ctx->tempInt4[3] = 0;
	ctx->tempFloat4[0] = ctx->tempFloat4[1] = ctx->tempFloat4[2] = ctx->tempFloat4[3] = 0.0f;
	
	Plat::memset(&(ctx->prevFBData), 0, sizeof(FramebufferData));
	Plat::memset(&(ctx->curFBData),  0, sizeof(FramebufferData));

	ctx->VPPSEnable = false;
	gfSetFrameBuffer(ctx,0, 0);

}

#ifdef USE_3D_PM
GLboolean PM_GLES2SetSurfaceData(GLES2SurfaceData * surfdata);

extern "C" GLboolean GLES2SetSurfaceData(GLES2SurfaceData * surfdata)
{
	GLboolean retVal = GL_FALSE;
	lock3DCriticalSection();
	retVal = PM_GLES2SetSurfaceData(surfdata);
	unlock3DCriticalSection();
	return retVal;
}

GLboolean PM_GLES2SetSurfaceData(GLES2SurfaceData * surfdata)
#else
extern "C" GLboolean GLES2SetSurfaceData(GLES2SurfaceData * surfdata)
#endif

{
	if(surfdata)
	{
		GET_GL_STATE(ctx);
		if(!ctx)
		{
			//LOGMSG("ERROR: ctx is 0 while trying to set SurfaceData");
		}

		//FimgFinish(ctx); 

#if 0
		FramebufferData* fbData;
		fbData = (FramebufferData*)Plat::malloc(sizeof(FramebufferData));
		
		fbData->colorAddr.vaddr = surfdata->colorAddr2.vaddr;
		fbData->colorAddr.paddr = surfdata->colorAddr2.paddr;
		fbData->depthStencilAddr.vaddr = surfdata->depthStencilAddr.vaddr;
		fbData->depthStencilAddr.paddr = surfdata->depthStencilAddr.paddr;
		fbData->width = surfdata->width;
		fbData->height = surfdata->height;
		fbData->nativeColorFormat = surfdata->nativeColorFormat;
		fbData->nativeDepthStencilFormat = surfdata->nativeDepthStencilFormat;
		fbData->flipped = surfdata->flipped;
#endif
		//Plat::printf("id %d egl make current = %p \n" ,pthread_self(), surfdata->colorAddr.paddr);
		Plat::memcpy(&(ctx->defFBData), surfdata, sizeof(FramebufferData));

#if 0   
		ctx->colorBuff1.vaddr = surfdata->colorAddr1.vaddr;
		ctx->colorBuff1.paddr = surfdata->colorAddr1.paddr;
		ctx->colorBuff2.vaddr = surfdata->colorAddr2.vaddr;
		ctx->colorBuff2.paddr = surfdata->colorAddr2.paddr;
		ctx->colorBuff3.vaddr = surfdata->colorAddr3.vaddr;
		ctx->colorBuff3.paddr = surfdata->colorAddr3.paddr;

#endif
		if(ctx->init_dimensions)
		{
			gfSetFrameBuffer(ctx,ctx->defFBData.width,ctx->defFBData.height);
			ctx->init_dimensions = GL_FALSE;
		}
	}
	return GL_TRUE;
}

extern "C" GLboolean GLES2SetDrawSurface(GLES2SurfaceData *surface)
{
	if(surface)
	{
		GET_GL_STATE(ctx);
		
		if(!ctx)
		{
			//LOGMSG("ERROR: ctx is 0 while trying to set DrawSurfaceData");
		}
		
		//FimgFinish(ctx);      // 090827 : "color touch lite" app bug patch

		Plat::memcpy(&(ctx->defFBData), surface, sizeof(FramebufferData));
		if(ctx->init_dimensions)
		{
			gfSetFrameBuffer(ctx,ctx->defFBData.width, ctx->defFBData.height);
			ctx->init_dimensions = GL_FALSE;
		}
	}
	return GL_TRUE;
}

#ifdef USE_3D_PM
GLboolean PM_GLES2SetReadSurface(GLES2SurfaceData *surface);

extern "C" GLboolean GLES2SetReadSurface(GLES2SurfaceData *surface)
{
	GLboolean retVal = GL_FALSE;
	lock3DCriticalSection();
	retVal =  PM_GLES2SetReadSurface(surface);
	unlock3DCriticalSection();
	return retVal;
}

GLboolean PM_GLES2SetReadSurface(GLES2SurfaceData *surface)
#else
extern "C" GLboolean GLES2SetReadSurface(GLES2SurfaceData *surface)
#endif
{
	if(surface)
	{
		GET_GL_STATE(ctx);
		
		if(!ctx)
		{
			//LOGMSG("ERROR: ctx is 0 while trying to set DrawSurfaceData");
		}

		//FimgFinish(ctx);      // 090827 : "color touch lite" app bug patch

		Plat::memcpy(&(ctx->readFBData), surface, sizeof(FramebufferData));
	}
	return GL_TRUE;
}

#ifdef USE_3D_PM
GLES2Context PM_GLES2CreateContext(GLES2Context sharedctx);

extern "C" GLES2Context GLES2CreateContext(GLES2Context sharedctx)
{
	GLES2Context retVal = NULL;
	lock3DCriticalSection();
	retVal = PM_GLES2CreateContext(sharedctx);
	unlock3DCriticalSection();
	return retVal;
}

GLES2Context PM_GLES2CreateContext(GLES2Context sharedctx)
#else
extern "C" GLES2Context GLES2CreateContext(GLES2Context sharedctx)
#endif
{

#ifndef MULTI_CONTEXT
	if(sharedctx != GLES_NO_CONTEXT){
		//LOGMSG("ERROR: Sharing texture object withour using multi-context");
		sharedctx = GLES_NO_CONTEXT;
	}
#endif

#ifdef FIMG_GLES_11
	OGLState *ctx = new OGLState();
#else
#ifdef MULTI_CONTEXT
	OGLState *ctx = new OGLState();
	SET_GL_STATE(ctx)
#else
	pgles2State = new OGLState(); //ctx = new OGLState();
	GET_GL_STATE(ctx);
#endif
#endif

	//part of context not being shared so need to be initialized everytime
	set_default_values(ctx);        
	InitShaderState(ctx);
	glfInit(ctx);                   
	
	//Init FBO state
	ctx->renderbuffState.setDefaults();
	ctx->framebuffState.setDefaults();
	//Init local texture state
	InitLocalTextureState(ctx);
	SharedState* sharedState = NULL;

	if(sharedctx == GLES_NO_CONTEXT){
		sharedState = new SharedState();
		
#ifdef SHARED_CONTEXT_DEBUG     
		//LOGMSG("\n \"shared context\" being created \n");
#endif

	}
	else
	{
		//as to share the state need to make it equal to the ctx being passed as argument
		sharedState =   ((OGLState *)sharedctx)->sharedState;
#ifdef SHARED_CONTEXT_DEBUG     
		//LOGMSG("\n \"shared context\" being initailized from another context \n");
#endif          

	}

	ctx->sharedState = sharedState;
	lockGLSharedState(ctx);         //lock the shared state before any opeartion 
	ctx->sharedState->sharedStateRefCount++;
	unlockGLSharedState(ctx);               //unlock the shared state before any opeartion
	
#ifdef SHARED_CONTEXT_DEBUG     
	//LOGMSG("\n shared state ref count = %d : gles2createContext \n" , ctx->sharedState->sharedStateRefCount);
#endif


#ifdef FSO_JITO
	ctx->cgd = constructCodegenData();
#endif
	return ctx;
}

extern "C" GLES2Context GLES2SetContext(GLES2Context glctx)
{
	GET_GL_STATE_NOCHECK(prev_ctx);

	if(glctx != NULL)
	{
#ifndef FIMG_GLES_11
#ifdef MULTI_CONTEXT
		SET_GL_STATE(glctx)
#else
		pgles2State = (OGLState *)glctx;
#endif
#endif
	}

	return prev_ctx;
}

extern "C" GLboolean GLES2DestroyContext(GLES2Context _ctx)
{
	OGLState* ctx = (OGLState*)_ctx;

	//GET_GL_STATE(ctx);
	if(!ctx)
	{
		return GL_FALSE;
	}
	
	((OGLState*) ctx)->framebuffState.setDefaults();
	((OGLState*) ctx)->renderbuffState.setDefaults();
	
	for(int i = 0; i < ((OGLState*) ctx)->buffer_object_data.buffer_objects_last_index; i++){
		if(  ((OGLState*) ctx)->buffer_object_data.buffer_objects[i].data){
			free(ctx->buffer_object_data.buffer_objects[i].data);
			ctx->buffer_object_data.buffer_objects[i].data=NULL;
		}
	}
	DeInitShaderState((OGLState*)ctx);
	lockGLSharedTextureState(ctx);
	DeInitLocalTextureState(ctx);
	unlockGLSharedTextureState(ctx);

	lockGLSharedState(ctx);         //lock the shared state before any opeartion
	ctx->sharedState->sharedStateRefCount--;
#ifdef SHARED_CONTEXT_DEBUG     
	//LOGMSG("\n shared state ref count = %d : GLES2DestroyContext \n" , ctx->sharedState->sharedStateRefCount);
#endif
	unlockGLSharedState(ctx);               //unlock the shared state before any opeartion  


	if(ctx->sharedState->sharedStateRefCount == 0){
		
		delete(ctx->sharedState);
		ctx->sharedState = NULL;
	}else{
		ctx->sharedState = NULL;

	}
	

	
#ifdef FSO_JITO
	destroyCodegenData(((OGLState*) ctx)->cgd);
	((OGLState*) ctx)->cgd = 0;
#endif

#ifndef MULTI_CONTEXT    
	delete pgles2State;
	pgles2State = 0;
#else

	delete ctx;
	ctx = NULL;
#endif    

	return GL_TRUE;
	
}

#ifdef USE_3D_PM
GLboolean PM_GLES2Flush(void);

extern "C" GLboolean GLES2Flush(void)
{
	GLboolean retVal = GL_FALSE;    
	lock3DCriticalSection();
	retVal = PM_GLES2Flush();
	unlock3DCriticalSection();
	return retVal;
}

GLboolean PM_GLES2Flush(void)
#else
extern "C" GLboolean GLES2Flush(void)
#endif
{
	GET_GL_STATE(ctx);
	
	FimgFinish(ctx); 

	return GL_TRUE;
}

glfError FimgFinish(OGLState* pState)
{
	Plat::lockGPUSFR(__FUNCTION__);
	glfFinish(pState);
	Plat::unlockGPUSFR(__FUNCTION__);

	return GLF_ERR_SUCCESS;
}

void set_err(GLenum err)
{
	GET_GL_STATE(ctx);

	//    //LOGMSG("\nGLERROR: %s from %s\n", getGLErrorString(err), getLastGLFunc());
#ifdef _MSC_VER
	//    __asm int 3;
#endif
	if(ctx->error == GL_NO_ERROR)       //Do not overwrite older error
	{
		ctx->error = err;
	}
}


void set_err(OGLState *ctx, GLenum err)
{

	//    //LOGMSG("\nGLERROR: %s from %s\n", getGLErrorString(err), getLastGLFunc());
#ifdef _MSC_VER
	//TODO: remove this? This is useful while testnig the statemanagement with
	//      visual studio
	//    __asm int 3;
#endif
	if(ctx->error == GL_NO_ERROR)       //Do not overwrite older error
	{
		ctx->error = err;
	}

	//   LOGE("ERROR in 2.0 ::%d",ctx->error);      
}

/* determines the number of bytes per pixel based on 'format' and 'type' */
int determine_bpp(GLenum format, GLenum type)
{
	int bpp = 0;

	if((format == GL_RGBA) && (type == GL_UNSIGNED_BYTE))
	{
		bpp = 4;
	}
	else if((format == GL_RGB) && (type == GL_UNSIGNED_BYTE))
	{
		bpp = 3;
	}
	else if((format == GL_RGBA) && (type == GL_UNSIGNED_SHORT_4_4_4_4))
	{
		bpp = 2;
	}
	else if((format == GL_RGBA) && (type == GL_UNSIGNED_SHORT_5_5_5_1))
	{
		bpp = 2;
	}
	else if((format == GL_RGB) && (type == GL_UNSIGNED_SHORT_5_6_5))
	{
		bpp = 2;
	}
	else if((format == GL_LUMINANCE_ALPHA) && (type == GL_UNSIGNED_BYTE))
	{
		bpp = 2;
	}
	else if((format == GL_LUMINANCE) && (type == GL_UNSIGNED_BYTE))
	{
		bpp = 1;
	}
	else if((format == GL_ALPHA) && (type == GL_UNSIGNED_BYTE))
	{
		bpp = 1;
	}
	else
	{
		set_err(GL_INVALID_ENUM);
	}

	return bpp;
}

GLboolean isValidBlendFunc(GLenum factor)
{
	switch(factor)
	{
	case GL_ZERO:
	case GL_ONE:
	case GL_SRC_COLOR:
	case GL_ONE_MINUS_SRC_COLOR:
	case GL_SRC_ALPHA:
	case GL_ONE_MINUS_SRC_ALPHA:
	case GL_DST_ALPHA:
	case GL_ONE_MINUS_DST_ALPHA:
	case GL_DST_COLOR:
	case GL_ONE_MINUS_DST_COLOR:
	case GL_SRC_ALPHA_SATURATE:

	case GL_CONSTANT_COLOR:
	case GL_ONE_MINUS_CONSTANT_COLOR:
	case GL_CONSTANT_ALPHA:
	case GL_ONE_MINUS_CONSTANT_ALPHA:
		return GL_TRUE;                 //intended fall through
		break;
	}

	return GL_FALSE;
}

GLboolean isValidDepthFunc(GLenum func)
{
	switch(func)
	{
	case GL_NEVER:
	case GL_LESS:
	case GL_EQUAL:
	case GL_LEQUAL:
	case GL_GREATER:
	case GL_NOTEQUAL:
	case GL_GEQUAL:
	case GL_ALWAYS:
		return GL_TRUE;
		break;
	}

	return GL_FALSE;

}
GLboolean isValidStencilOp(GLenum op)
{
	//should be GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_DECR, and GL_INVERT
	switch(op)
	{
	case GL_KEEP:
	case GL_ZERO:
	case GL_REPLACE:
	case GL_INCR:
	case GL_DECR:
	case GL_INVERT:
		return GL_TRUE;         //intended fall though
	}

	return GL_FALSE;
}

GL_API void GL_APIENTRY glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	GET_GL_STATE(ctx);

	ctx->blend_data.blnd_clr_red   = clamp(red);
	ctx->blend_data.blnd_clr_green = clamp(green);
	ctx->blend_data.blnd_clr_blue  = clamp(blue);
	ctx->blend_data.blnd_clr_alpha = clamp(alpha);
	SetUpDateFlagPerFragment(ctx,GLF_UF_Blend);
}

GL_API void GL_APIENTRY glBlendEquation( GLenum mode )
{
	//GET_GL_STATE(ctx);

	glBlendEquationSeparate(mode, mode);
}

GL_API void     GL_APIENTRY glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
	GET_GL_STATE(ctx);

	switch(modeRGB)
	{
	case GL_FUNC_ADD:
	case GL_FUNC_SUBTRACT:
	case GL_FUNC_REVERSE_SUBTRACT:
		break;                                  //Intended fallthough
	default:
		set_err(GL_INVALID_ENUM);
		return;
	}

	switch(modeAlpha)
	{
	case GL_FUNC_ADD:
	case GL_FUNC_SUBTRACT:
	case GL_FUNC_REVERSE_SUBTRACT:
		break;                                  //Intended fallthough
	default:
		set_err(GL_INVALID_ENUM);
		return;
	}

	ctx->blend_data.eqn_modeRGB = modeRGB;
	ctx->blend_data.eqn_modeAlpha = modeAlpha;
	SetUpDateFlagPerFragment(ctx,GLF_UF_Blend);
}

GL_API void GL_APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor)
{
	//GET_GL_STATE(ctx);

	glBlendFuncSeparate(sfactor, dfactor, sfactor, dfactor);
}


GL_API void GL_APIENTRY glBlendFuncSeparate (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
	GET_GL_STATE(ctx);

	if(isValidBlendFunc(srcRGB) && isValidBlendFunc(dstRGB) && isValidBlendFunc(srcAlpha) && isValidBlendFunc(dstAlpha))
	{
		ctx->blend_data.fn_srcRGB    = srcRGB;
		ctx->blend_data.fn_srcAlpha  = srcAlpha;

		ctx->blend_data.fn_dstRGB    = dstRGB;
		ctx->blend_data.fn_dstAlpha  = dstAlpha;
		SetUpDateFlagPerFragment(ctx,GLF_UF_Blend);
	}
	else
	{
		set_err(GL_INVALID_ENUM);
	}
}

#ifdef USE_3D_PM
void PM_glClear (GLbitfield mask);

GL_API void GL_APIENTRY glClear (GLbitfield mask)
{
	lock3DCriticalSection();
	PM_glClear(mask);
	unlock3DCriticalSection();
}

void PM_glClear (GLbitfield mask)
#else
GL_API void GL_APIENTRY glClear (GLbitfield mask)
#endif

{
	GET_GL_STATE(ctx);

	//Check if FB is complete Sandeep K.
	if(!isFBrenderable(ctx))
	{
		set_err(GL_INVALID_FRAMEBUFFER_OPERATION);
		return;
	}

	//glfFinish(ctx); //this is done inside glfClear.
	//SPEC: mask can only be a bitwise or of GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, GL_STENCIL_BUFFER_BIT

	GLbitfield temp = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT|GL_COLOR_BUFFER_WITH_IMAGE_BIT);

	if( (mask & temp)!= mask)
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	Plat::lockGPUSFR(__FUNCTION__);
	
#if FIMG_PLATFORM == 1
#if GLF_DRAW == ENABLE
	glfClear(ctx, mask);
#endif
#ifdef CACHE_MEM
	clean_cached_buffer();
#endif

#endif

	Plat::unlockGPUSFR(__FUNCTION__);
}

GL_API void GL_APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	GET_GL_STATE(ctx);

	//SPEC: NOTE: clear color's clamping behaviour must change if float textures are supported. The values should not be in that case
	//  and instead are clamped by the hw dependent code to suit the specific rendertarget's format.

	ctx->clear_color_data.r = clamp(red);
	ctx->clear_color_data.b = clamp(blue);
	ctx->clear_color_data.g = clamp(green);
	ctx->clear_color_data.a = clamp(alpha);
}

GL_API void GL_APIENTRY glClearDepthf (GLclampf depth)
{
	GET_GL_STATE(ctx);

	//SPEC: [value] is clamped to the range [0, 1] and converted to fixed-point according to the rules for a window z value given in section 2.11.1
	//TODO: check and code the computation to find the fixedpoint value of depth clear value.

	ctx->clear_depth = clamp(depth);

}

GL_API void GL_APIENTRY glClearStencil (GLint s)
{
	GET_GL_STATE(ctx);

	//SPEC: s is masked to the number of bitplanes available
	//TODO: who does the masking?
	ctx->clear_stencil = s;
}

GL_API void GL_APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	GET_GL_STATE(ctx);

	ctx->color_mask_data.r = red;
	ctx->color_mask_data.g = green;
	ctx->color_mask_data.b = blue;
	ctx->color_mask_data.a = alpha;
	SetUpDateFlagPerFragment(ctx,GLF_UF_ColorBuffer);
}

GL_API void GL_APIENTRY glCullFace (GLenum mode)
{
	GET_GL_STATE(ctx);

	switch(mode)
	{
	case GL_FRONT:
	case GL_BACK:
	case GL_FRONT_AND_BACK:
		ctx->cull_face_data.mode = mode;                //intended fallthrough
		SetUpDateFlagRasterEngine(ctx,GLF_UF_BackfaceCull);
		//ctx->updateBits.re = UB_RE_ALL;
		break;

	default:
		set_err(GL_INVALID_ENUM);
		return;
	}

}

GL_API void GL_APIENTRY glDepthFunc (GLenum func)
{
	GET_GL_STATE(ctx);
	if(isValidDepthFunc(func))
	{
		ctx->depth_test_func = func;
		SetUpDateFlagPerFragment(ctx,GLF_UF_DepthTest);
	}
	else
	{
		set_err(GL_INVALID_ENUM);
	}
}

GL_API void GL_APIENTRY glDepthMask (GLboolean flag)
{
	GET_GL_STATE(ctx);

	ctx->depth_write_mask = flag;
	SetUpDateFlagPerFragment(ctx,GLF_UF_DepthTest);
}

GL_API void GL_APIENTRY glDepthRangef (GLclampf zNear, GLclampf zFar)
{
	GET_GL_STATE(ctx);

	ctx->depth_range_data.near_depth  = clamp(zNear);
	ctx->depth_range_data.far_depth   = clamp(zFar);
	SetUpDateFlagPrimEngine(ctx,GLF_UF_DepthRange);
}

GL_API void GL_APIENTRY glDisable (GLenum cap)
{
	GET_GL_STATE(ctx);

	switch(cap)
	{
		// case GL_VERTEX_PROGRAM_POINT_SIZE:                ctx->is_enabled_vertex_program_point_size = GL_FALSE;           break;
	case GL_CULL_FACE:  
		ctx->cull_face_data.is_enabled = GL_FALSE;      
		SetUpDateFlagRasterEngine(ctx,GLF_UF_BackfaceCull);
		//ctx->updateBits.re = UB_RE_ALL;
		break;
	case GL_POLYGON_OFFSET_FILL:                
		ctx->polygon_offset_data.is_enabled_fill = GL_FALSE;
		SetUpDateFlagRasterEngine(ctx,GLF_UF_DepthOffset);
		//ctx->updateBits.re = UB_RE_ALL;
		break;
	case GL_SCISSOR_TEST:               
		ctx->scissor_test_data.is_enabled = GL_FALSE;
		#if GLF_SCISSOR_IN_RA == ENABLE
		SetUpDateFlagRasterEngine(ctx,GLF_UF_ScissorTest);
		#else
		SetUpDateFlagPerFragment(ctx, GLF_UF_ScissorTest);
		#endif
		break;
	case GL_SAMPLE_COVERAGE:                            ctx->sample_coverage_data.is_enabled_coverage = GL_FALSE;               break;
	case GL_SAMPLE_ALPHA_TO_COVERAGE:           ctx->sample_coverage_data.is_enabled_alpha_to_coverage = GL_FALSE;      break;
	case GL_STENCIL_TEST:       
		ctx->stencil_test_data.is_enabled        = GL_FALSE;
		SetUpDateFlagPerFragment(ctx,GLF_UF_StencilTest);
		break;
	case GL_DEPTH_TEST:         
		ctx->is_enabled_depth = GL_FALSE;       
		SetUpDateFlagPerFragment(ctx,GLF_UF_DepthTest);
		break;
	case GL_BLEND:              
		ctx->blend_data.is_enabled = GL_FALSE;  
		SetUpDateFlagPerFragment(ctx,GLF_UF_Blend);
		break;
	case GL_DITHER:                                                     ctx->is_enabled_dither  = GL_FALSE;             break;

#ifdef EN_EXT_ALPHA_TEST_EXP
	case GL_ALPHA_TEST_EXP:   
		ctx->alpha_test_data.is_enabled = GL_FALSE;
		SetUpDateFlagPerFragment(ctx,GLF_UF_AlphaTest);
		break;
#endif

#ifdef EN_EXT_LOGIC_OP_EXP
	case GL_COLOR_LOGIC_OP_EXP:     
		ctx->is_logicOp_enabled = GL_FALSE; 
		SetUpDateFlagPerFragment(ctx,GLF_UF_LogicalOp);
		break;
#endif

#ifdef EN_EXT_POINT_SPRITE_EXP
	case GL_POINT_SPRITE_OES_EXP:
		ctx->is_point_sprite_enable = GL_FALSE;
		break;
#endif

	default:
		set_err(GL_INVALID_ENUM);
	}
}

GL_API void GL_APIENTRY glDisableVertexAttribArray (GLuint index)
{
	GET_GL_STATE(ctx);

	if(index >= MAX_VERTEX_ATTRIBS)
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	ctx->vertex_attrib_array_data[index].enabled = GL_FALSE;
}

bool isDrawVertexCountValid(GLenum mode, GLsizei count)
{

	if( count <= 0 )
	return false;

	switch(mode)
	{
	case GL_POINTS:             return count > 0;
	case GL_LINE_STRIP:         return count > 1;
	case GL_LINE_LOOP:          return count > 1;
	case GL_LINES:              return count > 1 ;
	case GL_TRIANGLES:          return count > 2 ;
	case GL_TRIANGLE_STRIP:     return count > 2;
	case GL_TRIANGLE_FAN:       return count > 2;
	}

	gAssert(false && "Invalid enum in mode");

	return false; //To silence the compiler
}

#ifdef USE_3D_PM
void PM_glDrawArrays (GLenum mode, GLint first, GLsizei count);

GL_API void GL_APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count)
{
	lock3DCriticalSection();
	PM_glDrawArrays( mode,  first, count);
	unlock3DCriticalSection();
}

void PM_glDrawArrays (GLenum mode, GLint first, GLsizei count)
#else
GL_API void GL_APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count)
#endif
{
	GET_GL_STATE(ctx);
	//Validation---------------

	//TODO: validate mode, first and count.
	//TODO: validate that this will not access out of the range?
	if(mode != GL_POINTS && mode != GL_LINE_STRIP && mode != GL_LINE_LOOP && mode != GL_LINES
			&& mode != GL_TRIANGLES && mode != GL_TRIANGLE_STRIP && mode != GL_TRIANGLE_FAN){
		set_err(GL_INVALID_ENUM);
		return;
	}

	if(first < 0)
	{
		set_err(GL_INVALID_VALUE);
		return;
	}
	if(!isDrawVertexCountValid(mode, count))
	{
		//set_err(GL_INVALID_VALUE); //Sliently ignore
		return;
	}

	//Check if FB is complete
	if(!isFBrenderable(ctx))
	{
		set_err(GL_INVALID_FRAMEBUFFER_OPERATION);
		return;
	}

	//Set whether VPPS should be enabled or not. All other VPPS dependent state configurations are set based on this value!
	Executable *pExe = ctx->current_executable;
	if(pExe->pointSizeIndex == -1 || (mode != GL_POINTS ) ) {
		ctx->VPPSEnable = false;
	}else {
		ctx->VPPSEnable = true;
	}

	Plat::lockGPUSFR(__FUNCTION__);

	//Drawing------------------------------------------
	if(!stateFlush(ctx))
	{

		//LOGMSG("\nWARNING: state has not been flushed to hw, current draw call ignored\n");
		Plat::unlockGPUSFR(__FUNCTION__);

		return;
	}


	glfSetDrawMode(ctx,mode);
	glfDraw(ctx, mode, first, count);

	Plat::unlockGPUSFR(__FUNCTION__);
}

#ifdef USE_3D_PM
void PM_glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices);

GL_API void GL_APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices)
{
	lock3DCriticalSection();
	PM_glDrawElements( mode,  count,  type,  indices);
	unlock3DCriticalSection();
}

void PM_glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices)
#else
GL_API void GL_APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices)
#endif
{	
	GET_GL_STATE(ctx);

	const void * inds = indices; 
	//Validation---------------

	if(mode != GL_POINTS && mode != GL_LINE_STRIP && mode != GL_LINE_LOOP && mode != GL_LINES
			&& mode != GL_TRIANGLES && mode != GL_TRIANGLE_STRIP && mode != GL_TRIANGLE_FAN){
		set_err(GL_INVALID_ENUM);
		return;
	}

	if(!isDrawVertexCountValid(mode, count)){
		//set_err(GL_INVALID_VALUE); //silently ignore
		return;
	}
	/*
		if(indices == NULL){
		set_err(GL_INVALID_VALUE);
		return;
	}
*/
	if(ctx->buffer_object_data.element_array_buffer_binding == NULL) {
		if(indices == NULL) {
			return;
		}
	} else {
		if(ctx->buffer_object_data.element_array_buffer_binding->data == NULL) {
			return;
		}
		int offset = (char *)indices - (char*)NULL;
		inds = (void*) ((char*)ctx->buffer_object_data.element_array_buffer_binding->data + offset);
		
	}
	
	if(type != GL_UNSIGNED_BYTE && type != GL_UNSIGNED_SHORT && type != GL_UNSIGNED_INT){
		set_err(GL_INVALID_ENUM);
		return;
	}

	//Check if FB is complete
	if(!isFBrenderable(ctx))
	{
		set_err(GL_INVALID_FRAMEBUFFER_OPERATION);
		return;
	}

	//Set whether VPPS should be enabled or not. All other VPPS dependent state configurations are set based on this value!
	Executable *pExe = ctx->current_executable;
	if(pExe->pointSizeIndex == -1 || (mode != GL_POINTS ) ) {
		ctx->VPPSEnable = false;
	}else {
		ctx->VPPSEnable = true;
	}

	Plat::lockGPUSFR(__FUNCTION__);
	//Drawing------------------------------------------

	// State flush
	
	if(!stateFlush(ctx))
	{
		//LOGMSG("\nWARNING: state has not been flushed to hw, current draw call ignored\n");
		Plat::unlockGPUSFR(__FUNCTION__);
		return;
	}
	
	glfSetDrawMode(ctx, mode);
	glfDrawElements(ctx, mode, count, type, inds);

	Plat::unlockGPUSFR(__FUNCTION__);
}

GL_API void GL_APIENTRY glEnable (GLenum cap)
{
	GET_GL_STATE(ctx);

	switch(cap)
	{
		//case GL_VERTEX_PROGRAM_POINT_SIZE:                ctx->is_enabled_vertex_program_point_size = GL_TRUE;            break;
	case GL_CULL_FACE:                  
		ctx->cull_face_data.is_enabled = GL_TRUE;
		SetUpDateFlagRasterEngine(ctx,GLF_UF_BackfaceCull);
		//ctx->updateBits.re = UB_RE_ALL;
		break;
	case GL_POLYGON_OFFSET_FILL:
		ctx->polygon_offset_data.is_enabled_fill = GL_TRUE;     
		SetUpDateFlagRasterEngine(ctx,GLF_UF_DepthOffset);
		//ctx->updateBits.re = UB_RE_ALL;
		break;
	case GL_SCISSOR_TEST:                                       
		ctx->scissor_test_data.is_enabled = GL_TRUE;
		#if GLF_SCISSOR_IN_RA == ENABLE
		SetUpDateFlagRasterEngine(ctx,GLF_UF_ScissorTest);
		#else
		SetUpDateFlagPerFragment(ctx, GLF_UF_ScissorTest);
		#endif
		break;
	case GL_SAMPLE_COVERAGE:                            ctx->sample_coverage_data.is_enabled_coverage = GL_TRUE;                break;
	case GL_SAMPLE_ALPHA_TO_COVERAGE:           ctx->sample_coverage_data.is_enabled_alpha_to_coverage = GL_TRUE;       break;
	case GL_STENCIL_TEST:       
		ctx->stencil_test_data.is_enabled        = GL_TRUE;     
		SetUpDateFlagPerFragment(ctx,GLF_UF_StencilTest);
		break;
	case GL_DEPTH_TEST:                                         
		ctx->is_enabled_depth = GL_TRUE;        
		SetUpDateFlagPerFragment(ctx,GLF_UF_DepthTest);
		break;
	case GL_BLEND:      
		ctx->blend_data.is_enabled = GL_TRUE;   
		SetUpDateFlagPerFragment(ctx,GLF_UF_Blend);
		break;
	case GL_DITHER:                                                     ctx->is_enabled_dither  = GL_TRUE;              break;

#ifdef EN_EXT_ALPHA_TEST_EXP
	case GL_ALPHA_TEST_EXP:             
		ctx->alpha_test_data.is_enabled = GL_TRUE;      
		SetUpDateFlagPerFragment(ctx,GLF_UF_AlphaTest);
		break;
#endif

#ifdef EN_EXT_LOGIC_OP_EXP
	case GL_COLOR_LOGIC_OP_EXP:             
		ctx->is_logicOp_enabled = GL_TRUE;
		SetUpDateFlagPerFragment(ctx,GLF_UF_LogicalOp);
		break;
#endif

#ifdef EN_EXT_POINT_SPRITE_EXP
	case GL_POINT_SPRITE_OES_EXP:
		ctx->is_point_sprite_enable = GL_TRUE;
		break;
#endif
	default:
		set_err(GL_INVALID_ENUM);
	}
}

GL_API void GL_APIENTRY glEnableVertexAttribArray (GLuint index)
{
	GET_GL_STATE(ctx);

	if(index >= MAX_VERTEX_ATTRIBS)
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	ctx->vertex_attrib_array_data[index].enabled = GL_TRUE;
}

static inline void updateBuiltinUniforms(ShaderExecutable& se, OGLState* ctx) {
	
	float* uniforms = se.constFloatMemBlock;
	const GLESBuiltinUniforms& indices = se.builtinUniformIndices;
	
	if(indices.depthRange_near != -1) {
		uniforms[indices.depthRange_near] = ctx->depth_range_data.near_depth;
	}
	if(indices.depthRange_far != -1) {
		uniforms[indices.depthRange_far] = ctx->depth_range_data.far_depth;
	}
	if(indices.depthRange_diff != -1) {
		uniforms[indices.depthRange_diff] = ctx->depth_range_data.far_depth - ctx->depth_range_data.near_depth;
	}
	
}

bool stateFlush(OGLState * ctx)
{
	TextureState* pTexState = &(ctx->texState);
	TextureObject*              pTexObj;
	
	bool zz=updateCurrentExecutable();
	
	if(zz)
	{
		Executable *pExe = ctx->current_executable;

		int texUnitUsage = 0;
		for(int i = 0; i < MAX_TEXTURE_UNITS; i++)
		{
			if(pExe->fsSamplerMappings[i].isUsed == false)
			continue;
			
			int texUnit = pExe->fsSamplerMappings[i].glTexUnit;
			if(texUnit >= MAX_TEXTURE_UNITS){
				//TO DO
				//LOGMSG(" the value of the texUNit is greater than max. texture unit : %s", __FUNCTION__);
				return false;
			}
			
			GLenum texType = pExe->fsSamplerMappings[i].type;   
			//instead of below mentioned thing need to call GLVALIDATEPROGRAM ??? CHECK IN SPEC TO DO
			//It is not allowed to have variables of different sampler types pointing to the
			//same texture image unit within a program object.
			int bitToSet = 1<<( 3 * texUnit + (texType - GL_SAMPLER_2D));
			int previousSamplerValue = (texUnitUsage >> (3 * texUnit)) & 7 ;

			if(previousSamplerValue == 0){
				texUnitUsage |=  bitToSet;
			}
			else if( previousSamplerValue != (1 << (texType - GL_SAMPLER_2D))){
				fprintf(stderr, "error /n");
				SET_ERR(ctx, GL_INVALID_OPERATION, "glTexImage2D");

				return false;
			}         
			
			if(texType == GL_SAMPLER_2D)
			{
#if STORE_TEX_OBJ_POINTER ==    ENABLE  
				pTexObj = pTexState->ptexUnitBinding[texUnit].texture2D;                        
#else 
				pTexObj = GetTextureObject(GL_TEXTURE_2D,true,texUnit);
				//   fprintf(stderr, " %p %p \n", pTexObj , &(pTexState->defaultTexObjects[TEX_2D_DEFAULT]);
#endif
				////LOGMSG(" pTexObj=%d   pTestTexObj=%d \n" , pTexObj , pTestTexObj);
				//gAssert((pTexObj == pTestTexObj) && "texture object pointer is wrong");
			}
			else if(texType == GL_SAMPLER_CUBE) 
			{
#if STORE_TEX_OBJ_POINTER ==    ENABLE                                  
				pTexObj = pTexState->ptexUnitBinding[texUnit].cubeMap;
#else
				pTexObj = GetTextureObject(GL_TEXTURE_CUBE_MAP,true,texUnit);
#endif

				////LOGMSG(" pTexObj=%d   pTestTexObj=%d \n" , pTexObj , pTestTexObj);
				//gAssert((pTexObj == pTestTexObj) && "texture object pointer is wrong");
			}
			else if(texType == GL_SAMPLER_3D)
			{

#if STORE_TEX_OBJ_POINTER ==    ENABLE                                  
				pTexObj = pTexState->ptexUnitBinding[texUnit].texture3D;
#else
				pTexObj = GetTextureObject(GL_TEXTURE_3D,true,texUnit);
#endif
				////LOGMSG(" pTexObj=%d   pTestTexObj=%d \n" , pTexObj , pTestTexObj);
				//gAssert((pTexObj == pTestTexObj) && "texture object pointer is wrong");
			}
			else
			{
				//LOGMSG("Type of Sampler unknown\n");

				return false;
			}   
			
			GLboolean res = pTexObj->Compile();
			
			if(res)
			{
				//texDim need not be a valid index since these special texDim uniforms
				//are generated only if a varying is used as a tex coord.
				if(pExe->dimensions.tex[i] != -1)
				{
					pExe->fs.constFloatMemBlock[pExe->dimensions.tex[i]] =  float(pTexObj->width);
					pExe->fs.constFloatMemBlock[pExe->dimensions.tex[i]+1] =  float(pTexObj->height);
				}
				pTexState->pTexObjectToConfigFimg[i].id = pTexObj->id;
				pTexState->pTexObjectToConfigFimg[i].pTexFGLState = &(pTexObj->texFGLState);
				
			}
			else
			{
				//as compilation fails need to update with fallback texture
				pTexState->pTexObjectToConfigFimg[i].id = 0;
				if(texType == GL_SAMPLER_2D) {
					
					pTexState->pTexObjectToConfigFimg[i].pTexFGLState =  &(ctx->sharedState->sharedTexState.fallBackTexFGLstate[0]);
				}
				else   if(texType == GL_SAMPLER_3D) {
					
					pTexState->pTexObjectToConfigFimg[i].pTexFGLState =  &(ctx->sharedState->sharedTexState.fallBackTexFGLstate[1]);
				}
				else if(texType == GL_SAMPLER_CUBE) {
					pTexState->pTexObjectToConfigFimg[i].pTexFGLState =  &(ctx->sharedState->sharedTexState.fallBackTexFGLstate[2]);
				}
				// //LOGMSG("Compile failed\n");
			}

		} //End of tex unit for loop

		updateBuiltinUniforms(pExe->vs, ctx);
		updateBuiltinUniforms(pExe->fs, ctx);

//#if FIMG_PLATFORM == 1
		//No fimg setting on win32 :)
//		glfFinish(ctx);  just to see what happens
		glfFlush(ctx);
//#endif  

		return true;
	}

	return false;
}

#ifdef USE_3D_PM
void PM_glFinish (void);

GL_API void GL_APIENTRY glFinish (void)
{
	lock3DCriticalSection();
	PM_glFinish();
	unlock3DCriticalSection();
}

void PM_glFinish (void)
#else
GL_API void GL_APIENTRY glFinish (void)
#endif
{
	Plat::lockGPUSFR(__FUNCTION__);
	
	GET_GL_STATE(ctx);

#if FIMG_PLATFORM == 1  
	glfFinish(ctx);
#endif 


#ifdef CACHE_MEM
	invalidate_cached_buffer();
#endif

	Plat::unlockGPUSFR(__FUNCTION__);
}



#ifdef USE_3D_PM
void PM_glFlush (void);

GL_API void GL_APIENTRY glFlush (void)
{
	lock3DCriticalSection();
	PM_glFlush();
	unlock3DCriticalSection();
}

void PM_glFlush (void)
#else
GL_API void GL_APIENTRY glFlush (void)
#endif
{
	Plat::lockGPUSFR(__FUNCTION__);
	GET_GL_STATE(ctx);
	
	if(!stateFlush(ctx))
	{
		//LOGMSG("\nWARNING: state has not been flushed to hw\n");
	}

	Plat::unlockGPUSFR(__FUNCTION__);
}

GL_API void GL_APIENTRY glFrontFace (GLenum mode)
{
	GET_GL_STATE(ctx);

	if((mode == GL_CW )||(mode == GL_CCW))
	{
		ctx->front_face_mode = mode;
		SetUpDateFlagRasterEngine(ctx,GLF_UF_BackfaceCull);
		//ctx->updateBits.re = UB_RE_ALL;
	}
	else
	{
		set_err(GL_INVALID_ENUM);
	}
}


GL_API void GL_APIENTRY glGetBooleanv (GLenum pname, GLboolean *params)
{
	
	GET_GL_STATE(ctx);
	FramebufferData fbData = getFBData();
	float tempValue[4] ;
	if(params == NULL)
	SET_ERR(ctx, GL_INVALID_VALUE, "glGetBooleanv*");
	else
	switch(pname)
	{
		//the state variable whose basic type is boolean
	case GL_SAMPLE_COVERAGE_INVERT:
		params[0] = ctx->sample_coverage_data.invert;
		break;
	case GL_COLOR_WRITEMASK: 
		params[0] = ctx->color_mask_data.r;
		params[1] = ctx->color_mask_data.g;
		params[2] = ctx->color_mask_data.b;
		params[3] = ctx->color_mask_data.a;
		break;
	case GL_DEPTH_WRITEMASK:
		params[0] = ctx->depth_write_mask;
		break;
	case GL_SHADER_COMPILER :
		params[0] = GL_FALSE;
		break;
		
		//The state variable supported by glIsEnabled         
	case GL_CULL_FACE:
	case GL_POLYGON_OFFSET_FILL:
	case GL_SAMPLE_COVERAGE:
	case GL_SAMPLE_ALPHA_TO_COVERAGE:
	case GL_SCISSOR_TEST:
	case GL_STENCIL_TEST:
	case GL_DEPTH_TEST:
	case GL_BLEND:
	case GL_DITHER: 
#ifdef EN_EXT_ALPHA_TEST_EXP
	case GL_ALPHA_TEST_EXP:       
#endif

#ifdef EN_EXT_LOGIC_OP_EXP
	case GL_COLOR_LOGIC_OP:             
#endif

		params[0] = glIsEnabled(pname);
		break;

		//The state variable added from glGetInteger* and glGetFloat*
	case GL_ARRAY_BUFFER_BINDING:
		params[0] = 0;
		if(ctx->buffer_object_data.array_buffer_binding != NULL){
			if(ctx->buffer_object_data.array_buffer_binding->id != 0)     params[0] = 1;        
		}
		break;
	case GL_ELEMENT_ARRAY_BUFFER_BINDING:
		params[0] = 0;
		if(ctx->buffer_object_data.element_array_buffer_binding != NULL){
			if(ctx->buffer_object_data.element_array_buffer_binding->id != 0) params[0] = 1;
		}
		break;
	case GL_VIEWPORT:
		if(ctx->viewport_data.x != 0){
			params[0] = 1;
		}else params[0] = 0;
		if(ctx->viewport_data.y != 0){
			params[1] = 1;
		}else params[0] = 0;
		if(ctx->viewport_data.w != 0){
			params[2] = 1;
		}else params[0] = 0;
		if(ctx->viewport_data.h != 0){
			params[3] = 1;
		}else params[0] = 0;
		break;  
	case GL_DEPTH_RANGE:
		if( ctx->depth_range_data.near_depth != 0){
			params[0] = 1;
		}else params[0] = 0;
		if(ctx->depth_range_data.far_depth != 0){
			params[1] = 1;
		}else params[0] = 0;
		break;
	case GL_LINE_WIDTH:
		if(ctx->line_width != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_CULL_FACE_MODE:
		if(ctx->cull_face_data.mode != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_FRONT_FACE:
		if(ctx->front_face_mode != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_POLYGON_OFFSET_FACTOR:
		if(ctx->polygon_offset_data.factor != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;  
	case GL_POLYGON_OFFSET_UNITS:
		if(ctx->polygon_offset_data.units != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_SAMPLE_COVERAGE_VALUE:
		if(ctx->sample_coverage_data.value != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
		
	case GL_TEXTURE_BINDING_2D: //to check
		if( ctx->texState.texUnitBinding[ctx->texState.activeTexUnit].texture2D != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_TEXTURE_BINDING_3D:
		if(ctx->texState.texUnitBinding[ctx->texState.activeTexUnit].texture3D != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_TEXTURE_BINDING_CUBE_MAP:
		if(ctx->texState.texUnitBinding[ctx->texState.activeTexUnit].cubeMap != 0){
			params[0] = 1;
		}else params[0] = 0;
		//params[0] = GetTexNameArrayIndex(params[0] );
		break;
	case GL_ACTIVE_TEXTURE:
		if(ctx->texState.activeTexUnit != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
		
	case GL_STENCIL_WRITEMASK :
		if(ctx->front_stencil_writemask != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_BACK_WRITEMASK :
		if(ctx->back_stencil_writemask != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_COLOR_CLEAR_VALUE :
		if( ctx->clear_color_data.r != 0){
			params[0] = 1;
		}else params[0] = 0;
		if(ctx->clear_color_data.g != 0){
			params[1] = 1;
		}else params[0] = 0;
		if( ctx->clear_color_data.b != 0){
			params[2] = 1;
		}else params[0] = 0;
		if(ctx->clear_color_data.a != 0){
			params[3] = 1;
		}else params[0] = 0;
		break;
	case GL_DEPTH_CLEAR_VALUE :
		if( ctx->clear_depth != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_CLEAR_VALUE: 
		if(ctx->clear_stencil != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_SCISSOR_BOX:
		if(ctx->scissor_test_data.x != 0){
			params[0] = 1;
		}else params[0] = 0;
		if(ctx->scissor_test_data.y != 0){
			params[1] = 1;
		}else params[0] = 0;
		if( ctx->scissor_test_data.w != 0){
			params[2] = 1;
		}else params[0] = 0;
		if(ctx->scissor_test_data.h != 0){
			params[3] = 1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_FUNC:
		if(ctx->stencil_test_data.front_face_func != 0){
			params[0] =1;   
			
		}else params[0] = 0;
		break;
	case GL_STENCIL_VALUE_MASK:
		if(ctx->stencil_test_data.front_face_mask != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_REF :
		if(ctx->stencil_test_data.front_face_ref != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_FAIL :
		if(ctx->stencil_test_data.front_face_fail != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_PASS_DEPTH_FAIL :
		if(ctx->stencil_test_data.front_face_zfail != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_PASS_DEPTH_PASS :
		if(ctx->stencil_test_data.front_face_zpass != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_BACK_FUNC :
		if( ctx->stencil_test_data.back_face_func != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_BACK_VALUE_MASK :
		if(ctx->stencil_test_data.back_face_mask != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_BACK_REF :
		if(ctx->stencil_test_data.back_face_ref != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;  
	case GL_STENCIL_BACK_FAIL :
		if(ctx->stencil_test_data.back_face_fail != 0){
			params[0] =1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_BACK_PASS_DEPTH_FAIL :
		if(ctx->stencil_test_data.back_face_zfail != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_BACK_PASS_DEPTH_PASS :
		if( ctx->stencil_test_data.back_face_zpass != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_DEPTH_FUNC:
		if(ctx->depth_test_func != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_BLEND_SRC_RGB :
		//if((params[0] = ctx->blend_data.fn_srcRG)!=0){
		//      params[0] =1;
		//}
		//break;
	case GL_BLEND_SRC_ALPHA:
		if(ctx->blend_data.fn_srcAlpha != 0){
			params[0] =1;
		}else params[0] = 0;
		break;
	case GL_BLEND_DST_RGB :
		if(ctx->blend_data.fn_dstRGB != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_BLEND_DST_ALPHA: 
		if(ctx->blend_data.fn_dstAlpha != 0){
			params[0] =1;
		}else params[0] = 0;
		break;
	case GL_BLEND_EQUATION_RGB :
		if(ctx->blend_data.eqn_modeRGB != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_BLEND_EQUATION_ALPHA: 
		if(ctx->blend_data.eqn_modeAlpha != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_BLEND_COLOR :
		if( ctx->blend_data.blnd_clr_red != 0){
			params[0] = 1;
		}else params[0] = 0;
		if(ctx->blend_data.blnd_clr_green != 0){
			params[0] = 1;
		}else params[0] = 0;
		if(ctx->blend_data.blnd_clr_blue != 0){
			params[2] = 1;
		}else params[0] = 0;
		if(ctx->blend_data.blnd_clr_alpha != 0){
			params[3] = 1;
		}else params[0] = 0;
		break;
	case GL_UNPACK_ALIGNMENT:
		if(ctx->pixel_store_data.unpack_alignment != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_PACK_ALIGNMENT:
		if(ctx->pixel_store_data.pack_alignment != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_CURRENT_PROGRAM:
		if(ctx->current_program != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_GENERATE_MIPMAP_HINT:
		if(ctx->hint_data.generate_mipmap_hint != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_MAX_TEXTURE_SIZE:
		if( MAX_TEXTURE_SIZE != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
		if( MAX_CUBEMAP_TEXTURE_SIZE != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_MAX_VIEWPORT_DIMS:
		if(MAX_VIEWPORT_DIMS != 0){
			params[0] = 1;
			params[1] = 1;
		}else{ params[0] = 0; params[1] = 0;}
		break;  
	case GL_MAX_ELEMENTS_INDICES :
		if(MAX_ELEMENTS_INDICES != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_MAX_ELEMENTS_VERTICES :
		if(MAX_ELEMENTS_VERTICES != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_SAMPLE_BUFFERS :
		if(NO_OF_SAMPLES != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_SAMPLES :
		if(MULTISAMPLING != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_COMPRESSED_TEXTURE_FORMATS : 
		if(NUM_COMPRESSED_TEXTURE_FORMATS != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_NUM_COMPRESSED_TEXTURE_FORMATS :
		if(NUM_COMPRESSED_TEXTURE_FORMATS != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_NUM_SHADER_BINARY_FORMATS :
		if((tempValue[0] = NUM_SHADER_BINARY_FORMATS) != 0){
			params[0] = 1;  
		}else params[0] = 0;
		break;
	case GL_MAX_VERTEX_ATTRIBS :
		if(MAX_VERTEX_ATTRIB_VARS != 0){
			params[0] = 1;  
		}else params[0] = 0;
		break;
	case GL_MAX_VERTEX_UNIFORM_VECTORS :
		if(MAX_UNIFORMS != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_MAX_VARYING_VECTORS :
		if(MAX_VARYING_VECTORS != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS :
		if(MAX_VERTEX_TEXTURE_UNITS + MAX_TEXTURE_UNITS != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS :
		if( MAX_VERTEX_TEXTURE_UNITS != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_MAX_TEXTURE_IMAGE_UNITS :
		if( MAX_TEXTURE_UNITS != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_MAX_FRAGMENT_UNIFORM_VECTORS :
		if(MAX_UNIFORMS != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_MAX_RENDERBUFFER_SIZE :
		if(MAX_RENDERBUFFER_SIZE != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_RED_BITS :
		if(determinePixelBitSize(fbData.nativeColorFormat,RED) != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_GREEN_BITS: 
		if(determinePixelBitSize(fbData.nativeColorFormat,GREEN) != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_BLUE_BITS :
		if(determinePixelBitSize(fbData.nativeColorFormat,BLUE) != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_ALPHA_BITS :
		if(determinePixelBitSize(fbData.nativeColorFormat,ALPHA) != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_DEPTH_BITS :
		if(determinePixelBitSize(fbData.nativeDepthStencilFormat,DEPTH) != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_STENCIL_BITS: 
		if(determinePixelBitSize(fbData.nativeDepthStencilFormat,STENCIL) != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_IMPLEMENTATION_COLOR_READ_TYPE: 
		
		if(determineTypeFormat(translateToGLenum(fbData.nativeColorFormat), GL_FALSE) != 0){
			params[0] = 1;
		}else params[0] = 0;
		break;
	case GL_IMPLEMENTATION_COLOR_READ_FORMAT:
		if(determineTypeFormat(translateToGLenum(ctx->defFBData.nativeColorFormat), GL_TRUE) != 0){
			params[0] = 1;
		}else params[0] = 0;    
		break; 
	case GL_ALIASED_POINT_SIZE_RANGE:
		//TODO max value need to be asked
		break;
	case GL_ALIASED_LINE_WIDTH_RANGE:
		//TODO max value need to be asked
		break;      
	case GL_SHADER_BINARY_FORMATS :
		//TODO  unknown format supported
		break;
	case GL_SUBPIXEL_BITS:
		//TODO need to be asked 
		break;

	default:
		SET_ERR(ctx, GL_INVALID_ENUM, "glGetBooleanv*");
	}
}

GL_API void GL_APIENTRY glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params)
{
	GET_GL_STATE(ctx);

	BufferObject*       buf = NULL;

	if(params == NULL)
	{
		set_err(GL_INVALID_VALUE);
		return;
	}

	switch(target)
	{
	case GL_ARRAY_BUFFER:
		buf = ctx->buffer_object_data.array_buffer_binding;
		break;

	case GL_ELEMENT_ARRAY_BUFFER:
		buf = ctx->buffer_object_data.element_array_buffer_binding;
		break;

	default:
		set_err(GL_INVALID_ENUM);
		return;
	}

	if(buf == NULL)
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	switch(pname)
	{
	case GL_BUFFER_SIZE:
		*params = buf->size;
		break;

	case GL_BUFFER_USAGE:
		*params = buf->usage;
		break;

	case GL_BUFFER_ACCESS:
		*params = buf->access;
		break;

	default:
		set_err(GL_INVALID_ENUM);
		return;
	}
}


GL_API GLenum GL_APIENTRY glGetError (void)
{

	GET_GL_STATE(ctx);

	GLenum error  = ctx->error;
	ctx->error = GL_NO_ERROR;
	return error;

}

GL_API void GL_APIENTRY glGetFloatv (GLenum pname, GLfloat *params)
{
	GET_GL_STATE(ctx);
	FramebufferData fbData = getFBData();
	if(params == NULL)
	SET_ERR(ctx, GL_INVALID_VALUE, "glGetFloatv*");
	else
	switch(pname)
	{
	case GL_ARRAY_BUFFER_BINDING:
		if(ctx->buffer_object_data.array_buffer_binding != NULL){
			params[0] = (float)ctx->buffer_object_data.array_buffer_binding->id;
		}else params[0] = 0.0f;
		break;
	case GL_ELEMENT_ARRAY_BUFFER_BINDING:
		if(ctx->buffer_object_data.element_array_buffer_binding != NULL){
			params[0] = (float)ctx->buffer_object_data.element_array_buffer_binding->id;
		}else params[0] = 0.0f;
		break;
	case GL_VIEWPORT:
		params[0] = (float)ctx->viewport_data.x;
		params[1] = (float)ctx->viewport_data.y;
		params[2] = (float)ctx->viewport_data.w ;
		params[3] = (float)ctx->viewport_data.h ;
		break;  
	case GL_DEPTH_RANGE:
		params[0] = ctx->depth_range_data.near_depth;
		params[1] = ctx->depth_range_data.far_depth;
		break;
	case GL_POINT_SIZE:
		params[0] = ctx->point_size;
		break;
	case GL_LINE_WIDTH:
		params[0] = ctx->line_width;
		break;
	case GL_CULL_FACE_MODE:
		params[0] = (float)ctx->cull_face_data.mode;
		break;
	case GL_FRONT_FACE:
		params[0] = (float)ctx->front_face_mode;
		break;
	case GL_POLYGON_OFFSET_FACTOR:
		params[0] = ctx->polygon_offset_data.factor;
		break;  
	case GL_POLYGON_OFFSET_UNITS:
		params[0] = ctx->polygon_offset_data.units;
		break;
	case GL_SAMPLE_COVERAGE_VALUE:
		params[0] = ctx->sample_coverage_data.value;//to check
		break;
	case GL_SAMPLE_COVERAGE_INVERT:
		params[0] = ctx->sample_coverage_data.invert;
		break;
	case GL_TEXTURE_BINDING_2D: //to check
		params[0] = (float)ctx->texState.texUnitBinding[ctx->texState.activeTexUnit].texture2D;
		//params[0] = GetTexNameArrayIndex(params[0] );
		break;
	case GL_TEXTURE_BINDING_3D:
		params[0] = (float)ctx->texState.texUnitBinding[ctx->texState.activeTexUnit].texture3D;
		//params[0] = GetTexNameArrayIndex(params[0] );
		break;
	case GL_TEXTURE_BINDING_CUBE_MAP:
		params[0] = (float)ctx->texState.texUnitBinding[ctx->texState.activeTexUnit].cubeMap;
		//params[0] = GetTexNameArrayIndex(params[0] );
		break;
	case GL_ACTIVE_TEXTURE:
		params[0] = (float)ctx->texState.activeTexUnit;
		break;
	case GL_COLOR_WRITEMASK: 
		params[0] = ctx->color_mask_data.r;
		params[1] = ctx->color_mask_data.g ;
		params[2] = ctx->color_mask_data.b ;
		params[3] = ctx->color_mask_data.a;
		break;
	case GL_DEPTH_WRITEMASK:
		params[0] = ctx->depth_write_mask;
		break;
	case GL_STENCIL_WRITEMASK :
		params[0] = (float)ctx->front_stencil_writemask ;
		break;
	case GL_STENCIL_BACK_WRITEMASK :
		params[0] = (float)ctx->back_stencil_writemask;
		break;
	case GL_COLOR_CLEAR_VALUE :
		params[0] = ctx->clear_color_data.r;
		params[1] = ctx->clear_color_data.g ;
		params[2] = ctx->clear_color_data.b ;
		params[3] = ctx->clear_color_data.a;
		break;
	case GL_DEPTH_CLEAR_VALUE :
		params[0] = ctx->clear_depth;
		break;
	case GL_STENCIL_CLEAR_VALUE: 
		params[0] = (float)ctx->clear_stencil;
		break;
	case GL_SCISSOR_BOX:
		params[0] = (float)ctx->scissor_test_data.x;
		params[1] =(float) ctx->scissor_test_data.y;
		params[2] = (float)ctx->scissor_test_data.w;
		params[3] = (float)ctx->scissor_test_data.h;
		break;
	case GL_STENCIL_FUNC:
		params[0] = (float)ctx->stencil_test_data.front_face_func; 
		break;
	case GL_STENCIL_VALUE_MASK:
		params[0] = (float)ctx->stencil_test_data.front_face_mask ;
		break;
	case GL_STENCIL_REF :
		params[0] = (float)ctx->stencil_test_data.front_face_ref;
		break;
	case GL_STENCIL_FAIL :
		params[0] = (float)ctx->stencil_test_data.front_face_fail;
		break;
	case GL_STENCIL_PASS_DEPTH_FAIL :
		params[0] = (float)ctx->stencil_test_data.front_face_zfail;
		break;
	case GL_STENCIL_PASS_DEPTH_PASS :
		params[0] = (float)ctx->stencil_test_data.front_face_zpass;
		break;
	case GL_STENCIL_BACK_FUNC :
		params[0] = (float)ctx->stencil_test_data.back_face_func;
		break;
	case GL_STENCIL_BACK_VALUE_MASK :
		params[0] = (float)ctx->stencil_test_data.back_face_mask;
		break;
	case GL_STENCIL_BACK_REF :
		params[0] = (float)ctx->stencil_test_data.back_face_ref;
		break;  
	case GL_STENCIL_BACK_FAIL :
		params[0] = (float)ctx->stencil_test_data.back_face_fail;
		break;
	case GL_STENCIL_BACK_PASS_DEPTH_FAIL :
		params[0] = (float)ctx->stencil_test_data.back_face_zfail;
		break;
	case GL_STENCIL_BACK_PASS_DEPTH_PASS :
		params[0] = (float)ctx->stencil_test_data.back_face_zpass; 
		break;
	case GL_DEPTH_FUNC:
		params[0] = (float)ctx->depth_test_func;
		break;
	case GL_BLEND_SRC_RGB :
		params[0] = (float)ctx->blend_data.fn_srcRGB;
		break;
	case GL_BLEND_SRC_ALPHA:
		params[0] = (float)ctx->blend_data.fn_srcAlpha;
		break;
	case GL_BLEND_DST_RGB :
		params[0] = (float)ctx->blend_data.fn_dstRGB;
		break;
	case GL_BLEND_DST_ALPHA: 
		params[0] = (float)ctx->blend_data.fn_dstAlpha;
		break;
	case GL_BLEND_EQUATION_RGB :
		params[0] =(float)ctx->blend_data.eqn_modeRGB ;
		break;
	case GL_BLEND_EQUATION_ALPHA: 
		params[0] = (float)ctx->blend_data.eqn_modeAlpha;
		break;
	case GL_BLEND_COLOR :
		params[0] = ctx->blend_data.blnd_clr_red  ;
		params[1] = ctx->blend_data.blnd_clr_green;
		params[2] = ctx->blend_data.blnd_clr_blue;
		params[3] = ctx->blend_data.blnd_clr_alpha;
		break;
	case GL_UNPACK_ALIGNMENT:
		params[0] = (float)ctx->pixel_store_data.unpack_alignment;
		break;
	case GL_PACK_ALIGNMENT:
		params[0] = (float)ctx->pixel_store_data.pack_alignment;
		break;
	case GL_CURRENT_PROGRAM:
		params[0] = (float)ctx->current_program;
		break;
	case GL_GENERATE_MIPMAP_HINT:
		params[0] = (float)ctx->hint_data.generate_mipmap_hint;
		break;
	case GL_SUBPIXEL_BITS:
		//TODO
		break;
	case GL_MAX_TEXTURE_SIZE:
		params[0] = MAX_TEXTURE_SIZE;
		break;
	case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
		params[0] = MAX_CUBEMAP_TEXTURE_SIZE;
		break;
	case GL_MAX_VIEWPORT_DIMS:
		params[0] = MAX_VIEWPORT_DIMS;
		params[1] = MAX_VIEWPORT_DIMS;
		break;  
	case GL_ALIASED_POINT_SIZE_RANGE:
		//TODO
		break;
	case GL_ALIASED_LINE_WIDTH_RANGE:
		//TODO
		break;
	case GL_MAX_ELEMENTS_INDICES :
		params[0] = MAX_ELEMENTS_INDICES;
		break;
	case GL_MAX_ELEMENTS_VERTICES :
		params[0] = MAX_ELEMENTS_VERTICES;
		break;
	case GL_SAMPLE_BUFFERS :
		params[0] = NO_OF_SAMPLES;
		break;
	case GL_SAMPLES :
		params[0] = MULTISAMPLING;
		break;
	case GL_COMPRESSED_TEXTURE_FORMATS : 
		params[0] = GL_PALETTE4_RGB8_OES;
		params[1] = GL_PALETTE4_RGBA8_OES;
		params[2] = GL_PALETTE4_R5_G6_B5_OES;
		params[3] = GL_PALETTE4_RGBA4_OES;
		params[4] = GL_PALETTE4_RGB5_A1_OES;
		params[5] = GL_PALETTE8_RGB8_OES;
		params[6] = GL_PALETTE8_RGBA8_OES;
		params[7] = GL_PALETTE8_R5_G6_B5_OES;
		params[8] = GL_PALETTE8_RGBA4_OES;
		params[9] = GL_PALETTE8_RGB5_A1_OES;
		params[10] = GL_RGB_S3TC_OES;
		params[11] = GL_RGBA_S3TC_OES;
		break;
	case GL_NUM_COMPRESSED_TEXTURE_FORMATS :
		params[0] = NUM_COMPRESSED_TEXTURE_FORMATS;
		break;
		//case GL_SHADER_BINARY_FORMATS :
		//TODO  unknown format supported
		//  break;
	case GL_NUM_SHADER_BINARY_FORMATS :
		params[0] = NUM_SHADER_BINARY_FORMATS;
		break;
	case GL_SHADER_COMPILER :
		params[0] = GL_FALSE;
		break;
	case GL_MAX_VERTEX_ATTRIBS :
		params[0] = MAX_VERTEX_ATTRIBS;
		break;
	case GL_MAX_VERTEX_UNIFORM_VECTORS :
		params[0] = MAX_UNIFORMS;
		break;
	case GL_MAX_VARYING_VECTORS :
		params[0] =MAX_VARYING_VECTORS;
		break;
	case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS :
		params[0] =MAX_VERTEX_TEXTURE_UNITS + MAX_TEXTURE_UNITS; 
		break;
	case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS :
		params[0] = MAX_VERTEX_TEXTURE_UNITS;
		break;
	case GL_MAX_TEXTURE_IMAGE_UNITS :
		params[0] = MAX_TEXTURE_UNITS;
		break;
	case GL_MAX_FRAGMENT_UNIFORM_VECTORS :
		params[0] =MAX_UNIFORMS;
		break;
	case GL_MAX_RENDERBUFFER_SIZE :
		params[0] = MAX_RENDERBUFFER_SIZE;
		break;
	case GL_RED_BITS :
		params[0] = (float)determinePixelBitSize(fbData.nativeColorFormat,RED) ;
		break;
	case GL_GREEN_BITS: 
		params[0] = (float)determinePixelBitSize(fbData.nativeColorFormat,GREEN) ;
		break;
	case GL_BLUE_BITS :
		params[0] = (float)determinePixelBitSize(fbData.nativeColorFormat,BLUE) ;
		break;
	case GL_ALPHA_BITS :
		params[0] = (float)determinePixelBitSize(fbData.nativeColorFormat,ALPHA) ;
		break;
	case GL_DEPTH_BITS :
		params[0] = (float)determinePixelBitSize(fbData.nativeDepthStencilFormat,DEPTH) ;
		break;
	case GL_STENCIL_BITS: 
		params[0] = (float)determinePixelBitSize(fbData.nativeDepthStencilFormat,STENCIL) ;
		break; 
	case GL_IMPLEMENTATION_COLOR_READ_TYPE: 
		params[0] = (float)determineTypeFormat(translateToGLenum(fbData.nativeColorFormat), GL_FALSE);
		break;
	case GL_IMPLEMENTATION_COLOR_READ_FORMAT:
		params[0] = (float)determineTypeFormat(translateToGLenum(ctx->defFBData.nativeColorFormat), GL_TRUE);   
		break;
#ifdef EN_EXT_ALPHA_TEST_EXP
	case GL_ALPHA_TEST_FUNC_EXP:
		params[0] = (float)ctx->alpha_test_data.func;
		break;

	case GL_ALPHA_TEST_REF_EXP:
		params[0] = ctx->alpha_test_data.refValue;
		break;
#endif


#ifdef EN_EXT_LOGIC_OP_EXP
	case GL_LOGIC_OP_MODE_EXP:
		params[0] = ctx->is_logicOp_enabled;
		break;
#endif
		
		//all the parameter supported by glIsEnabled ahs to be supported by glGetBooleanv        
	case GL_CULL_FACE:
	case GL_POLYGON_OFFSET_FILL:
	case GL_SAMPLE_COVERAGE:
	case GL_SAMPLE_ALPHA_TO_COVERAGE:
	case GL_SCISSOR_TEST:
	case GL_STENCIL_TEST:
	case GL_DEPTH_TEST:
	case GL_BLEND:
	case GL_DITHER: 
#ifdef EN_EXT_ALPHA_TEST_EXP
	case GL_ALPHA_TEST_EXP:       
#endif

#ifdef EN_EXT_LOGIC_OP_EXP
	case GL_COLOR_LOGIC_OP:             
#endif

		params[0] = glIsEnabled(pname);
		break;
	default:
		SET_ERR(ctx, GL_INVALID_ENUM, "glGetFloatv*");
	}
}
GL_API void GL_APIENTRY glGetIntegerv (GLenum pname, GLint *params)
{
	GET_GL_STATE(ctx);
	FramebufferData fbData = getFBData();
	
	if(params == NULL)
	SET_ERR(ctx, GL_INVALID_VALUE, "glGetFloatv*");
	else
	switch(pname)
	{
		//the array buffer object's id  currently binded to check
	case GL_ARRAY_BUFFER_BINDING:
		if(ctx->buffer_object_data.array_buffer_binding != NULL){
			params[0] = ctx->buffer_object_data.array_buffer_binding->id;
		}else params[0] = 0;
		break;
		//the element array buffer object's id currently binded to check        
	case GL_ELEMENT_ARRAY_BUFFER_BINDING:
		if(ctx->buffer_object_data.element_array_buffer_binding != NULL){
			params[0] = ctx->buffer_object_data.element_array_buffer_binding->id;
		}else params[0] = 0;
		
		break;
	case GL_VIEWPORT:
		params[0] = ctx->viewport_data.x;
		params[1] = ctx->viewport_data.y;
		params[2] = ctx->viewport_data.w ;
		params[3] = ctx->viewport_data.h ;
		break;
	case GL_DEPTH_RANGE:
		params[0] = (int)((pow(2.0, 32.0) - 1) * ctx->depth_range_data.near_depth -1)/2;
		params[1] = (int)((pow(2.0, 32.0) - 1) * ctx->depth_range_data.far_depth - 1)/2;
		break;
	case GL_POINT_SIZE:
		params[0] = (int)floor(ctx->point_size + 0.5);
		break;  
	case GL_LINE_WIDTH:
		params[0] = (int)floor(ctx->line_width + 0.5);
		break;
	case GL_CULL_FACE_MODE:
		params[0] = ctx->cull_face_data.mode;
		break;
	case GL_FRONT_FACE:
		params[0] = ctx->front_face_mode;
		break;
	case GL_POLYGON_OFFSET_FACTOR:
		params[0] = (int)floor(ctx->polygon_offset_data.factor + 0.5);
		break;  
	case GL_POLYGON_OFFSET_UNITS:
		params[0] = (int)floor(ctx->polygon_offset_data.units + 0.5);
		break;
	case GL_SAMPLE_COVERAGE_VALUE:
		params[0] = (int)floor(ctx->sample_coverage_data.value  + 0.5);//to check
		break;
	case GL_SAMPLE_COVERAGE_INVERT:
		params[0] = ctx->sample_coverage_data.invert;
		break;
	case GL_TEXTURE_BINDING_2D:
		params[0] = ctx->texState.texUnitBinding[ctx->texState.activeTexUnit].texture2D;//to check
		//params[0] = GetTexNameArrayIndex(params[0] );
		//params[0] = GetTextureObject(GL_TEXTURE_2D,false,0); not right commented
		
		break;
	case GL_TEXTURE_BINDING_3D:
		params[0] = ctx->texState.texUnitBinding[ctx->texState.activeTexUnit].texture3D;
		//params[0] = GetTexNameArrayIndex(params[0] );
		//params[0] = GetTextureObject(GL_TEXTURE_3D,false,0);
		break;
	case GL_TEXTURE_BINDING_CUBE_MAP:
		params[0] = ctx->texState.texUnitBinding[ctx->texState.activeTexUnit].cubeMap;
		//params[0] = GetTexNameArrayIndex(params[0] );
		//params[0] = GetTextureObject(GL_TEXTURE_CUBE_MAP,false,0);
		break;
	case GL_ACTIVE_TEXTURE:
		params[0] = ctx->texState.activeTexUnit;
		break;
	case GL_COLOR_WRITEMASK: 
		params[0] = ctx->color_mask_data.r;
		params[1] = ctx->color_mask_data.g ;
		params[2] = ctx->color_mask_data.b ;
		params[3] = ctx->color_mask_data.a;
		break;
	case GL_DEPTH_WRITEMASK:
		params[0] = ctx->depth_write_mask;
		break;
	case GL_STENCIL_WRITEMASK :
		params[0] = ctx->front_stencil_writemask ;
		break;
	case GL_STENCIL_BACK_WRITEMASK :
		params[0] = ctx->back_stencil_writemask;
		break;
	case GL_COLOR_CLEAR_VALUE :
		params[0] = (int)((pow(2.0, 32.0) - 1) * ctx->clear_color_data.r - 1)/2;
		params[1] = (int)((pow(2.0, 32.0) - 1) * ctx->clear_color_data.g - 1)/2;
		params[2] = (int)((pow(2.0, 32.0) - 1) * ctx->clear_color_data.b - 1)/2;
		params[3] = (int)((pow(2.0, 32.0) - 1) * ctx->clear_color_data.a - 1)/2;
		break;
	case GL_DEPTH_CLEAR_VALUE :
		params[0] = (GLint)ctx->clear_depth;
		break;
	case GL_STENCIL_CLEAR_VALUE: 
		params[0] =ctx->clear_stencil;
		break;
	case GL_SCISSOR_BOX:
		params[0] = ctx->scissor_test_data.x;
		params[1] = ctx->scissor_test_data.y;
		params[2] = ctx->scissor_test_data.w;
		params[3] = ctx->scissor_test_data.h;
		break;
	case GL_STENCIL_FUNC:
		params[0] = ctx->stencil_test_data.front_face_func;
		break;
	case GL_STENCIL_VALUE_MASK:
		params[0] = ctx->stencil_test_data.front_face_mask ;
		break;
	case GL_STENCIL_REF :
		params[0] = ctx->stencil_test_data.front_face_ref;
		break;
	case GL_STENCIL_FAIL :
		params[0] = ctx->stencil_test_data.front_face_fail;
		break;
	case GL_STENCIL_PASS_DEPTH_FAIL :
		params[0] = ctx->stencil_test_data.front_face_zfail;
		break;
	case GL_STENCIL_PASS_DEPTH_PASS :
		params[0] = ctx->stencil_test_data.front_face_zpass;
		break;
	case GL_STENCIL_BACK_FUNC :
		params[0] = ctx->stencil_test_data.back_face_func;
		break;
	case GL_STENCIL_BACK_VALUE_MASK :
		params[0] = ctx->stencil_test_data.back_face_mask;
		break;
	case GL_STENCIL_BACK_REF :
		params[0] = ctx->stencil_test_data.back_face_ref;
		break;      
	case GL_STENCIL_BACK_FAIL :
		params[0] = ctx->stencil_test_data.back_face_fail;
		break;
	case GL_STENCIL_BACK_PASS_DEPTH_FAIL :
		params[0] = ctx->stencil_test_data.back_face_zfail;
		break;
	case GL_STENCIL_BACK_PASS_DEPTH_PASS :
		params[0] = ctx->stencil_test_data.back_face_zpass; 
		break;
	case GL_DEPTH_FUNC:
		params[0] = ctx->depth_test_func;
		break;
	case GL_BLEND_SRC_RGB :
		params[0] = ctx->blend_data.fn_srcRGB;
		break;
	case GL_BLEND_SRC_ALPHA:
		params[0] = ctx->blend_data.fn_srcAlpha;
		break;
	case GL_BLEND_DST_RGB :
		params[0] = ctx->blend_data.fn_dstRGB;
		break;
	case GL_BLEND_DST_ALPHA: 
		params[0] =ctx->blend_data.fn_dstAlpha;
		break;
	case GL_BLEND_EQUATION_RGB :
		params[0] =ctx->blend_data.eqn_modeRGB ;
		break;
	case GL_BLEND_EQUATION_ALPHA: 
		params[0] =ctx->blend_data.eqn_modeAlpha;
		break;
	case GL_BLEND_COLOR :
		params[0] = (int)((pow(2.0, 32.0) - 1) * ctx->blend_data.blnd_clr_red - 1)/2  ;
		params[1] = (int)((pow(2.0, 32.0) - 1) * ctx->blend_data.blnd_clr_green - 1)/2;
		params[2] = (int)((pow(2.0, 32.0) - 1) * ctx->blend_data.blnd_clr_blue - 1)/2;
		params[3] = (int)((pow(2.0, 32.0) - 1) * ctx->blend_data.blnd_clr_alpha - 1)/2;
		break;
	case GL_UNPACK_ALIGNMENT:
		params[0] =ctx->pixel_store_data.unpack_alignment;
		break;
	case GL_PACK_ALIGNMENT:
		params[0] =ctx->pixel_store_data.pack_alignment;
		break;
	case GL_CURRENT_PROGRAM:
		params[0] = ctx->current_program;
		break;
	case GL_GENERATE_MIPMAP_HINT:
		params[0] = ctx->hint_data.generate_mipmap_hint;
		break;
	case GL_SUBPIXEL_BITS:
		//TODO
		break;
	case GL_MAX_TEXTURE_SIZE:
		params[0] = MAX_TEXTURE_SIZE;
		break;
	case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
		params[0] = MAX_CUBEMAP_TEXTURE_SIZE;
		break;
	case GL_MAX_VIEWPORT_DIMS:
		params[0] = MAX_VIEWPORT_DIMS;
		params[1] = MAX_VIEWPORT_DIMS;
		break;
	case GL_MAX_ELEMENTS_INDICES :
		params[0] = MAX_ELEMENTS_INDICES;
		break;
	case GL_MAX_ELEMENTS_VERTICES :
		params[0] = MAX_ELEMENTS_VERTICES;
		break;
	case GL_SAMPLE_BUFFERS :
		params[0] = NO_OF_SAMPLES;
		break;
	case GL_SAMPLES :
		params[0] = MULTISAMPLING;
		break;
	case GL_COMPRESSED_TEXTURE_FORMATS :
		params[0] = GL_PALETTE4_RGB8_OES;
		params[1] = GL_PALETTE4_RGBA8_OES;
		params[2] = GL_PALETTE4_R5_G6_B5_OES;
		params[3] = GL_PALETTE4_RGBA4_OES;
		params[4] = GL_PALETTE4_RGB5_A1_OES;
		params[5] = GL_PALETTE8_RGB8_OES;
		params[6] = GL_PALETTE8_RGBA8_OES;
		params[7] = GL_PALETTE8_R5_G6_B5_OES;
		params[8] = GL_PALETTE8_RGBA4_OES;
		params[9] = GL_PALETTE8_RGB5_A1_OES;
		params[10] = GL_RGB_S3TC_OES;
		params[11] = GL_RGBA_S3TC_OES;
		break;
	case GL_NUM_COMPRESSED_TEXTURE_FORMATS :
		params[0] = NUM_COMPRESSED_TEXTURE_FORMATS;
		break;
		//case GL_SHADER_BINARY_FORMATS :
		//TODO
		//  break; 
	case GL_NUM_SHADER_BINARY_FORMATS :
		params[0] = NUM_SHADER_BINARY_FORMATS;
		break;
	case GL_SHADER_COMPILER :
		params[0] = GL_FALSE;
		break;
	case GL_MAX_VERTEX_ATTRIBS :
		params[0] = MAX_VERTEX_ATTRIBS;
		break;
		
	case GL_MAX_VERTEX_UNIFORM_VECTORS :
		params[0] = MAX_UNIFORMS;
		break;
	case GL_MAX_VARYING_VECTORS :
		params[0] = MAX_VARYING_VECTORS; 
		break;
	case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS :
		params[0] =MAX_VERTEX_TEXTURE_UNITS + MAX_TEXTURE_UNITS;
		break;
	case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS :
		params[0] = MAX_VERTEX_TEXTURE_UNITS;
		break;
	case GL_MAX_TEXTURE_IMAGE_UNITS :
		params[0] = MAX_TEXTURE_UNITS;
		break;
	case GL_MAX_FRAGMENT_UNIFORM_VECTORS :
		params[0] = MAX_UNIFORMS;
		break;
	case GL_MAX_RENDERBUFFER_SIZE :
		params[0] = MAX_RENDERBUFFER_SIZE;
		break;
	case GL_RED_BITS :
		params[0] = determinePixelBitSize(fbData.nativeColorFormat,RED) ;
		break;
	case GL_GREEN_BITS: 
		params[0] = determinePixelBitSize(fbData.nativeColorFormat,GREEN) ;
		break;
	case GL_BLUE_BITS :
		params[0] = determinePixelBitSize(fbData.nativeColorFormat,BLUE) ;
		break;
	case GL_ALPHA_BITS :
		params[0] = determinePixelBitSize(fbData.nativeColorFormat,ALPHA) ;
		break;
	case GL_DEPTH_BITS :
		params[0] = determinePixelBitSize(fbData.nativeDepthStencilFormat,DEPTH);
		break;
	case GL_STENCIL_BITS: 
		params[0] = determinePixelBitSize(fbData.nativeDepthStencilFormat,STENCIL);
		break;
	case GL_IMPLEMENTATION_COLOR_READ_TYPE: 
		params[0] = determineTypeFormat(translateToGLenum(fbData.nativeColorFormat), GL_FALSE);
		break;
	case GL_IMPLEMENTATION_COLOR_READ_FORMAT:
		params[0] = determineTypeFormat( translateToGLenum(ctx->defFBData.nativeColorFormat), GL_TRUE);
		break;  
#ifdef EN_EXT_ALPHA_TEST_EXP
	case GL_ALPHA_TEST_FUNC_EXP:
		params[0] = ctx->alpha_test_data.func;
		break;

	case GL_ALPHA_TEST_REF_EXP:
		params[0] = (int)ctx->alpha_test_data.refValue;
		break;
#endif


#ifdef EN_EXT_LOGIC_OP_EXP
	case GL_LOGIC_OP_MODE_EXP:
		//  params[0] = ctx->is_logicOp_enabled;
		params[0] = ctx->logicOp;
		break;
#endif

	case GL_CULL_FACE:
	case GL_POLYGON_OFFSET_FILL:
	case GL_SAMPLE_COVERAGE:
	case GL_SAMPLE_ALPHA_TO_COVERAGE:
	case GL_SCISSOR_TEST:
	case GL_STENCIL_TEST:
	case GL_DEPTH_TEST:
	case GL_BLEND:
	case GL_DITHER: 
#ifdef EN_EXT_ALPHA_TEST_EXP
	case GL_ALPHA_TEST_EXP:       
#endif

#ifdef EN_EXT_LOGIC_OP_EXP
	case GL_COLOR_LOGIC_OP:             
#endif

		params[0] = glIsEnabled(pname);
		break;
	default:
		SET_ERR(ctx, GL_INVALID_ENUM, "glGetIntegerv*");
	}

}

GL_API void GL_APIENTRY glGetPointerv (GLenum pname, void **params)
{
	//GET_GL_STATE(ctx);
	//NOT TO BE IMPLEMENTED FOR ES2.0

}


GL_API const GLubyte * GL_APIENTRY glGetString (GLenum name)
{
	GET_GL_STATE(ctx);

	switch(name)
	{
	case GL_VERSION:
		//return (GLubyte*)ctx->str_data.version;
		return (const GLubyte*)GLES2_VERSION_STRING;
		break;

	case GL_VENDOR:
		return (const GLubyte*)ctx->str_data.vendor;
		break;

	case GL_EXTENSIONS:
		return (const GLubyte*)ctx->str_data.extensions;
		break;

	case GL_RENDERER:
		return (const GLubyte*)ctx->str_data.renderer;
		break;

	case GL_SHADING_LANGUAGE_VERSION:
		return (const GLubyte*)ctx->str_data.shadingLangVersion;
		break;

	default:
		SET_ERR(ctx, GL_INVALID_ENUM, "glGetString");
	};

	return 0;
}

GLuint GetVertexAttribfv (GLuint index, GLenum pname, GLfloat *params)
{
	GET_GL_STATE(ctx);

	if( (index < 0) || (index >=MAX_VERTEX_ATTRIBS) )               //modified by chanchal
	{
		set_err(GL_INVALID_VALUE);
		return 0;
	}

	switch(pname)
	{
	case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
		*params = ctx->vertex_attrib_array_data[index].enabled;
		return 1;
		break;

	case GL_VERTEX_ATTRIB_ARRAY_SIZE:
		*params = (GLfloat) ctx->vertex_attrib_array_data[index].size;
		return 1;
		break;

	case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
		*params = (GLfloat) ctx->vertex_attrib_array_data[index].stride;
		return 1;
		break;

	case GL_VERTEX_ATTRIB_ARRAY_TYPE:
		*params = (GLfloat) ctx->vertex_attrib_array_data[index].type;
		return 1;
		break;

	case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
		*params = (GLfloat) ctx->vertex_attrib_array_data[index].normalized;
		return 1;
		break;
		//has to be moved to glGetVertexAttributes but new gl.h doen not have this API
	case GL_CURRENT_VERTEX_ATTRIB:
		Plat::memcpy(params, ctx->vertex_attribs[index], 4 * sizeof(GLfloat));
		return 4;
		break;
	case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
		*params = (GLfloat) ctx->vertex_attrib_array_data[index].vboID; 
		return 1;
		break;
	default:
		set_err(GL_INVALID_ENUM);
		return 0;
	}
}


GL_API void GL_APIENTRY glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat *params)
{
	GetVertexAttribfv(index, pname, params);

}

GL_API void GL_APIENTRY glGetVertexAttribiv (GLuint index, GLenum pname, GLint *params)
{
	GLfloat     p[4];

	GLint count = GetVertexAttribfv(index, pname, p);
	switch(count){
	case 4:         params[3] = (GLint) p[3];
	case 3:         params[2] = (GLint) p[2];
	case 2:         params[1] = (GLint) p[1];
	case 1:         params[0] = (GLint) p[0];
	}
}

GL_API void GL_APIENTRY glGetVertexAttribPointerv (GLuint index, GLenum pname, void **pointer)
{
	GET_GL_STATE(ctx);

	if(pname != GL_VERTEX_ATTRIB_ARRAY_POINTER)
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	if((index < 0) ||(index >=MAX_VERTEX_ATTRIBS) || (pointer == NULL))             //modified by chanchal
	{
		set_err(GL_INVALID_VALUE);
		return;
	}
	if (ctx->vertex_attrib_array_data[index].vboID == 0)
	{
		*pointer = (void *) ctx->vertex_attrib_array_data[index].ptr;
	}
	else
	{
		*pointer = (void *) ((int)ctx->vertex_attrib_array_data[index].ptr -(int) ctx->vertex_attrib_array_data[index].vertex_attrib_ptr_binding->data );
	}
}

GL_API void GL_APIENTRY glHint (GLenum target, GLenum mode)
{
	GET_GL_STATE(ctx);

	//validate mode
	switch(mode)
	{
	case GL_DONT_CARE:
	case GL_NICEST:
	case GL_FASTEST:
		break;                  //intended fallthrough

	default:
		set_err(GL_INVALID_ENUM);
		return;
	}

	//validate target and set the state for the target
	switch(target)
	{
	case GL_GENERATE_MIPMAP_HINT:
		ctx->hint_data.generate_mipmap_hint = mode;
		break;

	case GL_FRAGMENT_SHADER_DERIVATIVE_HINT:
		ctx->hint_data.fragment_shader_derivative_hint = mode;
		break;

	default:
		set_err(GL_INVALID_ENUM);
		return;
	}
}

GL_API GLboolean GL_APIENTRY glIsBuffer (GLuint buffer)
{
	GET_GL_STATE(ctx);

	int                   i;
	BufferObjectData*   p = &ctx->buffer_object_data;

	for(i = 0; i < p->buffer_objects_last_index; i++)
	{
		if( p->buffer_objects[i].in_use &&
				p->buffer_objects[i].id == buffer)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GL_API GLboolean GL_APIENTRY glIsEnabled (GLenum cap)
{
	GET_GL_STATE(ctx);

	switch(cap)
	{
	case GL_CULL_FACE:
		return ctx->cull_face_data.is_enabled;
		break;

	case GL_POLYGON_OFFSET_FILL:
		return ctx->polygon_offset_data.is_enabled_fill;
		break;

	case GL_SAMPLE_COVERAGE:                            
		return ctx->sample_coverage_data.is_enabled_coverage; 
		break;

	case GL_SAMPLE_ALPHA_TO_COVERAGE:           
		return ctx->sample_coverage_data.is_enabled_alpha_to_coverage;
		break;

	case GL_SCISSOR_TEST:                                       
		return ctx->scissor_test_data.is_enabled ;                              
		break;

	case GL_STENCIL_TEST:                                       
		return ctx->stencil_test_data.is_enabled;                               
		break;
	case GL_DEPTH_TEST:                                         
		return ctx->is_enabled_depth ;          
		break;

	case GL_BLEND:                                                      
		return ctx->blend_data.is_enabled;              
		break;

	case GL_DITHER:                                                     
		return ctx->is_enabled_dither;          
		break;

#ifdef EN_EXT_ALPHA_TEST_EXP
	case GL_ALPHA_TEST_EXP:
		return ctx->alpha_test_data.is_enabled;
		break;
#endif

#ifdef EN_EXT_LOGIC_OP_EXP
	case GL_COLOR_LOGIC_OP:
		return ctx->is_logicOp_enabled;
		break;
#endif

#ifdef EN_EXT_POINT_SPRITE_EXP
	case GL_POINT_SPRITE_OES_EXP:
		return ctx->is_point_sprite_enable;
		break;
#endif

	default:
		set_err(GL_INVALID_ENUM);               
	}
	return GL_FALSE;
}

GL_API void GL_APIENTRY glLineWidth (GLfloat width)
{
	GET_GL_STATE(ctx);

	if(width <= 0.0)
	{
		set_err(GL_INVALID_VALUE);
		return;
	}

	//TODO: clamp to max value?
	ctx->line_width = width;
	SetUpDateFlagRasterEngine(ctx,GLF_UF_LWidth);
	//ctx->updateBits.re = UB_RE_ALL;
}

GL_API void GL_APIENTRY glPixelStorei (GLenum pname, GLint param)
{
	GET_GL_STATE(ctx);

	switch(param)
	{
	case 1:
	case 2:
	case 4:
	case 8:
		break;  //Intended fallthough
	default:
		set_err(GL_INVALID_VALUE);
		return;
	}

	switch (pname)
	{
	case GL_PACK_ALIGNMENT:
		ctx->pixel_store_data.pack_alignment = param;
		break ;
	case GL_UNPACK_ALIGNMENT:
		ctx->pixel_store_data.unpack_alignment = param;
		break;
	default:
		set_err(GL_INVALID_ENUM);
		return;
	}
}

GL_API void GL_APIENTRY glPointSize (GLfloat size)
{
	GET_GL_STATE(ctx);

	if(size <= 0.0)
	{
		set_err(GL_INVALID_VALUE);
	}

	//TODO: some clamping to be done?
	ctx->point_size = size;
	SetUpDateFlagRasterEngine(ctx,GLF_UF_PWidth);
	//ctx->updateBits.re = UB_RE_ALL;
}

GL_API void GL_APIENTRY glPolygonOffset (GLfloat factor, GLfloat units)
{
	GET_GL_STATE(ctx);

	ctx->polygon_offset_data.factor = factor;
	ctx->polygon_offset_data.units = units;
	SetUpDateFlagRasterEngine(ctx,GLF_UF_DepthOffset);
	//ctx->updateBits.re = UB_RE_ALL;
}

#ifdef USE_3D_PM
void PM_glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);

GL_API void GL_APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels)
{
	lock3DCriticalSection();
	PM_glReadPixels ( x,  y,  width,  height,  format,  type, pixels);
	unlock3DCriticalSection();
}

void PM_glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels)
#else
GL_API void GL_APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels)
#endif
{
	GET_GL_STATE(ctx);
	FramebufferData fbData = getFBData();
	
	GLenum nativeType = determineTypeFormat(translateToGLenum(fbData.nativeColorFormat), GL_FALSE);
	GLenum nativeFmt  =  determineTypeFormat( translateToGLenum(ctx->defFBData.nativeColorFormat), GL_TRUE);

	
	if(!((format == GL_RGBA && type== GL_UNSIGNED_BYTE) || (format == nativeFmt) && (type == nativeType)))
	{
		SET_ERR(ctx, GL_INVALID_ENUM, "glReadPixels");
		return;
	}

	if(width < 0 || height < 0)
	{
		SET_ERR(ctx, GL_INVALID_VALUE, "glReadPixels");
		return;
	}
	
	//Check if FB is complete
	if(!isFBrenderable(ctx))
	{
		set_err(GL_INVALID_FRAMEBUFFER_OPERATION);
		return;
	}

	//Get info from FrameBuffer...
	//  FramebufferData fbData = getFBData();
	//GLenum FrameBufFormat = GLenum(fbData.colorFormat);
	//GLenum FrameType = GLenum(fbData.colorType);

	void* FramePixels = fbData.colorAddr.vaddr;
	int FrameWidth = fbData.width;
	int FrameHeight = fbData.height;
	
	int cpyWidth = width;
	int cpyHeight = height;
	
	int dstX = 0;
	int dstY = 0;
	
	/*To take care of the case when the point x,y is such that the rectangle to be copied is outside the system's FB area*/
	if( (x + width) <= 0 || (x >= FrameWidth) || (y + height) <= 0 || (y >= FrameHeight) )
	return;

	if(x < 0)
	{
		cpyWidth += x;  
		dstX = -x;
		x = 0;
	}
	if( (cpyWidth + x) > FrameWidth) 
	cpyWidth = FrameWidth - x;
	
	if(y < 0)
	{
		cpyHeight += y;  
		dstY = -y;
		y = 0;
	}
	if( (cpyHeight + y) > FrameHeight) 
	cpyHeight = FrameHeight - y;


	GLint alignment = ctx->pixel_store_data.pack_alignment;
	int pixelSize = GetPixelSize(format,type);      
	GLint rowSize = width * pixelSize;
	GLint padding = rowSize%alignment;
	
	PxFmt fmtData =  translateGLInternal(format,type);
	//PxFmt fmtFB =  translateGLSizedInternal(FrameBufFormat); 
	PxFmt fmtFB = fbData.nativeColorFormat;

	GLint dir = 1;
	if(fbData.flipped == 0)
	dir = -1;//y = FrameHeight - y - height + 1*(!(!y));//-1 so as to read in reverse direction...
	else dir = 1;
	////LOGMSG("dir = %d",dir);
	
	FimgFinish(ctx); 
	
	convertPixels( fmtData, pixels, width, height, 0,
	dstX,  dstY,  0,
	fmtFB, FramePixels, FrameWidth, FrameHeight, 0,
	x, y, 0,
	cpyWidth, cpyHeight, 0 , padding , 0,dir);
	SwapRB((GLubyte*)pixels,format,type,width,height,0,0,0,0,width,height,0,pixelSize);
#ifdef CACHE_MEM
	clean_invalidate_cached_buffer();
#endif        
}

GL_API void GL_APIENTRY glSampleCoverage (GLclampf value, GLboolean invert)
{
	GET_GL_STATE(ctx);

	ctx->sample_coverage_data.value = clamp(value);
	ctx->sample_coverage_data.invert = invert;
}

GL_API void GL_APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
	GET_GL_STATE(ctx);

	//TODO: x<0 and y<0 is not an invalid value according to the spec, so clamp to zero? Spec doesn't say anything about this!
	if(/*(x < 0) || (y < 0) || */(width < 0) || (height < 0))
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	ctx->scissor_test_data.x = x;
	ctx->scissor_test_data.y = y;
	ctx->scissor_test_data.w = width;
	ctx->scissor_test_data.h = height;

#if GLF_SCISSOR_IN_RA == ENABLE
	SetUpDateFlagRasterEngine(ctx,GLF_UF_ScissorTest);
#else
	SetUpDateFlagPerFragment(ctx, GLF_UF_ScissorTest);
#endif
}

GL_API void GL_APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask)
{
	//GET_GL_STATE(ctx);

	glStencilFuncSeparate(GL_FRONT_AND_BACK, func, ref, mask);
}

GL_API void GL_APIENTRY glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask)
{
	GET_GL_STATE(ctx);

	//TODO: ref should be clamped to the range [0, 2^n -1], where n is the number of bitplanes in the stencil buffer.
	// should this be done here or while setting the state to the hw? What should glGet return for this?

	//TODO: use the common func .. the one used by depth
	switch(func)  //check if func is a valid enum
	{
	case GL_NEVER:
	case GL_ALWAYS:
	case GL_LESS:
	case GL_LEQUAL:
	case GL_EQUAL:
	case GL_GEQUAL:
	case GL_GREATER:
	case GL_NOTEQUAL:
		break; //intended fallthrough

	default:
		set_err(GL_INVALID_ENUM);
		return;
	}


	switch(face)
	{
	case GL_FRONT:
		ctx->stencil_test_data.front_face_func  = func;
		ctx->stencil_test_data.front_face_ref   = ref;
		ctx->stencil_test_data.front_face_mask  = mask;
		SetUpDateFlagPerFragment(ctx,GLF_UF_StencilTest); 
		break;

	case GL_BACK:
		ctx->stencil_test_data.back_face_func  = func;
		ctx->stencil_test_data.back_face_ref   = ref;
		ctx->stencil_test_data.back_face_mask  = mask;
		SetUpDateFlagPerFragment(ctx,GLF_UF_StencilTest); 
		break;

	case GL_FRONT_AND_BACK:
		ctx->stencil_test_data.front_face_func  = func;
		ctx->stencil_test_data.front_face_ref   = ref;
		ctx->stencil_test_data.front_face_mask  = mask;

		ctx->stencil_test_data.back_face_func  = func;
		ctx->stencil_test_data.back_face_ref   = ref;
		ctx->stencil_test_data.back_face_mask  = mask;
		SetUpDateFlagPerFragment(ctx,GLF_UF_StencilTest); 
		break;

	default:
		set_err(GL_INVALID_ENUM);
		return;
	}

}

GL_API void GL_APIENTRY glStencilMask (GLuint mask)
{
	//GET_GL_STATE(ctx);

	glStencilMaskSeparate(GL_FRONT_AND_BACK, mask);
}

GL_API void GL_APIENTRY glStencilMaskSeparate (GLenum face, GLuint mask)
{
	GET_GL_STATE(ctx);

	switch(face)
	{
	case GL_FRONT:
		ctx->front_stencil_writemask  = mask;
		SetUpDateFlagPerFragment(ctx,GLF_UF_StencilTest); 
		break;

	case GL_BACK:
		ctx->front_stencil_writemask  = mask;
		SetUpDateFlagPerFragment(ctx,GLF_UF_StencilTest);
		break;

	case GL_FRONT_AND_BACK:
		ctx->front_stencil_writemask  = mask;
		ctx->back_stencil_writemask  = mask;
		SetUpDateFlagPerFragment(ctx,GLF_UF_StencilTest); 
		break;

	default:
		set_err(GL_INVALID_ENUM);
		return;
	}
}

GL_API void GL_APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
	//GET_GL_STATE(ctx);

	glStencilOpSeparate(GL_FRONT_AND_BACK, fail, zfail, zpass);
}

GL_API void GL_APIENTRY glStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
	GET_GL_STATE(ctx);

	// fail, zfail, zpass can only take the following values: GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_DECR, and GL_INVERT
	//so validate

	if( (isValidStencilOp(fail) == GL_FALSE)
			|| (isValidStencilOp(zfail) == GL_FALSE)
			|| (isValidStencilOp(zpass) == GL_FALSE))
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	switch(face)
	{
	case GL_FRONT:
		ctx->stencil_test_data.front_face_fail  = fail;
		ctx->stencil_test_data.front_face_zfail = zfail;
		ctx->stencil_test_data.front_face_zpass = zpass;
		SetUpDateFlagPerFragment(ctx,GLF_UF_StencilTest);
		break;

	case GL_BACK:
		ctx->stencil_test_data.back_face_fail  = fail;
		ctx->stencil_test_data.back_face_zfail = zfail;
		ctx->stencil_test_data.back_face_zpass = zpass;
		SetUpDateFlagPerFragment(ctx,GLF_UF_StencilTest); 
		break;

	case GL_FRONT_AND_BACK:
		ctx->stencil_test_data.front_face_fail  = fail;
		ctx->stencil_test_data.front_face_zfail = zfail;
		ctx->stencil_test_data.front_face_zpass = zpass;

		ctx->stencil_test_data.back_face_fail  = fail;
		ctx->stencil_test_data.back_face_zfail = zfail;
		ctx->stencil_test_data.back_face_zpass = zpass;
		SetUpDateFlagPerFragment(ctx,GLF_UF_StencilTest);
		break;

	default:
		set_err(GL_INVALID_ENUM);
		return;
	}
}

#ifdef EN_EXT_ALPHA_TEST_EXP
GL_API void GL_APIENTRY glAlphaFuncEXP (GLenum func, GLclampf ref)
{
	GET_GL_STATE(ctx);

	switch(func)  //check if func is a valid enum
	{
	case GL_NEVER:
	case GL_ALWAYS:
	case GL_LESS:
	case GL_LEQUAL:
	case GL_EQUAL:
	case GL_GEQUAL:
	case GL_GREATER:
	case GL_NOTEQUAL:
		break; //intended fallthrough

	default:
		set_err(GL_INVALID_ENUM);
		return;
	}

	ctx->alpha_test_data.func = func;
	ctx->alpha_test_data.refValue = ref;
	SetUpDateFlagPerFragment(ctx,GLF_UF_AlphaTest);

}
#endif

#ifdef EN_EXT_LOGIC_OP_EXP
GL_API void GL_APIENTRY glLogicOpEXP(GLenum op)
{
	GET_GL_STATE(ctx);
	switch(op)
	{

	case GL_CLEAR_EXP:
	case GL_AND_EXP:
	case GL_AND_REVERSE_EXP:
	case GL_COPY_EXP:
	case GL_AND_INVERTED_EXP:
	case GL_NOOP_EXP:
	case GL_XOR_EXP:
	case GL_OR_EXP:
	case GL_NOR_EXP:
	case GL_EQUIV_EXP:
	case GL_INVERT_EXP:
	case GL_OR_REVERSE_EXP:
	case GL_COPY_INVERTED_EXP:
	case GL_OR_INVERTED_EXP:
	case GL_NAND_EXP:
	case GL_SET_EXP:
		ctx->logicOp = op;
		SetUpDateFlagPerFragment(ctx,GLF_UF_LogicalOp);
		break;
	default:
		set_err(GL_INVALID_ENUM);
	}
}

#endif

GL_API void GL_APIENTRY glVertexAttrib1f (GLuint indx, GLfloat x)
{
	//GET_GL_STATE(ctx);

	glVertexAttrib4f(indx, x, 0.0, 0.0, 1.0);
}

GL_API void GL_APIENTRY glVertexAttrib2f (GLuint indx, GLfloat x, GLfloat y)
{
	//GET_GL_STATE(ctx);

	glVertexAttrib4f(indx, x, y, 0.0, 1.0);
}

GL_API void GL_APIENTRY glVertexAttrib3f (GLuint indx, GLfloat x, GLfloat y, GLfloat z)
{
	//GET_GL_STATE(ctx);

	glVertexAttrib4f(indx, x, y, z, 1.0);
}

GL_API void GL_APIENTRY glVertexAttrib4f (GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GET_GL_STATE(ctx);

	if((indx >= MAX_VERTEX_ATTRIBS))
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	ctx->vertex_attribs[indx][0] = x;
	ctx->vertex_attribs[indx][1] = y;
	ctx->vertex_attribs[indx][2] = z;
	ctx->vertex_attribs[indx][3] = w;

}

GL_API void GL_APIENTRY glVertexAttrib1fv (GLuint indx, const GLfloat *values)
{
	//GET_GL_STATE(ctx);

	if(values == NULL)
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	glVertexAttrib4f(indx, values[0], 0.0, 0.0, 1.0);
}

GL_API void GL_APIENTRY glVertexAttrib2fv (GLuint indx, const GLfloat *values)
{
	//GET_GL_STATE(ctx);

	if(values == NULL)
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	glVertexAttrib4f(indx, values[0], values[1], 0.0, 1.0);
}

GL_API void GL_APIENTRY glVertexAttrib3fv (GLuint indx, const GLfloat *values)
{
	//GET_GL_STATE(ctx);

	if(values == NULL)
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	glVertexAttrib4f(indx, values[0], values[1], values[2], 1.0);
}

GL_API void GL_APIENTRY glVertexAttrib4fv (GLuint indx, const GLfloat *values)
{
	//GET_GL_STATE(ctx);

	if(values == NULL)
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	glVertexAttrib4f(indx, values[0], values[1], values[2], values[3]);
}

GL_API void GL_APIENTRY glVertexAttribPointer (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *ptr)
{
	GET_GL_STATE(ctx);

	if((indx < 0) || (indx >= MAX_VERTEX_ATTRIBS))
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	ctx->vertex_attrib_array_data[indx].size        = size;
	ctx->vertex_attrib_array_data[indx].type        = type;
	ctx->vertex_attrib_array_data[indx].normalized  = normalized;
	ctx->vertex_attrib_array_data[indx].stride      = stride;
	
	if(ctx->buffer_object_data.array_buffer_binding)
	{
		//Vertex buffer object

		int offset = (char*)ptr - (char*)NULL;

		if(ctx->buffer_object_data.array_buffer_binding->data)
		{
			ctx->vertex_attrib_array_data[indx].ptr = (char*)(ctx->buffer_object_data.array_buffer_binding->data)+offset;
			ctx->vertex_attrib_array_data[indx].vboID               = ctx->buffer_object_data.array_buffer_binding->id;
			ctx->vertex_attrib_array_data[indx].vertex_attrib_ptr_binding = ctx->buffer_object_data.array_buffer_binding;
		}

	}
	else
	{
		//Vertex arrays
		ctx->vertex_attrib_array_data[indx].ptr         = ptr;
		ctx->vertex_attrib_array_data[indx].vboID               = 0;
		ctx->vertex_attrib_array_data[indx].vertex_attrib_ptr_binding = NULL;
	}

	/*
	//Working for vertex arrays.
	ctx->buffer_object_data.vertex_attrib_ptr_binding[indx] = ctx->buffer_object_data.array_buffer_binding;

	ctx->vertex_attrib_array_data[indx].size        = size;
	ctx->vertex_attrib_array_data[indx].type        = type;
	ctx->vertex_attrib_array_data[indx].normalized  = normalized;
	ctx->vertex_attrib_array_data[indx].stride      = stride;
	ctx->vertex_attrib_array_data[indx].ptr         = ptr;
	*/

}

GL_API void GL_APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
	GET_GL_STATE(ctx);

	if((width < 0) || (height < 0))
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	ctx->viewport_data.x = x;
	ctx->viewport_data.y = y;
	ctx->viewport_data.w = width;
	ctx->viewport_data.h = height;
	SetUpDateFlagPrimEngine(ctx,GLF_UF_ViewPort);
	SetUpDateFlagRasterEngine(ctx,GLF_UF_Clipping);
}

/****************************** BUFFER OBJECTS ******************************/

GL_API void GL_APIENTRY glBindBuffer (GLenum target, GLuint buffer)
{
	GET_GL_STATE(ctx);

	int                   i;
	BufferObjectData*       p = &ctx->buffer_object_data;


	if( (target != GL_ARRAY_BUFFER && target != GL_ELEMENT_ARRAY_BUFFER))
	{
		set_err(GL_INVALID_VALUE);
		return;
	}

	if(buffer == 0)
	{
		if(target == GL_ARRAY_BUFFER)
		{
			p->array_buffer_binding = NULL;
		}
		else
		{
			p->element_array_buffer_binding = NULL;
		}

		return;
	}

	/* if the Buffer Object already exists, bind it to (Element) Array Buffer */
	for(i = 0; i < p->buffer_objects_last_index; i++)
	{
		if( p->buffer_objects[i].in_use &&
				p->buffer_objects[i].id == buffer)
		{
			if(target == GL_ARRAY_BUFFER)
			{
				p->array_buffer_binding = &p->buffer_objects[i];
			}
			else
			{
				p->element_array_buffer_binding = &p->buffer_objects[i];
			}
			break;
		}
	}

	/* else, create a new Buffer Object, and then bind to (Element) Array Buffer */
	if(i == p->buffer_objects_last_index)
	{
		for(i = 0; i < p->buffer_objects_last_index; i++)
		{
			if(!p->buffer_objects[i].in_use)
			{
				break;
			}
		}

		if(i >= MAX_BUFFER_OBJECTS )
		{
			set_err(GL_OUT_OF_MEMORY);
			return;
		}

		p->buffer_objects[i].data    = NULL;
		p->buffer_objects[i].size    = 0;
		p->buffer_objects[i].usage   = GL_STATIC_DRAW;
		p->buffer_objects[i].access  = GL_WRITE_ONLY;
		p->buffer_objects[i].in_use  = GL_TRUE;
		p->buffer_objects[i].id      = buffer;
		//added for buffer map
		p->buffer_objects[i].mapped      = GL_FALSE;
		p->buffer_objects[i].map_ptr = NULL;

		if(i == p->buffer_objects_last_index)
		{
			p->buffer_objects_last_index++;
		}

		if(target == GL_ARRAY_BUFFER)
		{
			p->array_buffer_binding = &p->buffer_objects[i];
		}
		else
		{
			p->element_array_buffer_binding = &p->buffer_objects[i];
		}
	}

	if(buffer > p->generated_so_far)
	{
		p->generated_so_far = buffer;
	}
}


GL_API void GL_APIENTRY glGenBuffers (GLsizei n, GLuint *buffers)
{
	GET_GL_STATE(ctx);

	int     i;

	if(!buffers)
	{
		return;
	}

	if(n < 0)
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	for(i = 0; i < n; i++)
	{
		buffers[i] = ++ctx->buffer_object_data.generated_so_far;
	}
}


GL_API void GL_APIENTRY glDeleteBuffers (GLsizei n, const GLuint *buffers)
{
	GET_GL_STATE(ctx);

	int                 i;
	int                 j;
	int                 k;
	BufferObjectData* buf = &ctx->buffer_object_data;

	if((n < 0) || (buffers == NULL))
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	for (i = 0 ; i < n; i ++)
	{
		for(j = 0; j < buf->buffer_objects_last_index; j++)
		{
			if(buf->buffer_objects[j].id == buffers[i])
			{
				/* remove all existing bindings */
				if(buf->array_buffer_binding == &buf->buffer_objects[j])
				{
					buf->array_buffer_binding = NULL;
				}
				if(buf->element_array_buffer_binding == &buf->buffer_objects[j])
				{
					buf->element_array_buffer_binding = NULL;
				}
				for(k = 0; k < MAX_VERTEX_ATTRIBS; k++)
				{
					if(ctx->vertex_attrib_array_data[k].vertex_attrib_ptr_binding == &buf->buffer_objects[j])
					{
						//remove all bindings from vertex attrib 
						ctx->vertex_attrib_array_data[k].vertex_attrib_ptr_binding = NULL;
						ctx->vertex_attrib_array_data[k].vboID = 0;
					}
				}

				/* free buffer object */
				Plat::safe_free(buf->buffer_objects[j].data);
				buf->buffer_objects[j].data = NULL;
				buf->buffer_objects[j].in_use = GL_FALSE;
				// not sure added
				buf->buffer_objects[i].map_ptr = NULL;
				break;
			}
		}
	}
}

GL_API void GL_APIENTRY glBufferData (GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{
	GET_GL_STATE(ctx);

	BufferObject*   buf;

	if(( target != GL_ARRAY_BUFFER ) && (target != GL_ELEMENT_ARRAY_BUFFER ))
	{
		set_err(GL_INVALID_ENUM);
		return ;
	}
	if(( usage != GL_STATIC_DRAW  ) && (usage != GL_DYNAMIC_DRAW  ) && (usage != GL_STREAM_DRAW))
	{
		set_err(GL_INVALID_ENUM);
		return ;
	}
	if(size < 0) //TODO: spec ambiguity
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	if( target == GL_ARRAY_BUFFER )
	{
		buf = ctx->buffer_object_data.array_buffer_binding;
	}
	else
	{
		buf = ctx->buffer_object_data.element_array_buffer_binding;
	}

	if(buf == NULL)
	{
		set_err(GL_INVALID_OPERATION);
		return ;
	}

	if(buf->mapped == GL_TRUE){
		buf->map_ptr = NULL;
	}


	Plat::safe_free(buf->data);
	buf->data   = Plat::malloc(size);
	if(buf->data == NULL)
	{
		set_err(GL_OUT_OF_MEMORY);
		return ;
	}

	buf->size   = size;
	buf->usage  = usage;

	if(data != NULL)
	{
		Plat::memcpy(buf->data, data, size);
	}
}

GL_API void GL_APIENTRY glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
{
	GET_GL_STATE(ctx);

	BufferObject*   buf;

	if (( target != GL_ARRAY_BUFFER ) && (target != GL_ELEMENT_ARRAY_BUFFER ))
	{
		set_err(GL_INVALID_ENUM);
		return ;
	}
	if((size < 0) || ( offset < 0 ))
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}
	
	if(data == NULL)
	{
		return;
	}

	if ( target == GL_ARRAY_BUFFER )
	{
		buf = ctx->buffer_object_data.array_buffer_binding ;
	}
	else
	{
		buf = ctx->buffer_object_data.element_array_buffer_binding;
	}

	if ( buf == NULL )
	{
		set_err(GL_INVALID_OPERATION);
		return ;
	}

	//added for the case of mapped buffer u can not modify the value
	if(buf->mapped == GL_TRUE){
		set_err(GL_INVALID_OPERATION);
		return ;        
	}

	if ( (size + offset) > buf->size)
	{
		set_err(GL_INVALID_VALUE);
		return ;
	}

	Plat::memcpy((char *) buf->data + offset, data, size);
}


GL_API void GL_APIENTRY *glMapBuffer (GLenum target, GLenum access)
{

	GET_GL_STATE(ctx);

	BufferObject*   buf;
	
	//target have to be ARRAY BUFFER
	if (target != GL_ARRAY_BUFFER)
	{
		set_err(GL_INVALID_ENUM);
		return NULL;
	}
	
	if(access != GL_WRITE_ONLY)
	{
		set_err(GL_INVALID_ENUM); 
		return NULL;
	}
	
	buf = ctx->buffer_object_data.array_buffer_binding;

	// if no binding has been done so far
	if(buf == NULL)
	{
		set_err(GL_INVALID_OPERATION);
		return NULL ;
	}

	//If the buffer data store is already in the mapped state
	if(buf->mapped == GL_TRUE){
		set_err(GL_INVALID_OPERATION);  
		return NULL;
	}
	
	//if buffer data has not been mapped and data is available
	if(buf->data != NULL){
		buf->mapped = GL_TRUE;
		buf->access = access;
		return buf->data;
	}
	
	//in all other cases ie. buf is present but the data is not present???
	set_err(GL_OUT_OF_MEMORY);      
	return NULL;
	
	
}



GL_API GLboolean GL_APIENTRY glUnmapBuffer (GLenum target)
{

	GET_GL_STATE(ctx);

	BufferObject*   buf;
	
	//target have to be ARRAY BUFFER
	if (target != GL_ARRAY_BUFFER)
	{
		set_err(GL_INVALID_ENUM);
		return GL_FALSE;
	}

	buf = ctx->buffer_object_data.array_buffer_binding;
	
	//unmapping that occurs as a side effect of buffer deletion or reinitialization is not an error???
	if(buf == NULL)
	{
		//set_err(GL_INVALID_OPERATION); //TO DO
		return GL_FALSE;
	}

	//unmapping that occur as the efefct of buffer reinitialization is not na error
	if((buf->mapped == GL_TRUE)&&(buf->map_ptr == NULL)){
		return GL_FALSE;        

	}

	//if data is corrupted. not on our case.
	
	
	//if already in the unmapped state
	if(buf->mapped == GL_FALSE){
		set_err(GL_INVALID_OPERATION);
		return GL_FALSE ;       
	}
	else{
		buf->map_ptr = NULL;
		buf->mapped = GL_FALSE;
		return GL_TRUE;
	}

}
