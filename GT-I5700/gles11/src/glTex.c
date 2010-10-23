#include "gles11Context.h"
#include "includes.h"
#include "OGL2Func_for_gles11.h"
#include "attribs.h"

GL_API void GL_APIENTRY glBindTexture (GLenum target, GLuint texture)
{
	//GL11LOGMSG(LOG_GLES11_API,("glBindTexture(0x%x\t0x%x)\n",target,texture));

	OGL2_glBindTexture ( target,  texture);

	//DBGFUNCTION_EXIT("glBindTexture ")
}

GL_API void GL_APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures)
{

	//GL11LOGMSG(LOG_GLES11_API,("glDeleteTextures(0x%x\t0x%x)\n",n,textures));

	OGL2_glDeleteTextures(n,textures);

	//DBGFUNCTION_EXIT("glDeleteTextures ")
}

GL_API void GL_APIENTRY glGenTextures (GLsizei n, GLuint *textures)
{

	//GL11LOGMSG(LOG_GLES11_API,("glGenTextures(0x%x\t0x%x)\n",n,*textures));

	OGL2_glGenTextures (n, textures);

	//DBGFUNCTION_EXIT("glGenTextures ")
}

GL_API GLboolean GL_APIENTRY glIsTexture (GLuint texture)
{

	//GL11LOGMSG(LOG_GLES11_API,("glIsTexture(0x%x)\n",texture));

	return OGL2_glIsTexture(texture);

	//DBGFUNCTION_EXIT("glIsTexture ")
}

GL_API void GL_APIENTRY glActiveTexture (GLenum texture)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glActiveTexture(0x%x)\n",texture));

	//DBGFUNCTION_INIT("glActiveTexture ")
	//RETAILMSG(1,(TEXT("glActiveTexture  \r\n")));

	CHECK_GL_ENUM((texture!=GL_TEXTURE0) && (texture != GL_TEXTURE1))

	ctx->activeTexture = texture - GL_TEXTURE0 ;


	OGL2_glActiveTexture (texture);

	//DBGFUNCTION_EXIT("glActiveTexture ")
}

GL_API void GL_APIENTRY glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glMultiTexCoord4f(0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",target,s,t,r,q));

	//DBGFUNCTION_INIT("glMultiTexCoord4f ")
	//RETAILMSG(1,(TEXT("glMultiTexCoord4f  \r\n")));
	CHECK_GL_ENUM((target < GL_TEXTURE0) || (target > GL_TEXTURE1))

	ctx->texCoords[target - GL_TEXTURE0][0] = s;
	ctx->texCoords[target - GL_TEXTURE0][1] = t;
	ctx->texCoords[target - GL_TEXTURE0][2] = r;
	ctx->texCoords[target - GL_TEXTURE0][3] = q;
	if ((target - GL_TEXTURE0) == 0)
	OGL2_glVertexAttrib4fv (ATTRIB_LOC_MultiTexCoord[0], ctx->texCoords[0]);
	else if ((target - GL_TEXTURE0) == 1)
	OGL2_glVertexAttrib4fv (ATTRIB_LOC_MultiTexCoord[1], ctx->texCoords[1]);

	//DBGFUNCTION_EXIT("glMultiTexCoord4f ")
}

GL_API void GL_APIENTRY glMultiTexCoord4x (GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glMultiTexCoord4x(0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",target,s,t,r,q));

	//DBGFUNCTION_INIT("glMultiTexCoord4x ")
	//RETAILMSG(1,(TEXT("glMultiTexCoord4x  \r\n")));

	glMultiTexCoord4f(target,FIXED_TO_FLOAT(s),FIXED_TO_FLOAT(t),FIXED_TO_FLOAT(r),FIXED_TO_FLOAT(q));


	//DBGFUNCTION_EXIT("glMultiTexCoord4x ")
}

GL_API void GL_APIENTRY glClientActiveTexture (GLenum texture)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glClientActiveTexture(0x%x)\n",texture));

	//DBGFUNCTION_INIT("glClientActiveTexture ")
	//RETAILMSG(1,(TEXT("glClientActiveTexture  \r\n")));

	ctx->clientActiveTex = texture - GL_TEXTURE0 ;

	//DBGFUNCTION_EXIT("glClientActiveTexture ")
}

GL_API void GL_APIENTRY glTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glTexEnvf(0x%x\t0x%x\t%ff)\n",target,pname,param));

	//DBGFUNCTION_INIT("glTexEnvf ")
	//RETAILMSG(1,(TEXT("glTexEnvf  \r\n")));

	CHECK_GL_ENUM(pname==GL_TEXTURE_ENV_COLOR)

	glTexEnvfv(target, pname, &param ) ;

	//DBGFUNCTION_EXIT("glTexEnvf ")
}

GL_API void GL_APIENTRY glTexEnvi (GLenum target, GLenum pname, GLint param)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glTexEnvi(0x%x\t0x%x\t0x%x)\n",target,pname,param));

	//DBGFUNCTION_INIT("glTexEnvi ")
	//RETAILMSG(1,(TEXT("glTexEnvi  \r\n")));

	CHECK_GL_ENUM(pname==GL_TEXTURE_ENV_COLOR)

	glTexEnviv(target, pname, &param ) ;

	//DBGFUNCTION_EXIT("glTexEnvi ")
}

