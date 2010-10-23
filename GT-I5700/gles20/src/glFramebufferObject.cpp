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
* \file         glFramebufferObject.cpp
* \author       Sandeep Kakarlapudi (s.kakarla@samsung.com)
* \brief        Implementation of all FBO and renderbuffer fnuctionality
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*       06.02.2007      Sandeep Kakarlapudi             Initial version 
*
*******************************************************************************
*/

#include "glState.h"
#include "pixel.h"
#include "platform.h"


//---------------  NameManager ---------------

void NameManager::addName(GLuint id)
{
	//int size = nameList.size();
	
	NameContainer::iterator it = nameList.begin();
	while(it != nameList.end())
	{
		if(id == (*it))
		{
			//LOGMSG("\nWARNING: duplicate name found: %d\n",id);
			return;
		}
		else if( id < (*it))
		{
			nameList.insert(it, id);
			return;
		}
		
		++it;
	}

	nameList.push_back(id);
}

void NameManager::genNames(GLsizei n, GLuint* ids)
{
	if(!ids)
	{
		return;
	}

	for(int i=0; i<n; i++)
	{
		ids[i] = _genName();
	}
}

void NameManager::removeName(GLuint id)
{
	NameContainer::iterator it = nameList.begin();

	while(it != nameList.end())
	{
		if(id == (*it))
		{
			nameList.erase(it);
			return;
		}
		
		++it;
	}
}

void NameManager::clear()
{
	nameList.clear();
}


GLuint NameManager::_genName()        //TODO: validate/optimize me!!
{


	GLuint id = 1;
	
	NameContainer::iterator it = nameList.begin();
	while(it != nameList.end())
	{
		if(id < (*it))
		{
			nameList.insert(it,id);
			return id;
		}
		else
		{
			id = (*it) + 1;
		}

		++it;
	}

	nameList.push_back(id);

	return id;
}

//---------------  FramebufferState ---------------

FramebufferState::FramebufferState()
{ 
	setDefaults(); 
}

void FramebufferState::setDefaults()
{
	framebuffBinding = 0;
	framebuffNames.clear();

	FBOMap::iterator it = framebuffList.begin();

	while(it != framebuffList.end())
	{
		delete it->second;
		++it;
	}

	framebuffList.clear();
}

void FramebufferState::detachAttachment(GLenum objectType, GLuint objectName)
{
	if(framebuffBinding == 0)
	return;
	
	//Get Framebuffer Object
	FBOMap::iterator it = framebuffList.find(framebuffBinding);
	
	//Check its not null
	gAssert((it!= framebuffList.end()) && "\n framebuffBinding is invalid! Could not find the bound fbo!\n");

	FramebufferObject* fbo = it->second;
	
	gAssert((fbo!=0) && "\n framebuffList has a null pointer as an FBO!\n");

	if((fbo->colorAttachment.objectType == objectType) 
			&& (fbo->colorAttachment.objectName == objectName))
	{
		fbo->colorAttachment.setDefaults();
	}

	if((fbo->depthAttachment.objectType == objectType)
			&& (fbo->depthAttachment.objectName == objectName))
	{
		fbo->depthAttachment.setDefaults();
	}

	if((fbo->stencilAttachment.objectType == objectType)
			&& (fbo->stencilAttachment.objectName == objectName))
	{
		fbo->stencilAttachment.setDefaults();
	}
}

void FramebufferState::detachRenderbuffer(GLuint id)
{
	detachAttachment(GL_RENDERBUFFER, id);
}

void FramebufferState::detachTexture(GLuint id)
{
	detachAttachment(GL_TEXTURE, id);
}


GLenum sizedInternalFormat(GLenum format, GLenum type)
{
	if((format==GL_RGB)&&(type==GL_UNSIGNED_SHORT_5_6_5))
	{    
		return GL_RGB565;
	}
	else 
	{
		//LOGMSG("format, type combo unsupported! sizedInternalFormat\n");
		return GL_NONE;
	}
}


