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
* \file         glState.h
* \author       Sandeep Kakarlapudi (s.kakarla@samsung.com)
* \brief        GL state definitions
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*       07.08.2006      Sandeep Kakarlapudi             Initial version from ogl2_fimg.h
*
*******************************************************************************
*/


#ifndef __GLSTATE_H__
#define __GLSTATE_H__

/*
*******************************************************************************
* Includes
*******************************************************************************
*/

#include "gl.h"

#include "debugHelper.h"

#include "glConfig.h"
#include "glShader.h"
#include "glTex.h"
#include "glFramebufferObject.h"
#include "glContext.h"

#include "platform.h"

#include "time.h"
#include <cutils/log.h>

/*
*******************************************************************************
* Macro definitions and enumerations
*******************************************************************************
*/



//Temporary enums

enum {
	MAX_NUM_PROGRAMS                = 50,
	MAX_NUM_SHADERS                 = 100,
	ID_TEMP_PROGRAM                                     = 100,
	ID_TEMP_VERTEX_SHADER           = 1,
	ID_TEMP_FRAGMENT_SHADER                     = 2,
};

//-----------------------------------------------------------------------------
// Get state
//-----------------------------------------------------------------------------

class XXX
{public:
	XXX(const char* function, const char* file, int line) : function(function), file(file), line(line)
	{
		//LOGE("GL2 enter: %s: %s:%d", function, file, line);
	}

	~XXX()
	{
		//LOGE("GL2 exit: %s: %s:%d", function, file, line);
	}

	const char* function;
	const char* file;
	int line;
	
};

#ifdef FIMG_GLES_11
extern "C" GLES2Context GetGLstate20(void);
#define GET_OGL_STATE()         (OGLState *)GetGLstate20() 

#define GET_GL_STATE_NOCHECK(x)                 OGLState * x = (OGLState *)GetGLstate20();
#ifdef MULTI_CONTEXT
#define GET_GL_STATE(x)  XXX xxx(__FUNCTION__,__FILE__,__LINE__);               OGLState * x = (OGLState *)GetGLstate20(); if(x==NULL) LOGE("ERROR: ctx is NULL: %s line:%d pthreadid: %ld \n",__FUNCTION__,__LINE__,(long int)pthread_self())
#else
#define GET_GL_STATE(x)                 OGLState * x = (OGLState *)GetGLstate20(); if(x==NULL) LOGE("ERROR: ctx is NULL: %s line:%d \n",__FUNCTION__,__LINE__)
#endif
#else
#ifdef MULTI_CONTEXT
extern pthread_key_t tls_context_key20;

#define SET_GL_STATE(x)                                 pthread_setspecific(tls_context_key20,x);

#define GET_OGL_STATE()                         (OGLState *)pthread_getspecific(tls_context_key20) 
#define DECL_WITH_OGL_STATE(x)          OGLState * x = GET_OGL_STATE()

// *Pra* Older declaration. To be deleted.
#define GET_GL_STATE_NOCHECK(x)         OGLState * x = (OGLState *)pthread_getspecific(tls_context_key20);
#define GET_GL_STATE(x)                         OGLState * x = (OGLState *)pthread_getspecific(tls_context_key20); if(x==NULL) LOGE("ERROR: ctx is NULL: %s line:%d pthreadid: %ld \n",__FUNCTION__,__LINE__,(long int)pthread_self())
#else
#define GET_OGL_STATE()                         (pgles2State)
#define DECL_WITH_OGL_STATE(x)          OGLState * x = GET_OGL_STATE()

// *Pra* Older declaration. To be deleted.
#define GET_GL_STATE_NOCHECK(x)     OGLState * x = pgles2State
#define GET_GL_STATE(x)                         OGLState * x = pgles2State
#endif
#endif
//-----------------------------------------------------------------------------
// Error setting
//-----------------------------------------------------------------------------

#define SET_ERR(state, error, msg)      set_err(state, error)


/*
*******************************************************************************
* Type, Structure & Class Definitions
*******************************************************************************
*/

struct BufferObject
{
	void*     data;
	GLint     size;
	GLenum    usage;
	GLenum    access;
	//for mapBuffer and unmapBuffer
	GLboolean mapped; 
	void*     map_ptr; 

	GLboolean in_use;
	GLuint    id;
};