GL_API void GL_APIENTRY glTexEnvx (GLenum target, GLenum pname, GLfixed param)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glTexEnvx(0x%x\t0x%x\t0x%x)\n",target,pname,param));

	//DBGFUNCTION_INIT("glTexEnvx ")
	//RETAILMSG(1,(TEXT("glTexEnvx  \r\n")));

	CHECK_GL_ENUM(pname==GL_TEXTURE_ENV_COLOR)

	glTexEnvxv(target, pname, &param ) ;

	//DBGFUNCTION_EXIT("glTexEnvx ")
}
GL_API void GL_APIENTRY glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glTexEnvfv(0x%x\t0x%x\t%ff)\n",target,pname,*params));

	//DBGFUNCTION_INIT("glTexEnvfv ")
	//RETAILMSG(1,(TEXT("glTexEnvfv  \r\n")));
	CHECK_GL_ENUM((GL_TEXTURE_ENV != target )&&(GL_POINT_SPRITE_OES != target ))
	CHECK_GL_ENUM((GL_TEXTURE_ENV == target )&&
	((GL_TEXTURE_ENV_MODE != pname )
	&& (GL_COMBINE_RGB != pname ) && (GL_COMBINE_ALPHA != pname )
	&& (GL_TEXTURE_ENV_COLOR != pname )
	&& (GL_SRC0_RGB != pname ) && (GL_SRC1_RGB != pname )
	&& (GL_SRC2_RGB != pname ) && (GL_OPERAND0_RGB != pname )
	&& (GL_OPERAND1_RGB != pname ) && (GL_OPERAND2_RGB != pname )
	&& (GL_SRC0_ALPHA != pname ) && (GL_SRC1_ALPHA != pname )
	&& (GL_SRC2_ALPHA != pname ) && (GL_OPERAND0_ALPHA != pname )
	&& (GL_OPERAND1_ALPHA != pname ) && (GL_OPERAND2_ALPHA != pname )
	&& (GL_RGB_SCALE!= pname ) && (GL_ALPHA_SCALE!= pname )
	)
	)


	CHECK_GL_ENUM((GL_POINT_SPRITE_OES == target )&&((GL_COORD_REPLACE_OES != pname )))
	CHECK_GL_ENUM((GL_TEXTURE_ENV == target )&&((GL_TEXTURE_ENV_MODE == pname )
	&& (GL_REPLACE != params[0] ) && (GL_MODULATE != params[0] )
	&& (GL_DECAL != params[0] ) && (GL_BLEND != params[0] )
	&& (GL_ADD != params[0] ) && (GL_COMBINE != params[0] )
	&& (GL_SUBTRACT != params[0] ) && (GL_INTERPOLATE != params[0] )
	&& (GL_ADD_SIGNED != params[0] ) && (GL_DOT3_RGB != params[0] )
	&& (GL_DOT3_RGBA != params[0] )
	))
	if ( target == GL_TEXTURE_ENV)
	{
		switch (pname)
		{
		case GL_TEXTURE_ENV_MODE:
			ctx->texEnvMode[ctx->activeTexture] =(GLenum) params[0];
			break ;
		case GL_COMBINE_RGB:
			ctx->texEnvCombineRGB[ctx->activeTexture]  = params[0];
			break ;
		case GL_COMBINE_ALPHA:
			ctx->texEnvCombineAlpha[ctx->activeTexture]  = params[0];
			break ;
		case GL_TEXTURE_ENV_COLOR:
			ctx->texEnvColor[ctx->activeTexture] [0] = params[0];
			ctx->texEnvColor[ctx->activeTexture] [1] = params[1];
			ctx->texEnvColor[ctx->activeTexture] [2] = params[2];
			ctx->texEnvColor[ctx->activeTexture] [3] = params[3];
			break ;
		case GL_SRC0_RGB:ctx->mSrcRGB[ctx->activeTexture][0]  = (GLenum ) params[0] ; break ;
		case GL_SRC1_RGB:ctx->mSrcRGB[ctx->activeTexture][1]= (GLenum ) params[0] ; break ;
		case GL_SRC2_RGB:ctx->mSrcRGB[ctx->activeTexture][2]= (GLenum ) params[0] ; break ;
		case GL_SRC0_ALPHA:ctx->mSrcAlpha[ctx->activeTexture][0]= (GLenum ) params[0] ; break ;
		case GL_SRC1_ALPHA:ctx->mSrcAlpha[ctx->activeTexture][1]= (GLenum ) params[0] ; break ;
		case GL_SRC2_ALPHA:ctx->mSrcAlpha[ctx->activeTexture][2]= (GLenum ) params[0] ; break ;
		case GL_OPERAND0_RGB:ctx->mOperandRGB[ctx->activeTexture][0] = (GLenum ) params[0] ; break ;
		case GL_OPERAND1_RGB:ctx->mOperandRGB[ctx->activeTexture][1] = (GLenum ) params[0] ; break ;
		case GL_OPERAND2_RGB:ctx->mOperandRGB[ctx->activeTexture][2] = (GLenum ) params[0] ; break ;
		case GL_OPERAND0_ALPHA:ctx->mOperandAlpha[ctx->activeTexture][0] = (GLenum ) params[0] ; break ;
		case GL_OPERAND1_ALPHA:ctx->mOperandAlpha[ctx->activeTexture][1] = (GLenum ) params[0] ; break ;
		case GL_OPERAND2_ALPHA:ctx->mOperandAlpha[ctx->activeTexture][2] = (GLenum ) params[0] ; break ;
		case GL_RGB_SCALE:ctx->mRGBScale[ctx->activeTexture]  = (GLfloat ) params[0] ; break ;
		case GL_ALPHA_SCALE:ctx->mAlphaScale[ctx->activeTexture] = (GLfloat ) params[0] ; break ;
		}
	}
	else
	{
		switch(pname)
		{
		case GL_COORD_REPLACE_OES:
			ctx->texReplaceCoord[ctx->activeTexture] = *params;
			break;

		default:
			break;
		}
	}
	//DBGFUNCTION_EXIT("glTexEnvfv ")
}