#if 0
bool isColorRenderable(GLenum format)
{
	switch(format)
	{
	case GL_RGBA4:
	case GL_RGB5_A1:
	case GL_RGB565:
		return true;
	}

	return false;
}
#else
bool isColorRenderable(PxFmt nativeformat)
{
	switch(nativeformat)
	{
	case E_ARGB4:
	case E_ARGB1555:
	case E_RGB565:
	case E_RGB5:
	case E_ARGB8:
	case E_ARGB0888:
		return true;
	default:
		return false;
	}

	return false; //To silence compiler.
}
#endif

bool isDepthRenderable(PxFmt nativeformat)
{
	switch(nativeformat)
	{
	case E_Depth24:
		return true;
	default:
		return false;
	}
	return false; //To silence compiler.
}

bool isStencilRenderable(PxFmt nativeformat)
{
	switch(nativeformat)
	{
	case E_Stencil8:
		return true;
	default:
		return true;
	}
	return false; //To silence compiler.
}

//---------------  FramebufferObject ---------------

GLenum FramebufferObject::status()
{

	//bool attachmentComplete = true;
	bool colorAttComplete = true;
	bool depthAttComplete = true;
	bool stencilAttComplete = true;

	RTInfo attInfo[3];
	RTInfo& colorAttInfo = attInfo[0];
	RTInfo& depthAttInfo = attInfo[1];
	RTInfo& stencilAttInfo = attInfo[2];

	if(colorAttachment.objectType != GL_NONE)
	{
		colorAttInfo = colorAttachment.getInfo();
		if((!isColorRenderable(colorAttInfo.nativeFormat))||(colorAttInfo.width==0)||(colorAttInfo.height==0))
		{
			colorAttComplete = false;
		}
	}

	if(depthAttachment.objectType != GL_NONE)
	{
		depthAttInfo = depthAttachment.getInfo();
		if((!isDepthRenderable(depthAttInfo.nativeFormat))||(depthAttInfo.width==0)||(depthAttInfo.height==0))
		{
			depthAttComplete = false;
		}
	}

	if(stencilAttachment.objectName != GL_NONE)
	{
		stencilAttInfo = stencilAttachment.getInfo();
		if((!isStencilRenderable(stencilAttInfo.nativeFormat))||(stencilAttInfo.width==0)||(stencilAttInfo.height==0))
		{
			stencilAttComplete = false;
		}
	}

	if(! (colorAttComplete && depthAttComplete && stencilAttComplete))
	{
		return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
	}

	if( (colorAttachment.objectType == GL_NONE) && (depthAttachment.objectType == GL_NONE) && (stencilAttachment.objectType == GL_NONE))
	{
		return GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT;
	}

	bool dimEq = true;
	
	GLuint width = 0;
	GLuint height = 0;

	for(unsigned int i=0; i< sizeof(attInfo)/sizeof(attInfo[0]); i++)
	{
		if(attInfo[i].format)
		{
			if((width==0)||(height==0))
			{
				width = attInfo[i].width;
				height = attInfo[i].height;
			}
			else
			{
				if((width!= attInfo[i].width)||(height!=attInfo[i].height))
				{
					dimEq = false;
					break;
				}
			}
		}
	}

	if(!dimEq)
	{
		return GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS;
	}

	//internal format combinations validity
	if((depthAttInfo.format != GL_NONE)&&(stencilAttInfo.format != GL_NONE))
	{
		return GL_FRAMEBUFFER_UNSUPPORTED;
	}


	return GL_FRAMEBUFFER_COMPLETE;
}

//------------FBOAttachmentPointState----------------

