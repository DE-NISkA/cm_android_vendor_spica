#include"gles11Context.h"
#include "includes.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MULTI_CONTEXT
int gles11_initcount = 0;
pthread_key_t tls_glescontext_key11 = 0;
#else
void * g_ProcessTlsCtx11;
//void * g_ProcessTlsCtx20; //ramdev

#endif

//Initializes the 3D HW and the pool memory

//returns    GL_TRUE/GL_FALSE on success/failure

//GLboolean GLES11Initdriver(int g3d_fd)

GLboolean GLES11Initdriver(void)
{
#ifdef MULTI_CONTEXT
  if(gles11_initcount == 0)
  {
    pthread_key_create(&tls_glescontext_key11,NULL);
  }
  gles11_initcount++;
#endif

//  return GLES2Initdriver(g3d_fd);

  return GLES2Initdriver();
}

//Attaches the framebuffer (surface) to the current GL context

GLboolean GLES11SetSurfaceData(GLES11SurfaceData * surfdata)
{
    if(surfdata)
    return GLES2SetSurfaceData((GLES2SurfaceData *)surfdata);

  return GL_TRUE;
}

GLboolean GLES11SetDrawSurface(GLES11SurfaceData *surfdata)
{
      if(surfdata)
              return GLES2SetDrawSurface((GLES2SurfaceData *)surfdata);

      return GL_TRUE;
}

GLboolean GLES11SetReadSurface(GLES11SurfaceData *surfdata)
{
      if(surfdata)
              return GLES2SetReadSurface((GLES2SurfaceData *)surfdata);

      return GL_TRUE;
}

GLboolean GLES11Flush(void)
{
  return GLES2Flush();
}

#if 0
//Notifies the GL library that a swap buffer is performed.

//returns  GL_FALSE is no valid surface is bound to the context.

GLboolean GLES11SwapBuffer(void)
{
  return GLES2SwapBuffer();
}

#endif

//Creates the GLES 1.1 graphics context

GLES11Context GLES11CreateContext(GLES11Context sharedctx)
{
  GLES11Context ctx11 = CreateStateGLES11(sharedctx);
  //SET_GLES11_STATE(ctx11)

  return ctx11;
}

//Sets the context pointed to by ctx as the current context for

//  the current thread

GLES11Context GLES11SetContext(GLES11Context ctx1)
{
  GET_GLES_CONTEXT_NOCHECK

  if(ctx1 != NULL)
  {
    SET_GLES11_STATE(ctx1)
    //GLES2Context ctx20 =  ((GLState *) ctx1)->gl20context;

    //GET_GLES20_CONTEXT

    //GLES2Context ctx20 = pthread_getspecific(tls_glescontext_key20);

    //GLES2SetContext(ctx20);

  }

  return ctx;
}

//Destroys the graphics context

GLboolean GLES11DestroyContext(GLES11Context ctx1)
{
  //GLES2Context ctx20 = pthread_getspecific(tls_glescontext_key20);

#if 0
  GET_GLES20_CONTEXT
#else
  GLES2Context ctx20 = (GLES2Context ) ((GLState*) ctx1)->gl20context;
#endif
  GLES2DestroyContext(ctx20);

  //GET_GLES_CONTEXT

  //GLES11Context ctx11 = pthread_getspecific(tls_glescontext_key11);


  GET_GLES_CONTEXT_RETURN


  //when current context  is to be deleted

  if (ctx == ctx1){
    SET_GLES11_STATE(NULL)
    //SET_GLES20_STATE(NULL) //ramdev

  }

  //delete (GLState *)ctx1;

  //ctx1 = NULL;

  //FREE_MEM_CHKRST(ctx1);

        DestroyStateGLES11((GLState *)ctx1);
  return GL_TRUE;
}

//De initializes the driver. This frees up the pool memory.

//Should be done when no more GL contexts are active.

GLboolean GLES11DeInitdriver(void)
{
#ifdef MULTI_CONTEXT
    gles11_initcount--;
#endif

  return GLES2DeInitdriver();
}

GLES2Context GetGLstate20(void)
{
  //GET_GLES_CONTEXT

  GET_GLES_CONTEXT_RETURN
  return ctx->gl20context;
}

void SetGLstate20(GLES2Context context)
{
    return;
}

#ifdef __cplusplus
}
#endif
