#include "includes.h"
#include "OGL2Func_for_gles11.h"

void InitLight(int i, Light *l)
{
	l->acli[0] = 0.0f; l->acli[1] = 0.0f; l->acli[2] = 0.0f; l->acli[3] = 1.0f;
	if ( i == 0 )
	{
		l->dcli[0] = 1.0f; l->dcli[1] = 1.0f; l->dcli[2] = 1.0f; l->dcli[3] = 1.0f;
		l->scli[0] = 1.0f; l->scli[1] = 1.0f; l->scli[2] = 1.0f; l->scli[3] = 1.0f;
	}
	else
	{
		l->dcli[0] = 0.0f; l->dcli[1] = 0.0f; l->dcli[2] = 0.0f; l->dcli[3] = 1.0f;
		l->scli[0] = 0.0f; l->scli[1] = 0.0f; l->scli[2] = 0.0f; l->scli[3] = 1.0f;
	}
	l->ppli[0] = 0.0f; l->ppli[1] = 0.0f; l->ppli[2] = 1.0f; l->ppli[3] = 0.0f;
	l->sdli[0] = 0.0f; l->sdli[1] = 0.0f; l->sdli[2] = -1.0f;

	l->srli = 0.0f;
	l->crli = 180.0f ;
	l->k0i = 1.0f;
	l->k1i = 0.0f;
	l->k2i = 0.0f;
}

void InitMaterial(Material *mat )
{
	mat->acm[0] = 0.2f; mat->acm[1] = 0.2f; mat->acm[2] = 0.2f; mat->acm[3] = 1.0f;
	mat->dcm[0] = 0.8f; mat->dcm[1] = 0.8f; mat->dcm[2] = 0.8f; mat->dcm[3] = 1.0f;
	mat->scm[0] = 0.0f; mat->scm[1] = 0.0f; mat->scm[2] = 0.0f; mat->scm[3] = 1.0f;
	mat->ecm[0] = 0.0f; mat->ecm[1] = 0.0f; mat->ecm[2] = 0.0f; mat->ecm[3] = 1.0f;
	mat->srm=0.0f;
}

GL_API void GL_APIENTRY glLightModelf (GLenum pname, GLfloat param)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glLightModelf(0x%x\t0x%x)\n",pname,param));

	//DBGFUNCTION_INIT("glLightModelf ")
	////RETAILMSG(1,(TEXT("glLightModelf  \r\n")));

	CHECK_GL_ENUM((pname!=GL_LIGHT_MODEL_TWO_SIDE) )

	ctx->tbs = (GLboolean) param ;

	//DBGFUNCTION_EXIT("glLightModelf ")
}

GL_API void GL_APIENTRY glLightModelfv (GLenum pname, const GLfloat *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glLightModelfv(0x%x\t0x%x)\n",pname,params));

	//DBGFUNCTION_INIT("glLightModelfv ")
	////RETAILMSG(1,(TEXT("glLightModelfv  \r\n")));

	CHECK_GL_ENUM(((pname!=GL_LIGHT_MODEL_AMBIENT) && (pname!=GL_LIGHT_MODEL_TWO_SIDE) ))

	if ( pname == GL_LIGHT_MODEL_TWO_SIDE )
	{
		ctx->tbs = (GLboolean) params[0]  ;
	}
	if ( pname == GL_LIGHT_MODEL_AMBIENT )
	{
		memcpy(ctx->acs ,params ,sizeof(GLfloat) * 4 );
	}

	//DBGFUNCTION_EXIT("glLightModelfv ")
}

GL_API void GL_APIENTRY glLightf (GLenum light, GLenum pname, GLfloat param)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glLightf(0x%x\t0x%x\t0x%x)\n",light,pname,param));

	//DBGFUNCTION_INIT("glLightf ")
	////RETAILMSG(1,(TEXT("glLightf  \r\n")));

	CHECK_GL_ENUM(((light <GL_LIGHT0) || (light > GL_LIGHT7)))
	CHECK_GL_ENUM(((pname!=GL_SPOT_EXPONENT) && (pname!=GL_SPOT_CUTOFF) &&
	(pname!=GL_CONSTANT_ATTENUATION) && (pname!=GL_LINEAR_ATTENUATION) &&
	(pname!=GL_QUADRATIC_ATTENUATION)))

	glLightfv(light, pname, &param ) ;

	//DBGFUNCTION_EXIT("glLightf ")
}