GL_API void GL_APIENTRY glTexEnviv (GLenum target, GLenum pname, const GLint *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glTexEnviv(0x%x\t0x%x\t0x%x)\n",target,pname,params));

	//DBGFUNCTION_INIT("glTexEnviv ")
	//RETAILMSG(1,(TEXT("glTexEnviv  \r\n")));

	CHECK_GL_ENUM((GL_TEXTURE_ENV != target )&&(GL_POINT_SPRITE_OES != target ))
	CHECK_GL_ENUM((GL_TEXTURE_ENV == target )&&
	((GL_TEXTURE_ENV_MODE != pname )
	&& (GL_COMBINE_RGB != pname ) && (GL_COMBINE_ALPHA != pname )
	&& (GL_TEXTURE_ENV_COLOR != pname )
	&& (GL_SRC0_RGB != pname ) && (GL_SRC1_RGB != pname )
	&& (GL_SRC2_RGB != pname ) && (GL_OPERAND0_RGB != pname )
	&& (GL_OPERAND1_RGB != pname ) && (GL_OPERAND2_RGB != pname )
	&& (GL_SRC0_ALPHA != pname ) && (GL_SRC1_ALPHA != pname )
	&& (GL_SRC2_ALPHA != pname ) && (GL_OPERAND0_ALPHA != pname )
	&& (GL_OPERAND1_ALPHA != pname ) && (GL_OPERAND2_ALPHA != pname )
	&& (GL_RGB_SCALE!= pname ) && (GL_ALPHA_SCALE!= pname )
	)
	)
	CHECK_GL_ENUM((GL_POINT_SPRITE_OES == target )&&((GL_COORD_REPLACE_OES != pname )))
	CHECK_GL_ENUM((GL_TEXTURE_ENV == target )&&((GL_TEXTURE_ENV_MODE == pname )
	&& (GL_REPLACE != params[0] ) && (GL_MODULATE != params[0] )
	&& (GL_DECAL != params[0] ) && (GL_BLEND != params[0] )
	&& (GL_ADD != params[0] ) && (GL_COMBINE != params[0] )
	&& (GL_SUBTRACT != params[0] ) && (GL_INTERPOLATE != params[0] )
	&& (GL_ADD_SIGNED != params[0] ) && (GL_DOT3_RGB != params[0] )
	&& (GL_DOT3_RGBA != params[0] )
	))
	if ( target == GL_TEXTURE_ENV)
	{
		switch (pname)
		{
		case GL_TEXTURE_ENV_MODE:
			ctx->texEnvMode[ctx->activeTexture] = params[0];
			break ;
		case GL_COMBINE_RGB:
			ctx->texEnvCombineRGB[ctx->activeTexture]  = (GLfloat)params[0];
			break ;
		case GL_COMBINE_ALPHA:
			ctx->texEnvCombineAlpha[ctx->activeTexture]  =(GLfloat) params[0];
			break ;
		case GL_TEXTURE_ENV_COLOR:
			ctx->texEnvColor [ctx->activeTexture][0] = (GLfloat)params[0];
			ctx->texEnvColor[ctx->activeTexture] [1] = (GLfloat)params[1];
			ctx->texEnvColor[ctx->activeTexture] [2] = (GLfloat)params[2];
			ctx->texEnvColor[ctx->activeTexture] [3] = (GLfloat)params[3];
			break ;
		case GL_SRC0_RGB:ctx->mSrcRGB[ctx->activeTexture][0]  = (GLenum ) params[0] ; break ;
		case GL_SRC1_RGB:ctx->mSrcRGB[ctx->activeTexture][1]= (GLenum ) params[0] ; break ;
		case GL_SRC2_RGB:ctx->mSrcRGB[ctx->activeTexture][2]= (GLenum ) params[0] ; break ;
		case GL_SRC0_ALPHA:ctx->mSrcAlpha[ctx->activeTexture][0]= (GLenum ) params[0] ; break ;
		case GL_SRC1_ALPHA:ctx->mSrcAlpha[ctx->activeTexture][1]= (GLenum ) params[0] ; break ;
		case GL_SRC2_ALPHA:ctx->mSrcAlpha[ctx->activeTexture][2]= (GLenum ) params[0] ; break ;
		case GL_OPERAND0_RGB:ctx->mOperandRGB[ctx->activeTexture][0] = (GLenum ) params[0] ; break ;
		case GL_OPERAND1_RGB:ctx->mOperandRGB[ctx->activeTexture][1] = (GLenum ) params[0] ; break ;
		case GL_OPERAND2_RGB:ctx->mOperandRGB[ctx->activeTexture][2] = (GLenum ) params[0] ; break ;
		case GL_OPERAND0_ALPHA:ctx->mOperandAlpha[ctx->activeTexture][0] = (GLenum ) params[0] ; break ;
		case GL_OPERAND1_ALPHA:ctx->mOperandAlpha[ctx->activeTexture][1] = (GLenum ) params[0] ; break ;
		case GL_OPERAND2_ALPHA:ctx->mOperandAlpha[ctx->activeTexture][2] = (GLenum ) params[0] ; break ;
		case GL_RGB_SCALE:ctx->mRGBScale [ctx->activeTexture] = (GLfloat ) params[0] ; break ;
		case GL_ALPHA_SCALE:ctx->mAlphaScale [ctx->activeTexture]= (GLfloat ) params[0] ; break ;
		}
	}
	else
	{
		switch(pname)
		{
		case GL_COORD_REPLACE_OES:
			ctx->texReplaceCoord[ctx->activeTexture] = *params;
			break;

		default:
			break;
		}
	}

	//DBGFUNCTION_EXIT("glTexEnviv ")
}