struct VertexAttribArrayData
{
	/* used by glVertexAttribPointer() */
	GLint       size;
	GLenum      type;
	GLboolean   normalized;
	GLsizei     stride;
	const void* ptr;

	/* In case of VBO  */
	GLuint          vboID;
	BufferObject*   vertex_attrib_ptr_binding;

	/* used by glEnableVertexAttribArray() & glDisableVertexAttribArray() */
	GLboolean enabled;
};

struct BufferObjectData
{
	/* used by glBindBuffer() */
	BufferObject*   array_buffer_binding;
	BufferObject*   element_array_buffer_binding;

	/* used by glBindBuffer(), glDeleteBuffers(), glBufferData(), glBufferSubData() */
	BufferObject    buffer_objects[MAX_BUFFER_OBJECTS];
	GLint             buffer_objects_last_index;

	/* used by glGenBuffers(), glBindBuffer() */
	GLuint            generated_so_far;

};

struct DepthRangeData
{
	GLclampf        near_depth;
	GLclampf        far_depth;
};

struct ViewportData
{
	GLint        x;
	GLint        y;
	GLint        w;
	GLint        h;
};

struct CullFaceData
{
	GLenum         mode;
	GLboolean      is_enabled;
};

struct PolygonOffsetData
{
	GLfloat        factor;
	GLfloat        units;

	GLboolean      is_enabled_fill;
	//  GLboolean      is_enabled_line;     //OGL ES 2.0 only supports POLYGON_OFFSET_FILL
	//  GLboolean      is_enabled_point;
};

struct PixelStoreData
{
	GLint           pack_alignment;
	GLint           unpack_alignment;
};

struct ScissorTestData
{
	GLint         x;
	GLint         y;
	GLint         w;
	GLint         h;

	GLboolean     is_enabled;
};

struct SampleCoverageData
{
	GLclampf        value;
	GLboolean       invert;

	GLboolean       is_enabled_coverage;
	GLboolean       is_enabled_alpha_to_coverage;
};

struct StencilTestData
{
	/* used by  glStencilFunc(), glStencilFuncSeparate() */
	GLenum        front_face_func;
	GLint         front_face_ref;
	GLuint        front_face_mask;
	GLenum        back_face_func;
	GLint         back_face_ref;
	GLuint        back_face_mask;

	/* used by  glStencilOp(), glStencilOpSeparate() */
	GLenum        front_face_fail;
	GLenum        front_face_zfail;
	GLenum        front_face_zpass;
	GLenum        back_face_fail;
	GLenum        back_face_zfail;
	GLenum        back_face_zpass;

	/* used by glEnable(), glDisable() */
	GLboolean     is_enabled;
};

struct AlphaTestData
{
	GLenum        func;
	GLfloat       refValue;

	GLboolean     is_enabled;
};

struct BlendData
{
	/* used by  glBlendFunc(), glBlendFuncSeparate() */
	GLenum        fn_srcRGB;
	GLenum        fn_dstRGB;
	GLenum        fn_srcAlpha;
	GLenum        fn_dstAlpha;

	/* used by  glBlendEquation(), glBlendEquationSeparate() */
	GLenum        eqn_modeRGB;
	GLenum        eqn_modeAlpha;

	/* used by  glBlendColor() */
	GLclampf      blnd_clr_red;
	GLclampf      blnd_clr_green;
	GLclampf      blnd_clr_blue;
	GLclampf      blnd_clr_alpha;

	/* used by glEnable(), glDisable() */
	GLboolean     is_enabled;

};

struct ColorMaskData
{
	GLboolean   r;
	GLboolean   g;
	GLboolean   b;
	GLboolean   a;
};

struct ClearColorData
{
	GLclampf   r;
	GLclampf   g;
	GLclampf   b;
	GLclampf   a;
};

struct HintData
{
	GLenum    generate_mipmap_hint;
	GLenum    fragment_shader_derivative_hint;
};

//! Structure to store update bits
typedef struct {
	unsigned int    hi;
	unsigned int    vs;
	unsigned int    pe;
	unsigned int    re;
	unsigned int    fs;
	unsigned int    tu;
	unsigned int    pf;
	unsigned int    pfbuf;  //for per fragment color & depth buffer

} glfUpdateBits;