RTInfo FBOAttachmentPointState::getInfo()
{
	GET_GL_STATE(ctx);
	
	RTInfo info;

	if(objectType == GL_TEXTURE)
	{
		TextureObject* texObj = GetTexObject(objectName);
		if(!texObj)
		{
			
		}
		else if(texObj->texType == GL_TEXTURE_2D)
		{
			//format should no longer be used native format is sufficient.
			info.format = 0;//sizedInternalFormat(texObj->images.tex2D[textureLevel].internalFormat,texObj->images.tex2D[textureLevel].PixType);
			info.nativeFormat = PxFmt(texObj->images.tex2D[textureLevel].nativeFormat);
			info.width = texObj->images.tex2D[textureLevel].width;
			info.height = texObj->images.tex2D[textureLevel].height;
			
			//TODO: how to support render to mipmap level?
			
			info.addr.vaddr = texObj->hChunk->GetVirtAddr();
			info.addr.paddr = texObj->hChunk->GetPhyAddr();

		}
		else if(texObj->texType == GL_TEXTURE_CUBE_MAP)
		{
			//format should no longer be used native format is sufficient.
			info.format = 0;//sizedInternalFormat(texObj->images.cubeMap[textureCubeMapFace][textureLevel].internalFormat, texObj->images.cubeMap[textureCubeMapFace][textureLevel].PixType);
			info.nativeFormat = PxFmt(texObj->images.cubeMap[textureCubeMapFace][textureLevel].nativeFormat);
			info.width = texObj->images.cubeMap[textureCubeMapFace][textureLevel].width;
			info.height = texObj->images.cubeMap[textureCubeMapFace][textureLevel].height;
			
			//TODO: how to support render to mipmap level?
			//TODO: how to support to different cubemap levels?
			
			info.addr.vaddr = texObj->hChunk->GetVirtAddr();
			info.addr.paddr = texObj->hChunk->GetPhyAddr();
		}

		return info;
	}

	if(objectType == GL_RENDERBUFFER)
	{
		RenderbufferObject* rbo =0;
		
		if(!ctx->renderbuffState.getRBO(objectName, &rbo))
		{
			//TODO: warn?
		}
		else if(!rbo)
		{
			//TODO: warn?
		}
		else
		{
			//format should no longer be used native format is sufficient.
			info.format = 0;//rbo->internalFormat;
			info.nativeFormat = rbo->nativeFormat;
			info.width = rbo->width;
			info.height = rbo->height;
			info.addr.vaddr = rbo->hChunk->GetVirtAddr();
			info.addr.paddr = rbo->hChunk->GetPhyAddr();
		}
	}

	return info;
}

bool isValidFBAttachment(GLenum attachment)
{
	switch(attachment)
	{
	case GL_COLOR_ATTACHMENT0:
	case GL_DEPTH_ATTACHMENT:
	case GL_STENCIL_ATTACHMENT:
		return true;    //intendend fall-through
	}

	return false;
}

struct PixelFormatInfo
{
	GLenum format;
	GLuint  bpp;    //Bytes per pixel (allocation size)
};

PixelFormatInfo validRBformats[]={
	{GL_RGBA4, 2},
	{GL_RGB5_A1, 2},
	{GL_RGB565, 2},
	{GL_RGB8, 4},
	{GL_RGBA8, 4},
	{GL_DEPTH_COMPONENT16, 4},  //Fimg only supports depth24stencil8
	{GL_STENCIL_INDEX, 4},      //Fimg only supports depth24stencil8
};

bool isValidRBformat(GLenum format)
{
	const int numValidFormats = sizeof(validRBformats)/sizeof(validRBformats[0]);
	for(int i=0; i < numValidFormats; i++)
	{
		if(validRBformats[i].format == format)
		{
			return true;
		}
	}

	return false;
}

GLuint getFormatBpp(GLenum format)
{
	const int numValidFormats = sizeof(validRBformats)/sizeof(validRBformats[0]);
	for(int i=0; i < numValidFormats; i++)
	{
		if(validRBformats[i].format == format)
		{
			return validRBformats[i].bpp;
		}
	}

	return 0;
}


