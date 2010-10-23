#include "includes.h"
#include "OGL2Func_for_gles11.h"
#include "attribs.h"

GL_API void GL_APIENTRY glGetFixedv (GLenum pname, GLfixed *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glGetFixedv(0x%x\t0x%x)\n",pname,*params));

	//DBGFUNCTION_INIT("glGetFixedv ")
	//RETAILMSG(1,(TEXT("glGetFixedv  \r\n")));

	GetStateAttrib(ctx, pname , 0 , CopyToFixed, CopyToFixedE, CopyToFixedS, params ) ;

	//DBGFUNCTION_EXIT("glGetFixedv ")
}

GLboolean ConvertToFixedWithMantissa(GLfloat fVal, GLfixed *pi32Mantissa, GLint *pi32Exponent)
{
	GLint i32Exp;
	GLfloat fMantissa;

	fMantissa = frexp(fVal, &i32Exp);
	fMantissa = fMantissa * 2147483648.0f; /* (ie shift by 31) */

	*pi32Mantissa = (GLint) fMantissa;
	*pi32Exponent = i32Exp - 15;

	if(fMantissa != fMantissa)
	{
		return GL_TRUE;   /* fail on NaN, also could use isNan(mantissa) */
	}             /* should also fail on inf or failure of frexp */

	return GL_FALSE;
}

GL_API GLbitfield GL_APIENTRY glQueryMatrixxOES( GLfixed mantissa[16],GLint exponent[16] )
{
	int i , j ;
	GLint matrix = 0 ;
	GLuint result =0 ;
	GLfloat *data = (GLfloat * ) 0 ;
	GET_GLES_CONTEXT_RETURN

	//GL11LOGMSG(LOG_GLES11_API,("glQueryMatrixxOES(0x%x\t0x%x)\n",mantissa,exponent));

	//DBGFUNCTION_INIT("glQueryMatrixxOES ")
	//RETAILMSG(1,(TEXT("glQueryMatrixxOES  \r\n")));

	matrix = ctx->matrixMode - GL_MODELVIEW + ctx->activeTexture;
	data = ctx->stacks[matrix ].matrices[ctx->stacks[matrix].pos ]->data;

	for (  i = 0 ; i < 4 ; i++)
	{
		for (  j = 0 ; j < 4 ; j++)
		{
			if ( ConvertToFixedWithMantissa(data[i *4 + j ], &mantissa[i + j * 4 ] , &exponent[i + j * 4 ]))
			{
				result |= ( 1<< ( i+j*4 ) ) ;
			}
		}
	}

	return result ;
}

GL_API void GL_APIENTRY glGetFloatv (GLenum pname, GLfloat *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glGetFloatv(0x%x\t0x%x)\n",pname,params));

	//DBGFUNCTION_INIT("glGetFloatv ")
	//RETAILMSG(1,(TEXT("glGetFloatv  \r\n")));
	GetStateAttrib(ctx, pname , 0 , CopyToFloat, CopyToFloatE , CopyToFloatS, params ) ;

	//DBGFUNCTION_EXIT("glGetFloatv ")
}

GL_API void GL_APIENTRY glGetLightfv (GLenum light, GLenum pname, GLfloat *params)
{
	Light * l = (Light * ) 0;
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glGetLightfv(0x%x\t0x%x\t0x%x)\n",light,pname,params));

	//DBGFUNCTION_INIT("glGetLightfv ")
	//RETAILMSG(1,(TEXT("glGetLightfv  \r\n")));

	l = &ctx->lights[light-GL_LIGHT0] ;

	switch (pname)
	{
	case GL_AMBIENT:
		memcpy(params,l->acli , sizeof(GLfloat)*4 );
		break;
	case GL_DIFFUSE:
		memcpy(params, l->dcli ,sizeof(GLfloat)*4 );
		break;
	case GL_SPECULAR:
		memcpy(params, l->scli ,sizeof(GLfloat)*4 );
		break;
	case GL_POSITION:
		{
			memcpy(params, l->oppli ,sizeof(GLfloat)*4 );
			break;
		}
	case GL_SPOT_DIRECTION:
		{
			memcpy(params, l->osdli ,sizeof(GLfloat)*3);
			break;
		}
	case GL_SPOT_EXPONENT:
		params[0]  = l->srli ;
		break;
	case GL_SPOT_CUTOFF:
		params[0] =l->crli ;
		break;
	case GL_CONSTANT_ATTENUATION:
		params[0] = l->k0i ;
		break;
	case GL_LINEAR_ATTENUATION:
		params[0] =l->k1i ;
		break;
	case GL_QUADRATIC_ATTENUATION:
		params[0] = l->k2i ;
		break;

	}

	//DBGFUNCTION_EXIT("glLightfv")
}