GL_API void GL_APIENTRY glTexEnvxv (GLenum target, GLenum pname, const GLfixed *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glTexEnvxv(0x%x\t0x%x\t0x%x)\n",target,pname,params));

	//DBGFUNCTION_INIT("glTexEnvxv ")
	//RETAILMSG(1,(TEXT("glTexEnvxv  \r\n")));

	CHECK_GL_ENUM((GL_TEXTURE_ENV != target )&&(GL_POINT_SPRITE_OES != target ))
	CHECK_GL_ENUM((GL_TEXTURE_ENV == target )&&
	((GL_TEXTURE_ENV_MODE != pname )
	&& (GL_COMBINE_RGB != pname ) && (GL_COMBINE_ALPHA != pname )
	&& (GL_TEXTURE_ENV_COLOR != pname )
	&& (GL_SRC0_RGB != pname ) && (GL_SRC1_RGB != pname )
	&& (GL_SRC2_RGB != pname ) && (GL_OPERAND0_RGB != pname )
	&& (GL_OPERAND1_RGB != pname ) && (GL_OPERAND2_RGB != pname )
	&& (GL_SRC0_ALPHA != pname ) && (GL_SRC1_ALPHA != pname )
	&& (GL_SRC2_ALPHA != pname ) && (GL_OPERAND0_ALPHA != pname )
	&& (GL_OPERAND1_ALPHA != pname ) && (GL_OPERAND2_ALPHA != pname )
	&& (GL_RGB_SCALE!= pname ) && (GL_ALPHA_SCALE!= pname )
	)
	)

	CHECK_GL_ENUM((GL_POINT_SPRITE_OES == target )&&((GL_COORD_REPLACE_OES != pname )))
	CHECK_GL_ENUM((GL_TEXTURE_ENV == target )&&((GL_TEXTURE_ENV_MODE == pname )
	&& (GL_REPLACE != params[0] ) && (GL_MODULATE != params[0] )
	&& (GL_DECAL != params[0] ) && (GL_BLEND != params[0] )
	&& (GL_ADD != params[0] ) && (GL_COMBINE != params[0] )
	&& (GL_SUBTRACT != params[0] ) && (GL_INTERPOLATE != params[0] )
	&& (GL_ADD_SIGNED != params[0] ) && (GL_DOT3_RGB != params[0] )
	&& (GL_DOT3_RGBA != params[0] )
	))

	if ( target == GL_TEXTURE_ENV)
	{
		switch (pname)
		{
		case GL_TEXTURE_ENV_MODE:
			ctx->texEnvMode[ctx->activeTexture] = params[0];
			break ;
		case GL_COMBINE_RGB:
			ctx->texEnvCombineRGB[ctx->activeTexture]  =FIXED_TO_FLOAT( params[0]);
			break ;
		case GL_COMBINE_ALPHA:
			ctx->texEnvCombineAlpha[ctx->activeTexture]  = FIXED_TO_FLOAT( params[0]);
			break ;
		case GL_TEXTURE_ENV_COLOR:
			ctx->texEnvColor[ctx->activeTexture] [0] = FIXED_TO_FLOAT(params[0]);
			ctx->texEnvColor [ctx->activeTexture][1] = FIXED_TO_FLOAT(params[1]);
			ctx->texEnvColor [ctx->activeTexture][2] = FIXED_TO_FLOAT(params[2]);
			ctx->texEnvColor [ctx->activeTexture][3] = FIXED_TO_FLOAT(params[3]);
			break ;
		case GL_SRC0_RGB:ctx->mSrcRGB[ctx->activeTexture][0]  = (GLenum ) params[0] ; break ;
		case GL_SRC1_RGB:ctx->mSrcRGB[ctx->activeTexture][1]= (GLenum ) params[0] ; break ;
		case GL_SRC2_RGB:ctx->mSrcRGB[ctx->activeTexture][2]= (GLenum ) params[0] ; break ;
		case GL_SRC0_ALPHA:ctx->mSrcAlpha[ctx->activeTexture][0]= (GLenum ) params[0] ; break ;
		case GL_SRC1_ALPHA:ctx->mSrcAlpha[ctx->activeTexture][1]= (GLenum ) params[0] ; break ;
		case GL_SRC2_ALPHA:ctx->mSrcAlpha[ctx->activeTexture][2]= (GLenum ) params[0] ; break ;
		case GL_OPERAND0_RGB:ctx->mOperandRGB[ctx->activeTexture][0] = (GLenum ) params[0] ; break ;
		case GL_OPERAND1_RGB:ctx->mOperandRGB[ctx->activeTexture][1] = (GLenum ) params[0] ; break ;
		case GL_OPERAND2_RGB:ctx->mOperandRGB[ctx->activeTexture][2] = (GLenum ) params[0] ; break ;
		case GL_OPERAND0_ALPHA:ctx->mOperandAlpha[ctx->activeTexture][0] = (GLenum ) params[0] ; break ;
		case GL_OPERAND1_ALPHA:ctx->mOperandAlpha[ctx->activeTexture][1] = (GLenum ) params[0] ; break ;
		case GL_OPERAND2_ALPHA:ctx->mOperandAlpha[ctx->activeTexture][2] = (GLenum ) params[0] ; break ;
		case GL_RGB_SCALE:ctx->mRGBScale[ctx->activeTexture]  = FIXED_TO_FLOAT(params[0]) ; break ;
		case GL_ALPHA_SCALE:ctx->mAlphaScale[ctx->activeTexture] = FIXED_TO_FLOAT( params[0] ); break ;
		}
	}
	else
	{
		switch(pname)
		{
		case GL_COORD_REPLACE_OES:
			ctx->texReplaceCoord[ctx->activeTexture] = *params;
			break;

		default:
			break;
		}
	}
	//DBGFUNCTION_EXIT("glTexEnvxv ")
}

GL_API void GL_APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
	GET_GLES_CONTEXT
	// As there is no corresponding parameter for GL_GENERATE_MIPMAP in GLES 2.0,
	// it is store in 1.1 context
	//GL11LOGMSG(LOG_GLES11_API,("glTexParameterf(0x%x\t0x%x\t%ff)\n",target,pname,param));

	if ((GL_TEXTURE_2D == target) && (GL_GENERATE_MIPMAP == pname))
	{
		CHECK_GL_VALUE ((GL_TRUE != (GLuint)param) && (GL_FALSE != (GLuint)param))
		ctx->genMipmap = (GLboolean)param;
		//printf ("\n ctx->genMipmap =  %d\n", ctx->genMipmap);
		return;
	}
	OGL2_glTexParameterf ( target,  pname,  param);

	//DBGFUNCTION_EXIT("glTexParameterf ")
}

GL_API void GL_APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param)
{
	GET_GLES_CONTEXT
	// As there is no corresponding parameter for GL_GENERATE_MIPMAP in GLES 2.0,
	// it is store in 1.1 context
	//GL11LOGMSG(LOG_GLES11_API,("glTexParameteri(0x%x\t0x%x\t0x%x)\n",target,pname,param));

	if ((GL_TEXTURE_2D == target) && (GL_GENERATE_MIPMAP == pname))
	{
		CHECK_GL_VALUE ((GL_TRUE != param) && (GL_FALSE != param))
		ctx->genMipmap = (GLboolean)param;
		//printf ("\n ctx->genMipmap =  %d\n", ctx->genMipmap);
		return;
	}
	OGL2_glTexParameteri ( target,  pname,  param);

	//DBGFUNCTION_EXIT("glTexParameteri ")
}

GL_API void GL_APIENTRY glTexParameterx (GLenum target, GLenum pname, GLfixed param)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glTexParameterx(0x%x\t0x%x\t0x%x)\n",target,pname,param));

	//DBGFUNCTION_INIT("glTexParameterx ")
	//RETAILMSG(1,(TEXT("glTexParameterx  \r\n")));

	glTexParameterf(target , pname, (GLfloat ) param);
	//DBGFUNCTION_EXIT("glTexParameterx ")
}