bool isFBrenderable(OGLState* ctx)
{
	//GET_GL_STATE(ctx);

	if(ctx->framebuffState.framebuffBinding == 0)
	{
		return true;
	}

	FramebufferObject* fbo =0;
	if(!ctx->framebuffState.getFBO(ctx->framebuffState.framebuffBinding, &fbo))
	{
		gAssert(false && "Invalid fbo bound as current!\n");
		return false;
	}

	return (fbo->status() == GL_FRAMEBUFFER_COMPLETE);

}

FramebufferData getFBData()
{
	GET_GL_STATE(ctx);

	FramebufferData fbData;
	fbData.colorAddr.vaddr = fbData.colorAddr.paddr = 0;
	fbData.nativeColorFormat = E_INVALID_PIXEL_FORMAT;
	fbData.width = fbData.height = 0;
	fbData.depthStencilAddr.vaddr = fbData.depthStencilAddr.paddr = 0;
	fbData.nativeDepthStencilFormat = E_INVALID_PIXEL_FORMAT;
	fbData.flipped = 0;

	if(ctx->framebuffState.framebuffBinding == 0)
	{
		return ctx->defFBData;
	}

	FramebufferObject* fbo =0;
	if(!ctx->framebuffState.getFBO(ctx->framebuffState.framebuffBinding, &fbo))
	{
		gAssert(false && "Invalid fbo bound as current!\n");
		return fbData;
	}



	if(isFBrenderable(ctx))
	{

	}
	else
	{
		set_err(GL_INVALID_FRAMEBUFFER_OPERATION);
		//LOGMSG("Warning, fb not renderable");
	}
	//TODO: assert that fb is renderable.
	//Next fill out the fbData struct



	RTInfo colorAttInfo = fbo->colorAttachment.getInfo();
	RTInfo depthAttInfo = fbo->depthAttachment.getInfo();
	//RTInfo stencilAttInfo = fbo->depthAttachment.getInfo();

	fbData.colorAddr = colorAttInfo.addr;
	fbData.nativeColorFormat = PxFmt(colorAttInfo.nativeFormat);
	fbData.width    = colorAttInfo.width;
	fbData.height   = colorAttInfo.height;
	
	if(depthAttInfo.addr.vaddr)
	{
		fbData.depthStencilAddr = depthAttInfo.addr;
		fbData.nativeDepthStencilFormat = PxFmt(depthAttInfo.nativeFormat);
	}
	//TODO: stencil

	fbData.flipped = 1;

	return fbData;
	
}

FramebufferData getReadFBData()
{
	GET_GL_STATE(ctx);

	FramebufferData fbData;

	if(ctx->framebuffState.framebuffBinding == 0)
	{
		return ctx->readFBData;
	}

	FramebufferObject* fbo = 0;
	if(!ctx->framebuffState.getFBO(ctx->framebuffState.framebuffBinding, &fbo))
	{
		gAssert(false && "Invalid fbo bound as current!\n");
		fbData.colorAddr.vaddr = fbData.colorAddr.paddr = 0;
		fbData.nativeColorFormat = E_INVALID_PIXEL_FORMAT;
		fbData.width = fbData.height = 0;
		fbData.depthStencilAddr.vaddr = fbData.depthStencilAddr.paddr = 0;
		fbData.nativeDepthStencilFormat = E_INVALID_PIXEL_FORMAT;
		fbData.flipped = 0;
		return fbData;
	}



	if(isFBrenderable(ctx))
	{

	}
	else
	{
		set_err(GL_INVALID_FRAMEBUFFER_OPERATION);
		//LOGMSG("Warning, fb not renderable");
	}
	//TODO: assert that fb is renderable.
	//Next fill out the fbData struct



	RTInfo colorAttInfo = fbo->colorAttachment.getInfo();
	RTInfo depthAttInfo = fbo->depthAttachment.getInfo();
	//RTInfo stencilAttInfo = fbo->depthAttachment.getInfo();

	fbData.colorAddr = colorAttInfo.addr;
	fbData.nativeColorFormat = PxFmt(colorAttInfo.nativeFormat);
	fbData.width    = colorAttInfo.width;
	fbData.height   = colorAttInfo.height;
	
	if(depthAttInfo.addr.vaddr)
	{
		fbData.depthStencilAddr = depthAttInfo.addr;
		fbData.nativeDepthStencilFormat = PxFmt(depthAttInfo.nativeFormat);
	}
	else
	{
		fbData.depthStencilAddr.vaddr = fbData.depthStencilAddr.paddr = 0;
		fbData.nativeDepthStencilFormat = E_INVALID_PIXEL_FORMAT;
	}
	//TODO: stencil

	fbData.flipped = 1;

	return fbData;
	
}


