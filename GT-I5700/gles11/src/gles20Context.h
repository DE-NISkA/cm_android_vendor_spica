#ifndef _GLES20CONTEXT_H_
#define _GLES20CONTEXT_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifndef GLES_NO_CONTEXT 
#define GLES_NO_CONTEXT 0
#endif


	typedef void* GLES2Context;

	typedef struct
	{
		int         size;
		void*       colorVaddr;
		void*       colorPaddr;
		void*       depthVaddr;
		void*       depthPaddr;
		GLuint          width; //Width of Framebuffer surface

		GLuint          height; //Height of Framebuffer surface

		PxFmt           nativeColorFormat; //Pixel format of Framebuffer surface

		PxFmt           nativeDepthStencilFormat; //Depth buffer format

		GLuint          flipped; // 0 is rendering origin is top left, 1 if its bottom left     

	}GLES2SurfaceData;

	//GLboolean GLES2Initdriver(int g3d_fd);

	GLboolean GLES2Initdriver(void);
	GLboolean GLES2SetSurfaceData(GLES2SurfaceData * surfdata);
#if 0
	GLboolean GLES2SwapBuffer(void);
#endif
	GLES2Context GLES2CreateContext(GLES2Context sharedctx);
	GLES2Context GLES2SetContext(GLES2Context ctx);
	GLboolean GLES2DestroyContext(GLES2Context ctx);
	GLboolean GLES2DeInitdriver(void);
	void GLES2BindTexImage( const GLES2SurfaceData* pSurfData , GLenum target, GLint level , GLuint isMipmapped);
	void GLES2ReleaseTexImage(const GLES2SurfaceData* pSurfData );
	GLboolean GLES2SetDrawSurface(GLES2SurfaceData *surface);
	GLboolean GLES2SetReadSurface(GLES2SurfaceData *surface);
	GLboolean GLES2Flush(void);

#ifdef __cplusplus
}
#endif

#endif