struct SringData
{
	const char* vendor;
	const char* version;
	const char* renderer;
	const char* shadingLangVersion;
	const char* extensions;
};


typedef ustl::set<GLuint> TexNameSet;
typedef ustl::map<GLuint, TextureObject* > TexObjects;

//texture object can be shared between context in same address space.
struct SharedTextureState{

	TexNameSet              usedTexNames;
	//TextureObject texObjects[MAX_TEXTURE_OBJECTS ];                       //pointer to the texture objects
	TexObjects      texObjects;
	PtrChunkH       hFallBackTexChunk;
	TexFGLState         fallBackTexFGLstate[3];
	GLint                   maxUsedUnit;



	void ReleaseTexObj(struct OGLState* pState , GLuint id);

	inline
	void        InitFallBackTexObjects()
	{
		TextureObject * pTempFallBackTexObj = new(TextureObject);
		hFallBackTexChunk = pCA->New(1 * 1 * 6 * 4);

		pTempFallBackTexObj->reset();
		
		//fallback texture object for 2D
		pTempFallBackTexObj->texType = GL_TEXTURE_2D;
		pTempFallBackTexObj->isUsed = GL_TRUE;                          
		pTempFallBackTexObj->width= 1;                          
		pTempFallBackTexObj->height =1;                         
		pTempFallBackTexObj->depth =0;                          
		pTempFallBackTexObj->internalFormat =GL_RGBA;           
		pTempFallBackTexObj->nativeFormat =E_ARGB8;
		
		pTempFallBackTexObj->minFilter  = GL_LINEAR;
		pTempFallBackTexObj->magFilter =GL_LINEAR;      
		pTempFallBackTexObj->wrapS  = pTempFallBackTexObj->wrapT = pTempFallBackTexObj->wrapR = GL_REPEAT;
		pTempFallBackTexObj->levels =1;
		pTempFallBackTexObj->texFGLState.noOfpalette = 0;
		pTempFallBackTexObj->hChunk  = hFallBackTexChunk;
		Plat::memset(pTempFallBackTexObj->hChunk->GetVirtAddr(),255,1 * 1 * 6 * 4);//set it to white
		
		pTempFallBackTexObj->images.tex2D[0].nativeFormat  = E_ARGB8;
		pTempFallBackTexObj->images.tex2D[0].internalFormat  = GL_RGBA;
		pTempFallBackTexObj->images.tex2D[0].PixType = GL_UNSIGNED_BYTE; 
		pTempFallBackTexObj->images.tex2D[0].width =1;
		pTempFallBackTexObj->images.tex2D[0].height =1;
		pTempFallBackTexObj->images.tex2D[0].depth =1;
		
		pTempFallBackTexObj->updateTexFGLState();
		Plat::memcpy((void *)(&(fallBackTexFGLstate[0])) , (const void* ) &(pTempFallBackTexObj->texFGLState) , sizeof(TexFGLState));


		//fallback texture object for 3D
		pTempFallBackTexObj->texType = GL_TEXTURE_3D;                           
		pTempFallBackTexObj->depth =1;                                  
		pTempFallBackTexObj->images.tex3D[0].depth =1;
		
		pTempFallBackTexObj->updateTexFGLState();
		Plat::memcpy((void *) (&(fallBackTexFGLstate[1])) , (const void* ) &(pTempFallBackTexObj->texFGLState) , sizeof(TexFGLState));

		//fallback texture object for CUBEMAP
		pTempFallBackTexObj->texType = GL_TEXTURE_CUBE_MAP;                     
		pTempFallBackTexObj->depth =0;                          
		for(int j = 0; j < 6; j++) {
			
			pTempFallBackTexObj->images.cubeMap[j][0].nativeFormat  = E_ARGB8;
			pTempFallBackTexObj->images.cubeMap[j][0].internalFormat  = GL_RGBA;
			pTempFallBackTexObj->images.cubeMap[j][0].PixType = GL_UNSIGNED_BYTE; 
			pTempFallBackTexObj->images.cubeMap[j][0].width =1;
			pTempFallBackTexObj->images.cubeMap[j][0].height =1;
			
		}
		
		pTempFallBackTexObj->updateTexFGLState();
		Plat::memcpy((void*)(&(fallBackTexFGLstate[2])) ,(const void* ) &(pTempFallBackTexObj->texFGLState) , sizeof(TexFGLState));

		pTempFallBackTexObj->hChunk  = NULL;
		delete pTempFallBackTexObj;
	}

