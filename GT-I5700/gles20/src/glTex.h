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
* \file         glTex.h
* \author       Prashant Singh (prashant@samsung.com),
*                       Anurag Ved (anuragv@samsung.com)
* \brief        Texture state and functions
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*       31.07.2006      Anurag Ved              Basic texture state.
*
*        3.08.2006      Prashant Singh  Re-designed texture state and structures.
*                                                               Considered 2D, 3D, Cube maps and MipMaps.
*
*       22.08.2006      Prashant Singh  Compilable version
*
*******************************************************************************
*/

#ifndef __GL_TEX_H__
#define __GL_TEX_H__

/*
*******************************************************************************
* Includes
*******************************************************************************
*/

#include "gl.h"
#include "glConfig.h"
//#include "glState.h"
#include "ChunkAlloc.h"
#include "pixel.h"
#include "glContext.h"


/*
*******************************************************************************
* Macro definitions and enumerations
*******************************************************************************
*/
typedef struct Address{
                void* phyAdress;
                void* virAddress;
} phyVirAddress;


/*
*******************************************************************************
* Type, Structure & Class Definitions
*******************************************************************************
*/
typedef enum{
        NONE,
        IMAGECHUNK,
        TEXOBJCHUNK,
}ImageDataLoc;


struct TexFGLState{
    GLuint          texSFRtate[18];    //caching of the texture object state
    GLuint                  noOfpalette;        //no of palette entry
    GLuint*             palEntryFGLState;   //contain the palette value if present 
    GLuint          fglStateDirty;

};


struct TexStateToConfigFimg{
    GLuint  id;
    TexFGLState* pTexFGLState;     
};

/*-----------------------------------------------------------------------*//*!
* Individual image data specified through glTexImage*()
*//*------------------------------------------------------------------------*/

typedef struct {

        GLboolean           isUsed;                                 //!< Is the image being used
        GLsizei             width;                                  //!< Width of image
        GLsizei             height;                                 //!< Height of image
        GLsizei             depth;                                  //!< Depth of image (3D)
        GLenum              internalFormat;                 //!<used specified Internal format
        GLenum              PixType;                        //user specified pixType
        PxFmt               nativeFormat;                   // added for using instead of internal format & type 
        GLsizei         imgSize;                            //sored image size in bytes
        GLboolean       isCompressed;           //is image of compressed format
        PtrChunkH           hImgChunk;                      //!< Chunk of  image data
        ImageDataLoc    imagedataLocation;          //specfiy whether the data is in the image chunk , tex oibject hunck or null
        
} Image;

/*-----------------------------------------------------------------------*//*!
* Texture object
*//*------------------------------------------------------------------------*/

class TextureObject {

public:
        TextureObject() : isLocked(0) {}

        GLuint              id;                                 //!< Texture Id (Required if object storage changes)
        GLenum              texType;                            //!< Texture type
        GLboolean           isUsed;                             //!< Is the texture object being used

        GLenum              minFilter;                  //!< Min filter
        GLenum              magFilter;                  //!< Max filter
        GLenum              wrapS;                              //!< Wrap modes - S
        GLenum              wrapT;                              //!< Wrap modes - T
        GLenum              wrapR;                              //!< Wrap modes - R
    //Texture OES API - 2009.05.20
        GLint                   crop_rect[4];

        GLboolean           reCompile;                  //!< True if recompilation of texture image is required
        GLuint              isDirtyState;               //!<  set the dirty state. when ever a glTex* function is called this has to be set.
        //GLuint                    fglStateDirty;              //!< to set the texture registers
        GLenum              internalFormat;             //! user specified  Internal format
        PxFmt                   nativeFormat;       // stored image PxFmt
        GLint               width;                              //!< Width of base level image
        GLint               height;                             //!< Height of base level image
        GLint               depth;                              //!< Depth of base level image (3D)
        GLint               levels;                             //!< Number of levels in mipmap
        PtrChunkH           hChunk;                             //!< Chunk of consolidated image data
        TexFGLState     texFGLState;        
        //GLuint*               palEntryFGLState;   //contain the palette value if present
        //GLuint                    noOfpalette;
        GLint               texObjrefCount;             //count of the no. of context using the texture object
        GLboolean           deleteTexObj;               //marked true if texture object  needs to be deleted.
    GLboolean       isExtTex;           //if true it is external texture object
    AddressBase     pExtTexchunk;       //contain the pointer to external  texture object 
        