GL_API void GL_APIENTRY glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glGetMaterialfv(0x%x\t0x%x\t0x%x)\n",face,pname,params));

	//DBGFUNCTION_INIT("glGetMaterialfv ")
	//RETAILMSG(1,(TEXT("glGetMaterialfv  \r\n")));
	switch ( pname )
	{
	case GL_AMBIENT:
		VEC4CPY( params , ctx->frontNback.acm) ; break ;
	case GL_DIFFUSE:
		VEC4CPY( params , ctx->frontNback.dcm ) ; break ;
	case GL_AMBIENT_AND_DIFFUSE:
		VEC4CPY( params , ctx->frontNback.acm ) ;VEC4CPY( params , ctx->frontNback.dcm) ;  break ;
	case GL_SPECULAR:
		VEC4CPY( params , ctx->frontNback.scm ) ; break ;
	case GL_EMISSION:
		VEC4CPY( params , ctx->frontNback.ecm ) ; break ;
	case GL_SHININESS:
		params [0] = ctx->frontNback.srm; break ;
	}


	//DBGFUNCTION_EXIT("glGetMaterialfv ")
}

GL_API void GL_APIENTRY glGetTexEnvfv (GLenum env, GLenum pname, GLfloat *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glGetTexEnvfv(0x%x\t0x%x\t0x%x)\n",env,pname,params));

	//DBGFUNCTION_INIT("glGetTexEnvfv ")
	//RETAILMSG(1,(TEXT("glGetTexEnvfv  \r\n")));

	GetStateAttrib(ctx, pname , env , CopyToFloat, CopyToFloatE, CopyToFloatS, params ) ;


	//DBGFUNCTION_EXIT("glGetTexEnvfv ")
}

GL_API void GL_APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
{

	//GL11LOGMSG(LOG_GLES11_API,("glGetTexParameterfv(0x%x\t0x%x\t%ff)\n",target,pname,*params));

	OGL2_glGetTexParameterfv(target, pname, params);

	//DBGFUNCTION_EXIT("glGetTexParameterfv ")
}

GL_API void GL_APIENTRY glGetBooleanv (GLenum pname, GLboolean *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glGetBooleanv(0x%x\t0x%x)\n",pname,params));

	//DBGFUNCTION_INIT("glGetBooleanv ")
	//RETAILMSG(1,(TEXT("glGetBooleanv  \r\n")));
	GetStateAttrib(ctx, pname , 0 , CopyToBool, CopyToBoolE,CopyToBoolS,  params ) ;

	//DBGFUNCTION_EXIT("glGetBooleanv ")
}

GL_API GLenum GL_APIENTRY glGetError (void)
{
	GLenum error = GL_NO_ERROR;

	GET_GLES_CONTEXT_RETURN
	//GL11LOGMSG(LOG_GLES11_API,("glGetError\n"));
	//DBGFUNCTION_INIT("glGetError ")
	//RETAILMSG(1,(TEXT("glGetError  \r\n")));

	if ( ctx->error > 0 )
	{
		ctx->error-- ;
		error = ctx->errors[ctx->error];
		ctx->errors[ctx->error] = GL_NO_ERROR;
	}
	else
	error = OGL2_glGetError();

	return error ;

	//DBGFUNCTION_EXIT("glGetError ")
}

GL_API void GL_APIENTRY glGetLightxv (GLenum light, GLenum pname, GLfixed *params)
{
	Light * l = (Light * ) 0;
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glGetLightxv(0x%x\t0x%x\t0x%x)\n",light,pname,*params));

	//DBGFUNCTION_INIT("glGetLightxv ")
	//RETAILMSG(1,(TEXT("glGetLightxv  \r\n")));

	l = &ctx->lights[light-GL_LIGHT0] ;

	switch (pname)
	{
	case GL_AMBIENT:
		params[0] = FLOAT_TO_FIXED(l->acli [0] ) ;
		params[1] = FLOAT_TO_FIXED(l->acli [1] ) ;
		params[2] = FLOAT_TO_FIXED(l->acli [2] ) ;
		params[3] = FLOAT_TO_FIXED(l->acli [3] ) ;
		break;
	case GL_DIFFUSE:
		params[0] = FLOAT_TO_FIXED(l->dcli [0] ) ;
		params[1] = FLOAT_TO_FIXED(l->dcli [1] ) ;
		params[2] = FLOAT_TO_FIXED(l->dcli [2] ) ;
		params[3] = FLOAT_TO_FIXED(l->dcli [3] ) ;
		break;
	case GL_SPECULAR:
		params[0] = FLOAT_TO_FIXED(l->scli [0] ) ;
		params[1] = FLOAT_TO_FIXED(l->scli [1] ) ;
		params[2] = FLOAT_TO_FIXED(l->scli [2] ) ;
		params[3] = FLOAT_TO_FIXED(l->scli [3] ) ;
		break;
	case GL_POSITION:
		{
			params[0] = FLOAT_TO_FIXED(l->oppli [0] ) ;
			params[1] = FLOAT_TO_FIXED(l->oppli [1] ) ;
			params[2] = FLOAT_TO_FIXED(l->oppli [2] ) ;
			params[3] = FLOAT_TO_FIXED(l->oppli [3] ) ;
			break;
		}
	case GL_SPOT_DIRECTION:
		{
			params[0] = FLOAT_TO_FIXED(l->osdli [0] ) ;
			params[1] = FLOAT_TO_FIXED(l->osdli [1] ) ;
			params[2] = FLOAT_TO_FIXED(l->osdli [2] ) ;
			break;
		}
	case GL_SPOT_EXPONENT:
		params[0]  =FLOAT_TO_FIXED(l->srli );
		break;
	case GL_SPOT_CUTOFF:
		params[0] =FLOAT_TO_FIXED(l->crli );
		break;
	case GL_CONSTANT_ATTENUATION:
		params[0] = FLOAT_TO_FIXED(l->k0i );
		break;
	case GL_LINEAR_ATTENUATION:
		params[0] =FLOAT_TO_FIXED(l->k1i );
		break;
	case GL_QUADRATIC_ATTENUATION:
		params[0] = FLOAT_TO_FIXED(l->k2i) ;
		break;

	}

	//DBGFUNCTION_EXIT("glGetLightxv ")
}

