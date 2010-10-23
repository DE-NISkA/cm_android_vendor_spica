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
* \file     glTex.cpp
* \author   Prashant Singh (prashant@samsung.com),
*           Anurag Ved (anuragv@samsung.com)
* \brief    Texture handling
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*   31.07.2006  Anurag Ved      Initial draft implementation
*
*    3.08.2006  Prashant Singh  Functions with new state and texture object
*
*   22.08.2006  Prashant Singh  Compilable version with isComplete, Compile,
*                               etc. Complete image specification, tex copy
*                               and compressed image api is missing
*
*   24.08.2006  Prashant Singh  Priliminary testing on host completed
*
*******************************************************************************
*/

/*
*******************************************************************************
* Includes
*******************************************************************************
*/

#include "gl.h"
#include "platform.h"
#include "glState.h"
#include "glTex.h"
#include "buffers.h"
#include "texfglstate.h"
#include "glFimg.h"
//#include "pixel.h"
#include "ustl.h"

/*
*******************************************************************************
* Macro definitions and enumerations
*******************************************************************************
*/

#define TEX_2D_DEFAULT          (MAX_TEXTURE_OBJECTS)
#define TEX_3D_DEFAULT          (TEX_2D_DEFAULT + 1)
#define TEX_CUBE_MAP_DEFAULT    (TEX_3D_DEFAULT + 1)
#define MIN(X,Y)                ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y)                ((X) > (Y) ? (X) : (Y)) 

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

//static GLint GetTextureObject (OGLState* pState, GLenum target);
static GLenum checkTextureFormat (OGLState* pState, GLenum internalFormat, GLenum format);
static inline GLboolean IsPowerOf2 (GLuint n);
static bool s3tcCompressedTex(GLenum& texformat,GLenum internalformat,GLsizei width,GLsizei height,GLsizei imageSize);
static GLboolean CheckFormatConversion(GLenum dstFormat, GLenum srcFormat, GLenum* dstType);


/*
*******************************************************************************
* Function Definitions
*******************************************************************************
*/


static void decodePalette4(const void *data, int level, int width, int height,
                           void *surface, int stride, int format)

{
    int indexBits = 8;
    int entrySize = 0;
    switch (format) {
    case GL_PALETTE4_RGB8_OES:
        indexBits = 4;
        /* FALLTHROUGH */
    case GL_PALETTE8_RGB8_OES:
        entrySize = 3;
        break;

    case GL_PALETTE4_RGBA8_OES:
        indexBits = 4;
        /* FALLTHROUGH */
    case GL_PALETTE8_RGBA8_OES:
        entrySize = 4;
        break;

    case GL_PALETTE4_R5_G6_B5_OES:
    case GL_PALETTE4_RGBA4_OES:
    case GL_PALETTE4_RGB5_A1_OES:
        indexBits = 4;
        /* FALLTHROUGH */
    case GL_PALETTE8_R5_G6_B5_OES:
    case GL_PALETTE8_RGBA4_OES:
    case GL_PALETTE8_RGB5_A1_OES:
        entrySize = 2;
        break;
    }

    const int paletteSize = (1 << indexBits) * entrySize;  
    unsigned char const* pixels = (unsigned char  *)data + paletteSize;
    for (int i=0 ; i<level ; i++) {
        int w = (width  >> i) ? : 1;
        int h = (height >> i) ? : 1;
        pixels += h * ((w * indexBits) / 8);
    }
    width  = (width  >> level) ? : 1;
    height = (height >> level) ? : 1;

    if (entrySize == 2) {
        unsigned char  const* const palette = (unsigned char*)data;
        for (int y=0 ; y<height ; y++) {
            unsigned char* p = (unsigned char*)surface + y*stride*2;
            if (indexBits == 8) {
                for (int x=0 ; x<width ; x++) {
                    int index = 2 * (*pixels++);
                    *p++ = palette[index + 0];
                    *p++ = palette[index + 1];
                }
            } else {
                for (int x=0 ; x<width ; x+=2) {
                    int v = *pixels++;
                    int index = 2 * (v >> 4);
                    *p++ = palette[index + 0];
                    *p++ = palette[index + 1];
                    if (x+1 < width) {
                        index = 2 * (v & 0xF);
                        *p++ = palette[index + 0];
                        *p++ = palette[index + 1];
                    }
                }
            }
        }
    } else if (entrySize == 3) {
        unsigned char const* const palette = (unsigned char*)data;
        for (int y=0 ; y<height ; y++) {
            unsigned char* p = (unsigned char*)surface + y*stride*3;
            if (indexBits == 8) {
                for (int x=0 ; x<width ; x++) {
                    int index = 3 * (*pixels++);
                    *p++ = palette[index + 0];
                    *p++ = palette[index + 1];
                    *p++ = palette[index + 2];
                }
            } else {
                for (int x=0 ; x<width ; x+=2) {
                    int v = *pixels++;
                    int index = 3 * (v >> 4);
                    *p++ = palette[index + 0];
                    *p++ = palette[index + 1];
                    *p++ = palette[index + 2];
                    if (x+1 < width) {
                        index = 3 * (v & 0xF);
                        *p++ = palette[index + 0];
                        *p++ = palette[index + 1];
                        *p++ = palette[index + 2];
                    }
                }
            }
        }
    } else if (entrySize == 4) {
        unsigned char const* const palette = (unsigned char*)data;
        for (int y=0 ; y<height ; y++) {
            unsigned char* p = (unsigned char*)surface + y*stride*4;
            if (indexBits == 8) {
                for (int x=0 ; x<width ; x++) {
                    int index = 4 * (*pixels++);
                    *p++ = palette[index + 0];
                    *p++ = palette[index + 1];
                    *p++ = palette[index + 2];
                    *p++ = palette[index + 3];
                }
            } else {
                for (int x=0 ; x<width ; x+=2) {
                    int v = *pixels++;
                    int index = 4 * (v >> 4);
                    *p++ = palette[index + 0];
                    *p++ = palette[index + 1];
                    *p++ = palette[index + 2];
                    *p++ = palette[index + 3];
                    if (x+1 < width) {
                        index = 4 * (v & 0xF);
                        *p++ = palette[index + 0];
                        *p++ = palette[index + 1];
                        *p++ = palette[index + 2];
                        *p++ = palette[index + 3];
                    }
                }
            }
        }
    }
}



#if 1

GLboolean getTextureNames( SharedTextureState* pSharedTexState ,GLsizei n, GLuint *textures)
{
        GLuint lastValue = 1;
        GLsizei toAssign  = n;
        int (* unusedValueRange)[2]=NULL;  // The first element will store the starting unused name (not present in the set) and the second element will store the range
        unusedValueRange = (int(*)[2])Plat::malloc ( 2*sizeof(int));
        int index = 0 ;
        //right from the beginning to the end of the set there might be unused value , to assign these value first
        TexNameSet::iterator it = pSharedTexState->usedTexNames.begin();
         while(it != pSharedTexState->usedTexNames.end()){
            //if the values are not equal it means atleast 1 value is missing
            if(*it == lastValue){
                it++;
                lastValue++;
                }
            else{  //storing the unused name and the range
               GLsizei noOfConsecUnusedValue = *it - lastValue;  //the no of unused name(not present in the set)
                 unusedValueRange[index][0] = lastValue;
                unusedValueRange[index][1] = noOfConsecUnusedValue;
                 index++;
                 unusedValueRange = (int(*)[2])Plat::realloc(unusedValueRange, 2*(index+1)*sizeof(int));
                 lastValue = lastValue + noOfConsecUnusedValue;        
 
          }
        }
         
        //inserting the unused name in the set
         for(GLsizei var = 0; var < index; var++)
        {
                for (GLsizei count = 0; count < unusedValueRange[var][1] ; count++)
                {
                       *textures++ = unusedValueRange[var][0];
                        toAssign--;
                        pSharedTexState->usedTexNames.insert(unusedValueRange[var][0]);
                        unusedValueRange[var][0]++;
                        if (toAssign == 0)
                      {
                            free(unusedValueRange);
                            return GL_TRUE;
                        }
                }
        }

         //some texture might be left to be assigned
         for( ; toAssign > 0 ; toAssign--){
            *textures++ = lastValue;    
            pSharedTexState->usedTexNames.insert(lastValue);
            lastValue++;
            }
            Plat::safe_free(unusedValueRange);
            return GL_TRUE;
}

#else

GLboolean getTextureNames( SharedTextureState* pSharedTexState ,GLsizei n, GLuint *textures)
{
    GLuint lastValue = 1;
    GLsizei toAssign  = n;
    
    TexNameSet::iterator it = pSharedTexState->usedTexNames.begin();
    
    //right from the beginning to the end of the set there might be unused value , to assign these value first
    while(it != pSharedTexState->usedTexNames.end()){
        //if the values are not equal it means atleast 1 value is missing
        if(*it == lastValue){
            lastValue ++ ;
            it++;
        }else{
            GLsizei noOfValue = *it - lastValue;  //the no of unused name(not present in the set)
            for( GLsizei count = 0; count < noOfValue  ; count++){
                *textures++ = lastValue;
                 toAssign --;
                 pSharedTexState->usedTexNames.insert(lastValue);
                 lastValue++;
                 if(toAssign == 0) return GL_TRUE;   
            }
        }
    }

    //some texture might be left to be assigned
    for( ; toAssign > 0 ; toAssign--){
        *textures++ = lastValue;    
        pSharedTexState->usedTexNames.insert(lastValue);
        lastValue++;
    }
    return GL_TRUE;

}
#endif

static
 void deleteTexImage(TextureObject* pTexObj )
{
    Image *pImgObj = NULL;
    switch(pTexObj->texType)
    {
        case GL_TEXTURE_2D:
            for(int i = 0; i < MAX_MIPMAP_LEVELS; i++)  
            {
                pImgObj = &(pTexObj->images.tex2D[i]);
                if(pImgObj->hImgChunk != NULL){
                    pCA->Free(pImgObj->hImgChunk);
                   pImgObj->hImgChunk = NULL;
                   pImgObj->imagedataLocation = NONE;
                }
                
                //Resetting the values of the image structure.
                pImgObj->imgSize = 0;
                pImgObj->isUsed = GL_FALSE;
                pImgObj->width = 0;
                pImgObj->height = 0;
                pImgObj->depth = 0;
                //not needed added just to be sure
                pImgObj->internalFormat = GLenum(-1);
                pImgObj->PixType = GLenum(-1);
                pImgObj->nativeFormat = E_INVALID_PIXEL_FORMAT;
                pImgObj->isCompressed = GL_FALSE;
            }
            break;
        case GL_TEXTURE_CUBE_MAP:
            for(int j = 0 ; j < 6; j++)
                for(int i = 0; i < MAX_MIPMAP_LEVELS; i++)
                {
                    pImgObj = &(pTexObj->images.cubeMap[j][i]);
                    if(pImgObj->hImgChunk != NULL){
                        pCA->Free(pImgObj->hImgChunk);
                        pImgObj->hImgChunk = NULL;
                        pImgObj->imagedataLocation = NONE;
                    }
                    //Resetting the values of the image structure.
                    pImgObj->imgSize = 0;
                    pImgObj->isUsed = GL_FALSE;
                    pImgObj->width = 0;
                    pImgObj->height = 0;
                    pImgObj->depth = 0;
                    //not needed added just to be sure
                    pImgObj->internalFormat = GLenum(-1);
                    pImgObj->PixType = GLenum(-1);
                    pImgObj->nativeFormat = E_INVALID_PIXEL_FORMAT;
                    pImgObj->isCompressed = GL_FALSE;
                
                }
            break;
        case GL_TEXTURE_3D:
            for(int i = 0; i < MAX_MIPMAP_LEVELS; i++)
            {
                pImgObj = &(pTexObj->images.tex3D[i]);
                if(pImgObj->hImgChunk != NULL){
                    pCA->Free(pImgObj->hImgChunk);
                    pImgObj->hImgChunk = NULL;
                    pImgObj->imagedataLocation = NONE;
                }
                //Resetting the values of the image structure.
                pImgObj->imgSize = 0;
                pImgObj->isUsed = GL_FALSE;
                pImgObj->width = 0;
                pImgObj->height = 0;
                pImgObj->depth = 0;
                //not needed added just to be sure
                pImgObj->internalFormat = GLenum(-1);
                pImgObj->PixType = GLenum(-1);
                pImgObj->nativeFormat = E_INVALID_PIXEL_FORMAT;
                pImgObj->isCompressed = GL_FALSE;
                
            }
            break;
    }

}



 inline 
void    transferImageFromTexChunckToImageChunck(TextureObject* pTexObj)
{

    Image *pImgObj = NULL;
    GLubyte* pImgBuf = NULL;
     
   if(pTexObj->texType == GL_TEXTURE_2D){
        int    bpp =0;
       if(pTexObj->images.tex2D[0].isCompressed == false)  bpp = pixelSize(pTexObj->nativeFormat);  //TO DO ONCE SURE HOW THE MIPMAPPED COMPRESSED IMAGE ARE DEALT 
        GLubyte* texObjImagedata = (GLubyte*) pTexObj->hChunk->GetVirtAddr();     
        for(int level = 0; level < pTexObj->levels; level++) {
            pImgObj = &(pTexObj->images.tex2D[level]);
            pImgObj->hImgChunk = pCA->New(pImgObj->imgSize);
            if(pImgObj->hImgChunk == NULL || (pImgBuf = (GLubyte*) pImgObj->hImgChunk->GetVirtAddr()) == NULL) 
            {
                        //LOGMSG("OUT OF MEMORY \n");
                return ;
            }
            Plat::memcpy( pImgBuf, texObjImagedata+ pTexObj->Offsets[level] * bpp, pImgObj->imgSize );
            pImgObj->imagedataLocation = IMAGECHUNK;
            pTexObj->Offsets[level] = 0;
        }
         pCA->Free(pTexObj->hChunk);
        pTexObj->hChunk = NULL;
        pTexObj->levels = 0;
        texObjImagedata = NULL;
    
   }else if(pTexObj->texType == GL_TEXTURE_3D){
         int bpp = pixelSize(pTexObj->nativeFormat); 
        GLubyte* texObjImagedata = (GLubyte*) pTexObj->hChunk->GetVirtAddr();
    for(int level = 0; level < pTexObj->levels; level++) {
        Image* pImageObj= &(pTexObj->images.tex3D[level]);
        pImageObj->hImgChunk = pCA->New( pImageObj->imgSize);
        if(pImageObj->hImgChunk == NULL || (pImgBuf = (GLubyte*) pImageObj->hImgChunk->GetVirtAddr()) == NULL) 
        {
                    //LOGMSG("OUT OF MEMORY \n");
            return ;
        }
        
        Plat::memcpy( pImgBuf, texObjImagedata+ pTexObj->Offsets[level] * bpp, pImgObj->imgSize );
        pTexObj->Offsets[level] = 0;
                 pImgObj->imagedataLocation = IMAGECHUNK;
               pImageObj = NULL;
    }
    pCA->Free(pTexObj->hChunk);
    pTexObj->hChunk = NULL;
        pTexObj->levels = 0;
        texObjImagedata = NULL;
        
   }else if(pTexObj->texType == GL_TEXTURE_CUBE_MAP){
        int faceOffset = pTexObj->Offsets[pTexObj->levels - 1] + 1 * 1 ;
          int bpp = pixelSize(pTexObj->nativeFormat); 
          GLubyte* texObjImagedata = (GLubyte*) pTexObj->hChunk->GetVirtAddr();
    for(int faceNo = 0; faceNo < 6; faceNo++){
        for(int level = 0; level < pTexObj->levels; level++){
            Image* pImageObj= &(pTexObj->images.cubeMap[faceNo][level]);
            pImageObj->hImgChunk = pCA->New(pImageObj->imgSize);
            if(pImageObj->hImgChunk == NULL || (pImgBuf = (GLubyte*) pImageObj->hImgChunk->GetVirtAddr()) == NULL) 
            {
                         //LOGMSG("OUT OF MEMORY \n");
                return ;
            }
            Plat::memcpy( pImgBuf, texObjImagedata + (faceOffset * faceNo + pTexObj->Offsets[level]) * bpp, pImageObj->imgSize );
            pTexObj->Offsets[level] = 0;
                        pImgObj->imagedataLocation = IMAGECHUNK;
                        pImageObj = NULL;
        }
    }
        pCA->Free(pTexObj->hChunk);
        pTexObj->hChunk = NULL;
                pTexObj->levels = 0;
                 texObjImagedata = NULL;     
    }

}

//called by glCopyTex* and glTex* function. If needed memory can be allocated
GLubyte* getImageDataLocation(TextureObject* pTexObj,GLint level , GLint faceNo, GLint width , GLint height ,int depth ,GLenum format, GLenum type, GLint imgSize , GLint isExt)
{

   Image *pImgObj = NULL;
   GLubyte* pImgBuf = NULL;
    
    int newDepth =1;
    if(depth != 0) newDepth =depth;     //for 3D

   //get the pointer to the image structure
   if(pTexObj->texType == GL_TEXTURE_2D){
            pImgObj = &(pTexObj->images.tex2D[level]);
   }else if(pTexObj->texType == GL_TEXTURE_3D){
               pImgObj = &(pTexObj->images.tex3D[level]);
   }else if(pTexObj->texType == GL_TEXTURE_CUBE_MAP){
                pImgObj = &(pTexObj->images.cubeMap[faceNo][level]);
   }else{
            gAssert(false && "undetermined texture format in function getImageDataLocation");
            return NULL;
   }

     //if previous was externally managed texture
    if(pTexObj->isExtTex == GL_TRUE){
         pTexObj->releaseMem(); 
         pTexObj->images.tex2D[0].imagedataLocation  = NONE;
    }

    //if external managed tex image: then delete all the previous mipmap level
   if(isExt == GL_TRUE){
        pTexObj->releaseMem();   
        deleteTexImage(pTexObj);
        return NULL;
   }

    //if the image data is NULL(i.e. not present in imagechunk or tex object chunck)
    if(pImgObj->imagedataLocation == NONE){
        pImgObj->hImgChunk = pCA->New(imgSize);
    if(pImgObj->hImgChunk == NULL || (pImgBuf = (GLubyte*) pImgObj->hImgChunk->GetVirtAddr()) == NULL) 
    {
               // SET_ERR(pState, GL_OUT_OF_MEMORY, "glTexImage2D");
        return NULL;
    }else{
            pImgObj->imagedataLocation = IMAGECHUNK;
                pTexObj->reCompile = GL_TRUE;
                return pImgBuf;     
        }
    }

    //if the image is present in the texture chunk .
    if(pImgObj->imagedataLocation == TEXOBJCHUNK){
        //if texture format, width , heigth , depth are same then we can use the texture chunk memory itself
        if((pImgObj->internalFormat == format) && (pImgObj->PixType == type) && 
            (pImgObj->width == width)  && (pImgObj->height == height) && (pImgObj->depth == depth)){
                if(pImgObj->isCompressed == false){ 
                
                    int bpp = GetPixelSize(format, type);           //of the format specified for the current gldrawCall
                    unsigned int faceOffset = (pTexObj->Offsets[pTexObj->levels - 1]) + 1 * 1;
                    return(((GLubyte*)( pTexObj->hChunk->GetVirtAddr())) +(faceNo*  faceOffset *  bpp) + (pTexObj->Offsets[level] * bpp)); 
                }else{
                            //TO DO ONCE SURE HOW THE MIPMAPPED COMPRESSED IMAGE ARE DEALT 
                          return(GLubyte*)( pTexObj->hChunk->GetVirtAddr()) ;        
                }
        }else{
            //spill back the texture image from texture chunk to the image chunk
            transferImageFromTexChunckToImageChunck(pTexObj);
                        
        }
    }
    
      //if the image is present in the image chunk .This can even happen when the image has been transfered from texture chunk to image chunck ( above)
        if(pImgObj->imagedataLocation == IMAGECHUNK){
             //if image size are same we can use the same image chunk memory location //TO DO WRONG
            if( pImgObj->imgSize  == imgSize){
                        pTexObj->reCompile = GL_TRUE;
                        return ((GLubyte* )pImgObj->hImgChunk->GetVirtAddr());
            }else{
                //free the previous image data and then allocate the new one 
                  pCA->Free(pImgObj->hImgChunk);
                   pImgObj->hImgChunk = NULL;  
                   pImgObj->hImgChunk = pCA->New(imgSize);
            if(pImgObj->hImgChunk == NULL || (pImgBuf = (GLubyte*) pImgObj->hImgChunk->GetVirtAddr()) == NULL)  {
                //LOGMSG("OUT OF MEMORY \n");
                return NULL;
            }else{
                 pTexObj->reCompile = GL_TRUE;
                    return pImgBuf;

                }
         
            }

      }  

  //LOGMSG(" should not have reached the end of the function :getImageDataLocation \n") ;
  return NULL;
        
}

//called by  glTexSub* glCopyTexSub* function.As the memry has to be already allocated need to return the pointer
GLubyte* getImageDataLocation(TextureObject* pTexObj, Image *pImgObj , GLint level , GLint faceNo)
{
   
    if(pImgObj->imagedataLocation == NONE){
        gAssert(false && "subimage called but memory not allocated : an errror \n");
    }else if(pImgObj->imagedataLocation == TEXOBJCHUNK){
        if(pImgObj->isCompressed == false){ 
                 int bpp = pixelSize(pTexObj->nativeFormat );          
                unsigned int faceOffset = (pTexObj->Offsets[pTexObj->levels - 1]) + 1 * 1;
                return(((GLubyte*)( pTexObj->hChunk->GetVirtAddr())) +(faceNo*  faceOffset *  bpp) + (pTexObj->Offsets[level] * bpp)); 
        }else{
                       //TO DO ONCE SURE HOW THE MIPMAPPED COMPRESSED IMAGE ARE DEALT 
                      return(GLubyte*)( pTexObj->hChunk->GetVirtAddr()) ;        
        }    
    }else if(pImgObj->imagedataLocation == IMAGECHUNK){
        return ((GLubyte* )pImgObj->hImgChunk->GetVirtAddr());
    }
    return NULL;
}


GLboolean isMipmapFilter(GLenum minFilter)
{
    if((minFilter == GL_LINEAR) || (minFilter == GL_NEAREST)){
            return GL_FALSE;
    }else if((minFilter == GL_NEAREST_MIPMAP_NEAREST) ||(minFilter == GL_LINEAR_MIPMAP_NEAREST)
                 || (minFilter == GL_NEAREST_MIPMAP_LINEAR ) || (minFilter == GL_LINEAR_MIPMAP_LINEAR)){
            return GL_TRUE;

}else{
        gAssert(false && "undetermined mimfilter in function  isMipmapFilter \n");
         return GL_TRUE;
  }

}

/*
*******************************************************************************
* OpenGL API functions
*******************************************************************************
*/

/*-----------------------------------------------------------------------*//*!
* OpenGL active texture function.
*
* This function sets the active texture unit in texture state. The steps
* performed are:
* - Sanity checks and Set error.
* - Store the index to active texture unit.
*
*//*------------------------------------------------------------------------*/

GL_API void GL_APIENTRY
glActiveTexture (GLenum texture)
{
    OGLState* pState = GET_OGL_STATE();
    TextureState* pTexState = &(pState->texState);

    // Sanity checks and error reporting
    if(texture < GL_TEXTURE0 || texture >= (GL_TEXTURE0 + MAX_TEXTURE_UNITS)){
        SET_ERR(pState, GL_INVALID_OPERATION, "glActiveTexture");
        return;
    }

    // Store the index to texture unit
    pTexState->activeTexUnit = texture - GL_TEXTURE0;
}


/*-----------------------------------------------------------------------*//*!
* OpenGL generate textures function.
*
* This function finds the free texture names and sets it in the pointer passed.
* The steps performed are:
* - Search the object array for free texture names and store as required.
* - Record error if we don't find enough free texture names.
*
*//*------------------------------------------------------------------------*/

GL_API void GL_APIENTRY
glGenTextures (GLsizei n, GLuint *textures)
{
    OGLState* pState = GET_OGL_STATE();
    SharedTextureState* psharedTexState = &(pState->sharedState->sharedTexState);
    GLuint *texID = textures;

    // Validate parameters
    if(n < 0 || textures == NULL){
        SET_ERR(pState, GL_INVALID_VALUE, "glGenTextures");
        return;
    }

    // Search in the object array for free textures
    // Texture 0 is default texture. Skip it.

#if 1
    lockGLSharedTextureState(pState);

    if(getTextureNames(psharedTexState,n,textures) != GL_TRUE)
    {
        //this case is not expected to be reached
        SET_ERR(pState, GL_OUT_OF_MEMORY, "glGenTextures"); 
        
    }
 
    unlockGLSharedTextureState( pState);


#else
    for(i = 1; i < MAX_TEXTURE_OBJECTS; i++) {
        if((pTexState->texObjects[i]).isUsed == GL_FALSE) {
            *(textures + j) = i;
            j++;

            if(j >= n) return;     // We got all we wanted
        }
    }

    // We couldn't find the required no of free textures
    SET_ERR(pState, GL_OUT_OF_MEMORY, "glGenTextures");

#endif
    while(n--)
    {
        while(psharedTexState->texObjects.count(*texID) > 0)
            psharedTexState->texObjects.erase(*texID);
        texID++;
    }

    return;
}