GL_API GLboolean GL_APIENTRY glIsRenderbuffer(GLuint renderbuffer)
{
	GET_GL_STATE(ctx);

	RBOMap::iterator it =  ctx->renderbuffState.renderbuffList.find(renderbuffer);
	
	if(it == ctx->renderbuffState.renderbuffList.end())
	{
		return GL_FALSE;
	}

	return GL_TRUE;
	
}

GL_API void GL_APIENTRY glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
	GET_GL_STATE(ctx);

	if(target != GL_RENDERBUFFER)
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	if(renderbuffer == 0)
	{
		ctx->renderbuffState.renderbuffBinding = 0;
		return;
	}

	RBOMap::iterator it =  ctx->renderbuffState.renderbuffList.find(renderbuffer);
	
	//Does not exist? 
	if(it == ctx->renderbuffState.renderbuffList.end())
	{
		//Try creating a new one
		if(ctx->renderbuffState.createRenderbuffer(renderbuffer))
		{
			ctx->renderbuffState.renderbuffBinding = renderbuffer;
		}
		else
		{
			set_err(GL_OUT_OF_MEMORY);
		}
	}
	else   //exists
	{
		ctx->renderbuffState.renderbuffBinding = renderbuffer;
	}
	
}

GL_API void GL_APIENTRY glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
{
	GET_GL_STATE(ctx);
	
	if((n==0) || (renderbuffers==0))
	{
		return;
	}

	for(int i=0; i<n; i++)
	{
		RBOMap::iterator it =  ctx->renderbuffState.renderbuffList.find(renderbuffers[i]);

		//Is the renderbuffer id valid?
		if(it != ctx->renderbuffState.renderbuffList.end())
		{
			//Unbind if bound to GL_RENDERBUFFER
			if(renderbuffers[i] == ctx->renderbuffState.renderbuffBinding)
			{
				ctx->renderbuffState.renderbuffBinding = 0;
			}

			//Detach from current framebuffer
			ctx->framebuffState.detachRenderbuffer(renderbuffers[i]);
			
			pCA->Free(it->second->hChunk);

			ctx->renderbuffState.renderbuffList.erase(it);
			ctx->renderbuffState.renderbuffNames.removeName(renderbuffers[i]);
		}
	}
}

GL_API void GL_APIENTRY glGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
{
	GET_GL_STATE(ctx);

	if((n==0) || (renderbuffers == 0))
	{
		return;
	}

	ctx->renderbuffState.renderbuffNames.genNames(n, renderbuffers);
}