GL_API void GL_APIENTRY glLightfv (GLenum light, GLenum pname, const GLfloat *params)
{
	Light * l = (Light * ) 0;
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glLightfv(0x%x\t0x%x\t0x%x)\n",light,pname,params));

	//DBGFUNCTION_INIT("glLightfv ")
	////RETAILMSG(1,(TEXT("glLightfv  \r\n")));

	CHECK_GL_ENUM(((light <GL_LIGHT0) || (light > GL_LIGHT7)))
	CHECK_GL_ENUM(((pname!=GL_AMBIENT) && (pname!=GL_SPOT_CUTOFF) &&
	(pname!=GL_DIFFUSE) && (pname!=GL_SPOT_DIRECTION) &&
	(pname!=GL_SPECULAR) && (pname!=GL_SPOT_EXPONENT) &&
	(pname!=GL_CONSTANT_ATTENUATION) &&(pname!=GL_POSITION) &&
	(pname!=GL_LINEAR_ATTENUATION) && (pname!=GL_QUADRATIC_ATTENUATION) ))

	l = &ctx->lights[light-GL_LIGHT0] ;

	switch (pname)
	{
	case GL_AMBIENT:
		memcpy(l->acli ,params, sizeof(GLfloat)*4 );
		break;
	case GL_DIFFUSE:
		memcpy(l->dcli ,params, sizeof(GLfloat)*4 );
		break;
	case GL_SPECULAR:
		memcpy(l->scli ,params, sizeof(GLfloat)*4 );
		break;
	case GL_POSITION:
		{
			memcpy(l->oppli ,params, sizeof(GLfloat)*4);
			InitMatrices(ctx);
			MultMatrix4f(ctx->wa.modelView, (GLfloat *)params,  l->ppli ,0, 1);

			break;
		}
	case GL_SPOT_DIRECTION:
		{
			memcpy(l->osdli ,params, sizeof(GLfloat)*3);
			InitMatrices(ctx);
			MultMatrixNor3f(ctx->wa.modelView, (GLfloat *)params,  l->sdli ,0, 1);

			break;
		}
	case GL_SPOT_EXPONENT:
		l->srli = params[0] ;
		break;
	case GL_SPOT_CUTOFF:
		l->crli = params[0] ;
		break;
	case GL_CONSTANT_ATTENUATION:
		l->k0i = params[0] ;
		break;
	case GL_LINEAR_ATTENUATION:
		l->k1i = params[0] ;
		break;
	case GL_QUADRATIC_ATTENUATION:
		l->k2i = params[0] ;
		break;

	}

	//DBGFUNCTION_EXIT("glLightfv")
}

GL_API void GL_APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{

	//GL11LOGMSG(LOG_GLES11_API,("glColorMask(0x%x\t0x%x\t0x%x\t0x%x)\n",red,green,blue,alpha));

	OGL2_glColorMask(red,green,blue,alpha);

	//DBGFUNCTION_EXIT("glColorMask ")
}

GL_API void GL_APIENTRY glFogf (GLenum pname, GLfloat param)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glFogf(0x%x\t0x%x)\n",pname,param));

	//DBGFUNCTION_INIT("glFogf ")
	////RETAILMSG(1,( TEXT("glFogf \r\n")));

	CHECK_GL_ENUM( (pname != GL_FOG_MODE)&&
	(pname != GL_FOG_DENSITY)&&
	(pname != GL_FOG_START) &&
	(pname != GL_FOG_END ))

	glFogfv(pname, &param) ;
	//DBGFUNCTION_EXIT("glFogf ")
}