	inline
	void        deInitFallBackTexObjects()
	{

		pCA->Free(hFallBackTexChunk);

	}


	SharedTextureState(){
		maxUsedUnit = 0;

		//initialize the fallback textures 
		InitFallBackTexObjects();
		
	}

	
	~SharedTextureState(){
		for( TexObjects::iterator it = texObjects.begin() ;  it != texObjects.end() ; it++){

			it->second->Delete(); 
			delete it->second;
		}
		
		texObjects.clear();
		
		//free the used texture name set
		usedTexNames.clear();
		
		maxUsedUnit = 0;

		//Deinitialize the fallback textures    
		deInitFallBackTexObjects();
		
	}

	
};


struct SharedState{
	
	SharedTextureState          sharedTexState;                             //shared tex objexts
	
	mutex_t                         sharedStateMutex;                               //right now having 1 mutex can have 1 for each shared object
	GLint                                   sharedStateRefCount;                    //ref count of the no of context which share this structure
	bool                                    sharedStateMutexAcquired;       
	
	SharedState(){
		
		Plat::initMutex( (&sharedStateMutex),   "SharedState::SharedState");
		sharedStateRefCount = 0;
		sharedStateMutexAcquired = false;

	}
	~SharedState(){
		//TODO: delete the mutex!
		gAssert(sharedStateRefCount == 0 && "There should be no valid references to SharedState object when destroying it!");
		
	}
} ;

struct OGLState
{
	//Version and id must be the first two fields in the struct
	GLuint              version;    //OpenGL version
	GLuint              id;         //context id
	GLboolean       init_dimensions;
	/* used by glVertexAttrib*() */
	GLfloat                     vertex_attribs[MAX_VERTEX_ATTRIBS][4];

	/* used by glVertexAttribPointer(), glEnableVertexAttribArray(), glDisableVertexAttribArray() */
	VertexAttribArrayData     vertex_attrib_array_data[MAX_VERTEX_ATTRIBS];

	/* used by glBindBuffer(), glDeleteBuffers(), glGenBuffers(), glBufferData(), glBufferSubData() */
	BufferObjectData          buffer_object_data;

	/* used by glEnable(), glDisable() */
	GLboolean                   is_enabled_vertex_program_point_size;

	/* used by glDepthRangef() */
	DepthRangeData            depth_range_data;

	/* used by glViewport() */
	ViewportData              viewport_data;

	/* used by glFrontFace() */
	GLenum                      front_face_mode;

	/* used by glPointSize() */
	GLfloat                     point_size;

	/* used by glLineWidth() */
	GLfloat                     line_width;

	/* used by glCullFace(), glEnable(), glDisable() */
	CullFaceData              cull_face_data;

	/* used by glPolygonOffset(), glEnable(), glDisable() */
	PolygonOffsetData         polygon_offset_data;

	/* used by Texture */
	TextureState                            texState;

	/* pointer to the shared state of the contexs    */
	SharedState*      sharedState; 

	/* used by glScissor(), glEnable(), glDisable() */
	PixelStoreData            pixel_store_data;

	/* used by glSampleCoverage(), glEnable(), glDisable() */
	SampleCoverageData        sample_coverage_data;

	/* used by  glStencilFunc(), glStencilFuncSeparate(), glStencilMask(), glStencilMaskSeparate()
		glStencilOp(), glStencilOpSeparate(), glEnable(), glDisable() */
	StencilTestData           stencil_test_data;

	/* used by  glBlendFunc(), glBlendFuncSeparate(), glBlendColor(),
		glBlendEquation(), glBlendEquationSeparate(), glEnable(), glDisable() */
	BlendData                 blend_data;

	/* used by  glScissor */
	ScissorTestData           scissor_test_data;

	/* used by glDepthFunc */
	GLenum                      depth_test_func;
	GLboolean                   is_enabled_depth;

	/* used by glDepthMask */
	GLboolean                   depth_write_mask;

	/* used by glStencilMask() */
	GLuint                      front_stencil_writemask;
	GLuint                      back_stencil_writemask;


