#include "includes.h"
#include "OGL2Func_for_gles11.h"

GL_API void GL_APIENTRY glAlphaFunc (GLenum func, GLclampf ref)
{
         //GL11LOGMSG(LOG_GLES11_API,("glAlphaFunc(0x%x\t0x%x)\n",func,ref));

  OGL2_glAlphaFuncEXP (func, ref);

  //DBGFUNCTION_EXIT("glAlphaFunc ")
}

GL_API void GL_APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor)
{

         //GL11LOGMSG(LOG_GLES11_API,("glBlendFunc(0x%x\t0x%x)\n",sfactor,dfactor));

  OGL2_glBlendFunc (sfactor, dfactor);

  //DBGFUNCTION_EXIT("glBlendFunc ")
}

GL_API void GL_APIENTRY glAlphaFuncx (GLenum func, GLclampx ref)
{
  GET_GLES_CONTEXT

         //GL11LOGMSG(LOG_GLES11_API,("glAlphaFuncx(0x%x\t0x%x)\n",func,ref));

  //DBGFUNCTION_INIT("glAlphaFuncx ")
  //RETAILMSG(1,(TEXT("glAlphaFuncx  \r\n")));

  glAlphaFunc(func, FIXED_TO_FLOAT(ref));

  //DBGFUNCTION_EXIT("glAlphaFuncx ")
}

GL_API void GL_APIENTRY glDepthFunc (GLenum func)
{

         //GL11LOGMSG(LOG_GLES11_API,("glDepthFunc(0x%x)\n",func));

  OGL2_glDepthFunc(func);

  //DBGFUNCTION_EXIT("glDepthFunc ")
}

GL_API void GL_APIENTRY glDepthMask (GLboolean flag)
{

         //GL11LOGMSG(LOG_GLES11_API,("glDepthMask(0x%x)\n",flag));

  OGL2_glDepthMask(flag);

  //DBGFUNCTION_EXIT("glDepthMask ")
}

GL_API void GL_APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask)
{

         //GL11LOGMSG(LOG_GLES11_API,("glStencilFunc(0x%x\t0x%x\t0x%x)\n",func,ref,mask));

  OGL2_glStencilFunc(func,ref,mask);

  //DBGFUNCTION_EXIT("glStencilFunc ")
}

GL_API void GL_APIENTRY glStencilMask (GLuint mask)
{

         //GL11LOGMSG(LOG_GLES11_API,("glStencilMask(0x%x)\n",mask));

  OGL2_glStencilMask(mask);

  //DBGFUNCTION_EXIT("glStencilMask ")
}

GL_API void GL_APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{

         //GL11LOGMSG(LOG_GLES11_API,("glStencilOp(0x%x\t0x%x\t0x%x)\n",fail,zfail,zpass));

  OGL2_glStencilOp(fail,zfail,zpass);

  //DBGFUNCTION_EXIT("glStencilOp ")
}

GL_API void GL_APIENTRY glSampleCoverage (GLclampf value, GLboolean invert)
{

         //GL11LOGMSG(LOG_GLES11_API,("glSampleCoverage(0x%x\t0x%x)\n",value,invert));

  OGL2_glSampleCoverage(value,invert);

  //DBGFUNCTION_EXIT("glSampleCoverage ")
}

GL_API void GL_APIENTRY glSampleCoveragex (GLclampx value, GLboolean invert)
{
  GET_GLES_CONTEXT

         //GL11LOGMSG(LOG_GLES11_API,("glSampleCoveragex(0x%x\t0x%x)\n",value,invert));

  //DBGFUNCTION_INIT("glSampleCoveragex ")
  //RETAILMSG(1,(TEXT("glSampleCoveragex  \r\n")));

  glSampleCoverage(FIXED_TO_FLOAT(value), invert) ;

  //DBGFUNCTION_EXIT("glSampleCoveragex ")
}

GL_API void GL_APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{

         //GL11LOGMSG(LOG_GLES11_API,("glScissor(0x%x\t0x%x\t0x%x\t0x%x)\n",x,y,width,height));

  OGL2_glScissor(x,y,width,height);

  //DBGFUNCTION_EXIT("glScissor ")
}

GL_API void GL_APIENTRY glLogicOp (GLenum opcode)
{
/*  GET_GLES_CONTEXT

  //DBGFUNCTION_INIT("glLogicOp ")
  //RETAILMSG(1,(TEXT("glLogicOp  \r\n")));

  CHECK_GL_ENUM(((opcode!=GL_CLEAR) && (opcode!=GL_SET) &&(opcode!=GL_COPY) &&
          (opcode!=GL_COPY_INVERTED) && (opcode!=GL_NOOP) &&(opcode!=GL_INVERT) &&
          (opcode!=GL_AND) && (opcode!=GL_NAND) &&(opcode!=GL_OR) &&
          (opcode!=GL_NOR) && (opcode!=GL_XOR) &&(opcode!=GL_EQUIV) &&
          (opcode!=GL_AND_REVERSE) && (opcode!=GL_AND_INVERTED) &&
          (opcode!=GL_OR_REVERSE) &&(opcode!=GL_OR_INVERTED) ))

  ctx->logicOp = opcode ;
*/

         //GL11LOGMSG(LOG_GLES11_API,("glLogicOp(0x%x)\n",opcode));

  OGL2_glLogicOpEXP (opcode);

  //DBGFUNCTION_EXIT("glLogicOp ")
}