GL_API void GL_APIENTRY glGetMaterialxv (GLenum face, GLenum pname, GLfixed *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glGetMaterialxv(0x%x\t0x%x\t0x%x)\n",face,pname,*params));

	//DBGFUNCTION_INIT("glGetMaterialxv ")
	//RETAILMSG(1,(TEXT("glGetMaterialxv  \r\n")));


	//DBGFUNCTION_EXIT("glGetMaterialxv ")
}

GL_API void GL_APIENTRY glGetPointerv (GLenum pname, void **params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glGetPointerv(0x%x\t0x%x)\n",pname,params));

	//DBGFUNCTION_INIT("glGetPointerv ")
	//RETAILMSG(1,(TEXT("glGetPointerv  \r\n")));
	GetStateAttrib(ctx, pname , 0 , CopyToInt, CopyToIntE, CopyToIntS, params ) ;

	//DBGFUNCTION_EXIT("glGetPointerv ")
}

const unsigned long maxInt = 0xFFFFFFFF;

int INTVAL(float x )
{
	double tmp = maxInt;
	tmp *= x;
	tmp --;
	tmp /= 2.0 ;
	return tmp ;

}

GLenum texPalFormats[]= {GL_PALETTE4_RGB8_OES        ,GL_PALETTE4_RGBA8_OES      ,
	GL_PALETTE8_RGB8_OES  ,GL_PALETTE8_RGBA8_OES      ,
	GL_PALETTE4_RGBA4_OES      ,GL_PALETTE8_RGBA4_OES      ,
	GL_PALETTE4_R5_G6_B5_OES  ,GL_PALETTE8_R5_G6_B5_OES  ,
	GL_PALETTE4_RGB5_A1_OES   ,GL_PALETTE8_RGB5_A1_OES   };

