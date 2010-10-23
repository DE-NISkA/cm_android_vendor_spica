#ifndef __GLES_INCLUDES_H__
#define __GLES_INCLUDES_H__
#define RETAILMSG(x,y)
#include "gles.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "shader.h"
#ifdef MULTI_CONTEXT
#include "pthread.h"
#endif
#include <cutils/log.h>
#define fabsf fabs
#define __FIMG__
#include "hashMap.h"
#ifdef __cplusplus 

//#define printf
extern "C" 
{
#endif

	extern void ShaderInit();
	void UniformsInit();
	typedef enum EBuffers {ARRAY_BUFFER,ELEMENT_BUFFER, MAX_BUFFERS} 
	Buffers; 
	typedef enum EPointers {VERTEX, NORMAL, COLOR, TEXTURE0, TEXTURE1 , MATRIX_INDEX, WEIGHT, POINT_SIZE , MAX_POINTERS} 
	Pointers; 
	typedef struct Pixel32Type_TAG {
		unsigned char Blue;
		unsigned char Green;
		unsigned char Red;
		unsigned char Alpha;
	} Pixel32Type;

#define MBX_COORD_SIZE 3 
#define MBX_COORD_COL_SIZE 4
#define DBGFUNCTION_INIT(x) 
#define DBGFUNCTION_EXIT(x) 

	typedef void ( * pfnCopyDataEnum)(void *dst, GLint src, int count ) ; 
	typedef void ( * pfnCopyDataVal)(void *dst, GLfloat src, int count ) ; 
	typedef void ( * pfnCopyData)(void *dst, void * src, int count ) ; 
	typedef GLvoid (*pfCopyTex)(const GLubyte *,GLubyte *,int rowCnt);
	typedef void (*pFNMultMatrix) (GLfloat* m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) ;
	typedef void (*pFNMultMatrixT) (GLfloat* m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;
	typedef void (*pFNClipFn)(GLvoid *v1, GLvoid *v2,GLvoid *vout, GLfloat t);

	typedef void (*pFNCopyColor) (GLvoid *src1, GLuint *dst, GLint sstride, GLint dstride, GLint count ) ;
	GLuint GetUInt(GLfloat *col);
	GLuint GetUIntx(GLfixed *col);
	GLuint GetUIntb(GLubyte *col);

#ifdef MULTI_CONTEXT
	extern pthread_key_t tls_glescontext_key11;

#define SET_GLES11_STATE(x) pthread_setspecific(tls_glescontext_key11,x);

	// *Pra* Older declaration. To be deleted.
#define GET_GLES_CONTEXT_NOCHECK   GLState * ctx = (GLState *)pthread_getspecific(tls_glescontext_key11); 
#define GET_GLES_CONTEXT   GLState * ctx = (GLState *)pthread_getspecific(tls_glescontext_key11); if((ctx==NULL)||(ctx->magicNumber!=0xDEADBEEF)) {LOGE("ERROR: ctx is NULL: %s line:%d pthreadid: %ld \n",__FUNCTION__,__LINE__,(long int)pthread_self());return ;}
#define GET_GLES_CONTEXT_RETURN   GLState * ctx = (GLState *)pthread_getspecific(tls_glescontext_key11); if((ctx==NULL)||(ctx->magicNumber!=0xDEADBEEF)) {LOGE("ERROR: ctx is NULL: %s line:%d pthreadid: %ld \n",__FUNCTION__,__LINE__,(long int)pthread_self());return 0;}
#else
	extern void * g_ProcessTlsCtx11;  
#define SET_GLES11_STATE(x) g_ProcessTlsCtx11 = x;

#define GET_GLES_CONTEXT_NOCHECK GLState *ctx; ctx= (GLState *)g_ProcessTlsCtx11 ; 
#define GET_GLES_CONTEXT GLState *ctx; ctx= (GLState *)g_ProcessTlsCtx11 ; 
#endif


#define GL_ERROR(x) if ( ctx->error < C_MAX_ERRORS ) { ctx->errors[ctx->error] = x ; ctx->error = ctx->error + 1; /*LOGW("GL_ERROR from APP: %s line:%d ctx->error: %d \n",__FUNCTION__,__LINE__,ctx->error);*/} return ; 
#define GL_ERROR0(x) if ( ctx->error < C_MAX_ERRORS ) { ctx->errors[ctx->error] = x ;ctx->error = ctx->error + 1;} return 0; 
#define CHECK_GL_ENUM(x) if (x) {GL_ERROR(GL_INVALID_ENUM)}
#define CHECK_GL_VALUE(x) if (x) {GL_ERROR(GL_INVALID_VALUE)}
#define ALLOC_MEM(x) malloc(x)
#define FREE_MEM(x) free(x)
#define FREE_MEM_CHKRST(x) if (x) {FREE_MEM(x) ;x= (void *)0 ;}
#define ASSERT(x) //if ( !(x)) {LOGE("Assertion Failed in %d in %s \r\n", __LINE__, __FILE__);}

#define CLAMPF(x) ((x<0.0f)?0.0f:((x>1.0f)?1.0f:x))
	/* Make ARGB color from 4 clamped components r,g,b,a */
#define COLOR4CF(r,g,b,a) (( ( ((unsigned int) (a*255.0f)) & 0xFF) << 24 ) |( ( ((unsigned int) (r*255.0f)) & 0xFF) << 16 ) |( ( ((unsigned int) (g*255.0f)) & 0xFF) << 8 ) |( ( ((unsigned int) (b*255.0f)) & 0xFF) ) )        

#define FIXED_TO_FLOAT(x) ((x)/65536.0f)
#define FLOAT_TO_FIXED(x) ((unsigned int)((x)*65536.0f))
#include "glConsts.h"
#include "glState.h"
#include "funcs.h"
#define VEC4CPY(a,b) {a[0]=b[0];a[1]=b[1];a[2]=b[2];a[3]=b[3];}

	//************************ VERSION ************************ 

#define GLES_LIB_VERSION "(lib version 11.02.72)"
	//version string must be major_number.minor_number or major_number.minor_number.release_number followed by a space and 
	//  vendor specific format
#define GLES_VERSION_STRING   "1.1 " GLES_LIB_VERSION

#ifdef GLES11_LOG_ENABLE
#define LOG_GLES11_API bGles11Log
#define GL11LOGMSG(flag, printf_exp) if(flag){ LOGE printf_exp ;}

	extern unsigned int bGles11Log;
#else
#define GL11LOGMSG(flag, printf_exp)
#endif
#ifdef __cplusplus 
}
#endif
#endif /* __GLES_INCLUDES_H__ */