GL_API void GL_APIENTRY glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
	GET_GL_STATE(ctx);
	
	if(target != GL_RENDERBUFFER)
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	if(!isValidRBformat(internalformat))
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	if((width > MAX_RENDERBUFFER_SIZE) || (height > MAX_RENDERBUFFER_SIZE))
	{
		set_err(GL_INVALID_VALUE);
		return;
	}

	if(ctx->renderbuffState.renderbuffBinding == 0)
	{
		set_err(GL_INVALID_OPERATION); //TODO: spec ambiguity
		return;
	}

	RenderbufferObject* rbo = 0;
	if(!ctx->renderbuffState.getRBO(ctx->renderbuffState.renderbuffBinding,&rbo))
	{
		//LOGMSG("Current renderbuffer is not valid: %d!\n",ctx->renderbuffState.renderbuffBinding );
		gAssert(false);
		return;
	}

	if(rbo->hChunk)
	{
		pCA->Free(rbo->hChunk);
		rbo->hChunk = 0;
	}

	rbo->internalFormat = internalformat;
	rbo->nativeFormat = translateGLSizedInternal(internalformat);

	GLuint size = width * height * pixelSize(rbo->nativeFormat);
	
	rbo->hChunk = pCA->New(size);

	if((rbo->hChunk == 0) || ((GLubyte*) (rbo->hChunk->GetVirtAddr())) == 0) 
	{
		set_err(GL_OUT_OF_MEMORY);
		rbo->setDefaults();         //TODO: spec ambiguity
		return;
	}
	
	rbo->width = width;
	rbo->height = height;

}

GL_API void GL_APIENTRY glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	GET_GL_STATE(ctx);
	
	if(target != GL_RENDERBUFFER)
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	if(ctx->renderbuffState.renderbuffBinding == 0)
	{
		set_err(GL_INVALID_OPERATION);
		return;
	}

	if(!params)
	{
		return;
	}

	RenderbufferObject* rbo = 0;
	if(!ctx->renderbuffState.getRBO(ctx->renderbuffState.renderbuffBinding,&rbo))
	{
		//LOGMSG("Current renderbuffer is not valid: %d!\n",ctx->renderbuffState.renderbuffBinding );
		gAssert(false);
		return;
	}
	
	

	switch(pname)
	{
	case GL_RENDERBUFFER_WIDTH:         *params = rbo->width; break;

	case GL_RENDERBUFFER_HEIGHT:        *params = rbo->height; break;

	case GL_RENDERBUFFER_INTERNAL_FORMAT:   *params = rbo->internalFormat; break;

	case GL_RENDERBUFFER_RED_SIZE:      *params = 0; break; //TODO: return actual
		
	case GL_RENDERBUFFER_GREEN_SIZE:    *params = 0; break; //TODO: return actual

	case GL_RENDERBUFFER_BLUE_SIZE:     *params = 0; break; //TODO: return actual

	case GL_RENDERBUFFER_ALPHA_SIZE:    *params = 0; break; //TODO: return actual

	case GL_RENDERBUFFER_DEPTH_SIZE:    *params = 0; break; //TODO: return actual

	case GL_RENDERBUFFER_STENCIL_SIZE:  *params = 0; break; //TODO: return actual

	default:
		set_err(GL_INVALID_ENUM);
	}

}

//TODO: is this included in GLES 2.0?
GL_API void GL_APIENTRY glGetRenderbufferStorageFormatsiv(GLenum target, int n, GLint *listofformats, GLint *numsupportedformats)
{
}

GL_API GLboolean GL_APIENTRY glIsFramebuffer(GLuint framebuffer)
{
	GET_GL_STATE(ctx);

	FBOMap::iterator it =  ctx->framebuffState.framebuffList.find(framebuffer);

	if(it == ctx->framebuffState.framebuffList.end())
	{
		return GL_FALSE;
	}

	return GL_TRUE;
	
}