GL_API void GL_APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{
	GET_GLES_CONTEXT

	// As there is no corresponding parameter for GL_GENERATE_MIPMAP in GLES 2.0,
	// it is store in 1.1 context
	//GL11LOGMSG(LOG_GLES11_API,("glTexParameterfv(0x%x\t0x%x\t0x%x)\n",target,pname,*params));

	if ((GL_TEXTURE_2D == target) && (GL_GENERATE_MIPMAP == pname))
	{
		CHECK_GL_VALUE ((GL_TRUE != (GLuint)params[0]) && (GL_FALSE != (GLuint)params[0]))
		ctx->genMipmap = (GLboolean)params[0];
		//printf ("\n ctx->genMipmap =  %d\n", ctx->genMipmap);
		return;
	}
	OGL2_glTexParameterfv(target,pname,params);

	//DBGFUNCTION_EXIT("glTexParameterfv ")
}

GL_API void GL_APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params)
{
	GET_GLES_CONTEXT
	// As there is no corresponding parameter for GL_GENERATE_MIPMAP in GLES 2.0,
	// it is store in 1.1 context
	//GL11LOGMSG(LOG_GLES11_API,("glTexParameteriv(0x%x\t0x%x\t0x%x)\n",target,pname,*params));

	if ((GL_TEXTURE_2D == target) && (GL_GENERATE_MIPMAP == pname))
	{
		CHECK_GL_VALUE ((GL_TRUE != params[0]) && (GL_FALSE != params[0]))
		ctx->genMipmap = (GLboolean)params[0];
		//printf ("\n ctx->genMipmap =  %d\n", ctx->genMipmap);
		return;
	}
	OGL2_glTexParameteriv(target,pname,params);

	//DBGFUNCTION_EXIT("glTexParameteriv ")
}

GL_API void GL_APIENTRY glTexParameterxv (GLenum target, GLenum pname, const GLfixed *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glTexParameterxv(0x%x\t0x%x\t0x%x)\n",target,pname,*params));

	//DBGFUNCTION_INIT("glTexParameterxv ")
	//RETAILMSG(1,(TEXT("glTexParameterxv  \r\n")));

	glTexParameterf(target , pname, (GLfloat ) params[0]);
	//DBGFUNCTION_EXIT("glTexParameterxv ")
}

GL_API void GL_APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glCopyTexImage2D(0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",target,level,internalformat,x,y,width,height,border));

	OGL2_glCopyTexImage2D (target, level, internalformat, x, y, width, height, border);

	if ((level == 0) && (ctx->genMipmap == GL_TRUE))
	OGL2_glGenerateMipmap (target);

	//DBGFUNCTION_EXIT("glCopyTexImage2D ")
}

GL_API void GL_APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glCopyTexSubImage2D(0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",target,level,xoffset,yoffset,x,y,width,height));

	OGL2_glCopyTexSubImage2D (target, level, xoffset, yoffset, x, y, width, height);

	if ((level == 0) && (ctx->genMipmap == GL_TRUE))
	OGL2_glGenerateMipmap (target);

	//DBGFUNCTION_EXIT("glCopyTexSubImage2D ")
}

GL_API void GL_APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glTexImage2D(0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",target,level,internalformat,width,height,border,format,type,pixels));

	OGL2_glTexImage2D ( target,  level,  internalformat,  width,  height,  border,  format,  type, pixels);

	if ((level == 0) && (ctx->genMipmap == GL_TRUE))
	OGL2_glGenerateMipmap (target);

	//DBGFUNCTION_EXIT("glTexImage2D ")
}

GL_API void GL_APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glTexSubImage2D(0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",target,  level,  xoffset,  yoffset,  width,  height,  format,  type, pixels));

	OGL2_glTexSubImage2D ( target,  level,  xoffset,  yoffset,  width,  height,  format,  type, pixels);
	if ((level == 0) && (ctx->genMipmap == GL_TRUE))
	OGL2_glGenerateMipmap (target);

	//DBGFUNCTION_EXIT("glTexSubImage2D ")
}

GL_API void GL_APIENTRY glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{

	//GL11LOGMSG(LOG_GLES11_API,("glCompressedTexImage2D(0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",target, level, internalformat, width, height, border, imageSize, data));

	OGL2_glCompressedTexImage2D (target, level, internalformat, width, height, border, imageSize, data);

	//DBGFUNCTION_EXIT("glCompressedTexImage2D ")
}

GL_API void GL_APIENTRY glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
{

	//GL11LOGMSG(LOG_GLES11_API,("glCompressedTexSubImage2D(0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",target, level, xoffset, yoffset, width, height, format, imageSize, data));

	OGL2_glCompressedTexSubImage2D (target, level, xoffset, yoffset, width, height, format, imageSize, data);

	//DBGFUNCTION_EXIT("glCompressedTexSubImage2D ")
}

//used for defining  externally managed texture. right now only level 0 is supported.
void GLES11BindTexImage( const GLES11SurfaceData* pSurfData , GLenum target, GLint level , GLuint isMipmapped)
{

	GLES2BindTexImage( (GLES2SurfaceData *)pSurfData , target,  level , isMipmapped);
	//DBGFUNCTION_EXIT("GLES11BindTexImage ")
}


//used to release the externally managed texture.
void GLES11ReleaseTexImage(const GLES11SurfaceData* pSurfData )
{

	GLES2ReleaseTexImage((GLES2SurfaceData *)pSurfData );
	//DBGFUNCTION_EXIT("GLES11ReleaseTexImage ")

}

void* glMapTexture(GLenum target, GLint level, GLenum access)
{
	return OGL2_glMapTexture(target, level, access);

	//DBGFUNCTION_EXIT("glMapTexture")
}

GLboolean glUnmapTexture(GLenum target)
{
	return OGL2_glUnmapTexture(target);

	//DBGFUNCTION_EXIT("glUnmapTexture")
}

//Texture OES API - 2009.05.20
GL_API  void GL_APIENTRY glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glGetTexLevelParameteriv(0x%x\t0x%x\t0x%x\t0x%x)\n",target,level,pname,params));

	OGL2_glGetTexLevelParameteriv( target, level, pname, params);
}