GL_API void GL_APIENTRY glFogfv (GLenum pname, const GLfloat *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glFogfv(0x%x\t0x%x)\n",pname,params));

	//DBGFUNCTION_INIT("glFogfv ")
	////RETAILMSG(1,(TEXT(" glFogfv  \r\n")));

	CHECK_GL_ENUM( (pname != GL_FOG_MODE)&&
	(pname != GL_FOG_DENSITY)&&
	(pname != GL_FOG_START) &&
	(pname != GL_FOG_END )&&
	(pname != GL_FOG_COLOR ))

	switch (pname)
	{
	case GL_FOG_MODE:
		CHECK_GL_ENUM( (params[0] != GL_LINEAR)&&
		(params[0] != GL_EXP)&&
		(params[0] != GL_EXP2))
		ctx->fogMode = params[0];
		break ;
	case GL_FOG_DENSITY:
		CHECK_GL_VALUE( (params[0] <0))
		ctx->fogDensity = params[0];
		break ;
	case GL_FOG_START:
		ctx->fogStart = params[0];
		break ;
	case GL_FOG_END:
		ctx->fogEnd = params[0];
		break ;
	case GL_FOG_COLOR:
		memcpy(ctx->fogColor, params, sizeof(GLfloat) * 4 );
		break ;
	}
	//DBGFUNCTION_EXIT("glFogfv ")
}

GL_API void GL_APIENTRY glShadeModel (GLenum mode)
{
#if 0
	ShaderInit();
	OGL2_glVaryingInterpolationEXP("FrontColor",mode);


#else
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glShadeModel(0x%x)\n",mode));

	if(mode == GL_FLAT){
		ctx->isFlatShading = GL_TRUE;

	}else{
		ctx->isFlatShading = GL_FALSE;
	}

	ctx->shadeModel = mode;
#endif

	//DBGFUNCTION_EXIT("glShadeModel ")
}

GL_API void GL_APIENTRY glLightModelx (GLenum pname, GLfixed param)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glLightModelx(0x%x\t0x%x)\n",pname,param));

	//DBGFUNCTION_INIT("glLightModelx ")
	////RETAILMSG(1,(TEXT("glLightModelx  \r\n")));


	CHECK_GL_ENUM((pname!=GL_LIGHT_MODEL_TWO_SIDE) )

	ctx->tbs = param ;

	//DBGFUNCTION_EXIT("glLightModelx ")
}

GL_API void GL_APIENTRY glLightModelxv (GLenum pname, const GLfixed *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glLightModelxv(0x%x\t0x%x)\n",pname,params));

	//DBGFUNCTION_INIT("glLightModelxv ")
	////RETAILMSG(1,(TEXT("glLightModelxv  \r\n")));


	CHECK_GL_ENUM(((pname!=GL_LIGHT_MODEL_AMBIENT) && (pname!=GL_LIGHT_MODEL_TWO_SIDE) ))

	if ( pname == GL_LIGHT_MODEL_TWO_SIDE )
	{
		ctx->tbs = params[0] ;
	}
	if ( pname == GL_LIGHT_MODEL_AMBIENT )
	{
		ctx->acs [0] =FIXED_TO_FLOAT(params[0]);
		ctx->acs [1] =FIXED_TO_FLOAT(params[1]);
		ctx->acs [2] =FIXED_TO_FLOAT(params[2]);
		ctx->acs [3] =FIXED_TO_FLOAT(params[3]);
	}

	//DBGFUNCTION_EXIT("glLightModelxv ")
}

GL_API void GL_APIENTRY glLightx (GLenum light, GLenum pname, GLfixed param)
{

	//GL11LOGMSG(LOG_GLES11_API,("glLightx(0x%x\t0x%x\t0x%x)\n",light,pname,param));

	//DBGFUNCTION_INIT("glLightx ")
	////RETAILMSG(1,(TEXT("glLightx  \r\n")));

	glLightf(light, pname, FIXED_TO_FLOAT(param));

	//DBGFUNCTION_EXIT("glLightx ")
}

