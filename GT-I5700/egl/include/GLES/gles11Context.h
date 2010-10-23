#ifndef _GLES11CONTEXT_H_
#define _GLES11CONTEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pixelFmts.h"
//#include "gles.h"
#include <GLES/gles.h>

#ifndef GLES_NO_CONTEXT
#define GLES_NO_CONTEXT 0
#endif


typedef void* GLES11Context;

typedef struct {
	void*			paddr;
	void*			vaddr;
} AddressBase;

typedef void* BufferHandle;
typedef AddressBase BufferAddress;

typedef struct
{
	AddressBase 	colorAddr;
	AddressBase 	depthStencilAddr; //Address of depth buffer
	GLuint 		width; //Width of Framebuffer surface
	GLuint		height; //Height of Framebuffer surface
	PxFmt 		nativeColorFormat; //Pixel format of Framebuffer surface
	PxFmt		nativeDepthStencilFormat; //Depth buffer format
	GLuint		flipped; // 0 is rendering origin is top left, 1 if its bottom left
}GLES11SurfaceData;

//Initializes the 3D HW and the pool memory
//returns    GL_TRUE/GL_FALSE on success/failure
GLboolean GLES11Initdriver(int g3d_fd);

//Attaches the framebuffer (surface) to the current GL context
GLboolean GLES11SetSurfaceData(GLES11SurfaceData * surfdata);

//Notifies the GL library that a swap buffer is performed.
//returns  GL_FALSE is no valid surface is bound to the context.
GLboolean GLES11SwapBuffer(void);

//Creates the GLES 1.1 graphics context
GLES11Context GLES11CreateContext(GLES11Context sharedctx);

//Sets the context pointed to by ctx as the current context for
//  the current thread
GLES11Context GLES11SetContext(GLES11Context ctx);

//Destroys the graphics context
GLboolean GLES11DestroyContext(GLES11Context ctx);

//De initializes the driver. This frees up the pool memory.
//Should be done when no more GL contexts are active.
GLboolean GLES11DeInitdriver(void);

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


//used for defining  externally managed texture. right now only level 0 is supported.
void GLES11BindTexImage( const GLES11SurfaceData* pSurfData , GLenum target, GLint level , GLuint isMipmapped);


//used to release the externally managed texture.
void GLES11ReleaseTexImage(const GLES11SurfaceData* pSurfData );


#ifdef __cplusplus
}
#endif



#endif //_GLES11CONTEXT_H_