void StateSaveForDrawTexture (GlesStateDrawTex *glSaveState)
{
	GET_GLES_CONTEXT
	OGL2_glGetIntegerv(GL_VIEWPORT, glSaveState->viewport);
	OGL2_glGetIntegerv(GL_ACTIVE_TEXTURE, &(glSaveState->activeTexture));

	glSaveState->clipplane[0] = ctx->cflags[0];
	glSaveState->clipplane[1] = ctx->cflags[1];
	glSaveState->clipplane[2] = ctx->cflags[2];
	glSaveState->clipplane[3] = ctx->cflags[3];
	glSaveState->clipplane[4] = ctx->cflags[4];
	glSaveState->clipplane[5] = ctx->cflags[5];

	glSaveState->Lighting = (ctx->flags & F_LIGHTING);
	OGL2_glGetIntegerv(GL_ARRAY_BUFFER_BINDING,&(glSaveState->arrayBind));
	OGL2_glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,&(glSaveState->elementBind));

	OGL2_glGetVertexAttribiv(ATTRIB_LOC_Vertex, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &(glSaveState->ClientState[0]));
	OGL2_glGetVertexAttribiv(ATTRIB_LOC_MultiTexCoord[0], GL_VERTEX_ATTRIB_ARRAY_ENABLED, &(glSaveState->ClientState[1]));
	OGL2_glGetVertexAttribiv(ATTRIB_LOC_MultiTexCoord[1], GL_VERTEX_ATTRIB_ARRAY_ENABLED, &(glSaveState->ClientState[2]));

	OGL2_glGetVertexAttribiv(ATTRIB_LOC_Vertex, GL_VERTEX_ATTRIB_ARRAY_SIZE, &(glSaveState->AttributeArray[0].size));
	OGL2_glGetVertexAttribiv(ATTRIB_LOC_Vertex, GL_VERTEX_ATTRIB_ARRAY_TYPE, (GLint *)&(glSaveState->AttributeArray[0].type));
	OGL2_glGetVertexAttribiv(ATTRIB_LOC_Vertex, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &(glSaveState->AttributeArray[0].stride));
	OGL2_glGetVertexAttribPointerv(ATTRIB_LOC_Vertex, GL_VERTEX_ATTRIB_ARRAY_POINTER, &(glSaveState->AttributeArray[0].pointer));

	OGL2_glGetVertexAttribiv(ATTRIB_LOC_MultiTexCoord[0], GL_VERTEX_ATTRIB_ARRAY_SIZE, &(glSaveState->AttributeArray[1].size));
	OGL2_glGetVertexAttribiv(ATTRIB_LOC_MultiTexCoord[0], GL_VERTEX_ATTRIB_ARRAY_TYPE, (GLint *)&(glSaveState->AttributeArray[1].type));
	OGL2_glGetVertexAttribiv(ATTRIB_LOC_MultiTexCoord[0], GL_VERTEX_ATTRIB_ARRAY_STRIDE, &(glSaveState->AttributeArray[1].stride));
	OGL2_glGetVertexAttribPointerv(ATTRIB_LOC_MultiTexCoord[0], GL_VERTEX_ATTRIB_ARRAY_POINTER, &(glSaveState->AttributeArray[1].pointer));

	OGL2_glGetVertexAttribiv(ATTRIB_LOC_MultiTexCoord[1], GL_VERTEX_ATTRIB_ARRAY_SIZE, &(glSaveState->AttributeArray[2].size));
	OGL2_glGetVertexAttribiv(ATTRIB_LOC_MultiTexCoord[1], GL_VERTEX_ATTRIB_ARRAY_TYPE, (GLint *)&(glSaveState->AttributeArray[2].type));
	OGL2_glGetVertexAttribiv(ATTRIB_LOC_MultiTexCoord[1], GL_VERTEX_ATTRIB_ARRAY_STRIDE, &(glSaveState->AttributeArray[2].stride));
	OGL2_glGetVertexAttribPointerv(ATTRIB_LOC_MultiTexCoord[1], GL_VERTEX_ATTRIB_ARRAY_POINTER, &(glSaveState->AttributeArray[2].pointer));

}

void StateRestoreAfterDrawTexture (GlesStateDrawTex *glRestoreState)
{
	GET_GLES_CONTEXT
	ctx->enableDrawTexture = 0;
	OGL2_glViewport(glRestoreState->viewport[0], glRestoreState->viewport[1], glRestoreState->viewport[2], glRestoreState->viewport[3]);
	OGL2_glActiveTexture(glRestoreState->activeTexture + GL_TEXTURE0);
	OGL2_glBindBuffer(GL_ARRAY_BUFFER,glRestoreState->arrayBind);
	OGL2_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glRestoreState->elementBind);

	// user defined clip plane
	ctx->cflags[0] = glRestoreState->clipplane[0];
	ctx->cflags[1] = glRestoreState->clipplane[1];
	ctx->cflags[2] = glRestoreState->clipplane[2];
	ctx->cflags[3] = glRestoreState->clipplane[3];
	ctx->cflags[4] = glRestoreState->clipplane[4];
	ctx->cflags[5] = glRestoreState->clipplane[5];

	//Lighting
	ctx->flags |= glRestoreState->Lighting;

	//CLient Stete
	if (glRestoreState->ClientState[0] == GL_TRUE)
	OGL2_glEnableVertexAttribArray( ATTRIB_LOC_Vertex );
	else
	OGL2_glDisableVertexAttribArray( ATTRIB_LOC_Vertex );

	if (glRestoreState->ClientState[1] == GL_TRUE)
	OGL2_glEnableVertexAttribArray( ATTRIB_LOC_MultiTexCoord[0]);
	else
	OGL2_glDisableVertexAttribArray( ATTRIB_LOC_MultiTexCoord[0]);

	if (glRestoreState->ClientState[3] == GL_TRUE)
	OGL2_glEnableVertexAttribArray( ATTRIB_LOC_MultiTexCoord[1]);
	else
	OGL2_glDisableVertexAttribArray( ATTRIB_LOC_MultiTexCoord[1]);

	OGL2_glVertexAttribPointer(ATTRIB_LOC_Vertex,
	glRestoreState->AttributeArray[0].size,
	glRestoreState->AttributeArray[0].type,0,
	glRestoreState->AttributeArray[0].stride,
	glRestoreState->AttributeArray[0].pointer);

	OGL2_glVertexAttribPointer(ATTRIB_LOC_MultiTexCoord[0],
	glRestoreState->AttributeArray[1].size,
	glRestoreState->AttributeArray[1].type,0,
	glRestoreState->AttributeArray[1].stride,
	glRestoreState->AttributeArray[1].pointer);

	OGL2_glVertexAttribPointer(ATTRIB_LOC_MultiTexCoord[1],
	glRestoreState->AttributeArray[2].size,
	glRestoreState->AttributeArray[2].type,0,
	glRestoreState->AttributeArray[2].stride,
	glRestoreState->AttributeArray[2].pointer);


}

