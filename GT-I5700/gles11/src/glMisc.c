#include "includes.h"
#include "OGL2Func_for_gles11.h"
#include "attribs.h"

GL_API void GL_APIENTRY glDepthRangef (GLclampf zNear, GLclampf zFar)
{

         //GL11LOGMSG(LOG_GLES11_API,("glDepthRangef(0x%x\t0x%x)\n",zNear,zFar));

  OGL2_glDepthRangef(zNear,zFar);

  //DBGFUNCTION_EXIT("glDepthRangef ")
}

GL_API void GL_APIENTRY glDepthRangex (GLclampx zNear, GLclampx zFar)
{
  GLclampf near,far;

         //GL11LOGMSG(LOG_GLES11_API,("glDepthRangex(0x%x\t0x%x)\n",zNear,zFar));

  near = CLAMPF(FIXED_TO_FLOAT(zNear)) ;
  far = CLAMPF(FIXED_TO_FLOAT(zFar)) ;

  glDepthRangef(near,far);

  //DBGFUNCTION_EXIT("glDepthRangex ")
}

GL_API void GL_APIENTRY glPolygonOffset (GLfloat factor, GLfloat units)
{

         //GL11LOGMSG(LOG_GLES11_API,("glPolygonOffset(0x%x\t0x%x)\n",factor,units));

  OGL2_glPolygonOffset(factor,units);

  //DBGFUNCTION_EXIT("glPolygonOffset ")
}

GL_API void GL_APIENTRY glCullFace (GLenum mode)
{

         //GL11LOGMSG(LOG_GLES11_API,("glCullFace(0x%x)\n",mode));

  OGL2_glCullFace(mode);

  //DBGFUNCTION_EXIT("glCullFace ")
}

GL_API void GL_APIENTRY glFrontFace (GLenum mode)
{

         //GL11LOGMSG(LOG_GLES11_API,("glFrontFace(0x%x)\n",mode));

  OGL2_glFrontFace(mode);

  //DBGFUNCTION_EXIT("glFrontFace ")
}

GL_API void GL_APIENTRY glHint (GLenum target, GLenum mode)
{
  GET_GLES_CONTEXT

         //GL11LOGMSG(LOG_GLES11_API,("glHint(0x%x\t0x%x)\n",target,mode));

  //DBGFUNCTION_INIT("glHint ")
  //RETAILMSG(1,( TEXT("glHint \r\n")));

  CHECK_GL_ENUM((mode !=GL_FASTEST) &&(mode !=GL_NICEST) &&(mode !=GL_DONT_CARE))
  CHECK_GL_ENUM((target !=GL_GENERATE_MIPMAP_HINT) &&(target !=GL_FOG_HINT) &&(target !=GL_LINE_SMOOTH_HINT)
            &&(target !=GL_POINT_SMOOTH_HINT) &&(target !=GL_PERSPECTIVE_CORRECTION_HINT))

  switch ( target )
  {
    case GL_GENERATE_MIPMAP_HINT:
      //ctx->hGenMipmap = mode ;
      OGL2_glHint(target,mode);
      break ;
    case GL_FOG_HINT:
      ctx->hFog = mode ;
      break ;
    case GL_LINE_SMOOTH_HINT:
      ctx->hLineSmooth = mode ;
      break ;
    case GL_POINT_SMOOTH_HINT:
      ctx->hPointSmooth = mode ;
      break ;
    case GL_PERSPECTIVE_CORRECTION_HINT:
      ctx->hPerspective = mode ;
      break ;

  }

  //DBGFUNCTION_EXIT("glHint ")
}


GL_API void GL_APIENTRY glPixelStorei (GLenum pname, GLint param)
{

         //GL11LOGMSG(LOG_GLES11_API,("glPixelStorei(0x%x\t0x%x)\n",pname,param));

  OGL2_glPixelStorei (pname, param);

  //DBGFUNCTION_EXIT("glPixelStorei ")
}