	/* used by glColorMask() */
	ColorMaskData             color_mask_data;

	/* used by glEnable(), glDisable() */
	GLboolean                   is_enabled_dither;

	/* used by glClearColor() */
	ClearColorData            clear_color_data;

	/* used by glClearDepthf() */
	GLclampf                    clear_depth;

	/* used by glClearStencil() */
	GLint                       clear_stencil;

	/* used by glHint() */
	HintData                  hint_data;
	
	/*used by glGetString */
	SringData                                       str_data;
	
	ShaderProgNameList                      shaderProgNames;
	
	GLuint                      current_program;
	Program*                                        current_program_ptr;
	
	Executable*                     current_executable;
	GLuint                                          cur_exe_own_memory;  //whether current_executable has got it's own memory



	RenderbufferState           renderbuffState;
	FramebufferState            framebuffState;


	/* used by almost all GL functions */
	GLenum                      error;

	FramebufferData             defFBData;  //os provided fb data
	FramebufferData             readFBData;  //os provided fb data
	//  AddressBase                 colorBuff1; //initially this is the backbuffer
	//      AddressBase                 colorBuff2;
	//      AddressBase                 colorBuff3; 

	glfUpdateBits                               updateBits;

#ifdef EN_EXT_ALPHA_TEST_EXP
	AlphaTestData               alpha_test_data;
#endif

#ifdef EN_EXT_LOGIC_OP_EXP
	GLenum                      logicOp;
	GLboolean                   is_logicOp_enabled;
#endif

#ifdef EN_EXT_POINT_SPRITE_EXP
	GLboolean                       is_point_sprite_enable; //This is a extension to support point sprite for GLES 1.1 
	// as there is no corresponding API in GLES2.0 to 
	// control point sprite. For all 2.0 application this variable should be 
	// enabled(TRUE) as point spirite enabled or disable is decided 
	// if gl_PointCoord varying found in shader program. But in gles 1.1 application
	// this variable is enabled if glEnable(GL_POINT_SPRITE_OES) is called else
	// this should be always disabled.
#endif 
	//Temporary buffers //todo init in glCore.cpp
	char tempStrBuff[GLF_TEMP_STRING_BUFFER_LENGTH];
	GLint tempInt4[4];
	GLfloat tempFloat4[4];
	FramebufferData prevFBData;
	FramebufferData curFBData; //This is only for the use of glfSet* functions
	//Will not be valid when other functions are called
#ifdef FSO_JITO
	CodegenData                                     cgd; //pre allocated temp area for codegen
#endif

	bool                        VPPSEnable;

	struct timeval beginTime;
	struct timeval endTime;
	GLint renderStartTime;

};




/*
*******************************************************************************
* Global Variable Declarations
*******************************************************************************
*/
#ifndef MULTI_CONTEXT
extern OGLState* pgles2State;
#endif

/*
*******************************************************************************
* Function Declarations
*******************************************************************************
*/

void set_err(GLenum err);
void set_err(OGLState *ctx, GLenum err);


/*
*******************************************************************************
* Inline Function defintions
*******************************************************************************
*/


inline void initGLSharedTextureStateMutex(OGLState* pState)
{
	Plat::initMutex( (&(pState->sharedState->sharedStateMutex)),   "initGLSharedStateMutex");       
}

inline void lockGLSharedState(OGLState* pState)
{

	Plat::lock((&(pState->sharedState->sharedStateMutex)) , "lockGLSharedState");
	pState->sharedState->sharedStateMutexAcquired = true;

}


inline void lockGLSharedTextureState(OGLState* pState)
{
	
	Plat::lock((&(pState->sharedState->sharedStateMutex))  , "lockGLSharedTextureState");
	pState->sharedState->sharedStateMutexAcquired = true;

}

inline void unlockGLSharedState(OGLState* pState)
{
	Plat::unlock((&(pState->sharedState->sharedStateMutex)) ,  "unlockGLSharedState");
	pState->sharedState->sharedStateMutexAcquired = false;
}


inline void unlockGLSharedTextureState(OGLState* pState)
{
	Plat::unlock((&(pState->sharedState->sharedStateMutex)) , "unlockGLSharedTextureState");
	pState->sharedState->sharedStateMutexAcquired = false;

}


#endif //__GLSTATE_H__
