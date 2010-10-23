#include "includes.h"
#include "glState.h"
#include "attribs.h"
#include "OGL2Func_for_gles11.h"

extern "C" GLuint shaderProgram;

//int loadShaders2();

#if 0
extern "C" void ShaderInit()
{
  //GET_GLES_CONTEXT
//  static int InitDone = 0;

  fflush(stdout);
  //if(!ctx->InitDone)
  {
    //int pos;
    float c[] = {1.0f , 1.0f , 1.0f , 1.0f};
    OGL2_glVertexAttrib4fv(ATTRIB_LOC_Color,c);
    //ctx->InitDone = 1;
  }
}
#endif
 extern "C" void ShaderInit()
{
  GET_GLES_CONTEXT
  float c[] = {1.0f , 1.0f , 1.0f , 1.0f};
    OGL2_glVertexAttrib4fv(ATTRIB_LOC_Color,c);
        OGL2_glDisableVertexAttribArray(ATTRIB_LOC_Normal);
        OGL2_glVertexAttrib3f(ATTRIB_LOC_Normal,0.0f, 0.0f, 1.0f);
}

GL_API void GL_APIENTRY glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
  //int pos;
  //ShaderInit();
  //pos = OGL2_glGetAttribLocation( shaderProgram, "Vertex");
         GL11LOGMSG(LOG_GLES11_API,("glVertexPointer(0x%x\t0x%x\t0x%x\t0x%x)\n",size,type,stride,pointer));

  OGL2_glVertexAttribPointer(ATTRIB_LOC_Vertex,size,type,0,stride,pointer);
}

GL_API void GL_APIENTRY glNormalPointer( GLenum type, GLsizei stride, const GLvoid *pointer)
{
  //int pos;
  //ShaderInit();
  //pos = OGL2_glGetAttribLocation( shaderProgram, "Normal");
         GL11LOGMSG(LOG_GLES11_API,("glNormalPointer(0x%x\t0x%x\t0x%x)\n",type,stride,pointer));

  OGL2_glVertexAttribPointer(ATTRIB_LOC_Normal,3,type,1,stride,pointer);
}

GL_API void GL_APIENTRY glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
  GLboolean normal;
  //int pos;
  //ShaderInit();
  //pos = OGL2_glGetAttribLocation( shaderProgram, "Color");
         GL11LOGMSG(LOG_GLES11_API,("glColorPointer(0x%x\t0x%x\t0x%x\t0x%x)\n",size,type,stride,pointer));

  if(type == GL_FIXED) normal=0;
  else normal=1;

  OGL2_glVertexAttribPointer(ATTRIB_LOC_Color,size,type,normal,stride,pointer);
}

GL_API void GL_APIENTRY glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
  GET_GLES_CONTEXT
  //int pos;
  //ShaderInit();

         GL11LOGMSG(LOG_GLES11_API,("glTexCoordPointer(0x%x\t0x%x\t0x%x\t0x%x)\n",size,type,stride,pointer));

  switch (ctx->clientActiveTex)
  {
    case 0:
      //pos = OGL2_glGetAttribLocation( shaderProgram, "MultiTexCoord0"); // Temporarily changed
      OGL2_glVertexAttribPointer(ATTRIB_LOC_MultiTexCoord[0],size,type,0,stride,pointer);
      break;
    case 1:
      OGL2_glVertexAttribPointer(ATTRIB_LOC_MultiTexCoord[1],size,type,0,stride,pointer);
      break;
  }
}