/*-----------------------------------------------------------------------*//*!
* OpenGL bind texture function.
*
* This function creates or re-use texture objects and binds them to current
* texture. The steps followed are:
* - Validate input parameters.
* - Check if texture object referenced is already in use. Create new object
*   if required.
* - Check type if object already exists and raise error on mismatch. If the
*   texture referenced is default texture then type change will re-initialize
*   the default texture.
*
*//*------------------------------------------------------------------------*/

GL_API void GL_APIENTRY
glBindTexture (GLenum target, GLuint texture)
{
    OGLState* pState = GET_OGL_STATE();
    TextureState* pTexState = &(pState->texState);
    SharedTextureState* pSharedTexState = &(pState->sharedState->sharedTexState);

    GLint texRef;
    GLuint currentUnit = pTexState->activeTexUnit;//sanvd added this
    TextureObject* lastTexObj = NULL;
    TextureObject *pTexObj;
    CurrentTexture *currTex = &pTexState->texUnitBinding[currentUnit];
                                        
    // -----------------------------------------------------------------------
    // Validate input parameters
    // -----------------------------------------------------------------------

    if(!(target == GL_TEXTURE_2D || target == GL_TEXTURE_3D || target == GL_TEXTURE_CUBE_MAP)){
        SET_ERR(pState, GL_INVALID_ENUM, "glBindTexture");
        return;
    }

    // -----------------------------------------------------------------------
    // Handle texture 0 special case
    // -----------------------------------------------------------------------
    
    lockGLSharedTextureState(pState);

    if(texture == 0) {
        // Texture zero has 3 different objects associated with it. One each
        //  for different targets.
        switch(target) {

        case GL_TEXTURE_2D:
            //texRef = TEX_2D_DEFAULT;
            if(currTex->texture2D != 0){
                lastTexObj = GetTextureObject( target, false ,  0);
                lastTexObj->release(pState);
                lastTexObj = NULL;  
            }
            currTex->texture2D = texture;
#if STORE_TEX_OBJ_POINTER ==    ENABLE
            pTexState->ptexUnitBinding[currentUnit].texture2D = &(pTexState->defaultTexObjects[TEX_2D_DEFAULT -TEX_2D_DEFAULT]);
#endif
            break;

        case GL_TEXTURE_3D:
            //texRef = TEX_3D_DEFAULT;
            if(currTex->texture3D != 0){
                lastTexObj = GetTextureObject( target, false ,  0);
                lastTexObj->release(pState);
                lastTexObj = NULL;  
            }
            currTex->texture3D = texture;
#if STORE_TEX_OBJ_POINTER ==    ENABLE
            pTexState->ptexUnitBinding[currentUnit].texture3D = &(pTexState->defaultTexObjects[TEX_3D_DEFAULT -TEX_2D_DEFAULT]);
#endif
            break;

        case GL_TEXTURE_CUBE_MAP:
            //texRef = TEX_CUBE_MAP_DEFAULT;
                        if(currTex->cubeMap != 0){
                lastTexObj = GetTextureObject( target, false ,  0);
                lastTexObj->release(pState);
                lastTexObj = NULL;  
            }
            currTex->cubeMap = texture;
#if STORE_TEX_OBJ_POINTER ==    ENABLE
            pTexState->ptexUnitBinding[currentUnit].cubeMap = &(pTexState->defaultTexObjects[TEX_CUBE_MAP_DEFAULT -TEX_2D_DEFAULT]);
#endif
            break;
        }
    //default textures  are not shared
 
    // -----------------------------------------------------------------------
    // Handle other textures
    // -----------------------------------------------------------------------

    }
    else
    {
        texRef = GetTexNameArrayIndex(texture,false);
                
        if(texRef ==-1){
            SET_ERR(pState, GL_INVALID_VALUE, "glBindTexture");
            unlockGLSharedTextureState(pState);
            return;
        }
        
        // New texture object 
        pTexObj = pSharedTexState->texObjects[texRef];
        if(pTexObj != NULL)
        {
            if(pTexObj->isUsed != GL_TRUE) {
                // Initialize, mark used
                pTexObj->Init(texture, target);
                //reaching inside the loop means that this is the first time the texture has been called by glBindTexture
                // so it might have been called using the glGenTexture or without it 
                //this if condition can be removed as set contain only 1 copy of the value      
                if(pSharedTexState->usedTexNames.count(texture) == 0){
                    pSharedTexState->usedTexNames.insert(texture);      
                }

            // Texture object reuse

            } else if(pTexObj->texType != target){
                // Raise error if type mismatches
                SET_ERR(pState, GL_INVALID_OPERATION, "glBindTexture");
                unlockGLSharedTextureState(pState);
                return;
            }
            // for reference count of the texture objects
            // & Store current texture value
            switch(target) {
                case GL_TEXTURE_2D:     
                    if(currTex->texture2D != texture)
                    {   
                        if(currTex->texture2D != 0){    
                            lastTexObj = GetTextureObject( target, false ,  0);
       
                                 lastTexObj->release(pState);
                            lastTexObj = NULL;
                        }
                
                        pTexObj->acquire(pState);
                        currTex->texture2D = texture;
#if STORE_TEX_OBJ_POINTER ==    ENABLE
                        pTexState->ptexUnitBinding[currentUnit].texture2D = (pSharedTexState->texObjects[texRef]);
#endif
                    }
                    break;

                case GL_TEXTURE_3D:

                    if(currTex->texture3D != texture){
                        if(currTex->texture3D != 0){
                            lastTexObj = GetTextureObject( target, false ,  0);
                            lastTexObj->release(pState);
                            lastTexObj = NULL;
                        }
                        pTexObj->acquire(pState);
                        currTex->texture3D = texture;
#if STORE_TEX_OBJ_POINTER ==    ENABLE
                        pTexState->ptexUnitBinding[currentUnit].texture3D = (pSharedTexState->texObjects[texRef]);
#endif
                    }
                    
                    break;

                case GL_TEXTURE_CUBE_MAP:
                    if(currTex->cubeMap  != texture){
                        if(currTex->cubeMap != 0){
                            lastTexObj = GetTextureObject( target, false ,  0);
                            lastTexObj->release(pState);
                            lastTexObj = NULL;
                        }
                        pTexObj->acquire(pState);
                        currTex->cubeMap = texture;
#if STORE_TEX_OBJ_POINTER ==    ENABLE
                        pTexState->ptexUnitBinding[currentUnit].cubeMap = (pSharedTexState->texObjects[texRef]);
#endif
                    }
                    
                    break;
            };
        }
            
    }
    unlockGLSharedTextureState(pState); 
 
 
#if 0
    // -----------------------------------------------------------------------
    // Store current texture value
    // -----------------------------------------------------------------------

    switch(target) {

    case GL_TEXTURE_2D:
        pTexState->texUnitBinding[currentUnit].texture2D = texture;
        break;

    case GL_TEXTURE_3D:
        pTexState->texUnitBinding[currentUnit].texture3D = texture;
        break;

    case GL_TEXTURE_CUBE_MAP:
        pTexState->texUnitBinding[currentUnit].cubeMap = texture;
        break;
    };


#endif

}


/*-----------------------------------------------------------------------*//*!
* OpenGL delete texture function.
*//*------------------------------------------------------------------------*/

