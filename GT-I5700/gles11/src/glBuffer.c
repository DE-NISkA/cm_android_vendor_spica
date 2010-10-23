
/******************************************************************************
This file contains the GL API functions pertaining to buffer object management.
Buffer object names are created using glGenBuffers and cleared using
glDeleteBuffers. We create buffer object and put it in map in GenBuffers or at
at first bind time for random names.

Random names can also be used. The application can verify if the name is already
in use by using the glIsBuffer command.

A named buffer object is bound to a target (GL_ARRAY_BUFFER or
GL_ELEMENT_ARRAY_BUFFER) using glBindBuffer. When bound for the first time the
object is created. Binding 0 stops using buffer objects.

The buffer bound to target is modified using glBufferData & glBufferSubData.
If data given is NULL only memory is allocated and not initialized.

glGetBufferParameteriv .....????
BUFFERLIST......HashMap & Max Buffer Id used - for easy GenBuffers
        IDENTIFIER BUFFEROBJECTPTR (INIT NULL)

BUFFEROBJECTPTR
  Data Ptr
  Bound Object Ptr List - Array[2] - Array Buffer, Element Array Buffer
              - Array[7] - Vertex, Color, Tex, Normal,Matrix Index,
              Weight, Point Size
  Size
  Usage
******************************************************************************/
#include "includes.h"
#include "OGL2Func_for_gles11.h"

/******************************************************************************
            GL FUNCTIONS START HERE
******************************************************************************/
GL_API void GL_APIENTRY glBindBuffer (GLenum target, GLuint buffer)
{
         //GL11LOGMSG(LOG_GLES11_API,("glBindBuffer(0x%x\t0x%x)\n",target,buffer));

  OGL2_glBindBuffer(target,buffer);

  //DBGFUNCTION_EXIT("glBindBuffer")
}
GL_API void GL_APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
         //GL11LOGMSG(LOG_GLES11_API,("glBufferData(0x%x\t0x%x\t0x%x\t0x%x)\n",target,size,data,usage));

  OGL2_glBufferData(target,size,data,usage);

  //DBGFUNCTION_EXIT("glBufferData ")
}
GL_API void GL_APIENTRY glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
         //GL11LOGMSG(LOG_GLES11_API,("glBufferSubData(0x%x\t0x%x\t0x%x\t0x%x)\n",target,offset,size,data));

  OGL2_glBufferSubData(target,offset,size,data);

  //DBGFUNCTION_EXIT("glBufferSubData ")
}
GL_API void GL_APIENTRY glDeleteBuffers (GLsizei n, const GLuint *buffers)
{
         //GL11LOGMSG(LOG_GLES11_API,("glDeleteBuffers(0x%x\t0x%x)\n",n,buffers));

  OGL2_glDeleteBuffers(n,buffers);

  //DBGFUNCTION_EXIT("glDeleteBuffers ")
}
GL_API void GL_APIENTRY glGenBuffers (GLsizei n, GLuint *buffers)
{

         //GL11LOGMSG(LOG_GLES11_API,("glGenBuffers(0x%x\t0x%x)\n",n,buffers));

  OGL2_glGenBuffers(n,buffers);

  //DBGFUNCTION_EXIT("glGenBuffers ")
}
GL_API GLboolean GL_APIENTRY glIsBuffer (GLuint buffer)
{

         //GL11LOGMSG(LOG_GLES11_API,("glIsBuffer(0x%x)\n",buffer));

  return OGL2_glIsBuffer(buffer);

  //DBGFUNCTION_EXIT("glIsBuffer ")
}
GL_API void GL_APIENTRY glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params)
{

         //GL11LOGMSG(LOG_GLES11_API,("glGetBufferParameteriv(0x%x\t0x%x\t0x%x)\n",target,pname,params));

  OGL2_glGetBufferParameteriv(target,pname,params);

  //DBGFUNCTION_EXIT("glGetBufferParameteriv ")
}