GL_API void GL_APIENTRY glLightxv (GLenum light, GLenum pname, const GLfixed *params)
{
	//Light * l = (Light * ) 0 ;
	GLfloat tmp[4] ;
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glLightxv(0x%x\t0x%x\t0x%x)\n",light,pname,params));

	//DBGFUNCTION_INIT("glLightxv ")
	////RETAILMSG(1,(TEXT("glLightxv  \r\n")));

	CHECK_GL_ENUM(((light <GL_LIGHT0) || (light > GL_LIGHT7)))
	CHECK_GL_ENUM(((pname!=GL_AMBIENT) && (pname!=GL_SPOT_CUTOFF) &&
	(pname!=GL_DIFFUSE) && (pname!=GL_SPOT_DIRECTION) &&
	(pname!=GL_SPECULAR) && (pname!=GL_SPOT_EXPONENT) &&
	(pname!=GL_CONSTANT_ATTENUATION) &&(pname!=GL_POSITION) &&
	(pname!=GL_LINEAR_ATTENUATION) && (pname!=GL_QUADRATIC_ATTENUATION) ))

	switch (pname)
	{
	case GL_AMBIENT:
	case GL_DIFFUSE:
	case GL_SPECULAR:
	case GL_POSITION:
		tmp[3] =FIXED_TO_FLOAT(params[3]);
	case GL_SPOT_DIRECTION:
		tmp[1] =FIXED_TO_FLOAT(params[1]);
		tmp[2] =FIXED_TO_FLOAT(params[2]);
		//      break;     // Super G stunt game bluish bug
	case GL_SPOT_EXPONENT:
	case GL_SPOT_CUTOFF:
	case GL_CONSTANT_ATTENUATION:
	case GL_LINEAR_ATTENUATION:
	case GL_QUADRATIC_ATTENUATION:
		tmp[0] = FIXED_TO_FLOAT(params[0]) ;
		break;

	}
	glLightfv(light, pname , tmp  ) ;
	//DBGFUNCTION_EXIT("glLightxv ")
}

GL_API void GL_APIENTRY glMaterialf (GLenum face, GLenum pname, GLfloat param)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glMaterialf(0x%x\t0x%x\t0x%x)\n",face,pname,param));

	//DBGFUNCTION_INIT("glMaterialf ")
	////RETAILMSG(1,(TEXT("glMaterialf  \r\n")));

	CHECK_GL_ENUM((pname != GL_SHININESS))
	glMaterialfv ( face,  pname,&param);
	//DBGFUNCTION_EXIT("glMaterialf ")
}

#define VEC4CPYX(a,b) {a[0]=FIXED_TO_FLOAT(b[0]);a[1]=FIXED_TO_FLOAT(b[1]);a[2]=FIXED_TO_FLOAT(b[2]);a[3]=FIXED_TO_FLOAT(b[3]);}

GL_API void GL_APIENTRY glMaterialfv (GLenum face, GLenum pname, const GLfloat *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glMaterialfv(0x%x\t0x%x\t0x%x)\n",face,pname,params));

	//DBGFUNCTION_INIT("glMaterialfv ")
	////RETAILMSG(1,(TEXT("glMaterialfv  \r\n")));

	CHECK_GL_ENUM((face != GL_FRONT_AND_BACK))
	CHECK_GL_ENUM((pname != GL_AMBIENT)&&
	(pname != GL_DIFFUSE)&&
	(pname != GL_AMBIENT_AND_DIFFUSE)&&
	(pname != GL_SPECULAR)&&
	(pname != GL_EMISSION)&&
	(pname != GL_SHININESS)
	)

	switch ( pname )
	{
	case GL_AMBIENT:
		VEC4CPY( ctx->frontNback.acm, params ) ; break ;
	case GL_DIFFUSE:
		VEC4CPY( ctx->frontNback.dcm, params ) ; break ;
	case GL_AMBIENT_AND_DIFFUSE:
		VEC4CPY( ctx->frontNback.acm, params ) ;VEC4CPY( ctx->frontNback.dcm, params ) ;  break ;
	case GL_SPECULAR:
		VEC4CPY( ctx->frontNback.scm, params ) ; break ;
	case GL_EMISSION:
		VEC4CPY( ctx->frontNback.ecm, params ) ; break ;
	case GL_SHININESS:
		ctx->frontNback.srm = params [0] ; break ;
	}
	//DBGFUNCTION_EXIT("glMaterialfv ")
}

GL_API void GL_APIENTRY glMaterialx (GLenum face, GLenum pname, GLfixed param)
{

	//GL11LOGMSG(LOG_GLES11_API,("glMaterialx(0x%x\t0x%x\t0x%x)\n",face,pname,param));

	//DBGFUNCTION_INIT("glMaterialx ")
	////RETAILMSG(1,(TEXT("glMaterialx  \r\n")));

	glMaterialf(face, pname , FIXED_TO_FLOAT(param)) ;
	//DBGFUNCTION_EXIT("glMaterialx ")
}

