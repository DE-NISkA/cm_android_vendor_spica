#include "includes.h"
#include "attribs.h"
#include "OGL2Func_for_gles11.h"

extern GLuint shaderProgram;

int GetTypeSize(GLenum type )
{
	switch (type )
	{
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
		return 1 ;
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
		return 2 ;
	case GL_FLOAT:
	case GL_FIXED:
		return 4 ;
	}
	return 0 ;
}

GL_API void GL_APIENTRY glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glColor4f(%ff\t%ff\t%ff\t%ff)\n",red,green,blue,alpha));


	if (ctx->flags & F_COLOR_MATERIAL)
	{
		// If Color material is enabled then update the front and back ambient and diffuse material
		// properties to the current color, this change is permanent to the material properties.
		GLfloat temp[4];
		temp[0] = red;
		temp[1] = green;
		temp[2] = blue;
		temp[3] = alpha;
		VEC4CPY( ctx->frontNback.acm, temp );
		VEC4CPY( ctx->frontNback.dcm, temp );
	}

	ctx->glcolor = 0;
	ctx->AlphaorLuminance = 0;
	if(red !=1.0f || green !=1.0f || blue !=1.0f || alpha !=1.0f)
	{
	ctx->color[0]=red;
	ctx->color[1]=green;
	ctx->color[2]=blue;
	ctx->color[3]=alpha;

	ctx->glcolor = 1;
	
	if(red ==0.0f && green ==0.0f && blue ==0.0f)
		ctx->AlphaorLuminance = 1;
	}
	
	
	OGL2_glVertexAttrib4f(ATTRIB_LOC_Color,red,green,blue,alpha);

	//DBGFUNCTION_EXIT("glColor4f ")
}

GL_API void GL_APIENTRY glColor4x (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glColor4x(0x%x\t0x%x\t0x%x\t0x%x)\n",red,green,blue,alpha));

	//DBGFUNCTION_INIT("glColor4x ")
	//RETAILMSG(1,(TEXT("glColor4x  \r\n")));

	glColor4f(FIXED_TO_FLOAT(red),FIXED_TO_FLOAT(green),FIXED_TO_FLOAT(blue),FIXED_TO_FLOAT(alpha));

	//DBGFUNCTION_EXIT("glColor4x ")
}

GL_API void GL_APIENTRY glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	float red1,green1,blue1,alpha1;
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glColor4ub(0x%x\t0x%x\t0x%x\t0x%x)\n",red,green,blue,alpha));

	red1=((GLfloat) red)/255.0f;
	green1=((GLfloat) green)/255.0f;
	blue1=((GLfloat) blue)/255.0f;
	alpha1=((GLfloat) alpha)/255.0f;

	if (ctx->flags & F_COLOR_MATERIAL)
	{
		// If Color material is enabled then update the front and back ambient and diffuse material
		// properties to the current color, this change is permanent to the material properties.
		GLfloat temp[4];
		temp[0] = red1;
		temp[1] = green1;
		temp[2] = blue1;
		temp[3] = alpha1;
		VEC4CPY( ctx->frontNback.acm, temp );
		VEC4CPY( ctx->frontNback.dcm, temp );
	}

	OGL2_glVertexAttrib4f(ATTRIB_LOC_Color,red1,green1,blue1,alpha1);

	//DBGFUNCTION_EXIT("glColor4ub ")
}

GL_API void GL_APIENTRY glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
	//GL11LOGMSG(LOG_GLES11_API,("glNormal3f(%ff\t%ff\t%ff)\n",nx,ny,nz));

	OGL2_glDisableVertexAttribArray(ATTRIB_LOC_Normal);
	OGL2_glVertexAttrib3f(ATTRIB_LOC_Normal,nx,ny,nz);

	//DBGFUNCTION_EXIT("glNormal3f ")
}

GL_API void GL_APIENTRY glNormal3x (GLfixed nx, GLfixed ny, GLfixed nz)
{
	GLfloat normal[3];

	//GL11LOGMSG(LOG_GLES11_API,("glNormal3x(0x%x\t0x%x\t0x%x)\n",nx,ny,nz));

	normal[0] = FIXED_TO_FLOAT(nx) ;
	normal[1] = FIXED_TO_FLOAT(ny) ;
	normal[2] = FIXED_TO_FLOAT(nz) ;

	glNormal3f(normal[0],normal[1],normal[2]);

	//DBGFUNCTION_EXIT("glNormal3x ")
}

GL_API void GL_APIENTRY glPointSizePointerOES ( GLenum type, GLsizei stride, const GLvoid *pointer)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glPointSizePointerOES(0x%x\t0x%x\t0x%x)\n",type,stride,pointer));

	OGL2_glVertexAttribPointer(ATTRIB_LOC_PointSize,1,type,0,stride,pointer);

	//DBGFUNCTION_EXIT("glPointSizePointerOES ")
}

GL_API void GL_APIENTRY glMatrixIndexPointerOES (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glMatrixIndexPointerOES(0x%x\t0x%x\t0x%x\t0x%x)\n",size,type,stride,pointer));

	//DBGFUNCTION_INIT("glMatrixIndexPointerOES ")
	//RETAILMSG(1,(TEXT("glMatrixIndexPointerOES  \r\n")));

	CHECK_GL_VALUE((size > C_MAX_VERTEX_UNITS_OES))
	CHECK_GL_ENUM((type != GL_UNSIGNED_BYTE))
	CHECK_GL_VALUE((stride < 0))

	ctx->noOfVertexUnit = (GLfloat) size;

	OGL2_glVertexAttribPointer(ATTRIB_LOC_MatrixIndices,size,type,0,stride,pointer);

	//DBGFUNCTION_EXIT("glMatrixIndexPointerOES ")
}

GL_API void GL_APIENTRY glWeightPointerOES (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glWeightPointerOES(0x%x\t0x%x\t0x%x\t0x%x)\n",size,type,stride,pointer));

	//DBGFUNCTION_INIT("glWeightPointerOES ")
	//RETAILMSG(1,(TEXT("glWeightPointerOES  \r\n")));

	CHECK_GL_VALUE((size > C_MAX_VERTEX_UNITS_OES))
	CHECK_GL_ENUM((type != GL_FLOAT) &&(type != GL_FIXED))
	CHECK_GL_VALUE((stride < 0))

	ctx->noOfVertexUnit = (GLfloat)size;

	OGL2_glVertexAttribPointer(ATTRIB_LOC_Weights,size,type,0,stride,pointer);

	//DBGFUNCTION_EXIT("glWeightPointerOES ")
}