GL_API void GL_APIENTRY
glDeleteTextures (GLsizei n, const GLuint *textures)
{

    OGLState* pState = GET_OGL_STATE();
    TextureState* pTexState = &(pState->texState);
    GLuint currentUnit = pTexState->activeTexUnit;
    int i = 0 ;                          //for count of the no of texture unit


    // Validate parameters
    if(n < 0 || textures == NULL){
        SET_ERR(pState, GL_INVALID_VALUE, "glDeleteTextures");
        return;
    }
  
    lockGLSharedTextureState(pState);

    while(n--){

        if(*textures == 0){
            continue;       //silently ignore 0
        }

        
        
        GLint texRef = GetTexNameArrayIndex(*textures,false);//only delete userdefined tex objects..operation is heavy.....O(  n )

        SharedTextureState *pSTState = &(pState->sharedState->sharedTexState);
        TextureObject* pTexObj = pSTState->texObjects[texRef];
        if((texRef == -1) || (pTexObj->isUsed == GL_FALSE))//it means that texture is not found in the list and that textureObject array  is fulluy populated...
        //if((texRef == -1) || (pState->sharedState->sharedTexState.texObjects[texRef]->isUsed == GL_FALSE))//it means that texture is not found in the list and that textureObject array  is fulluy populated...
        {
            //Sandeep K. spec says silently ignore 0 and invalid texture names
                  //SET_ERR(pState, GL_INVALID_VALUE, "glDeleteTexture");
//            unlockGLSharedTextureState(pState);
            //return;
            continue; //it should try tio delete the other texture object
        }
        //check the reference count before deleting
        //the reference count need to be decreased for all the texture unit that is using the f=given texture object
        pTexObj->deleteTexObj = GL_TRUE;
            switch(pTexObj->texType){

            case GL_TEXTURE_2D:
                for( i =0 ; i < MAX_TEXTURE_UNITS ; i++){
                    if ((pTexObj->id == pTexState->texUnitBinding[i].texture2D)
                                && (pTexObj->id !=0)){
                        pTexState->texUnitBinding[i].texture2D = 0; 
#if STORE_TEX_OBJ_POINTER == ENABLE
                         pTexState->ptexUnitBinding[currentUnit].texture2D =
                             &(pTexState->defaultTexObjects[TEX_2D_DEFAULT
                                                            -TEX_2D_DEFAULT]);
#endif
                         int id = pTexObj->id;
                        pSTState->ReleaseTexObj(pState, pTexObj->id) ;
                    }
                    
                }
                //if the object is not bounded to any texture object 
                if( pTexObj->id != 0 && pSTState->texObjects.count(texRef)){
                     int id = pTexObj->id;
                     pSTState->ReleaseTexObj(pState, pTexObj->id);  
                 }
                 pTexObj = NULL; 
                 //pSTState->texObjects[texRef] = NULL;
                 break;
                 
             case GL_TEXTURE_3D:
                 for( i =0 ; i < MAX_TEXTURE_UNITS ; i++){
                     if ((pTexObj->id == pTexState->texUnitBinding[i].texture3D)
                                 && (pTexObj->id !=0)){
                         pTexState->texUnitBinding[i].texture3D = 0;
 #if STORE_TEX_OBJ_POINTER ==    ENABLE
                         pTexState->ptexUnitBinding[currentUnit].texture3D =
                             &(pTexState->defaultTexObjects[TEX_3D_DEFAULT
                                                         -TEX_2D_DEFAULT]);
 #endif
                         pSTState->ReleaseTexObj(pState, pTexObj->id);                                 
                     }
                 }
                 if( pTexObj->id != 0 && pSTState->texObjects.count(texRef)){
                     pSTState->ReleaseTexObj(pState, pTexObj->id);   
                 }
                 pTexObj = NULL;
                 //pSTState->texObjects[texRef] = NULL;
                 break;
                 
             case GL_TEXTURE_CUBE_MAP:
                 for( i =0 ; i < MAX_TEXTURE_UNITS ; i++){
                     if( (pTexObj->id == pTexState->texUnitBinding[i].cubeMap)
                                 && (pTexObj->id !=0)){
                         pTexState->texUnitBinding[i].cubeMap= 0;
 #if STORE_TEX_OBJ_POINTER ==    ENABLE
                         pTexState->ptexUnitBinding[currentUnit].cubeMap =
                             &(pTexState->defaultTexObjects[TEX_CUBE_MAP_DEFAULT
                                                         -TEX_2D_DEFAULT]);
 #endif
                        pSTState->ReleaseTexObj(pState, pTexObj->id);                
                     }
                 }
                 if( pTexObj->id != 0 && pSTState->texObjects.count(texRef)){
                     pSTState->ReleaseTexObj(pState, pTexObj->id);   
                 }
                 pTexObj = NULL;
                 //pSTState->texObjects[texRef] = NULL;
                 break;
             }
         
 
 #if 0
     
             //need to unbound from all texture unit  
             switch(pTexState->psharedTexObjects->ptexObjects[texRef].texType ){
             case GL_TEXTURE_2D:
                 for( i =0 ; i < MAX_TEXTURE_UNITS ; i++){
                     if (*textures ==   pTexState->texUnitBinding[i].texture2D){
                         pTexState->texUnitBinding[i].texture2D = 0; 
                     }
                 }
                 break;
             case GL_TEXTURE_3D:
                 for( i =0 ; i < MAX_TEXTURE_UNITS ; i++){
                     if (*textures ==   pTexState->texUnitBinding[i].texture3D){
                         pTexState->texUnitBinding[i].texture3D = 0; 
                     }
                 }
                 break;
             case GL_TEXTURE_CUBE_MAP:
                 for( i =0 ; i < MAX_TEXTURE_UNITS ; i++){
                     if (*textures ==   pTexState->texUnitBinding[i].cubeMap){
                         pTexState->texUnitBinding[i].cubeMap= 0;    
                     }
                 }
                 break;
             }
 
 
 
 
         if(*textures == pTexState->texUnitBinding[currentUnit].texture2D)
             pTexState->texUnitBinding[currentUnit].texture2D = 0;
         if(*textures == pTexState->texUnitBinding[currentUnit].texture3D)
             pTexState->texUnitBinding[currentUnit].texture3D = 0;
         if(*textures == pTexState->texUnitBinding[currentUnit].cubeMap)
             pTexState->texUnitBinding[currentUnit].cubeMap= 0;
         
     
         // Delete texture object
         if(*textures != 0 && pTexState->psharedTexObjects->ptexObjects[texRef].isUsed == GL_TRUE){
             pTexState->psharedTexObjects->ptexObjects[texRef].Delete();
         }
 
         //to remove form the used texture name set
         if(pTexState->psharedTexObjects->usedTexNames.count(*textures) != 0){
             pTexState->psharedTexObjects->usedTexNames.erase(*textures);
         }
 #endif      
 
 
         //Sandeep K.
         //FBO spec says that is a texture is bound to the currently bound fbo it must be detached from all attachment points
         // it is attached to.
         pState->framebuffState.detachTexture(*textures);
 
         textures++;     // Next object
     
     }
     unlockGLSharedTextureState(pState); 
 
 }
 
 
 /*-----------------------------------------------------------------------*//*!
 * OpenGL IsTexture function.
 *//*------------------------------------------------------------------------*/
 
 GL_API GLboolean GL_APIENTRY
 glIsTexture (GLuint texture)
 {
     OGLState* pState = GET_OGL_STATE();
 
 
     // Validate parameters
     if(texture >= MAX_TEXTURE_OBJECTS){
         SET_ERR(pState, GL_INVALID_VALUE, "glIsTexture");
         return GL_FALSE;
     }
     
     GLint texRef = GetTexNameArrayIndex(texture,false);//only checks the non-default tex objs//operation is heavy.....O(  n ) 
     if(texRef == -1) //it means that texture is not found in the list and that textureObject array  is fulluy populated...
         return GL_FALSE;
         
     if(pState->sharedState->sharedTexState.texObjects[texRef]->isUsed == GL_TRUE)
         return GL_TRUE;
     else
         return GL_FALSE;
 }
 
 /*-----------------------------------------------------------------------*//*!
 * OpenGL texture parameter function (int).
 *//*------------------------------------------------------------------------*/
 
 GL_API void GL_APIENTRY
 glTexParameteri (GLenum target, GLenum pname, GLint param)
 {
     OGLState* pState = GET_OGL_STATE();
 
     TextureObject* pTexObj = NULL;
     //GLuint texRef;
 
     //Sandeep
     GLenum eParam = (GLenum)(param);
 
     // Select object reference according to target
     // Return error on invalid enum
 
     if(target == GL_TEXTURE_2D || target == GL_TEXTURE_3D || target == GL_TEXTURE_CUBE_MAP){
         pTexObj = GetTextureObject(target,false,0);
         
 
     } else {
         SET_ERR(pState, GL_INVALID_ENUM, "glTexParameter*");
         return;
     }
 
         GLboolean isMipmapFilterPrevious  = isMipmapFilter(pTexObj->minFilter);
 
     // Store the parameter passed in texture state
     // Return error on invalid enum or value
 
     switch(pname) {
 
     case GL_TEXTURE_MIN_FILTER:
         if((eParam == GL_LINEAR) || (eParam == GL_NEAREST) || (eParam == GL_LINEAR_MIPMAP_LINEAR)
         || (eParam == GL_LINEAR_MIPMAP_NEAREST) || (eParam == GL_NEAREST_MIPMAP_LINEAR) ||
         (eParam == GL_NEAREST_MIPMAP_NEAREST)) {
             pTexObj->minFilter = eParam;
             break;
 
         } else {
             SET_ERR(pState, GL_INVALID_VALUE, "glTexParameter*");
             return;
         }
 
     case GL_TEXTURE_MAG_FILTER:
         if((eParam == GL_LINEAR) || (eParam == GL_NEAREST)){
             pTexObj->magFilter = eParam;
             break;
 
         } else {
             SET_ERR(pState, GL_INVALID_VALUE, "glTexParameter*");
             return;
         }
 
     case GL_TEXTURE_WRAP_S:
         if((eParam == GL_REPEAT) || (eParam == GL_CLAMP_TO_EDGE) || (eParam == GL_MIRRORED_REPEAT)) {
             pTexObj->wrapS = eParam;
             break;
 
         } else {
             SET_ERR(pState, GL_INVALID_VALUE, "glTexParameter*");
             return;
         }
 
     case GL_TEXTURE_WRAP_T:
         if((eParam == GL_REPEAT) || (eParam == GL_CLAMP_TO_EDGE) || (eParam == GL_MIRRORED_REPEAT)) {
             pTexObj->wrapT = eParam;
             break;
 
         } else {
             SET_ERR(pState, GL_INVALID_VALUE, "glTexParameter*");
             return;
         }
 
 
     case GL_TEXTURE_WRAP_R:
         if(target != GL_TEXTURE_3D) {
             SET_ERR(pState, GL_INVALID_ENUM, "glTexParameter*");
             return;
         }
 
         if((eParam == GL_REPEAT) || (eParam == GL_CLAMP_TO_EDGE) || (eParam == GL_MIRRORED_REPEAT)) {
             pTexObj->wrapR = eParam;
             break;
 
         } else {
             SET_ERR(pState, GL_INVALID_VALUE, "glTexParameter*");
             return;
         }
 
     default:
         SET_ERR(pState, GL_INVALID_ENUM, "glTexParameter*");
         return;
     };
 
         GLboolean isMipmapFilterPresent = isMipmapFilter(pTexObj->minFilter);
  
         if(isMipmapFilterPresent != isMipmapFilterPrevious){     
             pTexObj->reCompile = GL_TRUE; 
 
         }
     pTexObj->isDirtyState = GL_TRUE;
 }
 
 
 /*-----------------------------------------------------------------------*//*!
 * OpenGL texture parameter function (intv).
 *
 * See glTexParameteri() for details regarding glTexParameter* functions.
 *
 *//*------------------------------------------------------------------------*/
 
 GL_API void GL_APIENTRY
 glTexParameteriv (GLenum target, GLenum pname, const GLint *params)
 {
     OGLState* pState = GET_OGL_STATE();
 
     if(params == NULL) {
         SET_ERR(pState, GL_INVALID_VALUE, "glTexParameter*");
 
     } else {
 //Texture OES API - 2009.05.20    
 //#ifndef GL_OES_draw_texture
 //        glTexParameteri(EXPCTX_LOCAL target, pname, *params);
 //#else
         if ((target == GL_TEXTURE_2D) && (pname == GL_TEXTURE_CROP_RECT_OES)){
             TextureObject* pTexObj = NULL;
             pTexObj = GetTextureObject(target,false,0);
 
            // Plat::printf ("\nGL_TEXTURE_CROP_RECT_OES is called.\n");
 
             pTexObj->crop_rect[0] = params[0];
             pTexObj->crop_rect[1] = params[1];
             pTexObj->crop_rect[2] = params[2];
             pTexObj->crop_rect[3] = params[3];
         }else{
         glTexParameteri( target, pname, *params);
         }
 //#endif //GL_OES_draw_texture
     }
 }
 
 
 /*-----------------------------------------------------------------------*//*!
 * OpenGL texture parameter function (float).
 *
 * See glTexParameteri() for details regarding glTexParameter* functions.
 *
 *//*------------------------------------------------------------------------*/
 
 GL_API void GL_APIENTRY
 glTexParameterf (GLenum target, GLenum pname, GLfloat param)
 {
     glTexParameteri(target, pname, (GLint)param);
 }
 
 
 /*-----------------------------------------------------------------------*//*!
 * OpenGL texture parameter function (floatv).
 *
 * See glTexParameteri() for details regarding glTexParameter* functions.
 *
 *//*------------------------------------------------------------------------*/
 
 GL_API void GL_APIENTRY
 glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
 {
     OGLState* pState = GET_OGL_STATE();
 
     if(params == NULL) {
         SET_ERR(pState, GL_INVALID_VALUE, "glTexParameter*");
 
     } else {
 //Texture OES API - 2009.05.20    
 //#ifndef GL_OES_draw_texture
 //        glTexParameteri(EXPCTX_LOCAL target, pname, *params);
 //#else
         if ((target == GL_TEXTURE_2D) && (pname == GL_TEXTURE_CROP_RECT_OES)){
             TextureObject* pTexObj = NULL;
             pTexObj = GetTextureObject(target,false,0);
 
            // Plat::printf ("\nGL_TEXTURE_CROP_RECT_OES is called.\n");
 
             pTexObj->crop_rect[0] = params[0];
             pTexObj->crop_rect[1] = params[1];
             pTexObj->crop_rect[2] = params[2];
             pTexObj->crop_rect[3] = params[3];
         }else{
         glTexParameteri( target, pname, *params);
         }
 //#endif //GL_OES_draw_texture
     }
 }
 
 
 
 /* the function will determine the TextureFormat depeneding on the value of 
    internalformat & type.this value will be used internal by the library 
    the user TextureFormat is given  by the srcTextFormat & 
    the final format in which the texture data will be used is returned */
 PxFmt DetermineTextureFormat(GLenum pFormat, GLenum pType, PxFmt* srcTextFormat)
 {
     switch(pType)   
     {           
         case GL_UNSIGNED_BYTE:      
         {           
             switch(pFormat)     
                 {           
                 case GL_LUMINANCE:  
                     *srcTextFormat = E_LUMINANCE8;
                     return E_LUMINANCE_ALPHA80;
                                 
                 case GL_ALPHA:
                     *srcTextFormat = E_ALPHA8;
                     return E_LUMINANCE_ALPHA08;     
                         
                 case GL_LUMINANCE_ALPHA:    
                     *srcTextFormat = E_LUMINANCE_ALPHA88;
                     return E_LUMINANCE_ALPHA88; //TE_AI88;  
                             
                 case GL_RGB:
                     *srcTextFormat = E_BGR8;
                     return E_ARGB0888 ; //E_ARGB8;  //TODO: or should this be E_ARGB0888
 
                 case GL_RGBA:
                     *srcTextFormat = E_ABGR8;
                     return E_ARGB8 ; //TE_ARGB8888; 
                 case GL_BGRA:
                     *srcTextFormat = E_ARGB8;
                     return E_ARGB8 ; //TE_ARGB8888; 
                 }           
         }           
         break;  
         
     case GL_UNSIGNED_SHORT_4_4_4_4: 
         *srcTextFormat = E_RGBA4;
         return E_ARGB4; // changed as alpha location value is 0         
                                 
     case GL_UNSIGNED_SHORT_5_6_5:   
         *srcTextFormat = E_RGB565;
         return E_RGB565; //TE_RGB565;           
                             
     case GL_UNSIGNED_SHORT_5_5_5_1:     
         *srcTextFormat = E_RGBA5551;
         return E_ARGB1555; // changed as alpha location value is 0  
         
     case GL_PALETTE4_RGB8_OES:
     case GL_PALETTE8_RGB8_OES:
         *srcTextFormat = E_BGR8;    
         return E_ARGB8;
                     
     case GL_PALETTE4_RGBA8_OES:
     case GL_PALETTE8_RGBA8_OES:
         *srcTextFormat = E_ABGR8;   
         return E_ARGB8; 
         
     case GL_PALETTE4_RGBA4_OES:
     case GL_PALETTE8_RGBA4_OES:
         *srcTextFormat = E_RGBA4;
         return E_ARGB8;
             
     case GL_PALETTE4_R5_G6_B5_OES:  
     case GL_PALETTE8_R5_G6_B5_OES:  
         *srcTextFormat = E_RGB565;
         return E_ARGB8;     
             
     case GL_PALETTE8_RGB5_A1_OES:
     case GL_PALETTE4_RGB5_A1_OES:
         *srcTextFormat = E_RGBA5551;
         return E_ARGB8;     
         
     default: 
         return E_INVALID_PIXEL_FORMAT;
     }
     return E_INVALID_PIXEL_FORMAT; // added to remove warning
     
 }//DetermineTextureFormat() 
 
 
 /*cheks whether the current texture is alpha texture , Luminance texture or RGB texture. Only support for GLES1.1. implemented for 
 GL_TEXTURE_2D only.*/
  extern "C" GLboolean  isAlphaorLuminanceRGBTextureEXP (GLenum texture, GLboolean alpha)
 {
     OGLState* pState = GET_OGL_STATE();
     TextureObject* pTexObj =NULL;
     Image *pImgObj = NULL;
     GLint texUnit = 0;
     
     if(texture < GL_TEXTURE0 || texture >= (GL_TEXTURE0 + MAX_TEXTURE_UNITS)){
         SET_ERR(pState, GL_INVALID_OPERATION, "glIsAlphaTextureEXP");
         return false;
     }
     texUnit = texture - GL_TEXTURE0;
     pTexObj = GetTextureObject(GL_TEXTURE_2D,true,texUnit);
     pImgObj = &(pTexObj->images.tex2D[0]); // for level = 0
 
    if (alpha == 1){
             if (pImgObj->internalFormat != GL_ALPHA){
                 return false;
             }else{
                // Plat::printf("\n %s ::Current bound texture is alpha texture \n", __FUNCTION__);
                 return true;
         }
    }
    else{
             if (pImgObj->internalFormat == GL_LUMINANCE || pImgObj->internalFormat == GL_RGB){
                  return true;
                 }else{
                         return false;
                 }
 
         }
 }
 
 
 void convertPixelsTexure( PxFmt dstFmt, void* dstPixels, GLsizei width, GLsizei height,
                                         PxFmt srcFmt,const void* srcPixels); 
 
 
         
 /*-----------------------------------------------------------------------*//*!
 * OpenGL specify 2D texture function.
 *//*------------------------------------------------------------------------*/
 
 #ifdef USE_3D_PM
  void PM_glTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width,
                      GLsizei height, GLint border, GLenum format, GLenum type,
                      const void *pixels);
 
 
 GL_API void GL_APIENTRY
 glTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width,
                     GLsizei height, GLint border, GLenum format, GLenum type,
                     const void *pixels)
     {
         lock3DCriticalSection();
         PM_glTexImage2D ( target,  level,  internalformat,  width, height,  border,  format,  type, pixels);
         unlock3DCriticalSection();
     }
     
 void PM_glTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width,
                     GLsizei height, GLint border, GLenum format, GLenum type,
                     const void *pixels)
 #else
 GL_API void GL_APIENTRY
 glTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width,
                 GLsizei height, GLint border, GLenum format, GLenum type,
                 const void *pixels)
 #endif
 {
     OGLState* pState = GET_OGL_STATE();
     //TextureState* pTexState = &(pState->texState);
 
     //GLuint texRef;                // Reference to texture object
     GLenum texFormatError;          // Internal texture format
     GLsizei pixSize;            // Bytes per pixel
     GLubyte *pImgBuf = NULL;    // Pointer to image buffer
     Image *pImgObj = NULL;      // Pointer to image object
     GLuint cubeFace = 0;        // Index of cube face refered
     PxFmt DstTextureFormat ;   /* the texture format in which the data will be send */
     PxFmt srcTextureFormat ;   /* source texture format */
     //GLenum tmpPixelSize ;       /*temporary to check if value is same or not*/
     //GLubyte *pTempImgBuf = NULL;
     //GLuint isImagedata = 1;
     TextureObject* pTexObj =NULL;
 
     //-------------------------------------------------------------------------
     // Get texture object and VALIDATE INPUT PARAMETERS
     //-------------------------------------------------------------------------
 
     // Check MipMap level
     if(level < 0 || level >= MAX_MIPMAP_LEVELS) {
         SET_ERR(pState, GL_INVALID_VALUE, "glTexImage2D");
         return;
     };
     
     // Select object reference according to target
     // Return error on invalid enum
 
     if(target == GL_TEXTURE_2D){
         pTexObj = GetTextureObject(GL_TEXTURE_2D,false,0);
         pImgObj = &(pTexObj->images.tex2D[level]);
         
     } else if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) {
         pTexObj = GetTextureObject( GL_TEXTURE_CUBE_MAP,false,0);
         cubeFace = (target - GL_TEXTURE_CUBE_MAP_POSITIVE_X);
         pImgObj = &(pTexObj->images.cubeMap[cubeFace][level]);
     
     } else {
         SET_ERR(pState, GL_INVALID_ENUM, "glTexImage2D*");
         return;
     }
 
     
     // Check width and height should be less than zero or greater than GL_MAX_TEXTURE_SIZE????
     if(width > (1 << (MAX_MIPMAP_LEVELS-1)) || height > (1 << (MAX_MIPMAP_LEVELS-1))
             || width < 0  || height < 0 ) {
         SET_ERR(pState, GL_INVALID_VALUE, "glTexImage2D");
         return;
     }
 
         //in case of cubemap width and height should be same
         if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z){
             if(width != height)  {
                 SET_ERR(pState, GL_INVALID_VALUE, "glTexImage2D");
         return;
             }
         }
 
         // Border not supported in ES
     if(border != 0) {
         SET_ERR(pState, GL_INVALID_VALUE, "glTexImage2D");
         return;
     }
 
     
     //check the texture format  and internal format ?????TO DO
     texFormatError = checkTextureFormat(pState, internalformat, format);
     if(texFormatError != GL_NO_ERROR) {
         SET_ERR(pState, texFormatError, "glTexImage2D");
         return;
     }
 
     
     // check the type and format combination supported 
     if(CheckFormatTypeCombination(internalformat,type) == 1){
         SET_ERR(pState, GL_INVALID_OPERATION, "glTexImage2D");
         return;
     }
     
     DstTextureFormat = DetermineTextureFormat(internalformat, type, &srcTextureFormat);
     pImgObj->nativeFormat = DstTextureFormat;
 
         //TO DO WHEN WITDTH  0 AND HEIGHT IS 0
 
 
     //FLUSHING THE CACHE
     FimgFinish(pState); 
 
 
     //-------------------------------------------------------------------------
     //  COPY TEXTURE DATA
     //-------------------------------------------------------------------------
 
     pixSize = pixelSize(DstTextureFormat);
     if(pixSize <= 0) {
         return;
     }
         pImgBuf =  getImageDataLocation( pTexObj, level ,  cubeFace,  width ,  height , 0  ,internalformat, type,  width * height * pixSize, 0);
         if(pImgBuf == NULL){
             gAssert(false && " unbale to get the memory location \n");
             return;
       }
 
     // we need to copy only if pixel data from main memory is not null...
     if(pixels != NULL)  
     {
 
         /*convertPixels(DstTextureFormat,(void*)pImgBuf, width, height,0,0,0,0,
                   srcTextureFormat, (void*) pixels, width, height,0,0,0,0,
                   width,height,0,0,0,0);*/
 
         convertPixelsTexure(DstTextureFormat,(void*)pImgBuf, width, height,
                   srcTextureFormat, (void*) pixels);
        
         }
 #ifdef  CACHE_MEM
     pCA->cache_clean_invalid(pImgBuf, width * height * pixSize);    
 #endif
     //-------------------------------------------------------------------------
     //SET IMAGE PARAMETERS
     //-------------------------------------------------------------------------  
     pImgObj->isUsed = GL_TRUE;
     pImgObj->width = width;
     pImgObj->height = height;
     pImgObj->internalFormat = internalformat;
     pImgObj->PixType = type;
     pImgObj->imgSize = width * height * pixSize;
     pImgObj->isCompressed = false;  
     pTexObj->isDirtyState = GL_TRUE;    // Set dirty  flag
 
 
     
 }
 
 
 
 #ifdef USE_3D_PM
 void PM_glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
 
 GL_API void GL_APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
 {
     lock3DCriticalSection();
     PM_glTexSubImage2D ( target,  level,  xoffset,  yoffset,  width,  height,  format,  type,  pixels);
     unlock3DCriticalSection();
 }
 
 void PM_glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
 #else
 GL_API void GL_APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
 #endif
 {
     OGLState* pState = GET_OGL_STATE();
 
 
 //      LOGE("!!!!!!!!!!!!!!!!!!!!glTexSubImage2D!!!!!!!!!!!!!!!!!!!!");
 
         
     //GLuint texRef;                // Reference to texture object
     //GLenum texFormat;         // Internal texture format
     //GLsizei pixelSize;            // Bytes per pixel
     GLubyte *pImgBuf = NULL;    // Pointer to image buffer
     Image *pImgObj = NULL;      // Pointer to image object
     GLuint cubeFace = 0;        // Index of cube face refered
     TextureObject* pTexObj;
     //GLuint isImagedata = 1;
     
     PxFmt DstTextureFormat ;   // the texture format in which the data will be send 
     PxFmt srcTextureFormat ;   // source texture format
 
     //-------------------------------------------------------------------------
     //  VALIDATE INPUT PARAMETERS
     //-------------------------------------------------------------------------
     
     if(pixels == NULL) {
         //SET_ERR(pState, GL_INVALID_VALUE, "glTexSubImage2D"); no need for a error...Just return without modifying the original texture object
         return;
     }
 
          // Check MipMap level
     if( level < 0 || level >= MAX_MIPMAP_LEVELS ) 
     {
         SET_ERR(pState, GL_INVALID_VALUE, "glTexSubImage2D");
         return;
     }
 
     if(target == GL_TEXTURE_2D){
         pTexObj = GetTextureObject( target,false,0);
         pImgObj = &(pTexObj->images.tex2D[level]);
         
     } else if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) {
         pTexObj = GetTextureObject( GL_TEXTURE_CUBE_MAP,false,0);
         cubeFace = (target - GL_TEXTURE_CUBE_MAP_POSITIVE_X);
         pImgObj = &(pTexObj->images.cubeMap[cubeFace][level]);
     } else {
         SET_ERR(pState, GL_INVALID_ENUM, "glTexSubImage2D*");
         return;
     }
 
         //if texsubimage is called before calling glteximage function . 
         if(pImgObj->imagedataLocation == NONE){
             SET_ERR(pState, GL_INVALID_OPERATION, "glTexSubImage2D");
             return;
         }
     
     
     if( xoffset < 0 || yoffset < 0 || (xoffset + width) > pImgObj->width 
         || 
         (yoffset + height) > pImgObj->height || width < 0 || height < 0  
         
       ) 
     {
         SET_ERR(pState, GL_INVALID_VALUE, "glTexSubImage2D");
         return;
     }
 
     
     if(format != pImgObj->internalFormat || type != pImgObj->PixType) //format of the image should be identical to the internal format of the texture object created...
     {
         SET_ERR(pState, GL_INVALID_ENUM, "glTexSubImage2D");
         return;
     }
 
 
     DstTextureFormat = DetermineTextureFormat(format,type, &srcTextureFormat );
     
     //FLUSHING THE CACHE
     FimgFinish(pState); 
 
     //-------------------------------------------------------------------------
     // COPY TEXTURE DATA
     //-------------------------------------------------------------------------
 
     pImgBuf =  getImageDataLocation( pTexObj, pImgObj, level , cubeFace);
     if(pImgBuf == NULL){
         gAssert(false && " unbale to get the memory location \n");
         return;
     }
 
 #if 0    //liyue 090527
     convertPixels(DstTextureFormat,(void*)pImgBuf, pImgObj->width, pImgObj->height,0,xoffset,yoffset,0,
                   srcTextureFormat, (void*) pixels, width, height,0,0,0,0,
                   width,height,0,0,0,0);
 #else
         convertPixelsSubTexture(DstTextureFormat, (void*)pImgBuf,pImgObj->width,pImgObj->height, xoffset, yoffset,srcTextureFormat,(void*) pixels, width, height);
 #endif
 
 #ifdef  CACHE_MEM
     pCA->cache_clean_invalid(pImgBuf, pImgObj->imgSize);    
 #endif
 
     pImgObj->isUsed = GL_TRUE;      //do we really need these?
     pTexObj->isDirtyState = GL_TRUE;
 }
 
 #ifdef USE_3D_PM
 void PM_glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
 
 GL_API void GL_APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
 {
     lock3DCriticalSection();
     PM_glCopyTexImage2D ( target,  level,  internalformat,  x,  y,  width,  height,  border);
     unlock3DCriticalSection();
 }
 
 void PM_glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
 #else
 GL_API void GL_APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
 #endif
 {
     OGLState* pState = GET_OGL_STATE();
 
 
     //GLuint texRef;                // Reference to texture object
     GLenum texFormatError;          // Internal texture format
     GLsizei pixSize;            // Bytes per pixel
     GLubyte *pImgBuf = NULL;    // Pointer to image buffer
     Image *pImgObj = NULL;      // Pointer to image object
     GLuint cubeFace = 0;        // Index of cube face refered
     //GLubyte *pTempImgBuf = NULL;
     //GLuint isImagedata = 1;
     TextureObject* pTexObj =NULL;
     //PxFmt srcTextureFormat ;   // the texture format in which the data will be send 
 
     //-------------------------------------------------------------------------
     // VALIDATE INPUT PARAMETERS
     //-------------------------------------------------------------------------
 
     texFormatError = checkTextureFormat(pState, internalformat, internalformat);
      if(texFormatError != GL_NO_ERROR || border != 0 || level < 0 || level >= MAX_MIPMAP_LEVELS 
         || 
         width > (1 << (MAX_MIPMAP_LEVELS-1)) || height > (1 << (MAX_MIPMAP_LEVELS-1)) ||
         width < 0 || height < 0 ) 
     {
         SET_ERR(pState, GL_INVALID_VALUE, "glCopyTexImage2D");
         return;
     };
 
     if(target == GL_TEXTURE_2D){
         pTexObj = GetTextureObject(target,false,0);
         pImgObj = &(pTexObj->images.tex2D[level]);
 
     } else if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) {
         pTexObj = GetTextureObject(GL_TEXTURE_CUBE_MAP,false,0);
         cubeFace = (target - GL_TEXTURE_CUBE_MAP_POSITIVE_X);
         pImgObj = &(pTexObj->images.cubeMap[cubeFace][level]);
     } else {
         SET_ERR(pState, GL_INVALID_ENUM, "glCopyTexImage2D*");
         return;
     }
 
      //Check if FB is complete Sandeep K.  //TO DO ADD WHETHER THE FB ADDRESS IS NULL OR NOT
     if(!isFBrenderable(pState))
     {
         set_err(GL_INVALID_FRAMEBUFFER_OPERATION);
         return;
     }
 
     //Get info from FrameBuffer...
     FramebufferData fbData = getFBData();
 
     // to chekc whether this case is sufficient or needs to have the above case also TO DO
     if(fbData.colorAddr.vaddr  == 0){
           set_err(GL_INVALID_FRAMEBUFFER_OPERATION);
         return;   
         
     }
     
     PxFmt FrameBufFormat =  fbData.nativeColorFormat; 
     void* FramePixels = (unsigned char*)fbData.colorAddr.vaddr;
     unsigned int FrameWidth = fbData.width;
     unsigned int FrameHeight = fbData.height;
 
     GLenum FrameBufFormatEnum = translateToGLenum(FrameBufFormat); 
     
 
     GLenum dstType;
     if(!CheckFormatConversion( internalformat , FrameBufFormatEnum, &dstType) )
     {
         // Do a comparison of internalFormat and framebuftype...and throw error if GL specs are violated....
         SET_ERR(pState, GL_INVALID_OPERATION, "glCopyTexImage2D*");
         gAssert(false && "glCopyTexImage2D" && "Unsupproted format Conversion");
         return;
     }
 
 
     //PxFmt fmtFB =  translateGLSizedInternal(FrameBufFormat);
     PxFmt fmtFB = FrameBufFormat;
     PxFmt fmtData =  translateGLInternal(internalformat,dstType);
 
    
     //Wait till FIMG has finished rendering and all FB data is flushed from FIMG caches to RAM.
     FimgFinish(pState);
 
     //-------------------------------------------------------------------------
     // COPY IMAGE DATA
     //-------------------------------------------------------------------------
 
     pixSize = GetPixSize(fmtData); //the value is going to be stored in the texture format
     if(pixSize <= 0) {
         return;
     }
 
     pImgBuf =  getImageDataLocation( pTexObj, level , cubeFace,  width,  height , 0 ,internalformat, dstType,  width * height * pixSize , 0);
     if(pImgBuf == NULL){
     gAssert(false && " unbale to get the memory location \n");
     return;
     }
 
     GLint dir = 1;
      if(fbData.flipped == 0)
           dir = -1;//y = FrameHeight - y - height + 1*(!(!y));//-1 so as to read in reverse direction...
      else dir = 1;
 
     convertPixels( fmtData, (void*)pImgBuf, width, height, 0,
             0,  0,  0,
             fmtFB, FramePixels, FrameWidth, FrameHeight, 0,
             x, y, 0,
             width, height, 0 , 0 , 0,dir);
 
 
 #ifdef  CACHE_MEM
     pCA->cache_clean_invalid(pImgBuf, width * height * pixSize);    
 #endif
     //-------------------------------------------------------------------------
     // SET IMAGE PARAMETERS
     //-------------------------------------------------------------------------
 
 
     pImgObj->isUsed = GL_TRUE;
     pImgObj->width = width;
     pImgObj->height = height;
     pImgObj->internalFormat = internalformat;
     pImgObj->PixType = dstType;
     pImgObj->nativeFormat = fmtData;
     pImgObj->imgSize = width * height * pixSize;
     pImgObj->isCompressed = false;
     
     pTexObj->isDirtyState = GL_TRUE;
     
 }
 
 
 #ifdef USE_3D_PM
 void PM_glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
 
 GL_API void GL_APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
 {
     lock3DCriticalSection();
     PM_glCopyTexSubImage2D ( target,  level,  xoffset,  yoffset,  x,  y,  width,  height);
     unlock3DCriticalSection();
 }
 
 void PM_glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
 #else
 GL_API void GL_APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
 #endif
 {
     OGLState* pState = GET_OGL_STATE();
 
 
     //GLuint texRef;                // Reference to texture object
     //GLsizei pixelSize;            // Bytes per pixel
     GLubyte *pImgBuf = NULL;    // Pointer to image buffer
     Image *pImgObj = NULL;      // Pointer to image object
     GLuint cubeFace = 0;        // Index of cube face refered
     PxFmt textureFormat;
     PxFmt frameFormat;
     TextureObject *pTexObj =NULL;
     //GLuint isImagedata = 1;
 
 
     //-------------------------------------------------------------------------
     // VALIDATE IMAGE DATA
     //-------------------------------------------------------------------------
     // Check MipMap level
     if(level < 0 || level >= MAX_MIPMAP_LEVELS ) 
     {
         SET_ERR(pState, GL_INVALID_VALUE, "glCopyTexSubImage2D");
         return;
     };
 
     if(target == GL_TEXTURE_2D){
         pTexObj = GetTextureObject(target,false,0);
         pImgObj = &(pTexObj->images.tex2D[level]);
                 
     } else if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) {
         pTexObj = GetTextureObject(GL_TEXTURE_CUBE_MAP,false,0);
         cubeFace = (target - GL_TEXTURE_CUBE_MAP_POSITIVE_X);
         pImgObj = &(pTexObj->images.cubeMap[cubeFace][level]);
     } else {
         SET_ERR(pState, GL_INVALID_ENUM, "glCopyTexSubImage2D*");
         return;
     }
 
     if( x < 0 || y < 0 || xoffset < 0 || yoffset < 0 || (xoffset + width) > pImgObj->width 
         || 
         (yoffset + height) > pImgObj->height || width < 0 || height < 0  
         
       ) 
     {
         SET_ERR(pState, GL_INVALID_VALUE, "glCopyTexSubImage2D");
         return;
     }
 
 
     //if texsubimage is called before calling glteximage function . 
     if(pImgObj->imagedataLocation == NONE){
         SET_ERR(pState, GL_INVALID_OPERATION, "glCopyTexSubImage2D");
         return;
     }
 
     //Check if FB is complete Sandeep K.
     if(!isFBrenderable(pState))
     {
         set_err(GL_INVALID_FRAMEBUFFER_OPERATION);
         return;
     }
 
     //Get info from FrameBuffer...
     FramebufferData fbData = getFBData();
 
     // to chekc whether this case is sufficient or needs to have the above case also TO DO
     if(fbData.colorAddr.vaddr  == 0){
           set_err(GL_INVALID_FRAMEBUFFER_OPERATION);
         return;   
         
     }
     
     GLenum FrameBufFormat = translateToGLenum(fbData.nativeColorFormat); //GL_RGB; //GLenum(fbData.colorFormat);
     //need to convert to GL format
 
     //GLenum type = GL_UNSIGNED_SHORT_5_6_5; //GLenum(fbData.colorType);NOT USED?
 
     GLenum dstType;//not req here...
     // Do a comparison of internalFormat and framebuftype...and throw error if GL specs are violated....
     if(!CheckFormatConversion( pImgObj->internalFormat , FrameBufFormat , &dstType) )
     {
         SET_ERR(pState, GL_INVALID_OPERATION, "glCopyTexSubImage2D*");
         return;
     }
         
     unsigned char* FramePixels = (unsigned char*)fbData.colorAddr.vaddr;
     unsigned int FrameWidth = fbData.width;
     unsigned int FrameHeight = fbData.height;
 
     textureFormat = translateGLInternal(pImgObj->internalFormat,pImgObj->PixType);
     frameFormat = fbData.nativeColorFormat; //to check whether need to be hard coded 
 
     //TO DO CHECK WHICH WILL MAKE SURE THAT TEXTURE SUB IMAGE IS NOT CALLED BEFORE TEXIMAGE 
     //pImgObj->isUsed = GL_TRUE;
 
     //Wait till FIMG has finished rendering and all FB data is flushed from FIMG caches to RAM.
     FimgFinish(pState);
 
     //-------------------------------------------------------------------------
     // COPY TEXTURE  DATA
     //-------------------------------------------------------------------------
 
     pImgBuf =  getImageDataLocation( pTexObj, pImgObj, level , cubeFace);
     if(pImgBuf == NULL){
         gAssert(false && " unbale to get the memory location \n");
         return;
     }
     convertPixels( textureFormat, (void*)pImgBuf, pImgObj->width, pImgObj->height, 0,
         xoffset, yoffset, 0,
         frameFormat, FramePixels, FrameWidth, FrameHeight, 0,
         x, y, 0,
         width, height, 0 , 0 , 0,fbData.flipped ? 1 : -1);
 
 #ifdef  CACHE_MEM
     pCA->cache_clean_invalid(pImgBuf, pImgObj->imgSize);    
 #endif
 
 
     pImgObj->isUsed = GL_TRUE;
     pTexObj->isDirtyState = GL_TRUE;
 }
 
 /*******************************************************************************
 
 *******************************************************************************/
 
 
 #ifdef USE_3D_PM
 void PM_glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *pixels);
 
 GL_API void GL_APIENTRY glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *pixels)
 {
     lock3DCriticalSection();
     PM_glCompressedTexImage2D ( target,  level,  internalformat,  width,  height,  border,  imageSize, pixels);
     unlock3DCriticalSection();
 }
 
 void PM_glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *pixels)
 #else
 GL_API void GL_APIENTRY glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *pixels)
 #endif
 {
     OGLState* pState = GET_OGL_STATE();
     
     //GLuint texRef;                // Reference to texture object
     PxFmt texFormat;            // Internal texture format
     //GLsizei pixelSize;            // Bytes per pixel
     GLubyte *pImgBuf = NULL;    // Pointer to image buffer
     Image *pImgObj = NULL;      // Pointer to image object
     GLuint cubeFace = 0;        // Index of cube face refered
     //PtrChunkH pTmpChunk = NULL;  /* pointer to the ChunkHandle of new image data */
     //GLubyte* newPixels = NULL;     /* pointer to the new image data */
     GLuint newImageSize = 0;
     PxFmt DstTextureFormat ;   // the texture format in which the data will be send 
     PxFmt srcTextureFormat ;   // source texture format 
     //GLenum tmpPixelSize ;       //temporary to check if value is same or not
     //GLubyte *pTempImgBuf = NULL;
     unsigned int NoOfpaletteEntry = 0;  //determine the palette entry of table
     unsigned int  OldpaletteSize = 0;       // the size of the 1 palette entry in byte
     unsigned int  NewpaletteSize = 0;       // the size of the 1 palette entry in byte after convertPixel
     TextureObject* pTexObj = NULL;
     //-------------------------------------------------------------------------
     // Get texture object and VALIDATE INPUT PARAMETERS
     //-------------------------------------------------------------------------
 
     //TO DO LEVEL HAS TIO BE GREATER THAN 0 EXCEPT FOR PALETTE ONE WHERE NEGATIVE
     if(level > 0 || abs(level) >=MAX_MIPMAP_LEVELS)  {
         SET_ERR(pState, GL_INVALID_VALUE, "glCompressedTexImage2D");
         return;
     }
     // Select object reference according to target
     // Return error on invalid enum
 
     if(target == GL_TEXTURE_2D){
         pTexObj = GetTextureObject(target,false,0);
         pImgObj = &(pTexObj->images.tex2D[0]);
         //pTexObj->levels = abs(level) + 1;
 
     } else if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) {
         pTexObj = GetTextureObject(GL_TEXTURE_CUBE_MAP,false,0);
         cubeFace = (target - GL_TEXTURE_CUBE_MAP_POSITIVE_X);
         pImgObj = &(pTexObj->images.cubeMap[cubeFace][0]);
         //pTexObj->levels = abs(level) + 1;
         
     } else {
         SET_ERR(pState, GL_INVALID_ENUM, "glCompressedTexImage2D*");
         return;
     }
         
     // Check width and height
     if(width > (1 << (MAX_MIPMAP_LEVELS-1)) || height > (1 << (MAX_MIPMAP_LEVELS-1))) {
         SET_ERR(pState, GL_INVALID_VALUE, "glCompressedTexImage2D");
         return;
     }
 
     // Border not supported in ES
     if(border != 0) {
         SET_ERR(pState, GL_INVALID_VALUE, "glCompressedTexImage2D");
         return;
     }
 
     //-------------------------------------------------------------------------
     // Calculate internal format and space requirements
     //-------------------------------------------------------------------------
 
     void* tmpImgSrc = const_cast<void*>(pixels); //this variable will point to the user data and will be changed for palette case.
      GLenum bFormat  = GLenum(-1);
      GLenum bType  = GLenum(-1); 
     switch(internalformat)
     {
         case GL_RGB_S3TC_OES:
         {
             GLenum format;
             bool flag = s3tcCompressedTex(format,internalformat,width, height, imageSize);
             if(!flag) return;  
             newImageSize = imageSize;
             texFormat = E_RGB_S3TC_OES;
             break;
         }   
         case GL_RGBA_S3TC_OES:
         {
             GLenum format;
             bool flag = s3tcCompressedTex(format,internalformat,width, height, imageSize);
             if(!flag) return;  
             newImageSize = imageSize;
             texFormat = E_RGBA_S3TC_OES;
             break;
         }
                 
         case GL_PALETTE4_RGB8_OES: //24
             texFormat = E_PALETTE4_RGBA8_OES;
             NoOfpaletteEntry = 16;
             OldpaletteSize = 3; 
             NewpaletteSize = 4;
             bFormat = GL_RGB;
              bType =    GL_UNSIGNED_BYTE;
             break;
             
         case GL_PALETTE8_RGB8_OES:  //24
             texFormat = E_PALETTE8_RGBA8_OES;
             NoOfpaletteEntry = 256;
             OldpaletteSize = 3; 
             NewpaletteSize = 4;
              bFormat = GL_RGB;
              bType =    GL_UNSIGNED_BYTE;
             break;
 
         case GL_PALETTE4_RGBA8_OES:  //32
             texFormat = E_PALETTE4_RGBA8_OES;
             NoOfpaletteEntry = 16;
             OldpaletteSize = 4; 
             NewpaletteSize = 4;
              bFormat = GL_RGBA;
              bType =    GL_UNSIGNED_BYTE;
             break;
         
         case GL_PALETTE8_RGBA8_OES:  //32 
             texFormat = E_PALETTE8_RGBA8_OES;
             NoOfpaletteEntry = 256;
             OldpaletteSize = 4; 
             NewpaletteSize = 4;
             bFormat = GL_RGBA;
              bType =    GL_UNSIGNED_BYTE;
             break;
             
         case GL_PALETTE4_RGBA4_OES:   //16 
             texFormat = E_PALETTE4_RGBA8_OES;
             NoOfpaletteEntry = 16;
             OldpaletteSize = 2; 
             NewpaletteSize = 4;
              bFormat = GL_RGBA;
              bType =    GL_UNSIGNED_SHORT_4_4_4_4;
             break;
             
         case GL_PALETTE8_RGBA4_OES:  
             texFormat = E_PALETTE8_RGBA8_OES;
             NoOfpaletteEntry = 256;
             OldpaletteSize = 2; 
             NewpaletteSize = 4;
              bFormat = GL_RGBA;
              bType =    GL_UNSIGNED_SHORT_4_4_4_4;
             break;
             
         case GL_PALETTE4_RGB5_A1_OES:  //16   
             texFormat = E_PALETTE4_RGBA8_OES;
             NoOfpaletteEntry = 16;
             OldpaletteSize = 2; 
             NewpaletteSize = 4;
              bFormat = GL_RGBA;
              bType =    GL_UNSIGNED_SHORT_5_5_5_1;
             break;   
         
          case GL_PALETTE8_RGB5_A1_OES://16
             texFormat = E_PALETTE8_RGBA8_OES;
             NoOfpaletteEntry = 256;
             OldpaletteSize = 2; 
             NewpaletteSize = 4;
              bFormat = GL_RGBA;
              bType =    GL_UNSIGNED_SHORT_5_5_5_1;
             break;
             
             
         case GL_PALETTE8_R5_G6_B5_OES:  //16        
             texFormat = E_PALETTE8_RGBA8_OES;
             NoOfpaletteEntry = 256;
             OldpaletteSize = 2; 
             NewpaletteSize = 4;
              bFormat = GL_RGB;
              bType =    GL_UNSIGNED_SHORT_5_6_5;
             break;   
 
          case GL_PALETTE4_R5_G6_B5_OES: //16
             texFormat = E_PALETTE4_RGBA8_OES;
             NoOfpaletteEntry = 16;
             OldpaletteSize = 2; 
             NewpaletteSize = 4;
              bFormat = GL_RGB;
              bType =    GL_UNSIGNED_SHORT_5_6_5;
             break;   
             
             
         default:
         return;            
     }
 
         if((internalformat != GL_RGB_S3TC_OES)&& (internalformat != GL_RGBA_S3TC_OES))
         {
                 if(imageSize==0)
                         imageSize = (NoOfpaletteEntry*OldpaletteSize)+(width*height);
             newImageSize = imageSize + NoOfpaletteEntry * (NewpaletteSize -OldpaletteSize) ;
         }
 
         
     //FLUSHING THE CACHE
     FimgFinish(pState); 
 #ifdef PALETTE_SW_WORKAROUND    
 
        if ((internalformat >= GL_PALETTE4_RGB8_OES) && ( internalformat <= GL_PALETTE8_RGB5_A1_OES)) {
 
                   for (int count =0 ; count <= abs(level) ; count++) {
                         int lodWidth = (width  >> count) ? : 1;
                         int lodHeight = (height >> count) ? : 1;
                         GLubyte * pixValBuf = (GLubyte *)Plat::malloc(lodWidth * lodHeight * OldpaletteSize) ;
                         
                          decodePalette4(pixels, count, width, height, pixValBuf, lodWidth, internalformat); 
 #ifdef USE_3D_PM                    
         PM_glTexImage2D ( target,  count,  bFormat,  lodWidth, lodHeight,  border,  bFormat,  bType , pixValBuf);
 #else
         glTexImage2D ( target,  count,  bFormat,  lodWidth, lodHeight,  border,  bFormat,  bType , pixValBuf);
 #endif
         Plat::safe_free(pixValBuf);
         return ;
     }
    }
 #endif    
     
     //-------------------------------------------------------------------------
     // COPY IMAGE DATA
     //-------------------------------------------------------------------------
 
     pImgBuf =  getImageDataLocation( pTexObj, abs(level) , cubeFace,  width,  height , 0 ,internalformat,internalformat,  newImageSize, 0);
     if(pImgBuf == NULL){
         gAssert(false && " unbale to get the memory location \n");
         return;
     }
 
     if((internalformat == GL_RGB_S3TC_OES)||(internalformat == GL_RGBA_S3TC_OES))
     {
         if (pixels)
             Plat::memcpy(pImgBuf,(GLubyte*)tmpImgSrc,newImageSize); 
     }
     else
     {
         GLenum type =   internalformat;
         GLubyte* tempPixel = (GLubyte*)pixels;
 
         DstTextureFormat = DetermineTextureFormat(internalformat, type, &srcTextureFormat);
             
         newImageSize = imageSize + NoOfpaletteEntry * (NewpaletteSize -OldpaletteSize) ;
         
         if (pixels)
         {
             convertPixels(DstTextureFormat,(void*)pImgBuf, NoOfpaletteEntry, 1,0,0,0,0,
                           srcTextureFormat, (void*) pixels, NoOfpaletteEntry, 1,0,0,0,0,
                           NoOfpaletteEntry,1,0,0,0,1);  //paletteSize is the size of the table to be converted
             Plat::memcpy((pImgBuf+NoOfpaletteEntry * NewpaletteSize) ,
                         (tempPixel+NoOfpaletteEntry * OldpaletteSize) ,
                         imageSize -NoOfpaletteEntry * OldpaletteSize ); 
         }
 
         
     }
 
 #ifdef  CACHE_MEM
     pCA->cache_clean_invalid(pImgBuf, newImageSize);    
 #endif
 
     pImgObj->isUsed = GL_TRUE;
     pImgObj->width = width;
     pImgObj->height = height;
     pImgObj->internalFormat = internalformat;
     pImgObj->PixType = internalformat;  
     pImgObj->imgSize = newImageSize;
     pImgObj->isCompressed = true;
     pImgObj->nativeFormat = texFormat;
 
     pTexObj->isDirtyState = GL_TRUE;
     tmpImgSrc = NULL;
 
 //fprintf(stderr, " line = %d , func =%s \n", __LINE__ , __FUNCTION__);
     
 }
 
 #ifdef USE_3D_PM
 void PM_glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
 
 GL_API void GL_APIENTRY glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
 {
     lock3DCriticalSection();
     PM_glCompressedTexSubImage2D ( target,  level,  xoffset,  yoffset,  width,  height,  format,  imageSize,  data);
     unlock3DCriticalSection();
 }
 
 void PM_glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
 #else
 GL_API void GL_APIENTRY glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
 #endif
 {
     OGLState* pState = GET_OGL_STATE();
     
 
     //GLuint texRef;                // Reference to texture object
     GLenum texFormat;           // Internal texture format
     //GLsizei pixelSize;            // Bytes per pixel
     GLubyte *pImgBuf = NULL;    // Pointer to image buffer
     Image *pImgObj = NULL;      // Pointer to image object
     GLuint cubeFace = 0;        // Index of cube face refered
     TextureObject* pTexObj = NULL;
     
     //-------------------------------------------------------------------------
     // VALIDATE INPUT PARAMETERS
     //-------------------------------------------------------------------------
     // Check image pointer
     if(data == NULL) {
         return;
     }
     if(level < 0 || level >= MAX_MIPMAP_LEVELS) {
         SET_ERR(pState, GL_INVALID_VALUE, "glCompressedTexSubImage2D");
         return;
     };
     if(target == GL_TEXTURE_2D){
         pTexObj = GetTextureObject(target,false,0);
         pImgObj = &(pTexObj->images.tex2D[level]);
 
     } else if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) {
         pTexObj = GetTextureObject(GL_TEXTURE_CUBE_MAP,false,0);
         cubeFace = (target - GL_TEXTURE_CUBE_MAP_POSITIVE_X);
         pImgObj = &(pTexObj->images.cubeMap[cubeFace][level]);
 
     } else {
         SET_ERR(pState, GL_INVALID_ENUM, "glCompressedTexSubImage2D*");
         return;
     }
 
     // Check MipMap level
     
     if( xoffset < 0 || yoffset < 0 || (xoffset + width) > pImgObj->width 
         || 
         (yoffset + height) > pImgObj->height || width < 0 || height < 0  
         
       ) 
     {
         SET_ERR(pState, GL_INVALID_VALUE, "glCompressedTexSubImage2D");
         return;
     }
 #if 0
     if( pImgObj->hImgChunk == NULL /*pImgObj->data == NULL*/ ){ //probably this will take care if texSubImage is called before texImage2D......
         SET_ERR(pState, GL_INVALID_OPERATION, "glCompressedTexSubImage2D");
     }
 
  #else
     //TO DO MAKE SURE THAT SUB IMAGE IS NOT CALLED BEFORE IMAGE
 
  
 #endif
     int offset = 0;
     //int sizeToCopy =0;
     int ww =0;
     int hh =0;
     int BPG =0; //bytes per group
     int srcW = 0;
     int srcH = 0;
     switch(format)
     {
         case GL_RGB_S3TC_OES:
         case GL_RGBA_S3TC_OES:
         {
             ///in S3TC 4x4 block is taking 8 bytes
             ww = (xoffset+width)/4; 
             hh = (yoffset+height)/4;
             xoffset = xoffset/4;
             yoffset = yoffset/4;
             
             offset = (xoffset + yoffset * ww)*8 ; // 8 is number of bytes for 16 pixels.
             BPG = 8;
             srcW = pImgObj->width/4;
             srcH = pImgObj->height/4;
             
             bool flag = s3tcCompressedTex(texFormat,format,width, height, imageSize);
             if(!flag) return;  // if flag is false
             break;
         }
 
         //TO DO INVALID_OPERATION is generated when the given formats are present as internalformat
         case GL_PALETTE4_RGB8_OES:              
         case GL_PALETTE4_RGBA8_OES:             
         case GL_PALETTE4_R5_G6_B5_OES:
         case GL_PALETTE4_RGBA4_OES:             
         case GL_PALETTE4_RGB5_A1_OES:
         
         case GL_PALETTE8_RGB8_OES:              
         case GL_PALETTE8_RGBA8_OES:             
         case GL_PALETTE8_R5_G6_B5_OES:          
         case GL_PALETTE8_RGBA4_OES:             
         case GL_PALETTE8_RGB5_A1_OES:
 #if 1
             //INVALID_OPERATION is generated  CompressedTexSubImage2D if internalformat is PALETTE  
             SET_ERR(pState, GL_INVALID_OPERATION, "glCompressedTexSubImage2D");
             return ;
 #else
             //paletteCompressedTex();  //TODO:stuff related to palette based comp goes here
             ww = width;
             hh = height;
             BPG = 1;
             srcW = pImgObj->width;
             srcH = pImgObj->height;
             break;
 #endif          
         default:
         return;            
     }
     
     //FLUSHING THE CACHE
     FimgFinish(pState); 
 
     //-------------------------------------------------------------------------
     //  COPY TEXTURE DATA
     //-------------------------------------------------------------------------
     
     pImgBuf =  getImageDataLocation( pTexObj, level ,  cubeFace,  pImgObj->width,  pImgObj->height , 0 , pImgObj->internalFormat, pImgObj->PixType,  pImgObj->imgSize, 0);
     if(pImgBuf == NULL){
          gAssert(false && " unbale to get the memory location \n");
         return;
     }
     
 #ifdef  CACHE_MEM
     pCA->cache_clean_invalid(pImgBuf, pImgObj->imgSize);    
 #endif
     
     //TODO:  Copy of subsection of the texel array for compressed case is to be implemented....
     GLubyte* src = (GLubyte*)data;
     GLubyte* dst = pImgBuf + offset;
 
     for(int j = 0; j < hh; ++j)
         for(int i = 0; i < ww ; ++i)
         {
             for(int b = 0; b < BPG; ++b)
             {
                 *dst = *src;
                 dst++;
                 src++;
             }
         dst += (srcW - ww)*BPG;
         }   
     pImgObj->isUsed = GL_TRUE;
     // Set recompile flag
     pTexObj->reCompile = GL_TRUE;
     pTexObj->isDirtyState = GL_TRUE;
 
 }
 
 
 #ifdef USE_3D_PM
 void PM_glTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
 
 GL_API void GL_APIENTRY glTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
 {
     lock3DCriticalSection();
     PM_glTexImage3D ( target,  level,  internalformat,  width,  height,  depth,  border,  format,  type,  pixels);
     unlock3DCriticalSection();
 }
 
 void PM_glTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
 #else
 GL_API void GL_APIENTRY glTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
 #endif
 {
     OGLState* pState = GET_OGL_STATE();
     //TextureState* pTexState = &(pState->texState);
 
     //GLuint texRef;                // Reference to texture object
     GLenum texFormatError;          // Internal texture format
     GLsizei pixSize;            // Bytes per pixel
     GLubyte *pImgBuf = NULL;    // Pointer to image buffer
     Image *pImgObj = NULL;      // Pointer to image object
     PxFmt DstTextureFormat ;   /* the texture format in which the data will be send */
     PxFmt srcTextureFormat ;   /* source texture format */
     //GLenum tmpPixelSize ;       /*temporary to check if value is same or not*/
     //GLubyte *pTempImgBuf = NULL;
     //GLuint isImagedata = 1;
     TextureObject* pTexObj = NULL;
     
     //-------------------------------------------------------------------------
     // Get texture object and VALIDATE INPUT PARAMETER
     //-------------------------------------------------------------------------
     
     // Check MipMap level and border
     if(level < 0 || level >= MAX_MIPMAP_LEVELS || border!=0) {
         SET_ERR(pState, GL_INVALID_VALUE, "glTexImage3D");
         return;
     };
     
     // Select object reference according to target
     // Return error on invalid enum
     if(target == GL_TEXTURE_3D){
         pTexObj = GetTextureObject(target,false,0);
         pImgObj = &(pTexObj->images.tex3D[level]);
     } 
     else {
         SET_ERR(pState, GL_INVALID_ENUM, "glTexImage3D*");
         return;
     }
 
     // Check width and height
     if(width > (1 << (MAX_MIPMAP_LEVELS-1)) || height > (1 << (MAX_MIPMAP_LEVELS-1)) || depth > (1 << (MAX_MIPMAP_LEVELS-1)) ||
              width < 0 || height < 0 || depth  < 0) {
         SET_ERR(pState, GL_INVALID_VALUE, "glTexImage3D");
         return;
     }
     
     //check the  Texture format
     texFormatError = checkTextureFormat(pState, internalformat, format);
     if(texFormatError != GL_NO_ERROR) {
         SET_ERR(pState, texFormatError, "glTexImage3D");
         return;
     }
 
         // check the type and format combination supported 
     if(CheckFormatTypeCombination(internalformat,type) == 1){
         SET_ERR(pState, GL_INVALID_OPERATION, "glTexImage3D");
         return;
     }
     DstTextureFormat = DetermineTextureFormat(internalformat, type, &srcTextureFormat);
     pImgObj->nativeFormat = DstTextureFormat;
 
     //FLUSHING THE CACHE
     FimgFinish(pState); 
 
     //-------------------------------------------------------------------------
     // COPY TEXTURE DATA
     //-------------------------------------------------------------------------
 
         // Per-pixel size
     pixSize = pixelSize(DstTextureFormat);
     if(pixSize <= 0) {
         return;
     }
 
 
     pImgBuf =  getImageDataLocation( pTexObj, level ,  0,  width ,  height , depth  ,internalformat,type,  width * height * depth * pixSize , 0);
     if(pImgBuf == NULL){
         gAssert(false && " unbale to get the memory location \n");
         return;
   }
 
 
     if(pixels != NULL)
         convertPixels(DstTextureFormat,(void*)pImgBuf, width, height,depth,0,0,0,
                   srcTextureFormat, (void*) pixels, width, height,depth,0,0,0,
                   width,height,depth,0,0,0);
 
 #ifdef  CACHE_MEM
     pCA->cache_clean_invalid(pImgBuf, width * height * pixSize);    
 #endif
 
     //-------------------------------------------------------------------------
     // SET  IMAGE  PARAMETERS
     //-------------------------------------------------------------------------
     pImgObj->isUsed = GL_TRUE;
     pImgObj->width = width;
     pImgObj->height = height;
     pImgObj->depth = depth;
     pImgObj->internalFormat = internalformat;
     pImgObj->PixType = type;
     pImgObj->imgSize = width * height * depth* pixSize;
     pImgObj->isCompressed = false;
 
     pTexObj->isDirtyState = GL_TRUE;
 }
 
 
 #ifdef USE_3D_PM
 void PM_glTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
 
 GL_API void GL_APIENTRY glTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
 {
     lock3DCriticalSection();
     PM_glTexSubImage3D (target,  level,  xoffset,  yoffset,  zoffset,  width,  height,  depth,  format,  type,  pixels);
     unlock3DCriticalSection();
 }
 
 void PM_glTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
 #else
 GL_API void GL_APIENTRY glTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
 #endif
 {
     OGLState* pState = GET_OGL_STATE();
     //TextureState* pTexState = &(pState->texState);
 
     //GLuint texRef;                // Reference to texture object
     //GLenum texFormat;         // Internal texture format
     //GLsizei pixelSize;            // Bytes per pixel
     GLubyte *pImgBuf = NULL;    // Pointer to image buffer
     Image *pImgObj = NULL;      // Pointer to image object
     //TextureObject* pTexObj;
 
     //GLuint isImagedata = 0;
     PxFmt DstTextureFormat ;   // the texture format in which the data will be send 
     PxFmt srcTextureFormat ;   // source texture format
     TextureObject* pTexObj = NULL;
     //-------------------------------------------------------------------------
     // VALIDATE INPUT PARAMETERS
     //-------------------------------------------------------------------------
     if(pixels == NULL) {
         //SET_ERR(pState, GL_INVALID_VALUE, "glTexSubImage2D"); no need for a error...Just return without modifying the original texture object
         return;
     }
     // Check MipMap level
     if( level < 0 || level >= MAX_MIPMAP_LEVELS ) 
     {
         SET_ERR(pState, GL_INVALID_VALUE, "glTexSubImage2D");
         return;
     }
     
     if(target == GL_TEXTURE_3D){
         pTexObj = GetTextureObject(target,false,0);
         pImgObj = &(pTexObj->images.tex3D[level]);
 
     } else {
         SET_ERR(pState, GL_INVALID_ENUM, "glTexSubImage3D*");
         return;
     }
 
     if( xoffset < 0 || yoffset < 0 || zoffset < 0 ||(xoffset + width) > pImgObj->width 
         || 
         (yoffset + height) > pImgObj->height || (zoffset + depth) > pImgObj->depth
         || 
         width < 0 || height < 0  || depth < 0
       ) 
     {
         SET_ERR(pState, GL_INVALID_VALUE, "glTexSubImage3D");
         return;
     }
 
     if(format != pImgObj->internalFormat || type != pImgObj->PixType) //format of the image should be identical to the internal format of the texture object created...
     {
         SET_ERR(pState, GL_INVALID_ENUM, "glTexSubImage3D");
         return;
     }
 
         //if texsubimage is called before calling glteximage function . 
         if(pImgObj->imagedataLocation == NONE){
             SET_ERR(pState, GL_INVALID_OPERATION, "glTexSubImage3D");
             return;
         }
         
 
     //FLUSHING THE CACHE
     FimgFinish(pState); 
     
     //-------------------------------------------------------------------------
     // COPY TEXTURE  DATA
     //-------------------------------------------------------------------------
      pImgBuf =  getImageDataLocation( pTexObj, level ,  0,  pImgObj->width ,  pImgObj->height , pImgObj->depth ,pImgObj->internalFormat, pImgObj->PixType,  pImgObj->imgSize, 0);
         if(pImgBuf == NULL){
             gAssert(false && " unbale to get the memory location \n");
             return;
         }
 
     DstTextureFormat = DetermineTextureFormat(format,type, &srcTextureFormat );
     convertPixels(DstTextureFormat,(void*)pImgBuf, pImgObj->width, pImgObj->height, pImgObj->depth,xoffset,yoffset,zoffset,
                   srcTextureFormat, (void*) pixels, width, height,depth,0,0,0,
                   width,height,depth,0,0,0);
 
 #ifdef  CACHE_MEM
     pCA->cache_clean_invalid(pImgBuf, pImgObj->imgSize);    
 #endif
 
     pTexObj->isDirtyState = GL_TRUE;
     pImgObj->isUsed = GL_TRUE;
     
 }
 
 #ifdef USE_3D_PM
 void PM_glCopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
 
 GL_API void GL_APIENTRY glCopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
 {
     lock3DCriticalSection();
     PM_glCopyTexSubImage3D ( target,  level,  xoffset,  yoffset,  zoffset,  x,  y,  width,  height);
     unlock3DCriticalSection();
 }
 
 void PM_glCopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
 #else
 GL_API void GL_APIENTRY glCopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
 #endif
 {
     OGLState* pState = GET_OGL_STATE();
     //TextureState* pTexState = &(pState->texState);
 
     //GLuint texRef;                // Reference to texture object
     //GLsizei pixelSize;            // Bytes per pixel
     GLubyte *pImgBuf = NULL;    // Pointer to image buffer
     Image *pImgObj = NULL;      // Pointer to image object
     //GLuint cubeFace = 0;      // Index of cube face refered
     PxFmt textureFormat;
     PxFmt frameFormat;
     //GLuint isImagedata;
     TextureObject* pTexObj =NULL;
     //-------------------------------------------------------------------------
     // VALIDATE INPUT PARAMETERS
     //-------------------------------------------------------------------------
     // Check MipMap level
     if(level < 0 || level >= MAX_MIPMAP_LEVELS ) 
     {
         SET_ERR(pState, GL_INVALID_VALUE, "glCopyTexSubImage3D");
         return;
     };
     
     if(target == GL_TEXTURE_3D){
         pTexObj = GetTextureObject(target,false,0);
         pImgObj = &(pTexObj->images.tex3D[level]);
 
     } else {
         SET_ERR(pState, GL_INVALID_ENUM, "glCopyTexSubImage3D*");
         return;
     }
     
     if( x < 0 || y < 0 || xoffset < 0 || yoffset < 0 || zoffset < 0 || (xoffset + width) > pImgObj->width 
         || 
         (yoffset + height) > pImgObj->height || zoffset  > pImgObj->depth 
         || 
         width < 0 || height < 0
       ) 
     {
         SET_ERR(pState, GL_INVALID_VALUE, "glCopyTexSubImage3D");
         return;
     }
     
         //if texsubimage is called before calling glteximage function . 
         if(pImgObj->imagedataLocation == NONE){
             SET_ERR(pState, GL_INVALID_OPERATION, "glCopyTexSubImage3D");
             return;
         }
 
          //TO DO CHECK WHETHER FB is complete  CONDITION NEED TO BE TESTED OR NOT
          if(!isFBrenderable(pState))
         {
             set_err(GL_INVALID_FRAMEBUFFER_OPERATION);
             return;
         }
     
     //Get info from FrameBuffer...
     FramebufferData fbData = getFBData();
 
      // to chekc whether this case is sufficient or needs to have the above case also TO DO
     if(fbData.colorAddr.vaddr  == 0){
           set_err(GL_INVALID_FRAMEBUFFER_OPERATION);
         return;   
         
     }
     
     GLenum FrameBufFormat = translateToGLenum(fbData.nativeColorFormat);
     //GLenum type = GLenum(0);//GLenum(fbData.colorType);
     GLenum dstType;//not req here...
 
     // Do a comparison of internalFormat and framebuftype...and throw error if GL specs are violated....
     if(!CheckFormatConversion( pImgObj->internalFormat , FrameBufFormat , &dstType) )
     {
         SET_ERR(pState, GL_INVALID_OPERATION, "glCopyTexSubImage3D*");
         return;
     }
         
 
     pImgBuf = (GLubyte *)pImgObj->hImgChunk->GetVirtAddr();//pImgObj->data;
         
    unsigned char* FramePixels = (unsigned char*)fbData.colorAddr.vaddr;
     unsigned int FrameWidth = fbData.width;
     unsigned int FrameHeight = fbData.height;
 
     textureFormat = translateGLInternal(pImgObj->internalFormat,pImgObj->PixType);
     frameFormat = fbData.nativeColorFormat; //to check whether need to be hard coded 
 
 
     //Wait till FIMG has finished rendering and all FB data is flushed from FIMG caches to RAM.
     FimgFinish(pState);
 
     //-------------------------------------------------------------------------
     // COPY TEXTURE DATA
     //------------------------------------------------------------------------- 
    pImgBuf =  getImageDataLocation( pTexObj, pImgObj, level , 0);
     if(pImgBuf == NULL){
         gAssert(false && " unbale to get the memory location \n");
         return;
     }
 
     if(fbData.flipped == 0)
         y = FrameHeight - y - height + 1*(!(!y));
 
     convertPixels( textureFormat, (void*)pImgBuf, pImgObj->width, pImgObj->height, pImgObj->depth,
         xoffset, yoffset, zoffset,
         frameFormat, FramePixels, FrameWidth, FrameHeight, 0,
         x, y, 0,
         width, height, 0 , 0 , 0,1);
     
 #ifdef  CACHE_MEM
     pCA->cache_clean_invalid(pImgBuf, pImgObj->imgSize);    
 #endif
 
     pImgObj->isUsed = GL_TRUE;
     pTexObj->isDirtyState = GL_TRUE;
     
 }
 
 #ifdef USE_3D_PM
 void PM_glCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
 
 GL_API void GL_APIENTRY glCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
 {
     lock3DCriticalSection();
     PM_glCompressedTexImage3D ( target,  level,  internalformat,  width,  height,  depth,  border,  imageSize,  data);
     unlock3DCriticalSection();
 }
 
 void PM_glCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
 #else
 GL_API void GL_APIENTRY glCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
 #endif
 {
 
     OGLState* pState = GET_OGL_STATE();
     //TextureState* pTexState = &(pState->texState);
 
     //GLuint texRef;                // Reference to texture object
     PxFmt texFormat;            // Internal texture format
     //GLsizei pixelSize;            // Bytes per pixel
     GLubyte *pImgBuf = NULL;    // Pointer to image buffer
     Image *pImgObj = NULL;      // Pointer to image object
     //GLuint cubeFace = 0;      // Index of cube face refered
     //PtrChunkH pTmpChunk = NULL;  /* pointer to the ChunkHandle of new image data */
     //GLubyte* newPixels = NULL;     /* pointer to the new image data */
     GLuint newImageSize = 0;
     PxFmt DstTextureFormat ;   // the texture format in which the data will be send 
     PxFmt srcTextureFormat ;   // source texture format 
     //GLenum tmpPixelSize ;       //temporary to check if value is same or not
     //GLubyte *pTempImgBuf = NULL;
     unsigned int NoOfpaletteEntry = 0;  //determine the palette entry of table
     unsigned int  OldpaletteSize;       // the size of the 1 palette entry in byte
     unsigned int  NewpaletteSize;       // the size of the 1 palette entry in byte after convertPixel
     TextureObject* pTexObj =NULL;
     
     //-------------------------------------------------------------------------
     // Get texture object and VALIDATE INPUT PARAMETER
     //-------------------------------------------------------------------------
 
     //however FIMG  support 3D texture level 0 only.  TO DO LEVELS
     if(level > 0 || abs(level) >=MAX_MIPMAP_LEVELS)  {
         SET_ERR(pState, GL_INVALID_VALUE, "glCompressedTexImage2D");
         return;
     }
     // Select object reference according to target
     // Return error on invalid enum
 
     if(target == GL_TEXTURE_3D){
         pTexObj = GetTextureObject(target,false,0);
         pImgObj = &(pTexObj->images.tex3D[0]);
         pTexObj->levels = abs(level) + 1;
 
     } else{
         SET_ERR(pState, GL_INVALID_ENUM, "glCompressedTexImage3D*");
         return;
     }
         
     // Check width and height
     if(width > (1 << (MAX_MIPMAP_LEVELS-1)) || height > (1 << (MAX_MIPMAP_LEVELS-1))
        || depth > (1 << (MAX_MIPMAP_LEVELS-1)) || (width < 0) || (height < 0) || (depth < 0)) {
         SET_ERR(pState, GL_INVALID_VALUE, "glCompressedTexImage3D");
         return;
     }
 
     // Border not supported in ES, INVALID_OPERATION  should be set as error spec of palette
     if(border != 0) {
         SET_ERR(pState, GL_INVALID_OPERATION, "glCompressedTexImage3D");
         return;
     }
 
     //-------------------------------------------------------------------------
     // Calculate internal format and space requirements
     //-------------------------------------------------------------------------
 
     switch(internalformat)
     {
         //EXT_texture_compression_s3tc says 3D s3TC texture can't be specified
         case GL_RGB_S3TC_OES:
         case GL_RGBA_S3TC_OES:
             SET_ERR(pState, GL_INVALID_ENUM , "glCompressedTexImage3D");
             return ;               
                 
         case GL_PALETTE4_RGB8_OES: //24
             texFormat = E_PALETTE4_RGBA8_OES;
             NoOfpaletteEntry = 16;
             OldpaletteSize = 3; 
             NewpaletteSize = 4;
             break;
             
         case GL_PALETTE8_RGB8_OES:  //24
             texFormat = E_PALETTE8_RGBA8_OES;
             NoOfpaletteEntry = 256;
             OldpaletteSize = 3; 
             NewpaletteSize = 4;
             break;
 
         case GL_PALETTE4_RGBA8_OES:  //32
             texFormat = E_PALETTE4_RGBA8_OES;
             NoOfpaletteEntry = 16;
             OldpaletteSize = 4; 
             NewpaletteSize = 4;
             break;
         
         case GL_PALETTE8_RGBA8_OES:  //32 
             texFormat = E_PALETTE8_RGBA8_OES;
             NoOfpaletteEntry = 256;
             OldpaletteSize = 4; 
             NewpaletteSize = 4;
             break;
             
         case GL_PALETTE4_RGBA4_OES:   //16 
             texFormat = E_PALETTE4_RGBA8_OES;
             NoOfpaletteEntry = 16;
             OldpaletteSize = 2; 
             NewpaletteSize = 4;
             break;
             
         case GL_PALETTE8_RGBA4_OES:  
             texFormat = E_PALETTE8_RGBA8_OES;
             NoOfpaletteEntry = 256;
             OldpaletteSize = 2; 
             NewpaletteSize = 4;
             break;
             
         case GL_PALETTE4_RGB5_A1_OES:  //16   
             texFormat = E_PALETTE4_RGBA8_OES;
             NoOfpaletteEntry = 16;
             OldpaletteSize = 2; 
             NewpaletteSize = 4;
             break;   
         
         case GL_PALETTE4_R5_G6_B5_OES: //16
             texFormat = E_PALETTE4_RGBA8_OES;
             NoOfpaletteEntry = 16;
             OldpaletteSize = 2; 
             NewpaletteSize = 4;
             break;
             
         case GL_PALETTE8_R5_G6_B5_OES:  //16        
             texFormat = E_PALETTE8_RGBA8_OES;
             NoOfpaletteEntry = 256;
             OldpaletteSize = 2; 
             NewpaletteSize = 4;
             break;   
 
         case GL_PALETTE8_RGB5_A1_OES://16
             texFormat = E_PALETTE8_RGBA8_OES;
             NoOfpaletteEntry = 256;
             OldpaletteSize = 2; 
             NewpaletteSize = 4;
             break;   
             
             
         default:
         //SET_ERR(pState, GL_INVALID_ENUM , "glCompressedTexImage3D");
          return ;              
     }
     
 
     //-------------------------------------------------------------------------
     // COPY TEXTURE  DATA
     //-------------------------------------------------------------------------
     
     GLenum type =   internalformat;
     
 
     DstTextureFormat = DetermineTextureFormat(internalformat, type, &srcTextureFormat);
     newImageSize = imageSize + NoOfpaletteEntry * (NewpaletteSize -OldpaletteSize) ;
 
     
     //FLUSHING THE CACHE
     FimgFinish(pState); 
 
     
         pImgBuf =  getImageDataLocation( pTexObj, level , 0,  width,  height , depth ,internalformat,internalformat,  newImageSize, 0);
         if(pImgBuf == NULL){
              gAssert(false && " unbale to get the memory location \n");
             return;
         }
 
         GLubyte* tempPixel = (GLubyte*)data;
 
     convertPixels(DstTextureFormat,(void*)pImgBuf, NoOfpaletteEntry, 1,0,0,0,0,
                   srcTextureFormat, (void*) tempPixel, NoOfpaletteEntry, 1,0,0,0,0,
                   NoOfpaletteEntry,1,0,0,0,1);  //paletteSize is the size of the table to be converted
     Plat::memcpy((pImgBuf+NoOfpaletteEntry * NewpaletteSize) ,
                 (tempPixel+NoOfpaletteEntry * OldpaletteSize) ,
                 imageSize -NoOfpaletteEntry * OldpaletteSize ); 
 
 #ifdef  CACHE_MEM
     pCA->cache_clean_invalid(pImgBuf, newImageSize);    
 #endif
         tempPixel = NULL;
 
     //-------------------------------------------------------------------------
     // SET IMAGE PARAMETERS
     //-------------------------------------------------------------------------
     pImgObj->isUsed = GL_TRUE;
     pImgObj->width = width;
     pImgObj->height = height;
     pImgObj->depth = depth;
     pImgObj->internalFormat = internalformat;
     pImgObj->PixType = internalformat;  
     pImgObj->imgSize = newImageSize;
     pImgObj->isCompressed = true;
     pImgObj->nativeFormat = texFormat;
 
     // Set recompile flag
     pTexObj->reCompile = GL_TRUE;
     pTexObj->isDirtyState = GL_TRUE;
     
     //fprintf(stderr,"\n ###  leaving 3D COMPRESSED TEXTURE ###");
 }
 
 
 
 /* no need as the palette texture sub image can't be specified and the 
   S3TC image can't be used with 3D texture image*/
  GL_API void GL_APIENTRY glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
 {
     OGLState* pState = GET_OGL_STATE();
     //TextureState* pTexState = &(pState->texState);
 
     switch(format){
         case GL_PALETTE4_RGB8_OES:  
         case GL_PALETTE4_RGBA8_OES:
         case GL_PALETTE4_R5_G6_B5_OES:
         case GL_PALETTE4_RGBA4_OES:
         case GL_PALETTE4_RGB5_A1_OES:
         case GL_PALETTE8_RGB8_OES:
         case GL_PALETTE8_RGBA8_OES:
         case GL_PALETTE8_R5_G6_B5_OES:
         case GL_PALETTE8_RGBA4_OES:
         case GL_PALETTE8_RGB5_A1_OES:
             SET_ERR(pState, GL_INVALID_OPERATION, "glCompressedTexSubImage3D");
             break;
         default:
             //to check what error to be set
             SET_ERR(pState, GL_INVALID_ENUM , "glCompressedTexSubImage3D");
             break;  
     }
 }
 
 
 GL_API void GL_APIENTRY glGenerateMipmap(GLenum target)
 {
     OGLState* pState = GET_OGL_STATE();
     //TextureState* pTexState = &(pState->texState);
     
     //GLuint texRef;                // Reference to texture object
     //GLenum texFormat;         // Internal texture format
     GLsizei pixelSize;          // Bytes per pixel
     GLubyte *pImgBufBase = NULL;    // Pointer to image buffer
     Image *pImgObj = NULL;      // Pointer to image object
     GLuint cubeFace = 0;        // Index of cube face refered
     TextureObject* pTexObj = NULL;
     
     if(target == GL_TEXTURE_2D){
         pTexObj = GetTextureObject(target,false,0);
 
     } else if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) {
         pTexObj = GetTextureObject(GL_TEXTURE_CUBE_MAP,false,0);
         cubeFace = (target - GL_TEXTURE_CUBE_MAP_POSITIVE_X);
 
     } else {
         SET_ERR(pState, GL_INVALID_ENUM, "glGenerateMipMap*");
         return;
     }
     
     if(target == GL_TEXTURE_2D){
         pImgObj = &(pTexObj->images.tex2D[0]);  //0 = base mipmap level....For this api base MipMap level should be defined.
 
     } else {    // Cubemap
         pImgObj = &(pTexObj->images.cubeMap[cubeFace][0]);
     }
     
     if(pImgObj->isUsed != GL_TRUE /*&& pImgObj->data != NULL */)  //in case base level is not defined...before calling this api
     {
         SET_ERR(pState, GL_INVALID_OPERATION, "glGenerateMipMap");
         return ;
     }
         
     if ( (pImgBufBase = (GLubyte*)pImgObj[0].hImgChunk->GetPhyAddr()) == NULL) //base level data
         {
             SET_ERR(pState, GL_OUT_OF_MEMORY, "glGenerateMipMap");
             return;
         }
     pixelSize = GetPixelSize(pImgObj[0].internalFormat,pImgObj[0].PixType);
     if(pixelSize <= 0) {
         return;
     }
     
     
     int maxDim = MAX(pImgObj->width,pImgObj->height);
     //int levels = log(maxDim)/log(2.0) + 1;
     int levels = 0; // levels will be log(maxDim)
     while (maxDim >>= 1) // unroll for more speed...
         levels++;
 
     void *pImgBuf = NULL;
     void* pImgPrevLev = NULL;
     PxFmt fmt = translateGLInternal(pImgObj[0].internalFormat,pImgObj[0].PixType);
 
     for(int i = 1; i <= levels; ++i)
     {
         pImgObj[i].isUsed = GL_TRUE;
         pImgObj[i].width = MAX(1,pImgObj[0].width >> i);
         pImgObj[i].height = MAX(1,pImgObj[0].height >> i);
         pImgObj[i].internalFormat = pImgObj[0].internalFormat;
         pImgObj[i].PixType = pImgObj[0].PixType;
         pImgObj[i].imgSize = pImgObj[i].width * pImgObj[i].height * pixelSize; //what about compressed format??.....
         pImgObj[i].isCompressed = pImgObj[0].isCompressed;
         pImgObj[i].hImgChunk = pCA->New(pImgObj[i].imgSize);
 
         if(pImgObj[i].hImgChunk == NULL || (pImgBuf = pImgObj[i].hImgChunk->GetVirtAddr()) == NULL)
         {
             SET_ERR(pState, GL_OUT_OF_MEMORY, "glGenerateMipMap");
             return;
         }
         
         if( (pImgPrevLev = pImgObj[i -1].hImgChunk->GetVirtAddr()) == NULL)
         {
             SET_ERR(pState, GL_OUT_OF_MEMORY, "glGenerateMipMap");
             return;
         }
 
         // Batch ConvertTO RGBA();
         // Take AVG 
         // ConvertFromRGBA
         genMipMaps(fmt, pImgPrevLev, pImgBuf, pImgObj[i].width, pImgObj[i].height,pImgObj[i-1].width, pImgObj[i-1].height); 
         SwapRB((GLubyte*)pImgBuf,pImgObj[i].internalFormat,pImgObj[i].PixType,pImgObj[i].width,pImgObj[i].height,0,0,0,0,pImgObj[i].width, pImgObj[i].height,0,pixelSize );
     }
 }
 GL_API void GL_APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params)
 {
     GET_GL_STATE(ctx);
     
     //GLuint texRef;
     TextureObject* pTexObj = NULL;
 
     
     if(target == GL_TEXTURE_2D || target == GL_TEXTURE_3D || target == GL_TEXTURE_CUBE_MAP){
         pTexObj = GetTextureObject(target,false,0);
     } else {
         SET_ERR(ctx, GL_INVALID_ENUM, "glGetTexParameter");
         return;
     }
     switch(pname)
     {
         case GL_TEXTURE_MIN_FILTER:
         (*params) =pTexObj->minFilter;
         break;
         
         case GL_TEXTURE_MAG_FILTER:
         (*params) =pTexObj->magFilter;
         break;
         
         case GL_TEXTURE_WRAP_S:
         (*params) = pTexObj->wrapS;
         break;
         
         case GL_TEXTURE_WRAP_T:
         (*params) = pTexObj->wrapT;
         break;
         
         case GL_TEXTURE_WRAP_R:
         (*params) = pTexObj->wrapR;
         break;
 
         //Texture OES API - 2009.05.20
       
         case GL_TEXTURE_CROP_RECT_OES:
             params[0] = pTexObj->crop_rect[0];
             params[1] = pTexObj->crop_rect[1];
             params[2] = pTexObj->crop_rect[2];
             params[3] = pTexObj->crop_rect[3];
         break;
         
         default:
         SET_ERR(ctx, GL_INVALID_ENUM, "glGetTexParameter");
         return;
     }
 }
 
 GL_API void GL_APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
 {
 
         glGetTexParameteriv(target,pname,(GLint*)params);
 }
 
 //Texture OES API - 2009.05.20
 //#ifdef GL_OES_draw_texture
 GL_API  void GL_APIENTRY glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params)
 {
     GET_GL_STATE(ctx);
     TextureObject* pTexObj = NULL;
     Image *pImgObj = NULL;
 
     if(target == GL_TEXTURE_2D){
         pTexObj = GetTextureObject(target,false,0);
     } else {
         SET_ERR(ctx, GL_INVALID_ENUM, "glGetTexLevelParameteriv");
         return;
     }
 
     //TODO validate the level value  and check whether pTexObj->levels is updated or not
     // Ask shariq
 
     /*if (level >= pTexObj->levels)
     {
         SET_ERR(ctx, GL_INVALID_VALUE, "glGetTexLevelParameteriv");
         return;
     }*/
 
     switch (pname)
     {
         case GL_TEXTURE_WIDTH:
         *params = pTexObj->images.tex2D[level].width;
         break;
         
         case GL_TEXTURE_HEIGHT:
         *params = pTexObj->images.tex2D[level].height;    
         break;
 
         case  GL_TEXTURE_INTERNAL_FORMAT:
         *params = pTexObj->images.tex2D[level].internalFormat;
         break;
 
         default:
         SET_ERR(ctx, GL_INVALID_ENUM, "glGetTexLevelParameteriv");
         return;    
     }
 }
 
 
 GL_API  void GL_APIENTRY glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params)
 {
 
 }
 
 //#endif //GL_OES_draw_texture
 
 
 /*
 *******************************************************************************
 * Non-GL API functions
 *******************************************************************************
 */
 
 /*-----------------------------------------------------------------------*//*!
 *
 *//*------------------------------------------------------------------------*/
 
 
 void
 InitLocalTextureState (OGLState* pState)
 {
     int i;
 
     TextureState* pTexState = &(pState->texState);
 
         for(i = 0; i < 3 ; i++) {
         pTexState->defaultTexObjects[i].reset();
     }
 
 
     //TO DO added as memset might add 0 to the set 
     //pTexState->defaultTexObjectses.insert(0);
     
     pTexState->defaultTexObjects[TEX_2D_DEFAULT - TEX_2D_DEFAULT].Init(TEX_2D_DEFAULT,GL_TEXTURE_2D);
     pTexState->defaultTexObjects[TEX_3D_DEFAULT - TEX_2D_DEFAULT].Init(TEX_3D_DEFAULT,GL_TEXTURE_3D);
     pTexState->defaultTexObjects[TEX_CUBE_MAP_DEFAULT - TEX_2D_DEFAULT].Init(TEX_CUBE_MAP_DEFAULT,GL_TEXTURE_CUBE_MAP);
 
     // Current selection
     pTexState->activeTexUnit = 0;
     // Texture units
     for(i = 0; i < MAX_TEXTURE_UNITS; i++) {
         
         pTexState->texUnitBinding[i].texture2D = 0;
         pTexState->ptexUnitBinding[i].texture2D = &(pTexState->defaultTexObjects[TEX_2D_DEFAULT - TEX_2D_DEFAULT]);
         pTexState->texUnitBinding[i].texture3D = 0;
         pTexState->ptexUnitBinding[i].texture3D = &(pTexState->defaultTexObjects[TEX_3D_DEFAULT - TEX_2D_DEFAULT]);
         pTexState->texUnitBinding[i].cubeMap = 0;
         pTexState->ptexUnitBinding[i].cubeMap = &(pTexState->defaultTexObjects[TEX_CUBE_MAP_DEFAULT - TEX_2D_DEFAULT]);
 
         //added for dirty state tracking
         pTexState->prevTexUnitBinding[i] = 0;
                 
         pTexState->pTexObjectToConfigFimg[i].id = 0;
         pTexState->pTexObjectToConfigFimg[i].pTexFGLState = NULL;
                
     }
 }
 
 
 void
 DeInitLocalTextureState(OGLState* pState)
 {
     
     releaseBoundTextures(pState);
     unsigned int i =0;
     
     //default texture object 
     pState->texState.defaultTexObjects[TEX_2D_DEFAULT - TEX_2D_DEFAULT].Delete();
     pState->texState.defaultTexObjects[TEX_3D_DEFAULT - TEX_2D_DEFAULT].Delete();
     pState->texState.defaultTexObjects[TEX_CUBE_MAP_DEFAULT - TEX_2D_DEFAULT].Delete();
 
     // Texture units
     pState->texState.activeTexUnit = 0;
     for(i = 0; i < MAX_TEXTURE_UNITS; i++) {
         
         pState->texState.texUnitBinding[i].texture2D = 0;
         pState->texState.ptexUnitBinding[i].texture2D = &(pState->texState.defaultTexObjects[TEX_2D_DEFAULT - TEX_2D_DEFAULT]);
         pState->texState.texUnitBinding[i].texture3D = 0;
         pState->texState.ptexUnitBinding[i].texture3D =  &(pState->texState.defaultTexObjects[TEX_3D_DEFAULT - TEX_2D_DEFAULT]);
         pState->texState.texUnitBinding[i].cubeMap = 0;
         pState->texState.ptexUnitBinding[i].cubeMap = &(pState->texState.defaultTexObjects[TEX_CUBE_MAP_DEFAULT - TEX_2D_DEFAULT]);
         pState->texState.prevTexUnitBinding[i] = 0;
 
         pState->texState.pTexObjectToConfigFimg[i].id= 0;
         pState->texState.pTexObjectToConfigFimg[i].pTexFGLState = NULL;
     }
 
 }
 
 
 
 /*
 *******************************************************************************
 * Texture object member functions
 *******************************************************************************
 */
 
 /*-----------------------------------------------------------------------*//*!
 *
 *//*------------------------------------------------------------------------*/
 
 inline void
 TextureObject::releaseMem()
 {
     if(isExtTex == GL_TRUE){
         pExtTexchunk.paddr = NULL;
         pExtTexchunk.vaddr = NULL; 
         isExtTex = GL_FALSE;        // to check 
     }else if(hChunk != NULL){
                 pCA->Free(hChunk);
                 hChunk = NULL;                  
     }
 }
 
 
 //called during initialization of the state
 void
 TextureObject::reset()
 {
 
         id = 0;
         texType = GLenum(0);
         isUsed = GL_FALSE;
         reCompile = GL_FALSE;
         minFilter = GLenum(0);
         magFilter = GLenum(0);
         wrapS = GLenum(0);
         wrapT = GLenum(0);
         wrapR = GLenum(0);
         internalFormat = GLenum(0);
         nativeFormat = E_INVALID_PIXEL_FORMAT;
         isDirtyState = GL_FALSE;
         for(int j = 0 ; j < MAX_MIPMAP_LEVELS; ++j)
             Offsets[j] = 0;
         Plat::memset(texFGLState.texSFRtate, 0, sizeof(texFGLState.texSFRtate));
         hChunk = NULL;
         width = 0;
         height = 0;
         depth =0;
         texFGLState.palEntryFGLState = NULL;
         texFGLState.noOfpalette = 0;
         Plat::memset(&(images), 0, sizeof(images));
         texObjrefCount = 0;
         deleteTexObj = GL_FALSE;
         pExtTexchunk.paddr = NULL;
         pExtTexchunk.vaddr = NULL; 
         isExtTex = GL_FALSE; 
 
 }
 
 
 
 //called when texture object is created
 void
 TextureObject::Init(GLint tex, GLenum type)
 {
     id = tex;
     texType = type;
     isUsed = GL_TRUE;
     reCompile = GL_TRUE;
 
 //    minFilter = GL_NEAREST_MIPMAP_LINEAR;             // default value according to spec
     minFilter = GL_LINEAR;                                              // for Android
     magFilter = GL_LINEAR;
     wrapS = GL_REPEAT;
     wrapT = GL_REPEAT;
     wrapR = GL_REPEAT;
     internalFormat = GLenum(-1);
     nativeFormat = E_INVALID_PIXEL_FORMAT;
     isDirtyState = GL_TRUE;
     texFGLState.fglStateDirty = GL_TRUE;
     texFGLState.noOfpalette = 0;
     texFGLState.palEntryFGLState =NULL;
     isExtTex = GL_FALSE;
     pExtTexchunk.paddr = NULL;
     pExtTexchunk.vaddr = NULL;
 
     for(int i = 0 ; i < MAX_MIPMAP_LEVELS; ++i)
         Offsets[i] = 0;
 
     deleteTexObj = GL_FALSE;
 
      switch(type)
     {
         case GL_TEXTURE_2D:
             for(int i = 0; i < MAX_MIPMAP_LEVELS; i++)  
             {
                 
                 images.tex2D[i].imagedataLocation = NONE;
             }
             break;
         case GL_TEXTURE_CUBE_MAP:
             for(int j = 0 ; j < 6; j++)
                 for(int i = 0; i < MAX_MIPMAP_LEVELS; i++)
                 {
                     
                     images.cubeMap[j][i].imagedataLocation = NONE;
                 
                 }
             break;
         case GL_TEXTURE_3D:
             for(int i = 0; i < MAX_MIPMAP_LEVELS; i++)
             {
                 images.tex3D[i].imagedataLocation = NONE;
                 
             }
             break;
     }
 
     
 #ifdef SHARED_CONTEXT_DEBUG 
     //LOGMSG(" id being init is = % d \n" , id);
 
 #endif  
     
 }
 
 
 /*-----------------------------------------------------------------------*//*!
 *
 *//*------------------------------------------------------------------------*/
 
 
 //called when the texture object is deleted or deinitailization of the state
 //TO DO: Now no need to reset teh vlaue only need to release any memory used
 void
 TextureObject::Delete()
 {
    
 #if 0
     if(!(texObjrefCount == 0) ||(texObjrefCount == -1)) 
         return;
 #endif
    
     isUsed = GL_FALSE;
     reCompile = GL_TRUE;
     isDirtyState = GL_TRUE;
     
     // resetting the size
     width = 0;
     height = 0;
     depth = 0;
     
 #ifdef SHARED_CONTEXT_DEBUG 
     //LOGMSG("\n id being deleted is = % d \n" , id);
 #endif
       
     //not needed added just to be sure
     minFilter = GL_NEAREST_MIPMAP_LINEAR;
     magFilter = GL_LINEAR;
     wrapS = GL_REPEAT;
     wrapT = GL_REPEAT;
     wrapR = GL_REPEAT;
     internalFormat = GLenum(-1);
     nativeFormat = E_INVALID_PIXEL_FORMAT;
     if (texFGLState.palEntryFGLState != NULL){
         Plat::safe_free(texFGLState.palEntryFGLState);
     }
     //Plat::memset((void *)texFGLState.palEntryFGLState, 0, sizeof(texFGLState.palEntryFGLState));
       
 #if 0    
     //freeing the texture memory
     if(hChunk != NULL)
     {
         pCA->Free(hChunk);
         hChunk = NULL;
     }
      pExtTexchunk.paddr = NULL;
     pExtTexchunk.vaddr = NULL;
 #else
     releaseMem();
 #endif
     isExtTex = GL_FALSE;
 
 
     switch(texType)
     {
         case GL_TEXTURE_2D:
             for(int i = 0; i < MAX_TEXTURE_UNITS; i++)  // or it should be isUsed as uncompiled texture may also be deleted. TO DO
             {
                 if(images.tex2D[i].hImgChunk != NULL){
                     pCA->Free(images.tex2D[i].hImgChunk);
                     images.tex2D[i].hImgChunk = NULL;
                 }
                 //Resetting the values of the image structure.
                 images.tex2D[i].imgSize = 0;
                 images.tex2D[i].isUsed = GL_FALSE;
                 images.tex2D[i].width = 0;
                 images.tex2D[i].height = 0;
                 images.tex2D[i].depth = 0;
                 //not needed added just to be sure
                 images.tex2D[i].internalFormat = GLenum(-1);
                 images.tex2D[i].PixType = GLenum(-1);
                 images.tex2D[i].nativeFormat = E_INVALID_PIXEL_FORMAT;
                 images.tex2D[i].isCompressed = GL_FALSE;
             }
             break;
         case GL_TEXTURE_CUBE_MAP:
             for(int j = 0 ; j < 6; j++)
                 for(int i = 0; i < MAX_TEXTURE_UNITS; i++)
                 {
                     if(images.cubeMap[j][i].hImgChunk != NULL){
                         pCA->Free(images.cubeMap[j][i].hImgChunk);
                         images.cubeMap[j][i].hImgChunk = NULL;
                     }
                     //Resetting the values of the image structure.
                     images.cubeMap[j][i].imgSize = 0;
                     images.cubeMap[j][i].isUsed = GL_FALSE;
                     images.cubeMap[j][i].width = 0;
                     images.cubeMap[j][i].height = 0;
                     images.cubeMap[j][i].depth = 0;
                     //not needed added just to be sure
                     images.cubeMap[j][i].internalFormat = GLenum(-1);
                     images.cubeMap[j][i].PixType = GLenum(-1);
                     images.cubeMap[j][i].nativeFormat = E_INVALID_PIXEL_FORMAT;
                     images.cubeMap[j][i].isCompressed = GL_FALSE;
                 
                 }
             break;
         case GL_TEXTURE_3D:
             for(int i = 0; i < MAX_TEXTURE_UNITS; i++)
             {
                 if(images.tex3D[i].hImgChunk != NULL){
                     pCA->Free(images.tex3D[i].hImgChunk);
                     images.tex3D[i].hImgChunk = NULL;
                 }
                 //Resetting the values of the image structure.
                 images.tex3D[i].imgSize = 0;
                 images.tex3D[i].isUsed = GL_FALSE;
                 images.tex3D[i].width = 0;
                 images.tex3D[i].height = 0;
                 images.tex3D[i].depth = 0;
                 //not needed added just to be sure
                 images.tex3D[i].internalFormat = GLenum(-1);
                 images.tex3D[i].PixType = GLenum(-1);
                 images.tex3D[i].nativeFormat = E_INVALID_PIXEL_FORMAT;
                 images.tex3D[i].isCompressed = GL_FALSE;
                 
             }
             break;
     }
     // Free image pointer
     // Free each individual image
     deleteTexObj = GL_FALSE;
     levels = 0;
     texType = GLenum(-1);
       
           
 }
 
 
 
 void 
 inline TextureObject::acquire(OGLState* pState)
 {
 
     if(id ==  0){
         //LOGMSG(" zero can not be released . should have have entered the function : %s \n"  , __FUNCTION__);
         return;
     }
 
     if(deleteTexObj == GL_TRUE){
         //LOGMSG("  \n trying to acqire a texture object marked for deletion . ISSUE UNRESOLVED ");
     }
    
     gAssert(pState->sharedState->sharedStateMutexAcquired && "mutex should have been acquired");
     texObjrefCount++;
     
 #ifdef SHARED_CONTEXT_DEBUG
     //LOGMSG(" refCount of %d is % d : %s \n" , id , texObjrefCount , __FUNCTION__);
 #endif
 
 }
 
 
 GLboolean
 inline TextureObject::release(OGLState* pState)
 {
 
     if(id ==  0){
         //LOGMSG(" zero can not be released . should have have entered the function : %s \n"  , __FUNCTION__);
         return GL_FALSE;
     }
       
     gAssert(pState->sharedState->sharedStateMutexAcquired && "mutex should have been acquired");
     texObjrefCount--;
     //gAssert(texObjrefCount >= -1 && " ref count cannot be less than zero. " );
           
 #ifdef SHARED_CONTEXT_DEBUG
     //LOGMSG(" refCount of %d is % d :  %s  \n" , id , texObjrefCount , __FUNCTION__);
 #endif
     if((texObjrefCount < 0)   &&  (  deleteTexObj == GL_TRUE))
     {
   
         // Delete texture object
         if(isUsed == GL_TRUE){
             Delete();
         }
           
        
         return GL_TRUE;
 
     }
 
     return GL_FALSE;
 
 }
 
 
 
 
 
 /*-----------------------------------------------------------------------*//*!
 *
 *//*------------------------------------------------------------------------*/
 
 GLboolean
 TextureObject::IsComplete(int*  levels)
 {
     GLint currWidth = 1, currHeight = 1, currDepth = 1;
 
         int i =0;
     //-------------------------------------------------------------------------
     // Check texture object state
     //-------------------------------------------------------------------------
     if(isUsed != GL_TRUE)
         return GL_FALSE;
 
     // Texture type
     if(!(texType == GL_TEXTURE_2D || texType == GL_TEXTURE_3D || texType == GL_TEXTURE_CUBE_MAP))
         return GL_FALSE;
 
     // Minification and Magnification filters
 
     if(!((minFilter == GL_LINEAR) || (minFilter == GL_NEAREST) || (minFilter == GL_LINEAR_MIPMAP_LINEAR)
         || (minFilter == GL_LINEAR_MIPMAP_NEAREST) || (minFilter == GL_NEAREST_MIPMAP_LINEAR) ||
         (minFilter == GL_NEAREST_MIPMAP_NEAREST)))
         return GL_FALSE;
 
     if(!((magFilter == GL_LINEAR) || (magFilter == GL_NEAREST)))
         return GL_FALSE;
 
     // Wrap modes
 
     if(!((wrapS == GL_REPEAT) || (wrapS == GL_CLAMP_TO_EDGE) || (wrapS == GL_MIRRORED_REPEAT)))
         return GL_FALSE;
 
     if(!((wrapT == GL_REPEAT) || (wrapT == GL_CLAMP_TO_EDGE) || (wrapT == GL_MIRRORED_REPEAT)))
         return GL_FALSE;
 
     if(texType == GL_TEXTURE_3D && !((wrapR == GL_REPEAT) || (wrapR == GL_CLAMP_TO_EDGE) || (wrapR == GL_MIRRORED_REPEAT)))
         return GL_FALSE;
 
     //-------------------------------------------------------------------------
     // Check image format and specifications
     //-------------------------------------------------------------------------
 
     switch (texType) {
 
     case GL_TEXTURE_2D:
         if(images.tex2D[0].isUsed != GL_TRUE)
             return GL_FALSE;
 
         currWidth = images.tex2D[0].width;
         currHeight = images.tex2D[0].height;
         break;
 
     case GL_TEXTURE_3D:
         if(images.tex3D[0].isUsed != GL_TRUE)
             return GL_FALSE;
 
         currWidth = images.tex3D[0].width;
         currHeight = images.tex3D[0].height;
         currDepth = images.tex3D[0].depth;
         break;
 
     case GL_TEXTURE_CUBE_MAP:
         if(images.cubeMap[0][0].isUsed != GL_TRUE)
             return GL_FALSE;
 
         // Check base level cube-map images
 
         if(images.cubeMap[0][0].width != images.cubeMap[0][0].height)
             return GL_FALSE;
 
         for(i = 0; i < 6; i++) {
             if((images.cubeMap[i][0].internalFormat != images.cubeMap[0][0].internalFormat) ||
                 (images.cubeMap[i][0].width != images.cubeMap[0][0].width) ||
                 (images.cubeMap[i][0].height != images.cubeMap[0][0].height)) {
                 return GL_FALSE;
             }
         }
 
         currWidth = images.cubeMap[0][0].width;
         currHeight = images.cubeMap[0][0].height;
 
         break;
     }
 
 #if(OES_TEXTURE_NPOT == DISABLE)
     //-------------------------------------------------------------------------
     // Check Power of Two dimensions
     //-------------------------------------------------------------------------
 
     if(IsPowerOf2(currWidth) != GL_TRUE || IsPowerOf2(currHeight) != GL_TRUE ||
         (texType == GL_TEXTURE_3D && IsPowerOf2(currDepth) != GL_TRUE)) {
 
         // Minification: Nearest of Linear
         if(!(minFilter == GL_NEAREST || minFilter == GL_LINEAR))
             return GL_FALSE;
 
         // Wrap: Clamp to edge
         if(wrapS != GL_CLAMP_TO_EDGE || wrapT != GL_CLAMP_TO_EDGE ||
             (texType == GL_TEXTURE_3D && wrapR != GL_CLAMP_TO_EDGE))
             return GL_FALSE;
 
     }
 #endif
 
     //-------------------------------------------------------------------------
     // Check dimensions of each mip-map level
     //-------------------------------------------------------------------------
            int level =1;        //will determine the level to be used
     if((minFilter == GL_LINEAR_MIPMAP_LINEAR) || (minFilter == GL_LINEAR_MIPMAP_NEAREST) ||
         (minFilter == GL_NEAREST_MIPMAP_LINEAR) || (minFilter == GL_NEAREST_MIPMAP_NEAREST)) {
 
         for( ; level < MAX_MIPMAP_LEVELS; level++) {
                      
             // Check complete if we reach the level-max
             if(texType == GL_TEXTURE_3D){
                 if(currWidth == 1 && currHeight == 1 && currDepth == 1)
                 break;
 
             } else {
                 if(currWidth == 1 && currHeight == 1)
                 break;
             }
 
             // Current dimension (Divide by 2). Clamp to  a lower limit of 1
             currWidth = ((currWidth / 2) > 1) ? (currWidth / 2) : 1;
             currHeight = ((currHeight / 2) > 1) ? (currHeight / 2) : 1;
             currDepth = ((currDepth / 2) > 1) ? (currDepth / 2) : 1;
 
             switch (texType) {
 
             case GL_TEXTURE_2D:
                 if(images.tex2D[level].isUsed != GL_TRUE || images.tex2D[level].internalFormat != images.tex2D[0].internalFormat ||
                     images.tex2D[level].width != currWidth || images.tex2D[level].height != currHeight) {
                                         return GL_FALSE;
                 }
                 break;
 
             case GL_TEXTURE_3D:
                 if(images.tex3D[level].isUsed != GL_TRUE || images.tex3D[level].internalFormat != images.tex3D[0].internalFormat ||
                     images.tex3D[level].width != currWidth || images.tex3D[level].height != currHeight || images.tex3D[level].depth != currDepth) {
                     return GL_FALSE;
                 }
                 break;
 
             case GL_TEXTURE_CUBE_MAP:
                 if(images.cubeMap[0][level].isUsed != GL_TRUE || images.cubeMap[0][level].internalFormat != images.cubeMap[0][0].internalFormat ||
                     images.cubeMap[0][level].width != currWidth || images.cubeMap[0][level].height != currHeight) {
                     return GL_FALSE;
                 }
                 break;
             }
         }
 
         // We were finished with all mipmap levels before reaching the smallest image (1 x 1 x 1)
         if(level > MAX_MIPMAP_LEVELS){
             return GL_FALSE;
         }     
                 
     }
 
     // All tests are complete without failure
     *levels = level;
     return GL_TRUE;
 }
 
 
 /*-----------------------------------------------------------------------*//*!
 *
 *//*------------------------------------------------------------------------*/
 
 GLboolean
 TextureObject::Compile ()
 {
     GLubyte *pImgBuf = NULL;            // Image buffer
     GLuint imgSize, pixSize;            // Image size, Pixel size
     int i = 0, j = 0;                   // Temporary counter
         int level = 0;
      
     // Return if texture object state has not changed.
     if(isDirtyState == GL_FALSE) {  //fi dirty state is false the recompile will be false
         return GL_TRUE;
     }
 
 
      // As the object state has changed check the texture completeness
     if(IsComplete(&level) != GL_TRUE){
         return GL_FALSE;
     }
 
     //-------------------------------------------------------------------------
     // Image buffer handling
     //-------------------------------------------------------------------------
     if(reCompile == GL_TRUE){
 
           if( hChunk != NULL){
             transferImageFromTexChunckToImageChunck(this);
           }
                
 
         //-------------------------------------------------------------------------
        // Compute texture parameters and compile various images into a single image
        //-------------------------------------------------------------------------     
         switch (texType){
 
             case GL_TEXTURE_2D:
 
                 //  parameters
                 width = images.tex2D[0].width;
                 height = images.tex2D[0].height;
                 internalFormat = images.tex2D[0].internalFormat;
                 nativeFormat = images.tex2D[0].nativeFormat; 
                 levels = level;
                 
                 // *Pra* If loop completes without a break .. It is an error! .. But this should never happen in
                 // normal case of isComplete()
                 if(!images.tex2D[0].isCompressed)
                 {
                                    
 
 
                      //TO DO IF LEVEL IS 1 THEN NEED TO USE THAT TEXTURE IMAGE ONLY SAME FOR 3D
                     if( levels == 1){
                           hChunk =   images.tex2D[0].hImgChunk;
                           images.tex2D[0].hImgChunk = NULL;
                           images.tex2D[0].imagedataLocation = TEXOBJCHUNK;
                         
                     }else{
 
                         // Pixel size
                         pixSize = pixelSize(nativeFormat);
 
                         // Allocate memory chunk for the compiled image
                         hChunk = pCA->New(CompiledSize());
                         if(hChunk == NULL || (pImgBuf = (GLubyte*) hChunk->GetVirtAddr()) == NULL) {
                             //LOGMSG("Unable to get new chunk\n");
                             return GL_FALSE;
                         }
                         
                          //offset
                         for(int j=1; j < levels; j++){
                             Offsets[j]=Offsets[j-1]+ (images.tex2D[j-1].width * images.tex2D[j-1].height);
                                 
                         }
                         
                         // Copy images together
                         for(i = 0; i < levels; i++) {
                             imgSize = images.tex2D[i].width * images.tex2D[i].height * pixSize;
                             Plat::memcpy( pImgBuf , images.tex2D[i].hImgChunk->GetVirtAddr(), imgSize );
                             
                             pCA->Free(images.tex2D[i].hImgChunk);
                              // Increment pointer
                             pImgBuf += imgSize;
 
                             images.tex2D[i].hImgChunk = NULL;                                        
                             images.tex2D[i].imagedataLocation = TEXOBJCHUNK;
                         }
                     }
                     
                 }
             else
             { 
                 //compressed images...all the mipmap levels are in the data..it at all they are there. So no compiling is required.??????
                 // parameters
                 //HOW TO SET THE OFFSET TO DO
 
                 hChunk = pCA->New(images.tex2D[0].imgSize);                    
                 if(hChunk == NULL || (pImgBuf = (GLubyte*) hChunk->GetVirtAddr()) == NULL) {
                     //LOGMSG("Unable to get new chunk\n");
                     return GL_FALSE;
                 }
 
                 Plat::memcpy( pImgBuf , images.tex2D[0].hImgChunk->GetVirtAddr(), images.tex2D[0].imgSize);
                 pCA->Free(images.tex2D[0].hImgChunk);
                 images.tex2D[0].hImgChunk = NULL;
                  images.tex2D[0].imagedataLocation = TEXOBJCHUNK;
                 //images.tex2D[0].isCompressed = false;
             }
             break;
 
         case GL_TEXTURE_CUBE_MAP:
                  //  parameters
                 width = images.cubeMap[0][0].width;
                 height = images.cubeMap[0][0].height;
                 internalFormat = images.cubeMap[0][0].internalFormat;
                 nativeFormat = images.cubeMap[0][0].nativeFormat; //added 
                 levels = level;
             
                 if(!images.cubeMap[0][0].isCompressed)
                 {
                                 
                     // Pixel size
                     pixSize = pixelSize(nativeFormat);
 
                     // Allocate memory chunk for the compiled imagep
                     hChunk = pCA->New(CompiledSize());
                     if(hChunk == NULL || (pImgBuf = (GLubyte*) hChunk->GetVirtAddr()) == NULL) {
                             //LOGMSG("Unable to get new chunk\n");
                             return GL_FALSE;
                     }
                     
                     GLuint offSetSize =0;
 
                     //offset
                     for(i = 1; i < levels; i++) {
                             Offsets[i] = Offsets[i-1]+ (images.cubeMap[0][0].width >> (i-1) )* (images.cubeMap[0][0].height >> (i-1));
                             //LOGMSG("Offsets[%d] = %d \n", i, Offsets[i]);
                     }
 
 
                     //calculation for mipmap offset for storing Image data as In case of cubemap
                     // mipmap should be enabled. And for each face data should be stored in mipmap aligned memory not
                     // in continous order.
                     GLubyte *pfaceStartBuf = pImgBuf;
                     GLuint twidth = width;
                     GLuint toffset = 0;
 
                     for( ; twidth >= 1 ; twidth>>=1 )
                     {
                         toffset +=  twidth * twidth * pixSize;
 
                     }
                         
                     // Copy images together
                     for(j = 0; j < 6; j++) {
                         for(i = 0; i < levels; i++) {
 
                             imgSize = images.cubeMap[j][i].width  * images.cubeMap[j][i].height * pixSize;
                             offSetSize = (images.cubeMap[j][0].width >> i)  * (images.cubeMap[j][0].height >> i) * pixSize;
                             
                             //LOGMSG("ImageSize[Level = %d] = %d \n",i,imgSize);
                             //LOGMSG("offSetSize[Level = %d] = %d \n",i,offSetSize);
                             //LOGMSG("pImgBuf = %p\n",pImgBuf);
                             Plat::memcpy( pImgBuf , images.cubeMap[j][i].hImgChunk->GetVirtAddr() , imgSize);
                             pCA->Free(images.cubeMap[j][i].hImgChunk);
                             images.cubeMap[j][i].hImgChunk = NULL;
                             images.cubeMap[j][i].imagedataLocation = TEXOBJCHUNK; 
                                                 
                             // Increment pointer
                             pImgBuf += offSetSize;
                                                  
                         }
                          pImgBuf = pfaceStartBuf + ( j+1)  *  toffset;
                     }
                     fprintf(stderr, "\n image compiled ");
                             
                 }
             else
             {   
 
                 //level is already set in the glCompressed...... call.????
                 // Allocate buffer
                 hChunk = pCA->New(images.cubeMap[0][0].imgSize);
                 if(hChunk == NULL || (pImgBuf = (GLubyte*) hChunk->GetVirtAddr()) == NULL) {
                     //LOGMSG("Unable to get new chunk\n");
                     return GL_FALSE;
                 }
 
                         Plat::memcpy( pImgBuf /* Dst */, images.cubeMap[0][0].hImgChunk->GetVirtAddr()/*images.cubeMap[0][0].data  Src */, images.cubeMap[0][0].imgSize /* Size */);
                         pCA->Free(images.cubeMap[0][0].hImgChunk);
                         images.cubeMap[0][0].hImgChunk = NULL;
                            images.cubeMap[0][0].imagedataLocation = TEXOBJCHUNK; 
             }
             
             break;
 
         case GL_TEXTURE_3D:
                  //  parameters
                 width = images.tex3D[0].width;
                 height = images.tex3D[0].height;
                 depth = images.tex3D[0].depth;
                 internalFormat = images.tex3D[0].internalFormat;
                 nativeFormat = images.tex3D[0].nativeFormat; 
                 levels = level;
              
                 if(!images.tex3D[0].isCompressed)
                 {
                                 
                     if(levels == 1){
                    
                           hChunk = images.tex3D[0].hImgChunk;
                           images.tex3D[0].hImgChunk = NULL;
                           images.tex3D[0].imagedataLocation = TEXOBJCHUNK;
                                 
                      }else{
 
                         // Pixel size
                         pixSize = pixelSize(nativeFormat);
 
                         // Allocate memory chunk for the compiled image
                         hChunk = pCA->New(CompiledSize());
                         if(hChunk == NULL || (pImgBuf = (GLubyte*) hChunk->GetVirtAddr()) == NULL) {
                             //LOGMSG("Unable to get new chunk\n");
                             return GL_FALSE;
                         }
                         //offset
                         for(i = 1; i < levels; i++){
                             Offsets[i] = Offsets[i-1]+ (images.cubeMap[0][0].width >> (i-1) )* (images.cubeMap[0][0].height >> (i-1));
                             //LOGMSG("Offsets[%d] = %d \n", i, Offsets[i]);
                         }
 
                         // Copy images together
                         for(i = 0; i < levels; i++) {
                             imgSize = images.tex3D[i].width * images.tex3D[i].height * images.tex3D[i].depth * pixSize;
                             Plat::memcpy( pImgBuf , images.tex3D[i].hImgChunk->GetVirtAddr(), imgSize );
                             pCA->Free(images.tex3D[i].hImgChunk);
                             images.tex3D[i].hImgChunk = NULL;
                              // Increment pointer
                              pImgBuf += imgSize;
                                                    
                              images.tex3D[i].imagedataLocation = TEXOBJCHUNK; 
                         }
                    }
                 }
             else
             {
                 width = images.tex3D[0].width;
                 height = images.tex3D[0].height;
                 depth = images.tex3D[0].depth;
                 internalFormat = images.tex3D[0].internalFormat;
                 nativeFormat = images.tex3D[0].nativeFormat; 
                 //  levels has been set in the glCompressed..... call only.???
                 hChunk = pCA->New(images.tex3D[0].imgSize);
                 if(hChunk == NULL || (pImgBuf = (GLubyte*) hChunk->GetVirtAddr()) == NULL) {
                     //LOGMSG("Unable to get new chunk\n");
                     return GL_FALSE;
                 }
                             ////LOGMSG("pImageBuf %p imgsize %d \n",pImgBuf,images.tex3D[0].imgSize);
                             ////LOGMSG("virtAddr %p \n",images.tex3D[0].hImgChunk->GetVirtAddr());
                 Plat::memcpy( pImgBuf , images.tex3D[0].hImgChunk->GetVirtAddr(), images.tex3D[0].imgSize);
                 pCA->Free(images.tex3D[0].hImgChunk);
                 images.tex3D[0].hImgChunk = NULL;
                              images.tex3D[i].imagedataLocation = TEXOBJCHUNK; 
             }
             break;
         }
 
         // Compile done. Mark flag
         reCompile = GL_FALSE;
      }
 
     if(updateTexFGLState() == GL_FALSE){
         return GL_FALSE;
     }
     
     return GL_TRUE;
 }
 
 
 GLboolean 
 TextureObject:: updateTexFGLState()
 {
     
     FGL_TexStatusParams texParams = {
             FGL_TEX_2D,
             FGL_CKEY_DISABLE,
             FGL_TRUE, // Zero-padding
             FGL_PALETTE_ARGB8888,
             FGL_TEXEL_ARGB1555,
             FGL_TEX_WRAP_REPEAT,
             FGL_TEX_WRAP_REPEAT,
             FGL_FALSE,
             FGL_FALSE,
             FGL_FALSE,
             FGL_FILTER_DISABLE };
 
         if(1 == 1){
         switch(minFilter) 
         {                   
             case GL_LINEAR:
                 texParams.bUseMinFilter = FGL_TRUE;
                 texParams.eMipMapFilter = FGL_FILTER_DISABLE;
                 break;
 
             case GL_NEAREST:
                 texParams.bUseMinFilter = FGL_FALSE;
                 texParams.eMipMapFilter = FGL_FILTER_DISABLE;
                 break;
 
             case GL_LINEAR_MIPMAP_LINEAR:
                 texParams.bUseMinFilter = FGL_TRUE;
                 texParams.eMipMapFilter = FGL_FILTER_LINEAR;
                 break;
 
             case GL_LINEAR_MIPMAP_NEAREST:
                 texParams.bUseMinFilter = FGL_TRUE;
                 texParams.eMipMapFilter = FGL_FILTER_NEAREST;
                 break;
 
             case GL_NEAREST_MIPMAP_LINEAR:
                 texParams.bUseMinFilter = FGL_FALSE;
                 texParams.eMipMapFilter = FGL_FILTER_LINEAR;
                 break;
 
             case GL_NEAREST_MIPMAP_NEAREST:
                 texParams.bUseMinFilter = FGL_FALSE;
                 texParams.eMipMapFilter = FGL_FILTER_NEAREST;
                 break;
         }
 
         switch(magFilter)
         {
             case GL_NEAREST:
                 texParams.bUseMagFilter = FGL_FALSE;
                 break;
 
             case GL_LINEAR:
                 texParams.bUseMagFilter = FGL_TRUE;
         }
 
                 //GLenum pType,pFormat;
                 PxFmt texFormat=E_INVALID_PIXEL_FORMAT; // texture format to be used 
 
             // Texture type
             switch(texType) 
             {       
                 case GL_TEXTURE_2D:
                     texParams.eType = FGL_TEX_2D;
                     //pType = images.tex2D[0].PixType;
                     //pFormat = images.tex2D[0].internalFormat;
                     texFormat =   nativeFormat;      //images.tex2D[0].nativeFormat;  //nativeFormat TO DO
                     break;
 
                 case GL_TEXTURE_3D:
                     texParams.eType = FGL_TEX_3D;
                     // pType = images.tex3D[0].PixType;
                     //pFormat = images.tex3D[0].internalFormat;
                     texFormat = nativeFormat; //images.tex3D[0].nativeFormat;
                     break;
 
                 case GL_TEXTURE_CUBE_MAP:
                     texParams.eType = FGL_TEX_CUBE;
                     texParams.eMipMapFilter = FGL_FILTER_NEAREST;
                     //pType = images.cubeMap[0][0].PixType;
                    // pFormat = images.cubeMap[0][0].internalFormat;
                     texFormat = nativeFormat; //images.cubeMap[0][0].nativeFormat;
                     break;
             }
                 
                 int offsetFromHeader = 0;
                 unsigned short numOfPaletteEntry = 0;
                 
             switch(texFormat)
             {
                 
                 case E_LUMINANCE8:
                     texParams.eFormat = FGL_TEXEL_I8;
                 break;
                 
                 case E_LUMINANCE_ALPHA88:           //check the format 
                     texParams.eFormat = FGL_TEXEL_IA88;
                 break;
                 case E_LUMINANCE_ALPHA80:           //check the format 
                     texParams.eFormat = FGL_TEXEL_IA88;
                 break;
                 case E_LUMINANCE_ALPHA08:
                     texParams.eFormat = FGL_TEXEL_IA88;
                 break;
                 
                 case E_ARGB8:
                     texParams.eFormat = FGL_TEXEL_ARGB8888;
                 break;
                     
                 case E_ARGB4:
                     texParams.eFormat = FGL_TEXEL_ARGB4444;
                 break;
                 
                 case E_RGB565:
                     texParams.eFormat = FGL_TEXEL_RGB565;
                 break;
                 
                 case E_ARGB1555:
                     texParams.eFormat = FGL_TEXEL_ARGB1555;
                 break;
 
 
                 // to see for all these case later 
                 case E_RGB_S3TC_OES:
                 case E_RGBA_S3TC_OES:
                 texParams.eFormat = FGL_TEXEL_S3TC;
                 break;
                 
                 case E_PALETTE4_RGB8_OES:
                     texParams.eFormat = FGL_TEXEL_4BPP;
                     texParams.ePaletteFormat = FGL_PALETTE_ARGB8888;                    
                     numOfPaletteEntry = 16;
                     offsetFromHeader = numOfPaletteEntry * 3; //this is the byte offset from the start of a palette image. this header is the palette for the image.
                 break;          
                 
                 case E_PALETTE4_RGBA8_OES:
                     texParams.eFormat = FGL_TEXEL_4BPP;
                     texParams.ePaletteFormat = FGL_PALETTE_ARGB8888;
                     numOfPaletteEntry = 16;
                     offsetFromHeader = numOfPaletteEntry * 4; //this is the byte offset from the start of a palette image. this header is the palette for the image.                    
                 break;              
                 case E_PALETTE4_R5_G6_B5_OES:
                     texParams.eFormat = FGL_TEXEL_4BPP;
                     texParams.ePaletteFormat = FGL_PALETTE_RGB565;
                     numOfPaletteEntry = 16;
                     offsetFromHeader = numOfPaletteEntry * 2; //this is the byte offset from the start of a palette image. this header is the palette for the image.
                 break;              
                 case E_PALETTE4_RGBA4_OES:
                     texParams.eFormat = FGL_TEXEL_4BPP;
                     texParams.ePaletteFormat = FGL_PALETTE_ARGB4444;
                     numOfPaletteEntry = 16;
                     offsetFromHeader = numOfPaletteEntry * 2; //this is the byte offset from the start of a palette image. this header is the palette for the image.
                 break;              
                 case E_PALETTE4_RGB5_A1_OES:
                     texParams.eFormat = FGL_TEXEL_4BPP;
                     texParams.ePaletteFormat = FGL_PALETTE_ARGB1555;
                     numOfPaletteEntry = 16;
                     offsetFromHeader = numOfPaletteEntry * 2; 
                 break;              
                 
                 case E_PALETTE8_RGB8_OES:
                     texParams.eFormat = FGL_TEXEL_8BPP;
                     texParams.ePaletteFormat = FGL_PALETTE_ARGB8888;
                     numOfPaletteEntry = 256;
                     offsetFromHeader = numOfPaletteEntry * 3; 
                 break;              
                 case E_PALETTE8_RGBA8_OES:
                     texParams.eFormat = FGL_TEXEL_8BPP;
                     texParams.ePaletteFormat = FGL_PALETTE_ARGB8888;                    
                     numOfPaletteEntry = 256;
                     offsetFromHeader = numOfPaletteEntry * 4; 
                 break;              
                 case E_PALETTE8_R5_G6_B5_OES:          
                     texParams.eFormat = FGL_TEXEL_8BPP; 
                     texParams.ePaletteFormat = FGL_PALETTE_RGB565;
                     numOfPaletteEntry = 256;
                     offsetFromHeader = numOfPaletteEntry * 2; 
                     break;
                 case E_PALETTE8_RGBA4_OES:         
                     texParams.eFormat = FGL_TEXEL_8BPP; 
                     texParams.ePaletteFormat = FGL_PALETTE_ARGB4444;                    
                     numOfPaletteEntry = 256;
                     offsetFromHeader = numOfPaletteEntry * 2;                   
                 break;              
                 case E_PALETTE8_RGB5_A1_OES:
                     texParams.eFormat = FGL_TEXEL_8BPP;
                     texParams.ePaletteFormat = FGL_PALETTE_ARGB1555;
                     numOfPaletteEntry = 256;
                     offsetFromHeader = numOfPaletteEntry * 2; 
                 break;
                                         
                 default:
                     texParams.eFormat = FGL_TEXEL_ARGB8888;
             }
 
             texParams.eUMode = TexWrapMode(wrapS);  // Wrap
             texParams.eVMode = TexWrapMode(wrapT);
 
         if(texFGLState.palEntryFGLState)
                         Plat::safe_free(texFGLState.palEntryFGLState);
             
 #if 1
 
             texFGLState.noOfpalette = numOfPaletteEntry;
             if(texFGLState.noOfpalette != 0){   
                 unsigned int* pPaletteColor = (unsigned int*)hChunk->GetVirtAddr();
                 GLuint  * tempFGLState = texFGLState.palEntryFGLState = (GLuint* )Plat::malloc(numOfPaletteEntry * sizeof(GLuint ));
                 for(unsigned int x=0; x < texFGLState.noOfpalette; x++)
                 {
                     WRITEMEM(tempFGLState++, *pPaletteColor++); // paletted color
                 }
             
             }
 
 #endif
                 
             setTexStatusParam(texFGLState.texSFRtate, &texParams);             // Set texture status
 
             setTexUSize(texFGLState.texSFRtate, width);
             setTexVSize(texFGLState.texSFRtate, height);
             if(texParams.eType == FGL_TEX_3D) {
                 setTexPSize( texFGLState.texSFRtate, depth);
             }
 
 
             if(texParams.eMipMapFilter != FGL_FILTER_DISABLE) {
                 unsigned int maxLev;
                 if(texParams.eFormat == FGL_TEXEL_S3TC) {
                     calculateMipmapOffsetS3TC(texFGLState.texSFRtate,width, height, &maxLev);
                 }
                 else if(texParams.eFormat >= FGL_TEXEL_Y1VY0U) {
                     calculateMipmapOffsetYUV(texFGLState.texSFRtate, width, height, &maxLev);
                 }
                 else {
                     calculateMipmapOffset(texFGLState.texSFRtate, width, height, &maxLev);
                 }
 
                 setTexMipmapLevel(texFGLState.texSFRtate, FGL_MIPMAP_MIN_LEVEL, 0);
                 setTexMipmapLevel(texFGLState.texSFRtate, FGL_MIPMAP_MAX_LEVEL, levels - 1);
             }
            if(isExtTex == GL_TRUE){
                 setTexBaseAddr (texFGLState.texSFRtate, (unsigned int) pExtTexchunk.paddr);
                // fprintf(stderr, " line = %d func =%s \n" , __LINE__ , __FUNCTION__);
            }else{
                 setTexBaseAddr (texFGLState.texSFRtate, (unsigned int) hChunk->GetPhyAddr() + offsetFromHeader);
             }
 
             isDirtyState = GL_FALSE;
             //to set the bit so that the FGL registers are updated in glfSetTexture
             texFGLState.fglStateDirty = GL_TRUE;
             
                             
             } else {
             //-----------------------------------------------------------------
             // Vertex texture
             //-----------------------------------------------------------------
             return GL_FALSE;
         }
     return GL_TRUE;
 }
 
 
 
 
 /*-----------------------------------------------------------------------*//*!
 * Calculate the compiled data size for image object
 *//*------------------------------------------------------------------------*/
 
 GLsizei
 TextureObject::CompiledSize()
 {
     switch (texType){
         case GL_TEXTURE_2D:
             return (GLsizei)(width * height * 1.5 * GetPixelSize(internalFormat,images.tex2D[0].PixType));
 
         case GL_TEXTURE_CUBE_MAP:
             return (GLsizei)(width * height * 6 * 1.5 * GetPixelSize(internalFormat,images.cubeMap[0][0].PixType));
 
         case GL_TEXTURE_3D:
             return (GLsizei)(width * height * depth * GetPixelSize(internalFormat,images.tex3D[0].PixType));
                     // *Pra* No mipmapping in 3D textures as of now
         default:
             return 0;
     };
 }
 
 
 phyVirAddress
 TextureObject::GetTex2DMipLevel(GLint level)
 {
     unsigned int offset = 0;
     phyVirAddress phyAddVirAdd;
         
     if(hChunk != NULL){
         if(level >= levels){
             phyAddVirAdd.phyAdress = NULL;
             phyAddVirAdd.virAddress = NULL;
         }else{
             offset = Offsets[level] * pixelSize(nativeFormat);
             phyAddVirAdd.phyAdress = ((GLubyte*)hChunk->GetPhyAddr()) + offset;
             phyAddVirAdd.virAddress =  ((GLubyte*)hChunk->GetVirtAddr()) + offset;
             
         }
     }else{
         if(images.tex2D[level].hImgChunk != NULL){
             phyAddVirAdd.phyAdress  = images.tex2D[level].hImgChunk->GetPhyAddr();
             phyAddVirAdd.virAddress = images.tex2D[level].hImgChunk->GetVirtAddr();
             
         }else{
             phyAddVirAdd.phyAdress = NULL;
             phyAddVirAdd.virAddress = NULL; 
         }
     }
     return phyAddVirAdd;
 }
 
 phyVirAddress
 TextureObject::GetTex3DMipLevel(GLint level)
 {
     unsigned int offset = 0;
     phyVirAddress phyAddVirAdd;
     
     if(hChunk != NULL){
         if(level >= levels){
             phyAddVirAdd.phyAdress = NULL;
             phyAddVirAdd.virAddress = NULL;
         }else{
             offset = Offsets[level] * pixelSize(nativeFormat);
             phyAddVirAdd.phyAdress = ((GLubyte*)hChunk->GetPhyAddr()) + offset;
             phyAddVirAdd.virAddress =  ((GLubyte*)hChunk->GetVirtAddr()) + offset;
             
         }
     }else{
         if(images.tex3D[level].hImgChunk != NULL){
             phyAddVirAdd.phyAdress  = images.tex3D[level].hImgChunk->GetPhyAddr();
             phyAddVirAdd.virAddress = images.tex3D[level].hImgChunk->GetVirtAddr();
             
         }else{
             phyAddVirAdd.phyAdress = NULL;
             phyAddVirAdd.virAddress = NULL; 
         }
     }
     return phyAddVirAdd;
 }
 
 
 phyVirAddress
 TextureObject::GetTexCubeMipLevel(GLint level, GLint texCubeMapFace)
 {
     unsigned int offset = 0;
     unsigned int offsetFace0 = 0;
     phyVirAddress phyAddVirAdd;
     if(hChunk != NULL){
         if(level >= levels){
             phyAddVirAdd.phyAdress = NULL;
             phyAddVirAdd.virAddress = NULL;
         }else{
             offsetFace0 = Offsets[level] * pixelSize(nativeFormat);
             offset = offsetFace0 + texCubeMapFace * ( Offsets[levels - 1] + 
                      images.cubeMap[0][levels - 1].width * images.cubeMap[0][levels - 1].width);
             phyAddVirAdd.phyAdress = ((GLubyte*)hChunk->GetPhyAddr()) + offset;
             phyAddVirAdd.virAddress =  ((GLubyte*)hChunk->GetVirtAddr()) + offset;
             
         }
     }else{
         if(images.tex3D[level].hImgChunk != NULL){
             phyAddVirAdd.phyAdress  = images.cubeMap[texCubeMapFace][level].hImgChunk->GetPhyAddr();
             phyAddVirAdd.virAddress = images.cubeMap[texCubeMapFace][level].hImgChunk->GetVirtAddr();
             
         }else{
             phyAddVirAdd.phyAdress = NULL;
             phyAddVirAdd.virAddress = NULL; 
         }
     }
     return phyAddVirAdd;
 }
 
 
 
 GLenum 
 TextureObject::Validate()
 {
      
     //tex object is in use
     if(isUsed != GL_TRUE){
         //LOGMSG(" The texture object is not in use \n");
         return GL_FALSE;
     }
 
     //texture type
     if(!((texType == GL_TEXTURE_2D) || (texType == GL_TEXTURE_3D) 
        || (texType == GL_TEXTURE_CUBE_MAP))){
         //LOGMSG(" unknown texture type \n");
         return GL_FALSE;
     }
    
     //level range
     if((levels < 1) || (levels > MAX_MIPMAP_LEVELS)){
         //LOGMSG(" The value of level is outside the range");
         return GL_FALSE;
     }
 
     //width and height should be in range
     if(width > (1 << (MAX_MIPMAP_LEVELS-1)) || height > (1 << (MAX_MIPMAP_LEVELS-1))
             || width < 0  || height < 0 ) {
         //LOGMSG("The width or height value is out of range");
         return GL_FALSE;
     } 
 
     //native format should be 1 of those supprted by fimg
     if(ValidatePxFmt(nativeFormat) == GL_FALSE){
         //LOGMSG("undetermined vlaue of the native texture format \n");
     }
 
     //wrap mode S
     if(!((wrapS == GL_REPEAT) || (wrapS == GL_CLAMP_TO_EDGE) || (wrapS == GL_MIRRORED_REPEAT))){
         //LOGMSG(" unknown wrap mode for S ");
             return GL_FALSE;
 
     }
     //wrap mode R
     if(!((wrapR == GL_REPEAT) || (wrapR == GL_CLAMP_TO_EDGE) || (wrapR == GL_MIRRORED_REPEAT))){
         //LOGMSG(" unknown wrap mode for S ");
             return GL_FALSE;
 
     }
     //wrap mode T
     if(!((wrapT == GL_REPEAT) || (wrapT == GL_CLAMP_TO_EDGE) || (wrapT == GL_MIRRORED_REPEAT))){
         //LOGMSG(" unknown wrap mode for S ");
         return GL_FALSE;
 
     }
 
     //mag filter
     if(!((magFilter == GL_LINEAR) || (magFilter == GL_NEAREST))){
         //LOGMSG(" unknown mag filter type \n");
         return GL_FALSE;
     }
 
     int CurLevel = 1;
     //min filter and levels
     if((minFilter == GL_LINEAR) || (minFilter == GL_NEAREST)){
         CurLevel = 1;
     }else if((minFilter == GL_NEAREST_MIPMAP_NEAREST) ||(minFilter == GL_LINEAR_MIPMAP_NEAREST)
             || (minFilter == GL_NEAREST_MIPMAP_LINEAR ) || (minFilter == GL_LINEAR_MIPMAP_LINEAR)){
 
         int tempMax = MAX(width , height);
         int maxValue = MAX(tempMax, depth);
         while (maxValue >>= 1){
              CurLevel++;
         }            
     }else{
         //LOGMSG(" unknown min filter type \n");
         return GL_FALSE;
     }
         
     if(CurLevel != levels){
         //LOGMSG(" the level value is incorrect \n.");  
         return GL_FALSE;
     }
 
     //in case of cubemap width and height should be equal
     if(texType == GL_TEXTURE_CUBE_MAP){
         for(int j = 0; j < 6 ; j++){
             for(int i = 0 ; i < levels ; i++){
                 if(images.cubeMap[j][i].height != images.cubeMap[j][i].width){
                     //LOGMSG(" Cubemap but width is not equal to height \n");
                     return GL_FALSE;
                 }
             }
         }    
     }
 
     //tset the image structure    
     GLint  currWidth = width; 
     GLint  currHeight = height;
     GLint  currDepth = ((texType == GL_TEXTURE_3D)?  depth :1);
     for(int i = 0 ; i < levels ; i++) {                                    
 
         switch (texType){
 
             case GL_TEXTURE_2D:
                 if(images.tex2D[i].isUsed != GL_TRUE || images.tex2D[i].nativeFormat != images.tex2D[0].nativeFormat ||
                    images.tex2D[i].width != currWidth || images.tex2D[i].height != currHeight || images.tex2D[i].imagedataLocation != TEXOBJCHUNK){
                         //LOGMSG(" the image structure value are not proper \n."); 
                         return GL_FALSE ;
                 }
                 break;
 
             case GL_TEXTURE_3D:
                 if(images.tex3D[i].isUsed != GL_TRUE || images.tex3D[i].nativeFormat != images.tex3D[0].nativeFormat ||
                    images.tex3D[i].width != currWidth || images.tex3D[i].height != currHeight || images.tex3D[i].depth != currDepth || images.tex3D[i].imagedataLocation != TEXOBJCHUNK) {
                         //LOGMSG(" the image structure value are not proper  \n.");
                         return GL_FALSE ;
                 }
                 break;
 
             case GL_TEXTURE_CUBE_MAP:
                 for(int j =0 ; j < 6 ; j++){
                     if(images.cubeMap[j][i].isUsed != GL_TRUE || images.cubeMap[j][i].nativeFormat != images.cubeMap[0][0].nativeFormat ||
                        images.cubeMap[j][i].width != currWidth || images.cubeMap[j][i].height != currHeight|| images.cubeMap[j][i].imagedataLocation != TEXOBJCHUNK) {
                             //LOGMSG(" the image structure value are not proper  \n.");
                             return GL_FALSE;
                     }
                 }
                 break;                
         }
                
         currWidth = ((currWidth / 2) > 1) ? (currWidth / 2) : 1;
         currHeight = ((currHeight / 2) > 1) ? (currHeight / 2) : 1;
         currDepth = ((currDepth / 2) > 1) ? (currDepth / 2) : 1;
     }
 
 
     //memory should be present in compiled state
     if(isExtTex == GL_TRUE){
         if(pExtTexchunk.paddr == NULL){
             //LOGMSG("\n externall managed texture but memory is NULL \n");
             return GL_FALSE; 
         }    
     }else{
         if(hChunk == NULL){
             //LOGMSG(" the memory is NULL \n");
             return GL_FALSE;
         }
     }
 
     //LOGMSG("texture object is validated \n");
     return GL_TRUE;
       
 }
 
 
 
 extern "C" int isTexUnitMipMapped(GLuint texUnit)
 {
     //OGLState* pState = GET_OGL_STATE();
     //TextureState* pTexState = &(pState->texState);    
 
         TextureObject*  pTexObj = GetTextureObject(GL_TEXTURE_2D,true,texUnit); //index in array
 
 
         ////LOGMSG( "\n texIndex =%d", texid  );
         if((pTexObj->minFilter == GL_LINEAR_MIPMAP_LINEAR) ||
             (pTexObj->minFilter == GL_NEAREST_MIPMAP_LINEAR) || 
             (pTexObj->minFilter == GL_LINEAR_MIPMAP_NEAREST) ||
             (pTexObj->minFilter == GL_NEAREST_MIPMAP_NEAREST)){
             
             return 1;
         }else{
             return 0;
         }
 
         //LOGMSG("\nthere is some issue in determining if mipmap is used.\n");
         return -1;
 
 }
 
 
 
 void releaseBoundTextures(OGLState* pState)
 {
 
     TextureState* pTexState = &(pState->texState);
     int index = -1;
     for(int i =0 ; i < MAX_TEXTURE_UNITS ; i++){
 
         //reduce the reference count of the 2D texture object bound to the current context
         if(pTexState->texUnitBinding[i].texture2D != 0){
             index = GetTexNameArrayIndex( pTexState->texUnitBinding[i].texture2D,  0);
            pState->sharedState->sharedTexState.ReleaseTexObj(pState , index); 
                    
             
         }
         //reduce the reference count of the 3D texture object bound to the current context
         if(pTexState->texUnitBinding[i].texture3D != 0){
             index = GetTexNameArrayIndex( pTexState->texUnitBinding[i].texture3D,  0);
             pState->sharedState->sharedTexState.ReleaseTexObj(pState , index);
         }
 
         //reduce the reference count of the cubemap texture object bound to the current context
         if(pTexState->texUnitBinding[i].cubeMap != 0){
             index = GetTexNameArrayIndex( pTexState->texUnitBinding[i].cubeMap,  0);
             pState->sharedState->sharedTexState.ReleaseTexObj(pState , index);
         }
     }
 
 }
 
 
 
 
 void SharedTextureState::ReleaseTexObj(struct OGLState* pState , GLuint id)
 {
          
     if(texObjects[id]->release(pState) == GL_TRUE){
           
               //removing the texture id from used texture names
         pState->sharedState->sharedTexState.usedTexNames.erase(id); 
 
         delete (texObjects[id]);
         
         //removing the texture object from the map
         pState->sharedState->sharedTexState.texObjects.erase(id);
     }
    
 }
 
 
 
 
 
 /*
 *******************************************************************************
 * Local functions
 *******************************************************************************
 */
 
 /*-----------------------------------------------------------------------*//*!
 *
 *//*------------------------------------------------------------------------*/
 TextureObject*
 GetTextureObject ( GLenum target ,bool extTU,GLint texUnit)
 {
 
     
     OGLState* pState = GET_OGL_STATE();
     TextureState* pTexState = &(pState->texState);
     GLuint currentUnit ;
     if(extTU == true)
         currentUnit = texUnit;
     else
         currentUnit = pTexState->activeTexUnit;
     
     //sanvd added the code below
     GLint texRef ;
     bool flag;
  
     switch(target){
 
     case GL_TEXTURE_2D:
          if(pTexState->texUnitBinding[currentUnit].texture2D != 0)
          {
              texRef = pTexState->texUnitBinding[currentUnit].texture2D;
              flag = false;
          }
          else
          {
              texRef = TEX_2D_DEFAULT;
              flag = true;
          }
          break;
     case GL_TEXTURE_3D:
          if(pTexState->texUnitBinding[currentUnit].texture3D != 0)
          {
              texRef = pTexState->texUnitBinding[currentUnit].texture3D;
              flag = false;
          }
          else
          {
              texRef = TEX_3D_DEFAULT;
              flag = true;
          }
         break;
     case GL_TEXTURE_CUBE_MAP:
         
         if(pTexState->texUnitBinding[currentUnit].cubeMap != 0)
          {
              texRef = pTexState->texUnitBinding[currentUnit].cubeMap;
              flag = false;
          }
          else
          {
              texRef = TEX_CUBE_MAP_DEFAULT;
              flag = true;
          }
         break;
     default:
         gAssert(false && "Unexpected target"); //Sandeep
         texRef = TEX_2D_DEFAULT;
         flag = true;
     };
     
     int index = GetTexNameArrayIndex(texRef,flag);
     if(index == TEX_2D_DEFAULT){
         return &(pTexState->defaultTexObjects[TEX_2D_DEFAULT - TEX_2D_DEFAULT]);
     }else if(index == TEX_3D_DEFAULT){
         return &(pTexState->defaultTexObjects[TEX_3D_DEFAULT - TEX_2D_DEFAULT]);
     }else if(index == TEX_CUBE_MAP_DEFAULT){
         return &(pTexState->defaultTexObjects[TEX_CUBE_MAP_DEFAULT -TEX_2D_DEFAULT ]);
     }else{ 
        TexObjects::iterator iter = pState->sharedState->sharedTexState.texObjects.find(index);
         return (iter->second);
     }
 
 }
 
 /*-----------------------------------------------------------------------*//*!
 *
 *//*------------------------------------------------------------------------*/
 
 /*check whether the internal format & format are same or not 
    & are one of the supported type */
 static GLenum
 checkTextureFormat ( OGLState* pState, GLenum internalFormat, GLenum format)
 {
     // Checking of internalFormat, format, type is to be done here
     //return internalFormat;
     
     //sanvd added the code below
     if(internalFormat!=format)
         return GL_INVALID_OPERATION; 
         
     if( !(internalFormat == GL_RGBA || internalFormat == GL_BGRA || internalFormat == GL_RGB ||internalFormat == GL_ALPHA
         ||
         internalFormat == GL_LUMINANCE ||internalFormat == GL_LUMINANCE_ALPHA) )
     return GL_INVALID_VALUE;
     
     return GL_NO_ERROR;
         
     
 }
 
 /* Function will determine the no of pixels depending on the textureFormat*/
 GLuint GetPixSize(GLenum TextureFormat)
 {
     int bpp =2;
     if((TextureFormat == E_LUMINANCE8) || (TextureFormat == E_ALPHA8)){
         bpp = 1;
     }
     else if((TextureFormat == E_ALPHA_LUMINANCE88)|| 
         ( TextureFormat == E_LUMINANCE_ALPHA88) ||
         (TextureFormat == E_LUMINANCE_ALPHA80) ||
         (TextureFormat == E_LUMINANCE_ALPHA08) ||
              (TextureFormat == E_RGBA4) ||
              (TextureFormat == E_RGB565) ||
              (TextureFormat == E_RGBA5551)||
              (TextureFormat == E_ARGB4) ||
              (TextureFormat == E_ARGB1555))
     {
         bpp = 2;
     }
 
     else if((TextureFormat == E_ARGB8) || (TextureFormat == E_ABGR8) || (TextureFormat == E_ARGB0888)){
         bpp =4;
     }
     else{
         gAssert(false && "undetermined byte per pixel ");
     }
     return bpp;
 }
 
 
 
 
 GLenum CheckFormatTypeCombination(GLenum texFormat, GLenum type)
 {
 if(((texFormat == GL_BGRA || texFormat == GL_RGBA || texFormat == GL_RGB) && (type == GL_UNSIGNED_BYTE))
     
     
     |((texFormat == GL_RGBA) && (type == GL_UNSIGNED_SHORT_4_4_4_4))
     
     |((texFormat == GL_RGBA) && (type == GL_UNSIGNED_SHORT_5_5_5_1))
     
     |((texFormat == GL_RGB) && (type == GL_UNSIGNED_SHORT_5_6_5))
     
     |((texFormat == GL_LUMINANCE_ALPHA) && (type == GL_UNSIGNED_BYTE))
     
     |((texFormat == GL_LUMINANCE) && (type == GL_UNSIGNED_BYTE))
     
     |((texFormat == GL_ALPHA) && (type == GL_UNSIGNED_BYTE)))
         return GL_NO_ERROR;
 
     else{
         gAssert(false && "Unhandled or unexpected format and type combination"); //Sandeep
         return  1;
         //to check what type of error to return if the are not matched
     }
 }
 
 
 
 
 inline 
 void determineFormatAndType( GLenum * format , GLenum * type , PxFmt  nativeformat )
 {
 
     switch(nativeformat){
         case E_ARGB8:
            * format =  GL_RGBA;
            *type     =  GL_UNSIGNED_BYTE;
            break;
         case E_ABGR8:
            * format =  GL_BGRA;
            *type     =  GL_UNSIGNED_BYTE;
            break;
         case E_ARGB4:
             * format =  GL_RGBA;
            *type     =  GL_UNSIGNED_SHORT_4_4_4_4;
            break;
          case E_ARGB1555:
             *format = GL_RGBA;
             *type = GL_UNSIGNED_SHORT_5_5_5_1;
             break;
          case E_RGB565:
              *format = GL_RGB;
             *type = GL_UNSIGNED_SHORT_5_6_5;
             break;
           //default:
             //LOGMSG("unsupported PxFmt  as external managed texture \n");
          
     }
 }
 
 /**********************************************************************************************
 /  external texture object
 ************************************************************************************************/
 #ifdef USE_3D_PM
     void PM_GLES2BindTexImage(const GLES2SurfaceData* pSurfData , GLenum target, GLint level , GLuint isMipmapped);
     
     extern "C"
     void GLES2BindTexImage(const GLES2SurfaceData* pSurfData , GLenum target, GLint level , GLuint isMipmapped)
     {
         lock3DCriticalSection();
         PM_GLES2BindTexImage( pSurfData , target,  level , isMipmapped);
         unlock3DCriticalSection();
     }
         
     void PM_GLES2BindTexImage(const GLES2SurfaceData* pSurfData , GLenum target, GLint level , GLuint isMipmapped)
 #else
     extern "C"
     void GLES2BindTexImage(const GLES2SurfaceData* pSurfData , GLenum target, GLint level , GLuint isMipmapped)
 #endif
 {
 
     OGLState* pState = GET_OGL_STATE();
     GLsizei pixSize;                              // Bytes per pixel
     Image *pImgObj = NULL;            // Pointer to image object
     TextureObject* pTexObj =NULL;
     GLenum format = GLenum(-1);
     GLenum  type = GLenum(-1);
     
     //-------------------------------------------------------------------------
     // Get texture object and VALIDATE INPUT PARAMETERS
     //-------------------------------------------------------------------------
 
     // Check MipMap level //TO DO SHOULD BE ALLOWED OTHER THAN LEVEL 0 AS IMAGE CAN NOT BE COMPILED
     if(level != 0 ) {
         SET_ERR(pState, GL_INVALID_VALUE, "bindTexImage");
         return;
     };
     
     // Select object reference according to target
     // Return error on invalid enum
     if(target == GL_TEXTURE_2D){
         pTexObj = GetTextureObject(GL_TEXTURE_2D,false,0);
         pImgObj = &(pTexObj->images.tex2D[level]);
         
     }else {
         SET_ERR(pState, GL_INVALID_ENUM, "bindTexImage*");
         return;
     }
  
     // Check width and height should be less than zero or greater than GL_MAX_TEXTURE_SIZE????
     if(pSurfData->width > (1 << (MAX_MIPMAP_LEVELS-1)) || pSurfData->height > (1 << (MAX_MIPMAP_LEVELS-1))
             || pSurfData->width < 0  || pSurfData->height < 0 ) {
         SET_ERR(pState, GL_INVALID_VALUE, "bindTexImage");
         return;
     }
 
     if(!((pSurfData->nativeColorFormat == E_ARGB8) ||(pSurfData->nativeColorFormat == E_ARGB4)
         || (pSurfData->nativeColorFormat == E_ARGB1555) || (pSurfData->nativeColorFormat == E_RGB565)) ){
             SET_ERR(pState, GL_INVALID_VALUE, "bindTexImage");
             return;
     }
 
     determineFormatAndType(&format , &type , pSurfData->nativeColorFormat);
 
     if((pSurfData->colorPaddr== NULL) || (pSurfData->colorVaddr == NULL)){
         return;
     }
 
 
    //FLUSHING THE CACHE
     FimgFinish(pState); 
    
     pixSize = pixelSize(pSurfData->nativeColorFormat);
   
    if(getImageDataLocation(pTexObj , 0 , 0 , 0, 0, 0, 0 , 0, 0, 1) != NULL){
         gAssert(false && " getImageDataLocation function did not return null in case of the external managed texture");
         return;
    }
     pTexObj->pExtTexchunk.paddr = pSurfData->colorPaddr;
     pTexObj->pExtTexchunk.vaddr = pSurfData->colorVaddr;
     pTexObj->isExtTex  =GL_TRUE;
     pTexObj->width = pSurfData->width;;
     pTexObj->height = pSurfData->height;
     pTexObj->internalFormat = format;
     pTexObj->nativeFormat = pSurfData->nativeColorFormat; 
     pTexObj->levels = 0;
     pTexObj->isDirtyState = GL_TRUE;
     
     pImgObj->imagedataLocation =  TEXOBJCHUNK ; 
     pImgObj->isUsed = GL_TRUE;
     pImgObj->width = pSurfData->width;
     pImgObj->height = pSurfData->height;
     pImgObj->internalFormat =  format;
     pImgObj->PixType = type;
     pImgObj->imgSize = pSurfData->width * pSurfData->height * pixSize;
     pImgObj->isCompressed = false;
     pImgObj->nativeFormat  = pSurfData->nativeColorFormat;
 
     
 // fprintf(stderr, " line = %d func =%s" , __LINE__ , __FUNCTION__);
 }
 
 extern "C"
 void GLES2ReleaseTexImage(const GLES2SurfaceData* pSurfData )
 {
 
     OGLState* pState = GET_OGL_STATE();
      TextureObject* pTexObj =NULL;
 #if 0  
    
     //find the texture object using the surface buffer in shared texture object
      for(int i = 0; i <= pState->sharedState->sharedTexState.maxUsedUnit ; ++i){
         if(pState->sharedState->sharedTexState.texObjects[i].pExtTexchunk.paddr == pSurfData->colorAddr.paddr){
             pTexObj = &(pState->sharedState->sharedTexState.texObjects[i]); 
             break;
         }  
     }
 #else
     TexObjects::iterator iter = pState->sharedState->sharedTexState.texObjects.begin();
     for( ; iter != pState->sharedState->sharedTexState.texObjects.end() ; iter++){
         if(iter->second->pExtTexchunk.paddr == pSurfData->colorPaddr){
             pTexObj = (iter->second); 
             break;
         }     
     }
 
 
 #endif
 
      //check in default texture object
     if(pTexObj == NULL){
         if(pState->texState.defaultTexObjects[0].pExtTexchunk.paddr ==   pSurfData->colorPaddr){
             pTexObj = &(pState->texState.defaultTexObjects[0]);
         }
     }
 
      if(pTexObj == NULL){
          //not an error...
         return;
      }
 
     pTexObj->releaseMem();
     pTexObj->isExtTex = GL_FALSE;   
     pTexObj->images.tex2D[0].imagedataLocation = NONE;
     pTexObj->levels = 0;
     
 }
 
 
 
 /*-----------------------------------------------------------------------*//*!
 *
 *//*------------------------------------------------------------------------*/
 
 
 GLsizei
 GetPixelSize ( GLenum texFormat, GLenum type )
 {
     int bpp = 2;    //NOTE: HARDCODED must have some sanity checks and error values
 
 //  GLenum type = GL_UNSIGNED_BYTE; sanvd commented this
 
     if((texFormat == GL_BGRA || texFormat == GL_RGBA || texFormat == GL_RGB) && (type == GL_UNSIGNED_BYTE))
     {
         bpp = 4;
     }
     /*else if((texFormat == GL_RGB) && (type == GL_UNSIGNED_BYTE))
     {
         bpp = 3;
     }*/
     else if((texFormat == GL_RGBA) && (type == GL_UNSIGNED_SHORT_4_4_4_4))
     {
         bpp = 2;
     }
     else if((texFormat == GL_RGBA) && (type == GL_UNSIGNED_SHORT_5_5_5_1))
     {
         bpp = 2;
     }
     else if((texFormat == GL_RGB) && (type == GL_UNSIGNED_SHORT_5_6_5))
     {
         bpp = 2;
     }
     else if((texFormat == GL_LUMINANCE_ALPHA) && (type == GL_UNSIGNED_BYTE))
     {
         bpp = 2;
     }
     else if((texFormat == GL_LUMINANCE) && (type == GL_UNSIGNED_BYTE))
     {
         bpp=2; //bpp = 1; //TODO: the actual native formats should be used to determine the pixel size for tansfer image!!
     }
     else if((texFormat == GL_ALPHA) && (type == GL_UNSIGNED_BYTE))
     {
         bpp = 2;
     }
     else
     {
         gAssert(false && "GetPixelSize"&& "Unhandled or unexpected format and type combination"); //Sandeep
     }
     return bpp;
 }
 
 
 /*-----------------------------------------------------------------------*//*!
 *
 *//*------------------------------------------------------------------------*/
 
 static inline GLboolean
 IsPowerOf2 ( GLuint n )
 {
     if(n > 0 && (n & (n-1)) == 0)
         return GL_TRUE;
     else
         return GL_FALSE;
 }
 
 
 /*-----------------------------------------------------------------------*//*!
 *
 *//*------------------------------------------------------------------------*/
 
 
 //supported FGL format 
 GLenum  ValidatePxFmt(PxFmt pxFmtValue)
 {
     switch(pxFmtValue){
         case E_LUMINANCE8:     
         case E_LUMINANCE_ALPHA88:         
         case E_LUMINANCE_ALPHA80:         
         case E_LUMINANCE_ALPHA08:
         case E_ARGB8:
         case E_ABGR8:
         case E_ARGB4:
         case E_RGB565:
         case E_ARGB1555:
         case E_RGB_S3TC_OES:
         case E_RGBA_S3TC_OES:
         case E_PALETTE4_RGB8_OES:
         case E_PALETTE4_RGBA8_OES:    
         case E_PALETTE4_R5_G6_B5_OES:
         case E_PALETTE4_RGBA4_OES:
         case E_PALETTE4_RGB5_A1_OES:
         case E_PALETTE8_RGB8_OES:
         case E_PALETTE8_RGBA8_OES:
         case E_PALETTE8_R5_G6_B5_OES:          
         case E_PALETTE8_RGBA4_OES:         
         case E_PALETTE8_RGB5_A1_OES:
             return GL_TRUE;
          default:
             return GL_FALSE;
      }
 }
 
 
 
 static 
 GLboolean CheckFormatConversion(GLenum dstFormat, GLenum srcFormat, GLenum* dstType)
 {
     switch(srcFormat)
     {
         case GL_RGBA8:
             if(dstFormat == GL_RGBA || dstFormat == GL_RGB || dstFormat == GL_LUMINANCE_ALPHA || dstFormat == GL_LUMINANCE || dstFormat == GL_ALPHA)
             {   
                 *dstType = GL_UNSIGNED_BYTE;
                 return true;
             }           
             return false;
             break;
     
         case GL_RGBA:  //not supported as of now//can be converted to any format by dropping the channels
         case GL_BGRA:  //not supported as of now//can be converted to any format by dropping the channels
             return true; 
             break;
         case GL_RGB565:
         case GL_RGB:
             if(dstFormat == GL_RGB )
             {   
                 *dstType = GL_UNSIGNED_SHORT_5_6_5;
                 return true;
             }
             
             if( dstFormat == GL_LUMINANCE)
             {
                 *dstType = GL_UNSIGNED_BYTE;
                 return true;
             }
             return false;   
             break;
 
         case GL_LUMINANCE_ALPHA:
             if(dstFormat == GL_RGB || dstFormat == GL_RGBA)
                 return false;
             *dstType = GL_UNSIGNED_BYTE;    
             return true;
             break;
         
         case GL_LUMINANCE:
             if(dstFormat == GL_LUMINANCE)
             {
                 *dstType = GL_UNSIGNED_BYTE;
                 return true;
             }
             return false;
             break;
         
         case GL_ALPHA:
         if(dstFormat == GL_ALPHA)
             {
                 *dstType = GL_UNSIGNED_BYTE;
                 return true;
             }
             return false;
         break;
     }
     
     return false;   
 }
 
 static bool
 s3tcCompressedTex(GLenum& texformat,GLenum internalformat,GLsizei width,GLsizei height,GLsizei imageSize)
 {
 
     if( width%4 !=0 || height%4!=0 || imageSize ==0)
         return false;
     int expectedSize =0;
     
     switch(internalformat)
     {
         case GL_RGB_S3TC_OES:
 
             texformat = GL_RGB;
             width = (width+3) & ~3;
             height = (height+3) & ~3;
             expectedSize = width * height/2;
             if (expectedSize< 8)
                 expectedSize =8;
             if (expectedSize!= imageSize)
                 return false;
         break;      
         case GL_RGBA_S3TC_OES:
 
             texformat = GL_RGBA;
             /*width = (width+3) & ~3;
             height = (height+3) & ~3;
             expectedSize = width * height;
             if (expectedSize< 16)
                 expectedSize =16;
             if (expectedSize!= imageSize)
                 return false;*/
         break;
         return true;        
         
     }
 
     return true;
 }
 
  
 
  
 GLint GetTexNameArrayIndex(GLuint texName,bool IsDefault)
 {
 
     OGLState* pState = GET_OGL_STATE();
 
     //TextureState* pTexState = &(pState->texState);
     if(IsDefault)
     {
         if(texName == TEX_2D_DEFAULT)
             return TEX_2D_DEFAULT;
         else if(TEX_3D_DEFAULT)
             return TEX_3D_DEFAULT;
         else if(TEX_CUBE_MAP_DEFAULT)
             return TEX_CUBE_MAP_DEFAULT;
         else
             return -1;
         
     }
 
 
 #if 0  
     
     GLint FEP = -1;  //first position in the array whcih is unsued.
 
     SharedTextureState *pSTState = &pState->sharedState->sharedTexState;
     TextureObject *pTexObj;
 
     //TODO: is it i =1 or i=0? -Sandeep K. shariq now it should be 0
     for(int i = 1; i <= pSTState->maxUsedUnit + 1 ; ++i) //added 1 for new unit
     {
         if((pTexObj = pSTState->texObjects[i]) != NULL) {
             if(FEP == -1 && pTexObj->isUsed ==false)
             {
                 FEP = i;
             }
             
             //TODO: check for is used? -Sandeep K.
             if(texName == pTexObj->id)
             {
                 return i;
             }
         }
     }
 
  #else
 //    int count = pState->sharedState->sharedTexState.texObjects.count(texName);
 
 //    if(count == 0){
     if(pState->sharedState->sharedTexState.texObjects.find(texName) == pState->sharedState->sharedTexState.texObjects.end()){    //liyue 090316
   
 //      if(pState->sharedState->sharedTexState.texObjects.count(texName) == 0){
         TextureObject* pTexObj = new(TextureObject);
         pTexObj->reset();
         pState->sharedState->sharedTexState.texObjects[texName] = pTexObj;      
     }
     return texName;
  #endif
 }
 
 
 TextureObject*  GetTexObject(GLuint texName)
 {
    OGLState* pState = GET_OGL_STATE();
    //TextureState* pTexState = &(pState->texState);
 
    TextureObject *pTexObj;
 
    //TODO: is it i =1 or i=0? -Sandeep K.
    for(int i=1; i< MAX_TEXTURE_OBJECTS; i++)
    {
        //TODO: check for is used? -Sandeep K.
        if((pTexObj = pState->sharedState->sharedTexState.texObjects[i])
                     != NULL)
        {
            if(texName == pTexObj->id)
            {
                //TODO: Compile should not be required here
                pTexObj->Compile(); 
                return (pTexObj);
            }
        }
    }
 
    return 0;
 }
 
 
 
 void SwapRB(GLubyte* pixels,GLenum format,GLenum type, GLsizei dstWidth, GLsizei dstHeight,GLsizei dstDepth,
             int xoff,int yoff,int zoff, GLsizei width, GLsizei height,GLsizei depth, int pixelSize)
 {
     GLubyte tmp = 0;
     if(depth == 0)
         depth = 1;
     
     //if(format==GL_RGB && type == GL_UNSIGNED_BYTE)
     //  pixelSize = 3;
 
     GLubyte *imgBuf = pixels + (zoff*dstWidth*dstHeight + yoff*dstWidth + xoff)*pixelSize;
     if( (format == GL_RGB || format == GL_RGBA ) && type == GL_UNSIGNED_BYTE)
         for(int d = 0; d < depth ; d++)
         {
             for(int h = 0; h < height ; h ++)
             {
                 for(int w = 0; w < width ; w++)
                     {
                         tmp = imgBuf[0];
                         imgBuf[0] = imgBuf[2];
                         imgBuf[2] = tmp;
                         imgBuf+=pixelSize;
                     }
                 imgBuf += (dstWidth - width) * pixelSize ;
             }
             
             imgBuf += ((dstHeight - height)*dstWidth) * pixelSize ;
         }
 }
 
 void* glMapTexture(GLenum target, GLint level, GLenum access)
 {
     OGLState* pState = GET_OGL_STATE();
 
     TextureObject* pTexObj = 0;
     Image* pImgObj = 0;
 
     if (target == GL_TEXTURE_2D)
     {
         pTexObj = GetTextureObject(target,false,0);
         pImgObj = &(pTexObj->images.tex2D[level]);
     }
     else if (target == GL_TEXTURE_3D)
     {
         pTexObj = GetTextureObject(target,false,0);
         pImgObj = &(pTexObj->images.tex3D[level]);
     }
     else if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
     {
         pTexObj = GetTextureObject(target,false,0);
         pImgObj = &(pTexObj->images.cubeMap[target - GL_TEXTURE_CUBE_MAP_POSITIVE_X][level]);
     }
     else
     {
         SET_ERR(pState, GL_INVALID_ENUM, "glMapTexture*");
         return 0;
     }
 
     if (pTexObj->isLocked == 1)
     {
         SET_ERR(pState, GL_INVALID_OPERATION, "glMapTexture*");
         return 0;
     }
 
     if (pImgObj->imagedataLocation == IMAGECHUNK)
     {
         if (pImgObj->hImgChunk != 0)
         {
                 pTexObj->isLocked = 1;
                 return pImgObj->hImgChunk->GetVirtAddr();
         }
         else
         {
                 return 0;
         }
     }
     else if (pImgObj->imagedataLocation == TEXOBJCHUNK)
     {
         if (pTexObj->hChunk != 0)
         {
                 pTexObj->isLocked = 1;
                 return (char*)pTexObj->hChunk->GetVirtAddr() + pTexObj->Offsets[level];
         }
         else
         {
                 return 0;
         }
     }
 
     return 0;
 }
 
 GLboolean glUnmapTexture(GLenum target)
 {
     OGLState* pState = GET_OGL_STATE();
 
     TextureObject* pTexObj = 0;
 
     if (target == GL_TEXTURE_2D)
     {
         pTexObj = GetTextureObject(target,false,0);
     }
     else if (target == GL_TEXTURE_3D)
     {
         pTexObj = GetTextureObject(target,false,0);
     }
     else if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
     {
         pTexObj = GetTextureObject(target,false,0);
     }
     else
     {
         SET_ERR(pState, GL_INVALID_ENUM, "glMapTexture*");
         return GL_FALSE;
     }
 
     // not locked
     if (!pTexObj->isLocked)
     {
         set_err(GL_INVALID_OPERATION);
             return GL_FALSE;
     }
 
     pTexObj->isLocked = 0;
 
     return GL_TRUE;
 }