GL_API void GL_APIENTRY glMaterialxv (GLenum face, GLenum pname, const GLfixed *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glMaterialxv(0x%x\t0x%x\t0x%x)\n",face,pname,params));

	//DBGFUNCTION_INIT("glMaterialxv ")
	////RETAILMSG(1,(TEXT("glMaterialxv  \r\n")));

	CHECK_GL_ENUM((face != GL_FRONT_AND_BACK))
	CHECK_GL_ENUM((pname != GL_AMBIENT)&&
	(pname != GL_DIFFUSE)&&
	(pname != GL_AMBIENT_AND_DIFFUSE)&&
	(pname != GL_SPECULAR)&&
	(pname != GL_EMISSION)&&
	(pname != GL_SHININESS)
	)

	switch ( pname )
	{
	case GL_AMBIENT:
		VEC4CPYX( ctx->frontNback.acm, params ) ; break ;
	case GL_DIFFUSE:
		VEC4CPYX( ctx->frontNback.dcm, params ) ; break ;
	case GL_AMBIENT_AND_DIFFUSE:
		VEC4CPYX( ctx->frontNback.acm, params ) ;VEC4CPY( ctx->frontNback.dcm, params ) ;  break ;
	case GL_SPECULAR:
		VEC4CPYX( ctx->frontNback.scm, params ) ; break ;
	case GL_EMISSION:
		VEC4CPYX( ctx->frontNback.ecm, params ) ; break ;
	case GL_SHININESS:
		ctx->frontNback.srm = FIXED_TO_FLOAT(params [0]) ; break ;
	}
	//DBGFUNCTION_EXIT("glMaterialxv ")
}

GL_API void GL_APIENTRY glFogx (GLenum pname, GLfixed param)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glFogx(0x%x\t0x%x)\n",pname,param));

	//DBGFUNCTION_INIT("glFogx ")
	////RETAILMSG(1,( TEXT("glFogx \r\n")));

	CHECK_GL_ENUM( (pname != GL_FOG_MODE)&&
	(pname != GL_FOG_DENSITY)&&
	(pname != GL_FOG_START) &&
	(pname != GL_FOG_END ))

	glFogxv(pname, &param) ;

	//DBGFUNCTION_EXIT("glFogx ")
}

GL_API void GL_APIENTRY glFogxv (GLenum pname, const GLfixed *params)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glFogxv(0x%x\t0x%x)\n",pname,params));

	//DBGFUNCTION_INIT("glFogxv ")
	////RETAILMSG(1,(TEXT("glFogxv  \r\n")));

	CHECK_GL_ENUM( (pname != GL_FOG_MODE)&&
	(pname != GL_FOG_DENSITY)&&
	(pname != GL_FOG_START) &&
	(pname != GL_FOG_END )&&
	(pname != GL_FOG_COLOR ))

	switch (pname)
	{
	case GL_FOG_MODE:
		CHECK_GL_ENUM( (params[0] != GL_LINEAR)&&
		(params[0] != GL_EXP)&&
		(params[0] != GL_EXP2))
		ctx->fogMode = params[0];
		break ;
	case GL_FOG_DENSITY:
		CHECK_GL_VALUE( (FIXED_TO_FLOAT(params[0]) <0.0f))
		ctx->fogDensity = FIXED_TO_FLOAT(params[0]);
		break ;
	case GL_FOG_START:
		ctx->fogStart = FIXED_TO_FLOAT(params[0]);
		break ;
	case GL_FOG_END:
		ctx->fogEnd = FIXED_TO_FLOAT(params[0]);
		break ;
	case GL_FOG_COLOR:
		ctx->fogColor[0] = FIXED_TO_FLOAT(params[0]);
		ctx->fogColor[1] = FIXED_TO_FLOAT(params[1]);
		ctx->fogColor[2] = FIXED_TO_FLOAT(params[2]);
		ctx->fogColor[3] = FIXED_TO_FLOAT(params[3]);
		break ;
	}
	//DBGFUNCTION_EXIT("glFogxv ")
}