GL_API void GL_APIENTRY glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	GET_GL_STATE(ctx);

	if(target != GL_FRAMEBUFFER)
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	if(framebuffer == 0)
	{
		ctx->framebuffState.framebuffBinding = 0;
		return;
	}

	FBOMap::iterator it =  ctx->framebuffState.framebuffList.find(framebuffer);
	
	//Does not exist? 
	if(it == ctx->framebuffState.framebuffList.end())
	{
		//Try creating a new one
		if(ctx->framebuffState.createFramebuffer(framebuffer))
		{
			ctx->framebuffState.framebuffBinding = framebuffer;
		}
		else
		{
			set_err(GL_OUT_OF_MEMORY);
		}
	}
	else   //exists
	{
		ctx->framebuffState.framebuffBinding = framebuffer;
	}
}
GL_API void GL_APIENTRY glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
{
	GET_GL_STATE(ctx);
	
	if((n==0) || (framebuffers==0))
	{
		return;
	}

	for(int i=0; i<n; i++)
	{
		FBOMap::iterator it = ctx->framebuffState.framebuffList.find(framebuffers[i]);

		//Is the framebuffer id valid?
		if(it != ctx->framebuffState.framebuffList.end())
		{
			//Unbind if bound to GL_FRAMEBUFFER
			if(framebuffers[i] == ctx->framebuffState.framebuffBinding)
			{
				ctx->framebuffState.framebuffBinding = 0;
			}

			ctx->framebuffState.framebuffList.erase(it);
			ctx->framebuffState.framebuffNames.removeName(framebuffers[i]);
		}
	}
}

GL_API void GL_APIENTRY glGenFramebuffers(GLsizei n, GLuint *framebuffers)
{
	GET_GL_STATE(ctx);

	if((n==0) || (framebuffers == 0))
	{
		return;
	}

	ctx->framebuffState.framebuffNames.genNames(n, framebuffers);
}

GL_API GLenum GL_APIENTRY glCheckFramebufferStatus(GLenum target)
{
	GET_GL_STATE(ctx);

	if(GL_FRAMEBUFFER != target)
	{
		set_err(GL_INVALID_ENUM);
		return (GLenum)0;
	}

	if(ctx->framebuffState.framebuffBinding == 0)
	{
		//TOOD: spec ambiguity should an error be raised?
		return (GLenum)0;
	}

	FramebufferObject* fbo =0;
	if(!ctx->framebuffState.getFBO(ctx->framebuffState.framebuffBinding, &fbo))
	{
		gAssert(false && "Invalid fbo bound as current!\n");
		return (GLenum)0;
	}

	return fbo->status();
}


GL_API void GL_APIENTRY glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	//Note: texture completeness is not required here since we are only concerned with the level of the texture
	
	GET_GL_STATE(ctx);

	if((target != GL_FRAMEBUFFER)||(!isValidFBAttachment(attachment)))
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	if( (ctx->framebuffState.framebuffBinding == 0))
	{
		set_err(GL_INVALID_OPERATION);
	}

	FramebufferObject* fbo =0;
	if(!ctx->framebuffState.getFBO(ctx->framebuffState.framebuffBinding, &fbo))
	{
		gAssert(false && "Invalid fbo bound as current!\n");
		return;
	}
	FBOAttachmentPointState* att = fbo->getAttachment(attachment);
	
	if(!att)
	{
		gAssert(false && "Attachment pointer is null!\n");
		return;
	}

	if(texture==0)
	{
		att->setDefaults();
		return;
	}


	TextureObject* texObj = GetTexObject(texture);

	//If tex object is a valid object
	if(texObj == 0)
	{
		set_err(GL_INVALID_VALUE);
		return;
	}

	if(texObj->texType == GL_TEXTURE_2D)
	{
		if(textarget != GL_TEXTURE_2D)
		{
			set_err(GL_INVALID_OPERATION);
			return;
		}
	}
	else if(texObj->texType == GL_TEXTURE_CUBE_MAP)
	{
		switch(textarget)
		{
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
			break;
			
		default:
			set_err(GL_INVALID_OPERATION);
			return;
		}
	}
	else
	{
		set_err(GL_INVALID_OPERATION);
		return;
	}

	if(level != 0)
	{
		set_err(GL_INVALID_VALUE);
		return;
	}
	
	att->setDefaults();

	att->objectType = GL_TEXTURE;
	att->objectName = texture;
	att->textureLevel = level;
	if(texObj->texType == GL_TEXTURE_CUBE_MAP)
	{
		att->textureCubeMapFace = textarget;
	}


}

