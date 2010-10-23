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
* \file         glFimg.h
* \author       Prashant Singh (prashant@samsung.com)
* \brief        Hardware interface
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*       31.07.2006      Prashant Singh          Initial functionality for firmware demo
*
*       21.08.2006      Prashant Singh          API slightly modified for firmware demo
*
*   1.12.2006   Prashant Singh          Modifications for partial update
*
*******************************************************************************
*/

#ifndef __GL_FIMG_H__
#define __GL_FIMG_H__

/*
*******************************************************************************
* Includes
*******************************************************************************
*/

#include "gl.h"

/*
*******************************************************************************
* Macro definitions and enumerations
*******************************************************************************
*/

typedef enum {
        GLF_ERR_SUCCESS,
        GLF_ERR_ERROR
} glfError;

typedef enum {
        GLF_UF_VertexShader,
        GLF_UF_FragmentShader,
        GLF_UF_Uniforms,

        GLF_UF_DrawMode,

        GLF_UF_ViewPort,
        GLF_UF_DepthRange,

        GLF_UF_PixelSampler,
        GLF_UF_DepthOffset,
        GLF_UF_BackfaceCull,
        GLF_UF_LODControl,
        GLF_UF_Clipping,
        GLF_UF_PWidth,
        GLF_UF_LWidth,

        GLF_UF_ScissorTest,
        GLF_UF_AlphaTest,
        GLF_UF_StencilTest,
        GLF_UF_DepthTest,
        GLF_UF_Blend,
        //for depth mask & color mask
        GLF_UF_ColorBuffer,
        GLF_UF_DepthBuffer,
        GLF_UF_LogicalOp,

        GLF_UF_FragmentTexture,
        GLF_UF_FragmentTexture0 = GLF_UF_FragmentTexture,
        GLF_UF_FragmentTexture1,
        GLF_UF_FragmentTexture2,
        GLF_UF_FragmentTexture3,
        GLF_UF_FragmentTexture4,
        GLF_UF_FragmentTexture5,
        GLF_UF_FragmentTexture6,
        GLF_UF_FragmentTexture7

} glfUpdateFlag;


//! Update bits encoding
typedef enum {

        // Host interface
        UB_HI_ALL =  1 ,

        // Vertex shader
        UB_VS_ALL,

        // Primitive engine
        UB_PE_ALL,

        // Raster engine
        UB_RE_ALL ,

        // Fragment shader
        UB_FS_ALL ,

        // Texture unit
        UB_TU_ALL ,

        // Per-fragment unit
        UB_PF_ALL ,

        //buffer may be updated regularly so they are used as separate
        UB_PF_BUF_ALL,

} UpdateBitEncoding;




/*
*******************************************************************************
* Type, Structure & Class Definitions
*******************************************************************************
*/

enum DataTranslation
{
        DATA_TRANSLATION_NONE,
        DATA_TRANSLATION_SHORT_TO_FLOAT,
        DATA_TRANSLATION_BYTE_TO_FLOAT
};
struct AttribData
{
        GLint                           dataSize;
        GLsizei                         stride;
        GLenum                          dataType;
        const GLubyte*          pData;
        DataTranslation         dataTranslation;
};

struct AttribInfo
{
    AttribData attribData[MAX_VERTEX_ATTRIBS];
    unsigned int attribOrder[3];
    unsigned int vertexSize;
    const GLubyte*  firstAttribSrc;  //contains the pointer to the attrib data
                        // that is located first in memory, to be used for
                        // block transfer
        unsigned char numAttribs;
    
        bool nonDwordAttribUsed;
    bool vboAttribSources;
        bool packedArray; //true is no attrib pointer aliases and no holes are 
                      //present & strides of all attribs are equal
    bool packedVertex; //true if packedArray is true and vertexSize == stride
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
glfError FimgFinish(OGLState* pState);

glfError glfInit                        (OGLState* pState);                     //!< Initialize hardware

glfError glfFinish                      (OGLState* pState);                     //!< glFinish
glfError glfFlush                       (OGLState* pState);                     //!< glFlush

glfError glfClear                       (OGLState* pState, GLbitfield mask);    //!< glClear

glfError glfSetState            (OGLState* pState);                     //!< Set GL state in hardware
glfError glfSetHostIf           (OGLState* pState, GLboolean isBuf);    //!< Set host interface
glfError glfSetVertexShader     (OGLState* pState);                     //!< Set vertex shader hardware
glfError glfSetFragShader       (OGLState* pState);                     //!< Set pixel shader hardware
glfError glfSetPrimEngine       (OGLState* pState);                     //!< Set primitive engine
glfError glfSetDrawMode         (OGLState *pState, GLenum mode);                //!< Set primitive draw mode
glfError glfSetRasterEngine     (OGLState* pState);                     //!< Set raster engine
glfError glfSetPerFragUnit      (OGLState* pState);                     //!< Set per-fragment unit
glfError glfSetTextureUnit      (OGLState* pState);                     //!< Set texture unit

glfError glfDraw                    (OGLState* pState, GLenum primitive, GLint first, GLsizei count);
                    //!<
glfError glfDrawElements            (OGLState* pState, GLenum primitive, GLsizei count, GLenum type, const void* indices);

glfError glfTransferArrayUnbuf          (OGLState* pState, GLint first, GLsizei count);
                                        //!< Transfer vertex array data to host interface (Un-buffered)
glfError glfTransferElementsUnbuf       (OGLState* pState, GLsizei count, GLenum type, const void* indices);
                                        //!< Transfer vertex elements to host interface (Un-buffered)

glfError glfTransferArrayBuf            (OGLState* pState, GLint first, GLsizei count);
                                        //!< Transfer vertex array data to host interface (Buffered)
glfError glfTransferElementsBuf         (OGLState* pState, GLsizei count, GLenum type, const void* indices);

FGL_Error pickfglSetBlend (bool noAlphaBits, FGL_BOOL enable, pFGL_Blend blendParam );                          
                                        
                                        //!< Transfer vertex elements to host interface (Buffered)


inline void 
SetUpDateFlagPerFragment(OGLState* pState, glfUpdateFlag uFlag)
{
        #if TRACE_DIRTY_STATE == ENABLE
        glfUpdateBits *updateBits = &(pState->updateBits);

        updateBits->pf |= UB_PF_ALL;
        #endif

}

inline void 
SetUpDateFlagPrimEngine(OGLState* pState, glfUpdateFlag uFlag)
{
        #if TRACE_DIRTY_STATE == ENABLE
        glfUpdateBits *updateBits = &(pState->updateBits);

        updateBits->pe |= UB_PE_ALL;
        #endif

}

inline void 
SetUpDateFlagRasterEngine(OGLState* pState, glfUpdateFlag uFlag)
{
        #if TRACE_DIRTY_STATE == ENABLE
        glfUpdateBits *updateBits = &(pState->updateBits);

        updateBits->re |= UB_RE_ALL;
        #endif

}

#endif /*__GL_FIMG_H__*/