GL_API void GL_APIENTRY glPolygonOffsetx (GLfixed factor, GLfixed units)
{
  GET_GLES_CONTEXT

         //GL11LOGMSG(LOG_GLES11_API,("glPolygonOffsetx(0x%x\t0x%x)\n",factor,units));

  //DBGFUNCTION_INIT("glPolygonOffsetx ")
  //RETAILMSG(1,(TEXT("glPolygonOffsetx  \r\n")));

  glPolygonOffset(FIXED_TO_FLOAT(factor) , FIXED_TO_FLOAT(units));
  //DBGFUNCTION_EXIT("glPolygonOffsetx ")
}

GL_API void GL_APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
{

         //GL11LOGMSG(LOG_GLES11_API,("glReadPixels(0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",x,y,width,height,format,type,pixels));

  OGL2_glReadPixels (x, y, width, height, format, type, pixels);

  //DBGFUNCTION_EXIT("glReadPixels ")
}

GL_API void GL_APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{

         //GL11LOGMSG(LOG_GLES11_API,("glViewport(0x%x\t0x%x\t0x%x\t0x%x)\n",x,y,width,height));

  OGL2_glViewport(x, y, width, height);

  //DBGFUNCTION_EXIT("glViewport ")
}

GL_API void GL_APIENTRY glLineWidth (GLfloat width)
{

         //GL11LOGMSG(LOG_GLES11_API,("glLineWidth(0x%x)\n",width));

  OGL2_glLineWidth ( width);

  //DBGFUNCTION_EXIT("glLineWidth ")
}

GL_API void GL_APIENTRY glPointSize (GLfloat size)
{
  GET_GLES_CONTEXT

         //GL11LOGMSG(LOG_GLES11_API,("glPointSize(0x%x)\n",size));

  //DBGFUNCTION_INIT("glPointSize ")
  //RETAILMSG(1,(TEXT("glPointSize  \r\n")));

  ctx->pointSize = size ;

  OGL2_glDisableVertexAttribArray(ATTRIB_LOC_PointSize);

  //OGL2_glPointSize ( size );

  //DBGFUNCTION_EXIT("glPointSize ")
}

GL_API void GL_APIENTRY glLineWidthx (GLfixed width)
{
  GET_GLES_CONTEXT

         //GL11LOGMSG(LOG_GLES11_API,("glLineWidthx(0x%x)\n",width));

  //DBGFUNCTION_INIT("glLineWidthx ")
  //RETAILMSG(1,(TEXT("glLineWidthx  \r\n")));

  glLineWidth(FIXED_TO_FLOAT(width));
  //DBGFUNCTION_EXIT("glLineWidthx ")
}

GL_API void GL_APIENTRY glPointSizex (GLfixed size)
{
  GET_GLES_CONTEXT

         //GL11LOGMSG(LOG_GLES11_API,("glPointSizex(0x%x)\n",size));

  //DBGFUNCTION_INIT("glPointSizex ")
  //RETAILMSG(1,(TEXT("glPointSizex  \r\n")));

  ctx->pointSize = FIXED_TO_FLOAT(size) ;
  glPointSize(FIXED_TO_FLOAT(size));
  //DBGFUNCTION_EXIT("glPointSizex ")
}

GL_API void GL_APIENTRY glPointParameterf (GLenum pname, GLfloat param)
{
  GET_GLES_CONTEXT

         //GL11LOGMSG(LOG_GLES11_API,("glPointParameterf(0x%x\t0x%x)\n",pname,param));

  //DBGFUNCTION_INIT("glPointParameterf ")
  //RETAILMSG(1,(TEXT("glPointParameterf  \r\n")));
  CHECK_GL_ENUM((pname != GL_POINT_SIZE_MIN)&&
          (pname != GL_POINT_SIZE_MAX)&&
          (pname != GL_POINT_FADE_THRESHOLD_SIZE)
          )
  switch ( pname )
  {
    case GL_POINT_SIZE_MIN:
      ctx->pointSizeMin = param ;
      break ;
    case GL_POINT_SIZE_MAX:
      ctx->pointSizeMax = param ;
      break ;
    case GL_POINT_FADE_THRESHOLD_SIZE:
      ctx->pointFadeThresh = param ;
      break ;
  }

  //DBGFUNCTION_EXIT("glPointParameterf ")
}

