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
* \file         glFramebufferObject.h
* \author       Sandeep Kakarlapudi (s.kakarla@samsung.com)
* \brief        Struct & function Definitions for FBOs and renderbuffers
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*       06.02.2007      Sandeep Kakarlapudi             Initial Version
*
*******************************************************************************
*/

#ifndef __GL_FRAMEBUFFER_OBJECT__
#define __GL_FRAMEBUFFER_OBJECT__

/*
*******************************************************************************
* Includes
*******************************************************************************
*/

#include "gl.h"
#include "glConfig.h"
#include "ChunkAlloc.h"
#include "glContext.h"

//#include <map>
#include "ustl.h"

/*
*******************************************************************************
* Macro definitions and enumerations
*******************************************************************************
*/

/*
*******************************************************************************
* Type, Structure & Class Definitions
*******************************************************************************
*/

//! Renderbuffer Object
struct RenderbufferObject
{
    GLuint          id;
    GLuint          width;
    GLuint          height;
    GLenum          internalFormat;
        PxFmt                   nativeFormat;   
    PtrChunkH       hChunk;

    RenderbufferObject(GLuint id_)
    {   
        hChunk = 0;
        setDefaults(); 
        id = id_; 
    }

    void setDefaults()
    {
        id              = 0;
        width           = 0;
        height          = 0;
        internalFormat  = GL_RGBA;
                nativeFormat    = E_ARGB8;

        if(hChunk)
        {
            pCA->Free(hChunk);
            hChunk          = 0;
        }
    }
};
//! Rendertarget Info
struct RTInfo
{
    GLenum  format;
        PxFmt   nativeFormat;
    GLuint  height;
    GLuint  width;
    AddressBase addr;

    RTInfo()
    {
        format = GL_NONE;
                nativeFormat = E_INVALID_PIXEL_FORMAT;
        height = 0;
        width = 0;
        addr.paddr = 0;
        addr.vaddr = 0;
    }
};

//! Framebuffer object attachment point state
struct FBOAttachmentPointState
{
    GLenum      objectType;
    GLuint      objectName;
    GLuint      textureLevel;
    GLenum      textureCubeMapFace;

    FBOAttachmentPointState(){ setDefaults(); }
    
    void setDefaults()
    {
        objectType          = GL_NONE;
        objectName          = 0;
        textureLevel        = 0;
        textureCubeMapFace  = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    }

    RTInfo getInfo();
};

//! Framebuffer Object
struct FramebufferObject
{
    GLuint      id;
    FBOAttachmentPointState      colorAttachment;
    FBOAttachmentPointState      depthAttachment;
    FBOAttachmentPointState      stencilAttachment;

    FramebufferObject(GLuint id_) { setDefaults(); id = id_; }
    
    void setDefaults()
    {
        id = 0;
        colorAttachment.setDefaults();
        depthAttachment.setDefaults();
        stencilAttachment.setDefaults();
    }

    FBOAttachmentPointState* getAttachment(GLenum attachment)
    {
        switch(attachment)
        {
        case GL_COLOR_ATTACHMENT0:  return &colorAttachment;
        case GL_DEPTH_ATTACHMENT: return &depthAttachment;
        case GL_STENCIL_ATTACHMENT: return &stencilAttachment;
        }
        gAssert(false && "ERROR: Unexpected attachment type\n");
        return 0;
    }

    GLenum status();
};

typedef ustl::vector<GLuint> NameContainer;

struct NameManager
{
    NameContainer  nameList;

    void addName(GLuint id);
    void genNames(GLsizei n, GLuint* ids);
    void removeName(GLuint id);

    void clear();

private:

    GLuint _genName();
};

typedef ustl::map<GLuint, RenderbufferObject*> RBOMap;
typedef ustl::map<GLuint, FramebufferObject*> FBOMap;

//! Renderbuffer State
struct RenderbufferState
{
    GLuint          renderbuffBinding;

    NameManager     renderbuffNames;
    RBOMap          renderbuffList;


    RenderbufferState(){ setDefaults(); }
    
    void setDefaults()
    {
        renderbuffBinding = 0;
        renderbuffNames.clear();

        RBOMap::iterator it = renderbuffList.begin();

        while(it != renderbuffList.end())
        {
            delete it->second;
            ++it;
                }

        renderbuffList.clear();
    }

    RenderbufferObject* createRenderbuffer(GLuint id)
    {
        RenderbufferObject* rb = new RenderbufferObject(id);

        if(rb)
        {
            renderbuffList[id] = rb;
        }
        else
        {
            Plat::printf("ERROR: unable to create framebuffer object out of memory\n");
        }

        return rb;
    }

    bool    getRBO(GLuint id, RenderbufferObject** rbo)
    {
        RBOMap::iterator it =  renderbuffList.find(id);

        if(it == renderbuffList.end())
        {
            *rbo = 0;
            return false;
        }
        else
        {
            *rbo = it->second;
            if( *rbo == 0)
            {
                gAssert((rbo!=0) && "Renderbuffer id is mapped to null pointer!\n");
            }
        }
        return true;
    }

};

//! Framebuffer Objects State
struct FramebufferState
{
    GLuint          framebuffBinding;

    NameManager     framebuffNames;
    FBOMap          framebuffList;

    
    FramebufferState();
    
    FramebufferObject* createFramebuffer(GLuint id)
    {
        FramebufferObject* fb = new FramebufferObject(id);

        if(fb)
        {
            framebuffList[id] = fb;
        }
        else
        {
            Plat::printf("ERROR: unable to create framebuffer object out of memory\n");
        }
        return fb;
    }

    bool    getFBO(GLuint id, FramebufferObject** fbo)
    {
        FBOMap::iterator it =  framebuffList.find(id);

        if(it == framebuffList.end())
        {
            *fbo = 0;
            return false;
        }
        else
        {
            *fbo = it->second;
            if( *fbo == 0)
            {
                gAssert((fbo!=0) && "Renderbuffer id is mapped to null pointer!\n");
            }
        }
        return true;
    }

    void setDefaults();
    void detachAttachment(GLenum objectType, GLuint objectName);
    void detachRenderbuffer(GLuint id);
    void detachTexture(GLuint id);
};

/*
*******************************************************************************
* Global Variable Declarations
*******************************************************************************
*/

/*
*******************************************************************************
* Function Declarations
*******************************************************************************
*/

//Returns true if using system provided FB
//Returns true if current fbo is complete
// else returns false;
bool isFBrenderable(OGLState* ctx);

FramebufferData getFBData();
FramebufferData getReadFBData();

#endif //__GL_FRAMEBUFFER_OBJECT__