void StatePrepareForDrawTexture (void)
{
	GET_GLES_CONTEXT
	ctx->enableDrawTexture = 1;

	ctx->cflags[0] = 0;
	ctx->cflags[1] = 0;
	ctx->cflags[2] = 0;
	ctx->cflags[3] = 0;
	ctx->cflags[4] = 0;
	ctx->cflags[5] = 0;

	ctx->flags &= ~ F_LIGHTING;

	OGL2_glDisableVertexAttribArray( ATTRIB_LOC_Vertex );
	OGL2_glDisableVertexAttribArray( ATTRIB_LOC_MultiTexCoord[0]);
	OGL2_glDisableVertexAttribArray( ATTRIB_LOC_MultiTexCoord[1]);
}

void DrawTexfOES (GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height)
{

	GLfloat rect_texcoord[8] = {0.0f};

	GLfloat Vertex[] = {0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f};
	GlesStateDrawTex currentGLState;
	GLint crop_rect[4] = {0};
	GLint image_width = 0;
	GLint image_height = 0;
	GLint index;
	GLfloat viewportWidth, viewportHeight;
	GLfloat viewportCenterX, viewportCenterY;
	GLfloat depth_range[2] = {0.0f};
	GLfloat Zd = 0.0f, Zw = 0.0f;                     // Zd in Normalized device coordinate
	//int i = 0;
	int viewport[4];

	GET_GLES_CONTEXT

	OGL2_glGetIntegerv(GL_VIEWPORT, viewport);
	viewportWidth = (GLfloat) viewport[2];
	viewportHeight = (GLfloat) viewport[3];

	//  CHECK_GL_VALUE((width <= 0) || (height <= 0) || (width > (GLint)C_MAX_SCREEN_WIDTH) || (height > (GLint)C_MAX_SCREEN_HEIGHT))
	CHECK_GL_VALUE((width <= 0) || (height <= 0))

	//Save current openGL state
	StateSaveForDrawTexture (&currentGLState);
	//Prepare state for draw texture
	StatePrepareForDrawTexture();


	//Process Vertex attibute
	OGL2_glGetFloatv(GL_DEPTH_RANGE, depth_range);
	if (z <= 0.0f)
	Zw = depth_range[0];
	else if (z >= 1.0f)
	Zw = depth_range[1];
	else
	Zw = depth_range[0] +  z * (depth_range[1] - depth_range[0]);
	Zd = (Zw - ((depth_range[0] + depth_range[1])/2)) / ((depth_range[1] - depth_range[0])/2);

	// How normalised device coordinate is calculated from window (Refer 2.12.1 section of GL2.0 spec)?
	//    Xd = ((Xw - Ox) * 2) / Vw
	//    Yd = ((Yw - Oy) * 2) / Vh
	// where (Xd, Yd) = normalized device coordinate
	//     (Ox, Oy) = Viewport Center
	//     Vw, Vh =  viewport width and height

	viewportCenterX = (viewportWidth / 2.0f);
	viewportCenterY = (viewportHeight/ 2.0f);

	Vertex [0] = (GLfloat) (2*(x - viewportCenterX)) / viewportWidth ;
	Vertex [1] = (GLfloat) (2*(y - viewportCenterY))/ viewportHeight;
	Vertex [4] = (GLfloat) (2*(x + width - viewportCenterX)) / viewportWidth ;
	Vertex [5] = Vertex [1]; //(GLfloat) (2*(y- viewportCenterY)) / viewportHeight;
	Vertex [8] = Vertex [0];//(GLfloat) (2*(x - viewportCenterX)) / viewportWidth ;
	Vertex [9] = (GLfloat) (2*(y + height- viewportCenterY)) / viewportHeight;
	Vertex [12] = Vertex [4];//(GLfloat) (2*(x + width - viewportCenterX)) / viewportWidth;
	Vertex [13] = Vertex [9];//(GLfloat) (2*(y + height- viewportCenterY)) / viewportHeight;
	Vertex[2] = Vertex[6] = Vertex[10] = Vertex[14] = Zd;


	OGL2_glBindBuffer(GL_ARRAY_BUFFER , 0);
	OGL2_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , 0);

	OGL2_glEnableVertexAttribArray( ATTRIB_LOC_Vertex);
	OGL2_glVertexAttribPointer(ATTRIB_LOC_Vertex,4,GL_FLOAT,0,0,Vertex);

	if (ctx->flags & F_TEXTURE_2D_UNIT0)
	{
		glActiveTexture(GL_TEXTURE0);
		OGL2_glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &image_width);
		OGL2_glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &image_height);

		OGL2_glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop_rect);

		rect_texcoord [0] = (GLfloat) crop_rect[0] / (GLfloat)image_width; // Ucr/Wt
		rect_texcoord [1] = (GLfloat) crop_rect[1] / (GLfloat)image_height; // Vcr/Ht
		rect_texcoord [2] = (GLfloat) (crop_rect[0] + crop_rect[2]) / (GLfloat)image_width; //(Ucr+Wcr)/Wt
		rect_texcoord [3] = rect_texcoord [1]; // Vcr/Ht
		rect_texcoord [4] = rect_texcoord [0]; // Ucr/Wt;
		rect_texcoord [5] = (GLfloat) (crop_rect[1] + crop_rect[3]) / (GLfloat)image_height; // (Vcr+Hcr)/Ht;
		rect_texcoord [6] = rect_texcoord [2]; //(Ucr+Wcr)/Wt
		rect_texcoord [7] = rect_texcoord [5]; //(Vcr+Hcr)/Ht

		OGL2_glEnableVertexAttribArray( ATTRIB_LOC_MultiTexCoord[0]);
		OGL2_glVertexAttribPointer(ATTRIB_LOC_MultiTexCoord[0],2,GL_FLOAT,0,0,rect_texcoord);

	}

	if (ctx->flags & F_TEXTURE_2D_UNIT1)
	{
		glActiveTexture(GL_TEXTURE1);
		OGL2_glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &image_width);
		OGL2_glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &image_height);

		OGL2_glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop_rect);

		rect_texcoord [0] = (GLfloat) crop_rect[0] / (GLfloat)image_width; // Ucr/Wt
		rect_texcoord [1] = (GLfloat) crop_rect[1] / (GLfloat)image_height; // Vcr/Ht
		rect_texcoord [2] = (GLfloat) (crop_rect[0] + crop_rect[2]) / (GLfloat)image_width; //(Ucr+Wcr)/Wt
		rect_texcoord [3] = rect_texcoord [1]; // Vcr/Ht
		rect_texcoord [4] = rect_texcoord [0]; // Ucr/Wt;
		rect_texcoord [5] = (GLfloat) (crop_rect[1] + crop_rect[3]) / (GLfloat)image_height; // (Vcr+Hcr)/Ht;
		rect_texcoord [6] = rect_texcoord [2]; //(Ucr+Wcr)/Wt
		rect_texcoord [7] = rect_texcoord [5]; //(Vcr+Hcr)/Ht

		OGL2_glEnableVertexAttribArray( ATTRIB_LOC_MultiTexCoord[1]);
		OGL2_glVertexAttribPointer(ATTRIB_LOC_MultiTexCoord[1],2,GL_FLOAT,0,0,rect_texcoord);

	}

	if (ctx->flags & F_TEXTURE_2D_UNIT0){
		glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
		glPushMatrix();
		glLoadIdentity();
	}

	if (ctx->flags & F_TEXTURE_2D_UNIT1){
		glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE1);
		glPushMatrix();
		glLoadIdentity();
	}

	//glMatrixMode(GL_TEXTURE);
	// glPushMatrix();
	//glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//glOrthof(-viewportCenterX, viewportCenterX, -viewportCenterY, viewportCenterY, -viewportCenterY, viewportCenterY);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	index = selectShader();

	//loadShader2( index);

	InitMatrices(ctx);

	UniformsInit( index);
	OGL2_glDrawArrays(GL_TRIANGLE_STRIP, 0,  4);

	if (ctx->flags & F_TEXTURE_2D_UNIT0){
		glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
		glPopMatrix();
	}

	if (ctx->flags & F_TEXTURE_2D_UNIT1){
		glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE1);
		glPopMatrix();
	}

	//glMatrixMode(GL_TEXTURE);
	//glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


	//restore previous state settings
	StateRestoreAfterDrawTexture (&currentGLState);

}

