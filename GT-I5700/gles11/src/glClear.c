#include "includes.h"
#include "OGL2Func_for_gles11.h"

GL_API void GL_APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	//GL11LOGMSG(LOG_GLES11_API,("glClearColor(0x%x\t0x%x\t0x%x\t0x%x)\n",red,green,blue,alpha));

	OGL2_glClearColor(red, green, blue, alpha);

	//DBGFUNCTION_EXIT("glClearColor ")
}
GL_API void GL_APIENTRY glClearDepthf (GLclampf depth)
{
	//GL11LOGMSG(LOG_GLES11_API,("glClearDepthf(0x%x)\n",depth));

	OGL2_glClearDepthf(depth);

	//DBGFUNCTION_EXIT("glClearDepthf ")
}
void FlushStatePersistent(GLState *ctx)/* Call this before Start of frame , set mgl properties */;

GL_API void GL_APIENTRY glClear (GLbitfield mask)
{

	//GL11LOGMSG(LOG_GLES11_API,("glClear(0x%x)\n",mask));

	OGL2_glClear(mask);

	//DBGFUNCTION_EXIT("glClear ")
}
GL_API void GL_APIENTRY glClearColorx (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{

	//GL11LOGMSG(LOG_GLES11_API,("glClearColorx(0x%x\t0x%x\t0x%x\t0x%x)\n",red,green,blue,alpha));

	glClearColor(FIXED_TO_FLOAT(red), FIXED_TO_FLOAT(green), FIXED_TO_FLOAT(blue), FIXED_TO_FLOAT(alpha));

	//DBGFUNCTION_EXIT("glClearColorx ")
}
GL_API void GL_APIENTRY glClearDepthx (GLclampx depth)
{

	//GL11LOGMSG(LOG_GLES11_API,("glClearDepthx(0x%x)\n",depth));

	glClearDepthf(FIXED_TO_FLOAT(depth));

	//DBGFUNCTION_EXIT("glClearDepthx ")
}
GL_API void GL_APIENTRY glClearStencil (GLint s)
{

	//GL11LOGMSG(LOG_GLES11_API,("glClearStencil(0x%x)\n",s));

	OGL2_glClearStencil(s);

	//DBGFUNCTION_EXIT("glClearStencil ")
}