//TODO: This is not supported
GL_API void GL_APIENTRY glFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{

	//LOGMSG("glFramebufferTexture3D not implemented");
}

GL_API void GL_APIENTRY glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
	GET_GL_STATE(ctx);

	if((target != GL_FRAMEBUFFER)||(!isValidFBAttachment(attachment)))
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	if( (ctx->framebuffState.framebuffBinding == 0))
	{
		set_err(GL_INVALID_OPERATION);
	}

	FramebufferObject* fbo =0;
	if(!ctx->framebuffState.getFBO(ctx->framebuffState.framebuffBinding, &fbo))
	{
		gAssert(false && "Invalid fbo bound as current!\n");
		return;
	}
	FBOAttachmentPointState* att = fbo->getAttachment(attachment);
	
	if(!att)
	{
		gAssert(false && "Attachment pointer is null!\n");
		return;
	}

	if(renderbuffer == 0)
	{
		att->setDefaults();
		return;
	}
	
	RenderbufferObject* rbo =0;

	//Spec: If <renderbuffer> is zero, then the value of <renderbuffertarget> is
	//ignored.
	if(renderbuffertarget != GL_RENDERBUFFER)
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	if(!ctx->renderbuffState.getRBO(renderbuffer, &rbo))
	{
		set_err(GL_INVALID_OPERATION);
		return;
	}

	att->setDefaults();
	att->objectType = GL_RENDERBUFFER;
	att->objectName = renderbuffer;
}

//may be to be renamed as GetFramebufferParameteriv as this is present in ESSpec
// but not updated in new gl.h
GL_API void GL_APIENTRY glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params)
{
	GET_GL_STATE(ctx);
	GLuint index=0;

	if(target != GL_FRAMEBUFFER)
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	if(ctx->framebuffState.framebuffBinding == 0)
	{
		set_err(GL_INVALID_OPERATION);
		return;
	}

	if(!isValidFBAttachment(attachment))
	{
		set_err(GL_INVALID_ENUM);
		return;
	}

	if(!params)
	{
		return;
	}

	FBOMap::iterator it =  ctx->framebuffState.framebuffList.find(ctx->framebuffState.framebuffBinding);

	if(it == ctx->framebuffState.framebuffList.end())
	{
		//LOGMSG("Current framebuffer is not valid: %d!\n",ctx->framebuffState.framebuffBinding );
		gAssert(false);
		return;
	}

	FBOAttachmentPointState* att = it->second->getAttachment(attachment);
	
	if(!att)
	{
		gAssert(false);
		return;
	}

	if(pname == GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE)
	{
		*params = att->objectType;
		return;
	}

	if(att->objectType == GL_RENDERBUFFER)
	{
		if(pname == GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME)
		{
			*params = att->objectName;
			return;
		}
		
		set_err(GL_INVALID_ENUM);
		return;
	}
	else if(att->objectType == GL_TEXTURE)
	{
		switch(pname)
		{
		case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME:
			*params = att->objectName;
			return;
		case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL:
			*params = att->textureLevel;
			return;
		case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE:
			//TODO:
			//if is cube texture, 
			//*params = att->textureCubeMapFace;
			//else
			//*params = 0;
			index = GetTexNameArrayIndex(att->objectName,false);
			if(GL_TEXTURE_CUBE_MAP == ctx->sharedState->sharedTexState.texObjects[index]->texType)
			{
				*params = att->textureCubeMapFace;
			}
			else
			{
				*params = 0;
			}

			return;
		}
	}

	set_err(GL_INVALID_ENUM);
	return;

}
