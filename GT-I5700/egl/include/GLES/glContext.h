#ifndef __GLCONTEXT_H__
#define __GLCONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gl.h"
#include "pixelFmts.h"


#ifndef GLES_NO_CONTEXT
#define GLES_NO_CONTEXT 0
#endif


typedef void* GLES2Context;

/*typedef struct {
	void*			paddr;
	void*			vaddr;
} AddressBase;*/

typedef void* BufferHandle;
typedef AddressBase  BufferAddress;

//This structure is deprecated and is left here for backward compatibility and internal use. Use GLES2SurfaceData instead.
typedef struct
{
    AddressBase colorAddr;
    AddressBase depthStencilAddr;

    unsigned int width;
    unsigned int height;
	/*
    int       colorFormat;      //must be set to a gl enum
    int       colorType;
    int       depthStencilFormat;
	*/
	PxFmt	  nativeColorFormat;
	PxFmt	  nativeDepthStencilFormat;
    int       flipped;  //0 is rendering origin is top left, 1 if its bottom left.
} FramebufferData;

typedef struct
{
	AddressBase 	colorAddr;
	AddressBase 	depthStencilAddr; //Address of depth buffer
	GLuint 		width; //Width of Framebuffer surface
	GLuint		height; //Height of Framebuffer surface
	PxFmt 		nativeColorFormat; //Pixel format of Framebuffer surface
	PxFmt		nativeDepthStencilFormat; //Depth buffer format
	GLuint		flipped; // 0 is rendering origin is top left, 1 if its bottom left
}GLES2SurfaceData;

//Initializes the 3D HW and the pool memory
//returns    GL_TRUE/GL_FALSE on success/failure
GLboolean GLES2Initdriver(int g3dfd); //added by JJG

//Attaches the framebuffer (surface) to the current GL context
GLboolean GLES2SetSurfaceData(GLES2SurfaceData * surfdata);
#if 0
//Notifies the GL library that a swap buffer is performed.
//returns  GL_FALSE is no valid surface is bound to the context.
GLboolean GLES2SwapBuffer(void);
#endif
//Creates the GLES 2.0 graphics context
GLES2Context GLES2CreateContext(GLES2Context sharedctx);

//Sets the context pointed to by ctx as the current context for
//  the current thread
GLES2Context GLES2SetContext(GLES2Context ctx);

//Destroys the graphics context
GLboolean GLES2DestroyContext(GLES2Context ctx);

//De initializes the driver. This frees up the pool memory.
//Should be done when no more GL contexts are active.
GLboolean GLES2DeInitdriver(void);

//Wrapper function for allocating physically contigious memory buffer
// from the pool. This should be used to create the depthstencil
// buffer
BufferHandle BufferAlloc(GLsizei size);

//Wrapper function for freeing a previously allocated buffer from the
// pool.
int BufferFree(BufferHandle handle);

//Wrapper function to get the physical and virtual address of the buffer
//allocated from the pool
BufferAddress GetBufferAddress(BufferHandle handle);

BufferAddress AllocShareAddress(void* phyAddr, GLsizei size);

int FreeShareAddress(BufferAddress shareAddr, GLsizei size);


//used for defining  externally managed texture. right now only level 0 is supported.
void GLES2BindTexImage( const GLES2SurfaceData* pSurfData , GLenum target, GLint level , GLuint isMipmapped);


//used to release the externally managed texture.
void GLES2ReleaseTexImage(const GLES2SurfaceData* pSurfData );

GLboolean GLES2SetDrawSurface(GLES2SurfaceData *surface);
GLboolean GLES2SetReadSurface(GLES2SurfaceData *surface);
GLboolean GLES2Flush(void);



#ifdef __cplusplus
}
#endif

#endif //__GLCONTEXT_H__