GL_API void GL_APIENTRY glGetIntegerv(GLenum pname, GLint* params)
{
	float tmp[4];
	GET_GLES_CONTEXT
	//ShaderInit();

	//GL11LOGMSG(LOG_GLES11_API,("glGetIntegerv(0x%x\t0x%x)\n",pname,params));

	//DBGFUNCTION_INIT("glGetIntegerv ")

	switch (pname)
	{
	case GL_CURRENT_COLOR                  :
		OGL2_glGetVertexAttribfv(ATTRIB_LOC_Color, GL_CURRENT_VERTEX_ATTRIB, tmp);
		params[0] = (int)((((float)pow(2.0f,32.0f) - 1.0f) * tmp[0] - 1.0f) / 2.0f);
		params[1] = (int)((((float)pow(2.0f,32.0f) - 1.0f) * tmp[1] - 1.0f) / 2.0f);
		params[2] = (int)((((float)pow(2.0f,32.0f) - 1.0f) * tmp[2] - 1.0f) / 2.0f);
		params[3] = (int)((((float)pow(2.0f,32.0f) - 1.0f) * tmp[3] - 1.0f) / 2.0f);
		break ;
	case GL_CURRENT_NORMAL                 :
		OGL2_glGetVertexAttribiv(ATTRIB_LOC_Normal, GL_CURRENT_VERTEX_ATTRIB, params);
		break ;
	case GL_DEPTH_CLEAR_VALUE              :
		OGL2_glGetFloatv(GL_DEPTH_CLEAR_VALUE, tmp);
		params[0] =  (int)((((float)pow(2.0f,32.0f) - 1.0f) * tmp[0] - 1.0f) / 2.0f);
		break ;
	case GL_DEPTH_RANGE                    :
		OGL2_glGetFloatv(GL_DEPTH_RANGE,tmp);
		{
			params[0] =  INTVAL(tmp[0]) ;
			params[1] =  INTVAL(tmp[1]) ;
		}
		break ;
	case GL_COLOR_CLEAR_VALUE              :
		OGL2_glGetFloatv(GL_COLOR_CLEAR_VALUE,tmp);
		{
			params[0] =  INTVAL(tmp[0] ) ;
			params[1] =  INTVAL(tmp[1] ) ;
			params[2] =  INTVAL(tmp[2] ) ;
			params[3] =  INTVAL(tmp[3] ) ;
		}
		break ;
	case GL_ALPHA_TEST_REF                 :
		OGL2_glGetFloatv(GL_ALPHA_TEST_REF, tmp);
		params[0] = (int)((((float)pow(2.0f,32.0f) - 1.0f) * tmp[0] - 1.0f) / 2.0f);
		break ;
	case GL_COMPRESSED_TEXTURE_FORMATS :
		OGL2_glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS,params);
		break ;
	case GL_STENCIL_BITS:
		OGL2_glGetIntegerv(GL_STENCIL_BITS, params);
		break;

	default:
		GetStateAttrib(ctx, pname , 0 , CopyToInt, CopyToIntE, CopyToIntS, params ) ;
		break ;
	}
	//DBGFUNCTION_EXIT("glGetIntegerv ")

}

const GLubyte* glGetString(GLenum name)
{
	GET_GLES_CONTEXT_RETURN

	//GL11LOGMSG(LOG_GLES11_API,("glGetString(0x%x)\n",name));

	switch(name)
	{
	case GL_VERSION:
		return (const GLubyte*)GLES_VERSION_STRING;
		break;

	case GL_VENDOR:
		return OGL2_glGetString(GL_VENDOR);
		break;

	case GL_EXTENSIONS:
		return (const GLubyte*)ctx->extension;
		break;

	case GL_RENDERER:
		return OGL2_glGetString(GL_RENDERER);
		break;

	default:
		GL_ERROR0(GL_INVALID_ENUM)
		break;
	}

	//DBGFUNCTION_EXIT("glGetString ")
	return NULL;
}

GL_API void GL_APIENTRY glGetTexEnviv (GLenum env, GLenum pname, GLint *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glGetTexEnviv(0x%x\t0x%x\t0x%x)\n",env,pname,*params));

	//DBGFUNCTION_INIT("glGetTexEnviv ")
	//RETAILMSG(1,(TEXT("glGetTexEnviv  \r\n")));

	GetStateAttrib(ctx, pname , env , CopyToInt, CopyToIntE, CopyToIntS, params ) ;

	//DBGFUNCTION_EXIT("glGetTexEnviv ")
}

GL_API void GL_APIENTRY glGetTexEnvxv (GLenum env, GLenum pname, GLfixed *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glGetTexEnvxv(0x%x\t0x%x\t0x%x)\n",env,pname,params));


	//DBGFUNCTION_INIT("glGetTexEnvxv ")
	//RETAILMSG(1,(TEXT("glGetTexEnvxv  \r\n")));

	GetStateAttrib(ctx, pname , env , CopyToFixed, CopyToFixedE, CopyToFixedS, params ) ;

	//DBGFUNCTION_EXIT("glGetTexEnvxv ")
}

GL_API void GL_APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params)
{
	GET_GLES_CONTEXT
	// As there is no corresponding parameter for GL_GENERATE_MIPMAP in GLES 2.0,
	//GL11LOGMSG(LOG_GLES11_API,("glGetTexParameteriv(0x%x\t0x%x\t0x%x)\n",target,pname,params));

	// it is store in 1.1 context
	if ((GL_TEXTURE_2D == target) && (GL_GENERATE_MIPMAP == pname))
	{
		params[0] = (GLint)ctx->genMipmap;
		//printf ("\n ctx->genMipmap =  %d\n", params[0]);
		return;
	}
	OGL2_glGetTexParameteriv (target, pname, params);

	//DBGFUNCTION_EXIT("glGetTexParameteriv ")
}

GL_API void GL_APIENTRY glGetTexParameterxv (GLenum target, GLenum pname, GLfixed *params)
{

	//GL11LOGMSG(LOG_GLES11_API,("glGetTexParameterxv(0x%x\t0x%x\t0x%x)\n",target,pname,params));

	glGetTexParameteriv (target, pname, (GLint *)params);

	//DBGFUNCTION_EXIT("glGetTexParameterxv ")
}