GL_API void GL_APIENTRY glDrawTexiOES (GLint x, GLint y, GLint z, GLint width, GLint height)
{
	//GL11LOGMSG(LOG_GLES11_API,("glDrawTexiOES(0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",x,y,z,width,height));

	DrawTexfOES ((GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)width, (GLfloat)height);
}

GL_API void GL_APIENTRY glDrawTexfOES (GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height)
{
	//GL11LOGMSG(LOG_GLES11_API,("glDrawTexfOES(%ff\t%ff\t%ff\t%ff\t%ff)\n",x,y,z,width,height));
	DrawTexfOES (x, y, z, width, height);
}

GL_API void GL_APIENTRY glDrawTexsOES (GLshort x, GLshort y, GLshort z, GLshort width, GLshort height)
{
	//GL11LOGMSG(LOG_GLES11_API,("glDrawTexiOES(0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",x,y,z,width,height));
	DrawTexfOES ((GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)width, (GLfloat)height);
}

GL_API void GL_APIENTRY glDrawTexxOES(GLfixed x, GLfixed y, GLfixed z, GLfixed width, GLfixed height)
{
	//GL11LOGMSG(LOG_GLES11_API,("glDrawTexiOES(0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",x,y,z,width,height));
	DrawTexfOES (FIXED_TO_FLOAT(x), FIXED_TO_FLOAT(y), FIXED_TO_FLOAT(z), (GLfloat)width, (GLfloat)height);
}

GL_API void GL_APIENTRY glDrawTexivOES (const GLint *coords)
{
	GET_GLES_CONTEXT
	//GL11LOGMSG(LOG_GLES11_API,("glDrawTexivOES(0x%x)\n",*coords));
	CHECK_GL_VALUE ((coords == NULL))

	DrawTexfOES ((GLfloat) coords[0], (GLfloat) coords[1], (GLfloat) coords[2], (GLfloat) coords[3], (GLfloat) coords[4]);
}

GL_API void GL_APIENTRY glDrawTexfvOES (const GLfloat *coords)
{
	GET_GLES_CONTEXT
	//GL11LOGMSG(LOG_GLES11_API,("glDrawTexfvOES(%ff)\n",*coords));
	CHECK_GL_VALUE ((coords == NULL))

	DrawTexfOES (coords[0], coords[1], coords[2], coords[3], coords[4]);
}

GL_API void GL_APIENTRY glDrawTexsvOES (const GLshort *coords)
{
	GET_GLES_CONTEXT
	//GL11LOGMSG(LOG_GLES11_API,("glDrawTexsvOES(0x%x)\n",*coords));
	CHECK_GL_VALUE ((coords == NULL))

	DrawTexfOES ((GLfloat) coords[0], (GLfloat) coords[1], (GLfloat) coords[2], (GLfloat) coords[3], (GLfloat) coords[4]);
}

GL_API void GL_APIENTRY glDrawTexxvOES (const GLfixed *coords)
{
	GET_GLES_CONTEXT
	//GL11LOGMSG(LOG_GLES11_API,("glDrawTexxvOES(0x%x)\n",*coords));
	CHECK_GL_VALUE ((coords == NULL))

	DrawTexfOES (FIXED_TO_FLOAT( coords[0]), FIXED_TO_FLOAT(coords[1]), FIXED_TO_FLOAT(coords[2]),
	FIXED_TO_FLOAT(coords[3]), FIXED_TO_FLOAT(coords[4]));
}
