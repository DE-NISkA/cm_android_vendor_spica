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
* \file         pixel.h
* \author       Sandeep Kakarlapudi (s.kakarla@samsung.com)
* \brief        pixel format definitions and conversion function declarations
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*       03.04.2007      Sandeep Kakarlapudi             Initial Version
*
*******************************************************************************
*/

#ifndef __PIXEL_H__
#define __PIXEL_H__

#include "gl.h"

#include <stdio.h>
#include <math.h>
//#include <string>

#include "fgl.h"
#include "pixelFmts.h"

/*
*******************************************************************************
* Macro definitions and enumerations
*******************************************************************************
*/

//TODO: include some order comments here?

/*
*******************************************************************************
* Type, Structure & Class Definitions
*******************************************************************************
*/

union PxRGBAData
{
    unsigned int val;
    unsigned char c[4];
};

enum colorType{
                ALPHA = 0,
                RED = 1,
                GREEN = 2,
                BLUE = 3,
                DEPTH = 4,
                STENCIL =5,



        };
/*
*******************************************************************************
* Function Declarations
*******************************************************************************
*/

//Returns pixel size in number of bytes
unsigned int pixelSize(PxFmt fmt);
bool isValidPixelFmt(PxFmt fmt);
bool isValidFBColorFmt(PxFmt fmt);
bool isValidTexPixelFmt(PxFmt fmt);

PxFmt translateGLSizedInternal(GLenum format);
PxFmt translateGLInternal(GLenum format,GLenum type);
GLenum translateToGLenum(PxFmt fmt);
FGL_PixelFormat translateToFglPixelFormat(PxFmt fmt);
unsigned int convertFromDepthStencil(PxFmt dFmt, float depth, int stencil);
unsigned int getDepthStencilMask(PxFmt dFmt, bool depthMask, unsigned char stencilMask);
unsigned int getColorMask(PxFmt dFmt, bool redMask, bool greenMask, bool blueMask, bool alphaMask);

unsigned int convertFromRGBA(PxFmt dFmt, PxRGBAData s);
unsigned int convertFromRGBA(PxFmt dFmt, float rval, float gval, float bval, float aval);
unsigned int convertFromRGBA(PxFmt dFmt, unsigned char rval, unsigned char gval, unsigned char bval, unsigned char aval);

//liyue 090527
void convertPixelsSubTexture( PxFmt dstFmt, void* dstPixels, GLsizei DstWidth, GLsizei DstHeight,GLuint uiXOffset, GLuint uiYOffset,    PxFmt srcFmt,const void* srcPixels, GLsizei SrcWidth, GLsizei SrcHeight);

void convertPixels( PxFmt dstFmt, void* dstPixels, GLsizei dstWidth, GLsizei dstHeight, GLsizei dstDepth,
                                    GLuint dstX, GLuint dstY, GLuint dstZ,
                                        PxFmt srcFmt,const void* srcPixels, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth,
                                        GLuint srcX, GLuint srcY, GLuint srcZ,
                                        GLsizei cpyWidth, GLsizei cpyHeight, GLsizei cpyDepth , GLint dstPad, GLint srcPad,GLint dir) ;


void genMipMaps(PxFmt fmt, void* pIn, void* pOut, GLsizei widthOut, GLsizei heightOut,GLsizei widthIn, GLsizei heightIn);
GLenum determineTypeFormat(int format, int Isformat);//added
int determinePixelBitSize(PxFmt format, colorType colortype);//added
//TODO: other converters

inline bool hasDepth(PxFmt fmt) 
{
    return fmt == E_Depth24 || fmt == E_Stencil8Depth24;
}

inline bool hasStencil(PxFmt fmt)
{
    return fmt == E_Stencil8 || fmt == E_Stencil8Depth24;
}

inline bool hasAlpha(PxFmt fmt)
{
    return (E_ARGB8 <= fmt) && (fmt <= E_ALPHA8);
    
}
inline GLfloat clamp(GLclampf x)
{
    if( x< 0.0)
    {
        return 0.0f;
    }
    else if(x > 1.0)
    {
        return 1.0f;
    }

    return x;
}


#endif //__PIXEL_H__