GL_API void GL_APIENTRY glPointParameterfv (GLenum pname, const GLfloat *params)
{
  GET_GLES_CONTEXT

         //GL11LOGMSG(LOG_GLES11_API,("glPointParameterfv(0x%x\t0x%x)\n",pname,params));

  //DBGFUNCTION_INIT("glPointParameterfv ")
  //RETAILMSG(1,(TEXT("glPointParameterfv  \r\n")));
  CHECK_GL_ENUM((pname != GL_POINT_SIZE_MIN)&&
          (pname != GL_POINT_SIZE_MAX)&&
          (pname != GL_POINT_FADE_THRESHOLD_SIZE)&&
          (pname != GL_POINT_DISTANCE_ATTENUATION)
          )
  switch ( pname )
  {
    case GL_POINT_SIZE_MIN:
      ctx->pointSizeMin = params[0] ;
      break ;
    case GL_POINT_SIZE_MAX:
      ctx->pointSizeMax = params[0] ;
      break ;
    case GL_POINT_FADE_THRESHOLD_SIZE:
      ctx->pointFadeThresh = params[0] ;
      break ;
    case GL_POINT_DISTANCE_ATTENUATION:
      ctx->pointDistAtten[0] = params[0] ;
      ctx->pointDistAtten[1] = params[1] ;
      ctx->pointDistAtten[2] = params[2] ;
      break ;
  }

  //DBGFUNCTION_EXIT("glPointParameterfv ")
}

GL_API void GL_APIENTRY glPointParameterx (GLenum pname, GLfixed param)
{
  GET_GLES_CONTEXT

         //GL11LOGMSG(LOG_GLES11_API,("glPointParameterx(0x%x\t0x%x)\n",pname,param));

  //DBGFUNCTION_INIT("glPointParameterx ")
  //RETAILMSG(1,(TEXT("glPointParameterx  \r\n")));

  CHECK_GL_ENUM((pname != GL_POINT_SIZE_MIN)&&
          (pname != GL_POINT_SIZE_MAX)&&
          (pname != GL_POINT_FADE_THRESHOLD_SIZE)
          )
  glPointParameterxv (pname, &param);
  //DBGFUNCTION_EXIT("glPointParameterx ")
}

GL_API void GL_APIENTRY glPointParameterxv (GLenum pname, const GLfixed *params)
{
  GET_GLES_CONTEXT

         //GL11LOGMSG(LOG_GLES11_API,("glPointParameterxv(0x%x\t0x%x)\n",pname,params));

  //DBGFUNCTION_INIT("glPointParameterxv ")
  //RETAILMSG(1,(TEXT("glPointParameterxv  \r\n")));
  CHECK_GL_ENUM((pname != GL_POINT_SIZE_MIN)&&
          (pname != GL_POINT_SIZE_MAX)&&
          (pname != GL_POINT_FADE_THRESHOLD_SIZE)&&
          (pname != GL_POINT_DISTANCE_ATTENUATION)
          )
  switch ( pname )
  {
    case GL_POINT_SIZE_MIN:
      ctx->pointSizeMin = FLOAT_TO_FIXED(params[0] );
      break ;
    case GL_POINT_SIZE_MAX:
      ctx->pointSizeMax = FLOAT_TO_FIXED(params[0] );
      break ;
    case GL_POINT_FADE_THRESHOLD_SIZE:
      ctx->pointFadeThresh = FLOAT_TO_FIXED(params[0] );
      break ;
    case GL_POINT_DISTANCE_ATTENUATION:
      ctx->pointDistAtten[0] =FLOAT_TO_FIXED( params[0]) ;
      ctx->pointDistAtten[1] = FLOAT_TO_FIXED(params[1] );
      ctx->pointDistAtten[2] = FLOAT_TO_FIXED(params[2]) ;

      break ;

  }

  //DBGFUNCTION_EXIT("glPointParameterxv ")
}