        //! Texture images (either of 2D, 3D or CubeMap)
        union {
                Image           tex2D[MAX_MIPMAP_LEVELS];               //!< 2D texture images
                Image           tex3D[MAX_MIPMAP_LEVELS];               //!< 3D texture images
                Image           cubeMap[6][MAX_MIPMAP_LEVELS];  //!< CubeMap images

        }                       images;
         GLuint Offsets[MAX_MIPMAP_LEVELS];                                 //OFFSET TO THE MIPMAP LEVEL (WITHOUT PIXEL SIZE) 


         // - Member functions ----------------------------------------------------

        void            Init(GLint tex, GLenum type);   //!< Initialize texture object to default values, mark used 
        void            reset( );               // reset the texture structure to zero values
        void        Delete();                                           //!< Delete texture object, mark unused

        GLboolean       IsComplete(int* levels);                                        //!< Check if texture object is complete
        GLboolean       Compile();                                              //!< Consolidate texture data from various images
        phyVirAddress  GetTex2DMipLevel(GLint level);
        phyVirAddress  GetTex3DMipLevel(GLint level);
        phyVirAddress  GetTexCubeMipLevel(GLint level, GLint texCubeMapFace);

        GLboolean release(OGLState* ctx);
        void acquire(OGLState* ctx);
        GLboolean updateTexFGLState();  

    void releaseMem();
    GLenum Validate();

    GLboolean isLocked;

private:
        inline GLsizei  CompiledSize();                         //!< Size of the compiled image data
                                                                                                // Valid only if object is complete and width, height, etc are updated
                                                                                        

} ;

/*-----------------------------------------------------------------------*//*!
* Texture state data
*//*------------------------------------------------------------------------*/

typedef struct
{
        GLuint                  texture2D;                      //!< Current 2D txture id
        GLuint                  texture3D;                      //!< Current 3D texture id
        GLuint                  cubeMap;                                //!< Current Cube Map id
        
} CurrentTexture;


typedef struct
{
        TextureObject*  texture2D;
        TextureObject*  texture3D;
        TextureObject*  cubeMap;
}PCurrentTex;


typedef struct
{

        TextureObject               defaultTexObjects[3];                       //default texture object
        GLuint                          activeTexUnit;                                  //!< Active texture unit
        CurrentTexture          texUnitBinding[MAX_TEXTURE_UNITS];          //sanvd added this
        PCurrentTex                 ptexUnitBinding[MAX_TEXTURE_UNITS];         //store pointer to texture object
        GLuint                          prevTexUnitBinding[MAX_TEXTURE_UNITS];      // for tracing dirty state contain the previous id.
        TexStateToConfigFimg    pTexObjectToConfigFimg[MAX_TEXTURE_UNITS];  //pointer to the texture objects to be configured for each texture unit
} TextureState;

/*
*******************************************************************************
* Global Variable Declarations
*******************************************************************************
*/

/*
*******************************************************************************
* Function Declarations
*********************************s**********************************************
*/


void    InitTextureState ();
void    InitLocalTextureState (OGLState* pState);
void    DeInitLocalTextureState(OGLState* pState);

GLint GetTexNameArrayIndex(GLuint texName,bool IsDefault); //Ctx param removed by SandeepK.
TextureObject* GetTextureObject ( GLenum target,bool extTU,GLint texUnit );
GLsizei GetPixelSize (GLenum texFormat, GLenum type);
void SwapRB(GLubyte* pixels,GLenum format,GLenum type, GLsizei dstWidth, GLsizei dstHeight,GLsizei dstDepth,
                        int xoff,int yoff,int zoff, GLsizei width, GLsizei height,GLsizei depth, int pixelSize);
TextureObject*  GetTexObject(GLuint texName);
GLenum CheckFormatTypeCombination(GLenum texFormat, GLenum type);
GLuint GetPixSize(GLenum TextureFormat);
void releaseBoundTextures(OGLState* pState);
GLenum  ValidatePxFmt(PxFmt pxFmtValue);

#endif /*__GL_TEX_H__*/

