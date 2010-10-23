#include <assert.h>

#include "includes.h"
#include "gles.h"
#include "OGL2Func_for_gles11.h"
#include "attribs.h"
#include "shaderValues.h"
#include "shaderSplitting.h"

#include <cutils/log.h>
#define LOGMSG(printf_exp) { LOGE printf_exp ;}


#include <sys/time.h>

struct timeval __start;
struct timeval __end;

static inline double  __test_tval(struct timeval _tstart,struct timeval _tend)
{
  double t1 = 0.;
  double t2 = 0.;

  t1 = ((double)_tstart.tv_sec * 1000 + (double)_tstart.tv_usec/1000.0) ;
  t2 = ((double)_tend.tv_sec * 1000 + (double)_tend.tv_usec/1000.0) ;
  
  return t2-t1;
}
/*
*******************************************************************************
* Global Variable Definitions
*******************************************************************************
*/
static GLuint gGL11ContextIDCounter = 0;


extern  GLboolean  isAlphaorLuminanceRGBTextureEXP (GLenum texture, GLboolean flag);
extern void loadShaders();
extern void resetShaderInfo();
extern int  selectShaderIndex(GLState *ctx);
extern void matTranspose(float* m);

typedef enum
{
	MAT4,
	MAT3,
	MAT22,
	MAT4T,
	MAT3T,
	MAT2T,
	FLOAT_GL,
	VEC4,
	VEC3,
	VEC2,
	BOOL_GL,
	INT_GL

} DataType;

GLuint shaderProgram;

#ifdef GLES11_LOG_ENABLE
unsigned int bGles11Log;
#endif

void setArray(char *name,char *namepassed,int i)
{
	char buffer[50]={0,};
	strcpy(name ,namepassed);
	strcat(name ,"[");
	sprintf(buffer,"%d",i);
	strcat(name , buffer);
	strcat(name,"]");
}

Map iMap[] =
{
	// Texture environment modes
	{ GL_REPLACE,         TEX_ENV_MODE_REPLACE},
	{ GL_MODULATE ,       TEX_ENV_MODE_MODULATE  },
	{ GL_ADD ,            TEX_ENV_MODE_ADD},
	{ GL_DECAL ,          TEX_ENV_MODE_DECAL},
	{ GL_BLEND,           TEX_ENV_MODE_BLEND },
	{ GL_COMBINE ,        TEX_ENV_MODE_COMBINE},

	// Combine RGB
	{ GL_REPLACE ,        TEX_COMBINE_RGB_REPLACE },
	{ GL_MODULATE ,       TEX_COMBINE_RGB_MODULATE},
	{ GL_ADD ,            TEX_COMBINE_RGB_ADD},
	{ GL_ADD_SIGNED  ,    TEX_COMBINE_RGB_ADD_SIGNED},
	{ GL_INTERPOLATE ,    TEX_COMBINE_RGB_INTERPOLATE},
	{ GL_SUBTRACT ,       TEX_COMBINE_RGB_SUBTRACT},
	{ GL_DOT3_RGB ,       TEX_COMBINE_RGB_DOT3_RGB},
	{ GL_DOT3_RGBA ,      TEX_COMBINE_RGB_DOT3_RGBA},

	// Combine Alpha
	{ GL_REPLACE ,        TEX_COMBINE_ALPHA_REPLACE},
	{ GL_MODULATE ,       TEX_COMBINE_ALPHA_MODULATE},
	{ GL_ADD ,            TEX_COMBINE_ALPHA_ADD},
	{ GL_ADD_SIGNED ,     TEX_COMBINE_ALPHA_ADD_SIGNED},
	{ GL_INTERPOLATE ,    TEX_COMBINE_ALPHA_INTERPOLATE},
	{ GL_SUBTRACT ,       TEX_COMBINE_ALPHA_SUBTRACT},


	// Combiner RGB and ALPHA source arguments
	{ GL_TEXTURE ,        TEX_ENV_COMBINER_SRC_TEXTURE},
	{ GL_CONSTANT ,       TEX_ENV_COMBINER_SRC_CONSTANT},
	{ GL_PRIMARY_COLOR ,  TEX_ENV_COMBINER_SRC_PRIMARY_COLOR},
	{ GL_PREVIOUS ,       TEX_ENV_COMBINER_SRC_PREVIOUS},

	// Combiner RGB operands
	{ GL_SRC_COLOR  ,           TEX_ENV_COMBINER_RGB_OP_SRC_COLOR },
	{ GL_ONE_MINUS_SRC_COLOR ,  TEX_ENV_COMBINER_RGB_OP_ONE_MINUS_SRC_COLOR},
	{ GL_SRC_ALPHA ,            TEX_ENV_COMBINER_RGB_OP_SRC_ALPHA},
	{ GL_ONE_MINUS_SRC_ALPHA ,  TEX_ENV_COMBINER_RGB_OP_ONE_MINUS_SRC_ALPHA},

	// Combiner ALPHA operands
	{ GL_SRC_ALPHA , TEX_ENV_COMBINER_ALPHA_OP_SRC_ALPHA },
	{ GL_ONE_MINUS_SRC_ALPHA, TEX_ENV_COMBINER_ALPHA_OP_ONE_MINUS_SRC_ALPHA },

	// Fog operations
	{ GL_LINEAR , FOGMODE_LINEAR },
	{ GL_EXP, FOGMODE_EXP },
	{ GL_EXP2, FOGMODE_EXP2 },
};

int  Search(int OGL_1 , Map *iMap_OGL, int n)
{
	int i;
	for( i= 0 ; i < n ; i++)
	{
		if(iMap_OGL[i].OGL1 == OGL_1)
		return iMap_OGL[i].OGL2;
	}
	return -1;
}

int GetVertexWords(GLState *ctx)
{
	GLint vwords =0;
	GLint flags; GLint texCount;
	flags = ctx->flags ;
	texCount = ctx->wa.texCount;
	ASSERT(texCount <= 2) ;
	if (( ctx->flags & F_FOG )&&((ctx->fogDensity != 0.0f )||(ctx->fogMode == GL_LINEAR)))
	{
		ctx->flagspt |= F_FOG_USE;
	}
	else
	{
		ctx->flagspt &= ~F_FOG_USE;
	}

	if ( ctx->flagspt & F_FOG_USE)
	{
		if ( texCount == 0 ) vwords =(sizeof(ESVertexFog)/sizeof(GLfloat));
		if ( texCount == 1 ) vwords =(sizeof(ESVertexFogTex1)/sizeof(GLfloat));
		if ( texCount == 2 ) vwords =(sizeof(ESVertexFogTex2)/sizeof(GLfloat));
	}
	else
	{
		if ( texCount == 0 ) vwords =(sizeof(ESVertex)/sizeof(GLfloat));
		if ( texCount == 1 ) vwords =(sizeof(ESVertexTex1)/sizeof(GLfloat));
		if ( texCount == 2 ) vwords =(sizeof(ESVertexTex2)/sizeof(GLfloat));
	}
	ctx->wa.vwords = vwords ;
	return vwords ;
}

int FreeTnL(GLState *ctx,WorkArea *wa)
{
	FREE_MEM_CHKRST(wa->backColorStream );
	FREE_MEM_CHKRST(wa->vertexStream);
	FREE_MEM_CHKRST(wa->clipcoords);
	FREE_MEM_CHKRST(wa->eyecoords);
	FREE_MEM_CHKRST(wa->frontColor);
	return 0  ;
}

void InverseTranspose(GLfloat *src,GLfloat *dst);

void InitMatrices(GLState *ctx)
{
	/*static GLfloat zeromat []= {0.0f, 0.0f, 0.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f}; */

	if ( ctx->flags & F_MATRIX_PALETTE_OES)
	{
		/* Matrix palette is enabled. */

		//ctx->wa.modelView = ctx->wa.modelViewPal;
		ctx->wa.proj  = ctx->stacks[MAT_PROJECTION].matrices[ctx->stacks[MAT_PROJECTION].pos ]->data;

	}
	else
	{
		//int i = 0 , j = 0 ;
		/* This is the normal case of no palette */
#ifdef MATRIX_DIRTY_STATE
		if (ctx->mvprojDirtyState == GL_TRUE)
		{
#endif
			ctx->wa.modelView = (ctx->stacks[MAT_MODELVIEW].matrices[ctx->stacks[MAT_MODELVIEW].pos ])->data;
			ctx->wa.proj  = ctx->stacks[MAT_PROJECTION].matrices[ctx->stacks[MAT_PROJECTION].pos ]->data;
			MultMatrix44(ctx->wa.proj, ctx->wa.modelView ,ctx->wa.projModelView) ;
			InverseTranspose(ctx->wa.modelView  , ctx->wa.invModelView) ;
			//InverseTranspose(ctx->wa.projModelView  , ctx->wa.invProjModelView ) ;
#ifdef MATRIX_DIRTY_STATE

			ctx->mvprojDirtyState = GL_FALSE;
		}
#endif
	}
}

int GetStride(GLenum type)
{
	switch (type)
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
	ASSERT(0);
	return 1 ;
}

extern  int isTexUnitMipMapped(GLuint texUnit);

inline int isColorUniform()
{
	int colorAsAttri = 1;
	OGL2_glGetVertexAttribiv(ATTRIB_LOC_Color, GL_VERTEX_ATTRIB_ARRAY_ENABLED , &colorAsAttri);
	//printf(" colorAsAttri = %d \n", colorAsAttri);
	return !colorAsAttri;
}

void setShaderSelectInfo()
{
	//int count = 0;
	//int tempTexMode = 0;
	//int numLightUsed = 0;
	int tnf;

	GET_GLES_CONTEXT


	ctx->textureMode[0] =0;
	ctx->textureMode[1] = 0;
	ctx->numLightUsed  = 0;
	ctx->fogshaderMode = 0;
	ctx->isMipmapEnabled[0] = 1;
	ctx->isMipmapEnabled[1] = 1;
	ctx->userClipEnabled = 0;
	ctx->matrixPaletteEnabled = 0;
	ctx->pointSpriteEnabled[0] = 0;
	ctx->pointSpriteEnabled[1] = 0;
	ctx->colorAsUniform = 0;

	//texture 0 mode
	tnf = ctx->flags & F_TEXTURE_2D_UNIT0;
	if(tnf != 0){
		ctx->textureMode[0]  = Search(ctx->texEnvMode[0] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		if(ctx->textureMode[0]  == -1) {
			ctx->textureMode[0] = 1;
		}
		ctx->isMipmapEnabled[0]  = isTexUnitMipMapped(0);

	}

	//texture 1 mode
	tnf = ctx->flags & F_TEXTURE_2D_UNIT1;
	if(tnf != 0){
		ctx->textureMode[1]  = Search(ctx->texEnvMode[1] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		if(ctx->textureMode[1]  == -1) {
			ctx->textureMode[1] = 1;
		}
		ctx->isMipmapEnabled[1]  = isTexUnitMipMapped(1);
	}

	//  light enabled
	if((ctx->flags & F_LIGHTING) != 0){
		ctx->numLightUsed  = 1;
	}

	//fog  mode
	tnf =   (ctx->flags & (F_FOG))? 1:0;
	if( tnf  == 1){
		ctx->fogshaderMode = Search(ctx->fogMode , iMap , sizeof(iMap)/sizeof(iMap[0]) );
	}


	//point is already present in pointEnable varaible of context


	//user clip plane
	if( (ctx->cflags[0] || ctx->cflags[1] || ctx->cflags[2] ||ctx->cflags[3] || ctx->cflags[4] || ctx->cflags[5])){
		ctx->userClipEnabled  = 1;
	}

	//matrix palette
	if(ctx->flags & F_MATRIX_PALETTE_OES){
		ctx->matrixPaletteEnabled = 1;
	}

	if((ctx->flagspt & F_POINT_SPRITE_OES) && (ctx->texReplaceCoord[0])){
		ctx->pointSpriteEnabled[0] = 1;
	}



	if((ctx->flagspt & F_POINT_SPRITE_OES) && (ctx->texReplaceCoord[1])){
		ctx->pointSpriteEnabled[1] = 1;
	}


	if(isColorUniform() == 1){
		if((ctx->textureMode[0] ==0) && (ctx->textureMode[1] == 0) && (ctx->fogshaderMode == 0) && (ctx->userClipEnabled == 0) &&
				(ctx->matrixPaletteEnabled == 0) &&  (ctx->pointSpriteEnabled[0] ==0) && ( ctx->pointSpriteEnabled[1] == 0)){
			ctx->colorAsUniform = 1;
		}
	}

}



//change start
GL_API void GL_APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count)
{
//	gettimeofday(&__start,NULL);

	int index = -1;

	GET_GLES_CONTEXT

//	////GL11LOGMSG(LOG_GLES11_API,("glDrawArrays(0x%x\t0x%x\t0x%x)\n",mode,first,count));

//	//DBGFUNCTION_INIT("glDrawArrays ")
//	//RETAILMSG(1,(TEXT("glDrawArrays  \r\n")));

	ctx->mode = mode ;
	ctx->first = first ;
	ctx->count = count ;
	ctx->type = GL_UNSIGNED_BYTE;
	ctx->indices = (const GLvoid *)0; //not used
	ctx->isIndexed = 0 ;     //not being used

	ctx->pointEnable = 0;
	if(mode == GL_POINTS)
	{
		ctx->pointEnable = 1;
		if (!(ctx->pointSizeArrayEnable))
		OGL2_glVertexAttrib1f ( ATTRIB_LOC_PointSize, ctx->pointSize);
	}
	else
	{
		ctx->flagspt &= ~F_POINT_SPRITE_OES;
	}

	//  LOGE(">>>>>>>>>>>>>>>>glDrawArrays HW<<<<<<<<<<<<<<<<<");

	setShaderSelectInfo();

	index = selectShaderIndex(ctx);

	//	LOGMSG(("Selecting shader %u",index));
	//fprintf(stderr, "\n index = %d \n", index);
	if(index != ctx->prevIndex)
	{
	OGL2_glUseProgram(ctx->progInfo[index].programId);
	ctx->prevIndex=index;
	}
	
	InitMatrices(ctx);
	UniformsInit(index);

	if(ctx->isFlatShading == GL_TRUE){
		OGL2_glVaryingInterpolationEXP("FrontColor",GL_FLAT);

	}

	OGL2_glDrawArrays(mode, first,  count);

	//resetting the value to smooth(as this is most commonly used)
	if(ctx->isFlatShading == GL_TRUE){
		OGL2_glVaryingInterpolationEXP("FrontColor",GL_SMOOTH);
	}
//	gettimeofday(&__end,NULL);
//	LOGE("glDrawArrays completed in: %5.5f ms",__test_tval(__start,__end));
}

// Search for enumerated states
// setArray does string catenation for array of states
void multVector(int size,float oper1[],float oper2[],float result[])
{
	int i;
	for(i=0;i<size;i++)
	{
		result[i]=oper1[i]*oper2[i];
	}
}

void addVector(int size,float oper1[],float oper2[],float result[])
{
	int i;
	for(i=0;i<size;i++)
	{
		result[i]=oper1[i]+oper2[i];
	}
}

void SetUniform(int Pos, DataType dt,  void *ptr)
{
	if(Pos == -1) return ;
	switch(dt)
	{
	case MAT4:
		matTranspose(ptr);
		OGL2_glUniformMatrix4fv(Pos, 1, 0, ptr);
		matTranspose(ptr);
		break;
	case MAT3:
		matTranspose(ptr);
		OGL2_glUniformMatrix3fv(Pos, 1, 0, ptr);
		matTranspose(ptr);
		break;
	case MAT22:
		matTranspose(ptr);
		OGL2_glUniformMatrix2fv(Pos, 1, 0, ptr);
		matTranspose(ptr);
		break;
	case MAT4T:
		OGL2_glUniformMatrix4fv(Pos, 1, 0, ptr);
		break;
	case MAT3T:
		OGL2_glUniformMatrix3fv(Pos, 1, 0, ptr);
		break;
	case MAT2T:
		OGL2_glUniformMatrix2fv(Pos, 1, 0, ptr);
		break;
	case FLOAT_GL:
		OGL2_glUniform1f(Pos, *((float*)ptr));
		break;
	case VEC4:
		OGL2_glUniform4fv(Pos,1, ptr);
		break;
	case VEC3:
		OGL2_glUniform3fv(Pos,1, ptr);
		break;
	case VEC2:
		OGL2_glUniform2fv(Pos,1, ptr);
		break;
	case BOOL_GL:
		OGL2_glUniform1i(Pos, *((int*)ptr));
		break;
	case INT_GL:
		OGL2_glUniform1i(Pos, *((int*)ptr));
		break;
	}
}

int GetPaletteMatUniformLoc ( UniformLoc* pUnLoc , int index, int norFlag)
{
	// norFlag = 1 for normal matrix else palette vertex matrix
	if (!norFlag)
	{
		// These are the locations for paletteMatrices Uniform in vertex shader
		switch (index)
		{
		case 0:
			return pUnLoc->UNIFORM_LOC_paletteMatrices0;
		case 1:
			return pUnLoc->UNIFORM_LOC_paletteMatrices1;
		case 2:
			return pUnLoc->UNIFORM_LOC_paletteMatrices2;
		case 3:
			return pUnLoc->UNIFORM_LOC_paletteMatrices3;
		case 4:
			return pUnLoc->UNIFORM_LOC_paletteMatrices4;
		case 5:
			return pUnLoc->UNIFORM_LOC_paletteMatrices5;
		case 6:
			return pUnLoc->UNIFORM_LOC_paletteMatrices6;
		case 7:
			return pUnLoc->UNIFORM_LOC_paletteMatrices7;
		case 8:
			return pUnLoc->UNIFORM_LOC_paletteMatrices8;
		case 9:
			return pUnLoc->UNIFORM_LOC_paletteMatrices9;
		default:
			return -1;
		}
	}
	else
	{
		// These are the locations for paletteNormalMatrices Uniform in vertex shader
		switch (index)
		{
		case 0:
			return pUnLoc->UNIFORM_LOC_paletteNormalMatrices0;
		case 1:
			return pUnLoc->UNIFORM_LOC_paletteNormalMatrices1;
		case 2:
			return pUnLoc->UNIFORM_LOC_paletteNormalMatrices2;
		case 3:
			return pUnLoc->UNIFORM_LOC_paletteNormalMatrices3;
		case 4:
			return pUnLoc->UNIFORM_LOC_paletteNormalMatrices4;
		case 5:
			return pUnLoc->UNIFORM_LOC_paletteNormalMatrices5;
		case 6:
			return pUnLoc->UNIFORM_LOC_paletteNormalMatrices6;
		case 7:
			return pUnLoc->UNIFORM_LOC_paletteNormalMatrices7;
		case 8:
			return pUnLoc->UNIFORM_LOC_paletteNormalMatrices8;
		case 9:
			return pUnLoc->UNIFORM_LOC_paletteNormalMatrices9;
		default:
			return -1;
		}
	}

	return -1;
}


void normalizeVec(float * dst , float* src ,  int count)
{
	int i =0;
	float  norm = 0;

	for (i = 0 ; i < count ; i++){
		norm +=    src[i] * src[i];
	}

	norm = sqrt(norm);

	for( i =0 ; i < count ; i++){
		dst[i] = src[i] / norm;
	}

}




void UniformsInit(int index)
{
//	gettimeofday(&__start,NULL);
	int i,j,tnf[2];
	float norMatrix[16]={0,},normalMatrix[3][3],result[4];
	//char name[256]={0,}; // Amit Garg
	//float tempf;
	//char buffer[10]={0,};
	Light * l = (Light * ) 0;
	float  store;
	int temp = -1;
	int count = 0;
	int numLightUsed =0;

	GET_GLES_CONTEXT
	// Provides the model-view, projection, model-view-projection and texture matrices
	//to get the uniform location
	UniformLoc* pUnLoc = ctx->progInfo[index].pUniformLocation;

	if(ctx->progInfo[index].pUniformLocation == NULL){
		fprintf(stderr, "where the hell is the memory allocated");
	}

	if(index==0 || index==3)
	SetUniform(pUnLoc->UNIFORM_LOC_colorValue,  VEC4 , ctx->color);
	
	if(index==3)
	SetUniform(pUnLoc->UNIFORM_LOC_AlphaorLuminance,  BOOL_GL , &ctx->AlphaorLuminance);
	
	if(ctx->colorAsUniform == 1){
		GLfloat colorValue[4];
		OGL2_glGetVertexAttribfv(ATTRIB_LOC_Color ,GL_CURRENT_VERTEX_ATTRIB, colorValue);
		SetUniform(pUnLoc->UNIFORM_LOC_colorValue,  VEC4 , colorValue);
	}

	temp = (ctx->flags & F_MATRIX_PALETTE_OES);
	SetUniform(pUnLoc->UNIFORM_LOC_matrixPaletteEnable, BOOL_GL ,&temp);
	if (!temp){

		// Provides the model-view, , model-view-projection
		SetUniform(pUnLoc->UNIFORM_LOC_ModelViewMatrix,  MAT4 , ctx->wa.modelView);
		SetUniform(pUnLoc->UNIFORM_LOC_ModelViewProjectionMatrix, MAT4 , ctx->wa.projModelView) ;

		// transpose of the inverse of the upper leftmost 3x3 of gl_ModelViewMatrix. neded only by LIGHTING
		j = InverseMatrix33(ctx->wa.modelView , norMatrix );
		for(i=0;i<3;i++)
		for(j=0;j<3;j++)
		normalMatrix[i][j] = norMatrix[i*4+j];

		SetUniform(pUnLoc->UNIFORM_LOC_NormalMatrix, MAT3T ,normalMatrix );
	}
	else {
		int ind = 0;
		GLfloat *modelViewPal = NULL;
		int currentPalMatUniformLocation = -1;
		int currentPalNormalMatUniformLocation = -1;

		// In matrix palette case "ModelViewProjectionMatrix" should contain
		//projection matrix instead of modelview projection matrix.
		SetUniform(pUnLoc->UNIFORM_LOC_ModelViewProjectionMatrix, MAT4 , ctx->wa.proj);
		SetUniform(pUnLoc->UNIFORM_LOC_noOfVertexUnit, FLOAT_GL, &ctx->noOfVertexUnit);

		for (ind = 0; ind < C_MAX_PALETTE_MATRICES_OES; ind++)
		{
			modelViewPal = ctx->paletteMatrix[ind].data;
			currentPalMatUniformLocation =  GetPaletteMatUniformLoc (pUnLoc, ind, 0);
			currentPalNormalMatUniformLocation = GetPaletteMatUniformLoc (pUnLoc, ind, 1);
			SetUniform (currentPalMatUniformLocation, MAT4, modelViewPal);

			// transpose of the inverse of the upper leftmost 3x3 of gl_ModelViewMatrix
			j = InverseMatrix33(modelViewPal , norMatrix );

			for(i=0;i<3;i++)
			for(j=0;j<3;j++)
			normalMatrix[i][j] = norMatrix[i*4+j];

			SetUniform(currentPalNormalMatUniformLocation, MAT3T ,normalMatrix );
		}
	}

	//Point Parameters
	SetUniform(pUnLoc->UNIFORM_LOC_PointEnabled, BOOL_GL ,&ctx->pointEnable );
	if(!(ctx->pointSizeArrayEnable))
	{
		SetUniform(pUnLoc->UNIFORM_LOC_Point_size, FLOAT_GL ,&ctx->pointSize);
		SetUniform(pUnLoc->UNIFORM_LOC_Point_sizeMin, FLOAT_GL ,&ctx->pointSizeMin );
		SetUniform(pUnLoc->UNIFORM_LOC_Point_sizeMax, FLOAT_GL , &ctx->pointSizeMax);
		SetUniform(pUnLoc->UNIFORM_LOC_Point_fadeThresholdSize, FLOAT_GL , &ctx->pointFadeThresh);
		SetUniform(pUnLoc->UNIFORM_LOC_Point_distanceConstantAttenuation, FLOAT_GL ,&ctx->pointDistAtten[0] );
		SetUniform(pUnLoc->UNIFORM_LOC_Point_distanceLinearAttenuation, FLOAT_GL , &ctx->pointDistAtten[1]);
		SetUniform(pUnLoc->UNIFORM_LOC_Point_distanceQuadraticAttenuation, FLOAT_GL ,&ctx->pointDistAtten[2] );
	}

	//point sprite.
	temp = 0;
	SetUniform(pUnLoc->UNIFORM_LOC_PointSpriteEnabled0, BOOL_GL, &temp);
	SetUniform(pUnLoc->UNIFORM_LOC_PointSpriteEnabled1, BOOL_GL, &temp);
	temp = (ctx->flagspt & F_POINT_SPRITE_OES );
	if (temp)
	{
		temp = ctx->texReplaceCoord[0];
		if (temp)
		{
			SetUniform(pUnLoc->UNIFORM_LOC_PointSpriteEnabled0, BOOL_GL, &temp);

		}
		temp = ctx->texReplaceCoord[1];
		if (temp)
		{
			SetUniform(pUnLoc->UNIFORM_LOC_PointSpriteEnabled1, BOOL_GL, &temp);

		}
	}

	//color material
	temp = 0;
	SetUniform(pUnLoc->UNIFORM_LOC_colorMaterialEnabled, BOOL_GL ,&temp );
	if (ctx->flags &  F_COLOR_MATERIAL)
	{
		temp = 1;
		SetUniform(pUnLoc->UNIFORM_LOC_colorMaterialEnabled, BOOL_GL ,&temp );
	}

	// Lighting Parameters
	tnf[0] = !!(ctx->flags &  F_LIGHTING);
	SetUniform(pUnLoc->UNIFORM_LOC_LightingEnabled, BOOL_GL ,&tnf[0] );

	if(!!(ctx->flags &  F_LIGHTING))
	{

		// Derived state from products of light and material
		multVector(4,ctx->frontNback.acm,ctx->acs,result);
		addVector(4,ctx->frontNback.ecm,result,result);
		SetUniform(pUnLoc->UNIFORM_LOC_FrontNBackLightModelProduct_sceneColor, VEC4 ,result);//ctx->frontNback.ecm + ctx->frontNback.acm * ctx->acs );

		//  Material State
		//SetUniform(UNIFORM_LOC_FrontNBackMaterial_emission, VEC4 ,ctx->frontNback.ecm );
		SetUniform(pUnLoc->UNIFORM_LOC_FrontNBackMaterial_ambient, VEC4 ,ctx->frontNback.acm);
		SetUniform(pUnLoc->UNIFORM_LOC_FrontNBackMaterial_diffuse, VEC4 ,ctx->frontNback.dcm);
		SetUniform(pUnLoc->UNIFORM_LOC_FrontNBackMaterial_specular, VEC4 ,ctx->frontNback.scm );
		SetUniform(pUnLoc->UNIFORM_LOC_FrontNBackMaterial_shininess, FLOAT_GL ,&ctx->frontNback.srm );

		for(count = 0 ; count < 8 ; count++){
			if(ctx->lflags[count] == 1) numLightUsed++;
		}

		SetUniform(pUnLoc->UNIFORM_LOC_NumLightUsed, INT_GL ,&numLightUsed);

		//to set the various light parameter
		for(i =0 , j=0; j < numLightUsed;i++){
			if(ctx->lflags[i] == 1){

				l=&ctx->lights[i];

				SetUniform(pUnLoc->UNIFORM_LOC_LightSource[j].ambient, VEC4 ,l->acli);
				SetUniform(pUnLoc->UNIFORM_LOC_LightSource[j].diffuse, VEC4 ,l->dcli );
				SetUniform(pUnLoc->UNIFORM_LOC_LightSource[j].specular, VEC4 ,l->scli  );
				SetUniform(pUnLoc->UNIFORM_LOC_LightSource[j].position, VEC4 ,l->ppli );

				tnf[0] = !l->ppli[3];
				SetUniform(pUnLoc->UNIFORM_LOC_LightSource[j].isDirectional, BOOL_GL ,&tnf );

				if(l->ppli[3])
				{
					float norSdi[3] ;
					normalizeVec( norSdi , l->sdli , 3);
					SetUniform(pUnLoc->UNIFORM_LOC_LightSource[j].spotDirection, VEC3 ,norSdi );
					SetUniform(pUnLoc->UNIFORM_LOC_LightSource[j].spotExponent, FLOAT_GL ,&l->srli );
					SetUniform(pUnLoc->UNIFORM_LOC_LightSource[j].spotCutoff, FLOAT_GL ,&l->crli );

					store = (float)cos((l->crli * 3.14159)/180.0);
					SetUniform(pUnLoc->UNIFORM_LOC_LightSource[j].spotCosCutoff, FLOAT_GL ,&store );

					SetUniform(pUnLoc->UNIFORM_LOC_LightSource[j].constantAttenuation, FLOAT_GL ,&l->k0i );
					SetUniform(pUnLoc->UNIFORM_LOC_LightSource[j].linearAttenuation, FLOAT_GL ,&l->k1i );
					SetUniform(pUnLoc->UNIFORM_LOC_LightSource[j].quadraticAttenuation, FLOAT_GL ,&l->k2i );
				}
				j++;
			}
		}
	}

	//numTextureEnabled = ctx->numTextureUsed;
	tnf[0] = ctx->flags & F_TEXTURE_2D_UNIT0;
	tnf[1] = ctx->flags & F_TEXTURE_2D_UNIT1;

	// Textures parameter
	SetUniform(pUnLoc->UNIFORM_LOC_Texture2DEnabled0, BOOL_GL , &tnf[0]);
	if(tnf[0] != 0){

		temp = 0; // Enabling Texture unit 0

		//needed when TEXTURE0 IS BEING USED
		ctx->wa.texMatrix =  (ctx->stacks[MAT_TEXTURE0].matrices[ctx->stacks[MAT_TEXTURE0].pos ])->data;
		SetUniform(pUnLoc->UNIFORM_LOC_TextureMatrix0 , MAT4,ctx->wa.texMatrix  );

		SetUniform(pUnLoc->UNIFORM_LOC_TexImages[0], INT_GL, &temp);
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].color, VEC4 ,ctx->texEnvColor[0]);

		temp = Search(ctx->texEnvMode[0] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].mode, INT_GL, &temp);

		//alpha texture is present or not
		temp = (int) isAlphaorLuminanceRGBTextureEXP(GL_TEXTURE0,1);
		SetUniform(pUnLoc->UNIFORM_LOC_FS_alphaTextureEnable0, BOOL_GL , &temp);

		temp = (int) isAlphaorLuminanceRGBTextureEXP(GL_TEXTURE0,0);
		SetUniform(pUnLoc->UNIFORM_LOC_FS_noAlphaChannelTextureEnable0, BOOL_GL , &temp);

		temp = Search(ctx->texEnvCombineRGB[0] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].combineRgb, INT_GL ,&temp );

		temp = Search(ctx->texEnvCombineAlpha[0] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].combineAlpha, INT_GL ,&temp );

		temp = Search(ctx->mSrcRGB[0][0] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].srcRgb0, INT_GL ,&temp);
		temp = Search(ctx->mSrcRGB[0][1] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].srcRgb1, INT_GL ,&temp);
		temp = Search(ctx->mSrcRGB[0][2] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].srcRgb2, INT_GL ,&temp);

		temp = Search(ctx->mSrcAlpha[0][0] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].srcAlpha0, INT_GL ,&temp);
		temp = Search(ctx->mSrcAlpha[0][1] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].srcAlpha1, INT_GL ,&temp);
		temp = Search(ctx->mSrcAlpha[0][2] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].srcAlpha2, INT_GL ,&temp);

		temp = Search(ctx->mOperandRGB[0][0] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].operandRgb0, INT_GL ,&temp);
		temp = Search(ctx->mOperandRGB[0][1] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].operandRgb1, INT_GL ,&temp);
		temp = Search(ctx->mOperandRGB[0][2] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].operandRgb2, INT_GL ,&temp);

		temp = Search(ctx->mOperandAlpha[0][0] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		//Work around for operandAlpha: always (temp = temp -2) because search results in GL_SRC_ALPHA from
		// operandRgb. In iMap structure check  Combiner RGB operand and Combiner Alpha operand
		temp = temp - 2;
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].operandAlpha0, INT_GL ,&temp);

		temp = Search(ctx->mOperandAlpha[0][1] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		temp = temp - 2;
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].operandAlpha1, INT_GL ,&temp);
		temp = Search(ctx->mOperandAlpha[0][2] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		temp = temp - 2;
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].operandAlpha2, INT_GL ,&temp);

		//ctx->mRGBScale[i] = (float)1.0;
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].rgbScale, FLOAT_GL ,&ctx->mRGBScale[0]);

		//ctx->mAlphaScale[i] = 1.0;
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[0].alphaScale, FLOAT_GL ,&ctx->mAlphaScale[0]);

	}

	SetUniform(pUnLoc->UNIFORM_LOC_Texture2DEnabled1, BOOL_GL , &tnf[1]);
	if(tnf[1] != 0){

		temp = 1; // Enabling Texture unit 1

		//needed when TEXTURE1 IS BEING USED
		ctx->wa.texMatrix =  (ctx->stacks[MAT_TEXTURE1].matrices[ctx->stacks[MAT_TEXTURE1].pos ])->data;
		SetUniform(pUnLoc->UNIFORM_LOC_TextureMatrix1 , MAT4,ctx->wa.texMatrix);

		SetUniform(pUnLoc->UNIFORM_LOC_TexImages[1], INT_GL, &temp);
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].color, VEC4 ,ctx->texEnvColor[1]);

		temp = Search(ctx->texEnvMode[1] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].mode, INT_GL, &temp);

		//alpha texture is present or not
		temp = (int) isAlphaorLuminanceRGBTextureEXP(GL_TEXTURE1,1);
		SetUniform(pUnLoc->UNIFORM_LOC_FS_alphaTextureEnable1, BOOL_GL , &temp);

		temp = (int) isAlphaorLuminanceRGBTextureEXP(GL_TEXTURE1,0);
		SetUniform(pUnLoc->UNIFORM_LOC_FS_noAlphaChannelTextureEnable1, BOOL_GL , &temp);

		temp = Search(ctx->texEnvCombineRGB[1] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].combineRgb, INT_GL ,&temp );

		temp = Search(ctx->texEnvCombineAlpha[1] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].combineAlpha, INT_GL ,&temp );

		temp = Search(ctx->mSrcRGB[1][0] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].srcRgb0, INT_GL ,&temp);
		temp = Search(ctx->mSrcRGB[1][1] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].srcRgb1, INT_GL ,&temp);
		temp = Search(ctx->mSrcRGB[1][2] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].srcRgb2, INT_GL ,&temp);

		temp = Search(ctx->mSrcAlpha[1][0] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].srcAlpha0, INT_GL ,&temp);
		temp = Search(ctx->mSrcAlpha[1][1] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].srcAlpha1, INT_GL ,&temp);
		temp = Search(ctx->mSrcAlpha[1][2] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].srcAlpha2, INT_GL ,&temp);

		temp = Search(ctx->mOperandRGB[1][0] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].operandRgb0, INT_GL ,&temp);
		temp = Search(ctx->mOperandRGB[1][1] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].operandRgb1, INT_GL ,&temp);
		temp = Search(ctx->mOperandRGB[1][2] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].operandRgb2, INT_GL ,&temp);

		temp = Search(ctx->mOperandAlpha[1][0] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		//Work around for operandAlpha: always (temp = temp -2) because search results in GL_SRC_ALPHA from
		// operandRgb. In iMap structure check  Combiner RGB operand and Combiner Alpha operand
		temp = temp - 2;
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].operandAlpha0, INT_GL ,&temp);
		temp = Search(ctx->mOperandAlpha[1][1] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		temp = temp -2;
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].operandAlpha1, INT_GL ,&temp);
		temp = Search(ctx->mOperandAlpha[1][2] , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		temp = temp -2;
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].operandAlpha2, INT_GL ,&temp);

		//ctx->mRGBScale[i] = (float)1.0;
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].rgbScale, FLOAT_GL ,&ctx->mRGBScale[1]);

		//ctx->mAlphaScale[i] = 1.0;
		SetUniform(pUnLoc->UNIFORM_LOC_TexEnv[1].alphaScale, FLOAT_GL ,&ctx->mAlphaScale[1]);

	}

	// User defined Clip Plane
	temp = 0;
	SetUniform(pUnLoc->UNIFORM_LOC_UserClippingEnabled, BOOL_GL, &temp);
	if (ctx->cflags[0] || ctx->cflags[1] || ctx->cflags[2] ||ctx->cflags[3] || ctx->cflags[4] || ctx->cflags[5])
	{
		temp = 1;
		SetUniform(pUnLoc->UNIFORM_LOC_UserClippingEnabled, BOOL_GL, &temp);

		temp = ctx->cflags[0];
		SetUniform(pUnLoc->UNIFORM_LOC_ClipPlaneEnabled0, BOOL_GL, &temp);
		if (temp)
		{
			SetUniform(pUnLoc->UNIFORM_LOC_ClipPlane0, VEC4, ctx->clipPlanes[0]);
		}

		temp = ctx->cflags[1];
		SetUniform(pUnLoc->UNIFORM_LOC_ClipPlaneEnabled1, BOOL_GL, &temp);
		if (temp)
		{
			SetUniform(pUnLoc->UNIFORM_LOC_ClipPlane1, VEC4, ctx->clipPlanes[1]);
		}

		temp = ctx->cflags[2];
		SetUniform(pUnLoc->UNIFORM_LOC_ClipPlaneEnabled2, BOOL_GL, &temp);
		if (temp)
		{
			SetUniform(pUnLoc->UNIFORM_LOC_ClipPlane2, VEC4, ctx->clipPlanes[2]);
		}

		temp = ctx->cflags[3];
		SetUniform(pUnLoc->UNIFORM_LOC_ClipPlaneEnabled3, BOOL_GL, &temp);
		if (temp)
		{
			SetUniform(pUnLoc->UNIFORM_LOC_ClipPlane3, VEC4, ctx->clipPlanes[3]);
		}

		temp = ctx->cflags[4];
		SetUniform(pUnLoc->UNIFORM_LOC_ClipPlaneEnabled4, BOOL_GL, &temp);
		if (temp)
		{
			SetUniform(pUnLoc->UNIFORM_LOC_ClipPlane4, VEC4, ctx->clipPlanes[4]);
		}

		temp = ctx->cflags[5];
		SetUniform(pUnLoc->UNIFORM_LOC_ClipPlaneEnabled5, BOOL_GL, &temp);
		if (temp)
		{
			SetUniform(pUnLoc->UNIFORM_LOC_ClipPlane5, VEC4, ctx->clipPlanes[5]);
		}

	}

	// Fog state
	tnf[0] = ctx->flags & F_FOG;
	SetUniform(pUnLoc->UNIFORM_LOC_FogEnabled, BOOL_GL ,&tnf[0] );
	if(tnf[0]){
		SetUniform(pUnLoc->UNIFORM_LOC_Fog_density, FLOAT_GL ,&ctx->fogDensity );
		SetUniform(pUnLoc->UNIFORM_LOC_Fog_end, FLOAT_GL ,&ctx->fogEnd);
		SetUniform(pUnLoc->UNIFORM_LOC_Fog_color, VEC4 ,ctx->fogColor );

		store = 1.0/(ctx->fogEnd - ctx->fogStart);
		SetUniform(pUnLoc->UNIFORM_LOC_Fog_scale, FLOAT_GL ,&store );
		temp = Search(ctx->fogMode , iMap , sizeof(iMap)/sizeof(iMap[0]) );
		SetUniform(pUnLoc->UNIFORM_LOC_Fog_mode, INT_GL ,&temp );
	}
	
	
	
//	gettimeofday(&__end,NULL);
//	LOGE("UniformsInit completed in: %5.5f ms",__test_tval(__start,__end));
}


#define GET_VERT_FUNC GetMaxVertexS
#define IND_TYPE GLushort
#include "maxVert.inc"
unsigned int GetMaxVertexS(int count , GLushort *ind, int *pmin);
#define GET_VERT_FUNC GetMaxVertexB
#define IND_TYPE GLubyte
#include "maxVert.inc"
unsigned int GetMaxVertexB(int count , GLubyte *ind, int *pmin);

GL_API void GL_APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
//	gettimeofday(&__start,NULL);
	int index;
	GET_GLES_CONTEXT

//	//DBGFUNCTION_INIT("glDrawElements ")
//	//RETAILMSG(1,(TEXT("glDrawElements  \r\n")));

//	////GL11LOGMSG(LOG_GLES11_API,("glDrawElements(0x%x\t0x%x\t0x%x\t0x%x)\n",mode,count,type,indices));

//	CHECK_GL_ENUM(((type !=GL_UNSIGNED_SHORT ) &&(type !=GL_UNSIGNED_BYTE) ))
	ctx->mode = mode ;
	ctx->count = count ;
	ctx->type = type;
	if (!indices)
	ctx->indices = NULL;
	ctx->isIndexed = 1 ;

	ctx->pointEnable = 0;
	if(mode == GL_POINTS)
	{
		ctx->pointEnable = 1;
		if (!(ctx->pointSizeArrayEnable))
		OGL2_glVertexAttrib1f ( ATTRIB_LOC_PointSize, ctx->pointSize);
	}
	else
	{
		ctx->flagspt &= ~F_POINT_SPRITE_OES;
	}

	//LOGE(">>>>>>>>>>>>>>>>glDrawElements HW<<<<<<<<<<<<<<<<<");

	setShaderSelectInfo();


	index = selectShaderIndex(ctx);

	//	LOGMSG(("Selecting shader %u",index));

	if(index != ctx->prevIndex)
	{
	OGL2_glUseProgram(ctx->progInfo[index].programId);
	ctx->prevIndex=index;
	}
	
	InitMatrices(ctx);
	UniformsInit(index);

	if(ctx->isFlatShading == GL_TRUE){
		OGL2_glVaryingInterpolationEXP("FrontColor",GL_FLAT);

	}

	OGL2_glDrawElements (mode,  count,  type, indices);

	if(ctx->isFlatShading == GL_TRUE){
		OGL2_glVaryingInterpolationEXP("FrontColor",GL_SMOOTH);

	}
//	//DBGFUNCTION_EXIT("glDrawElements ")
//	gettimeofday(&__end,NULL);
//	LOGE("glDrawElements completed in: %5.5f ms",__test_tval(__start,__end));
}

GLuint selectShader()
{
	GLuint index;
	GET_GLES_CONTEXT_RETURN
	//set the current state
	setShaderSelectInfo() ;

	ctx->prevTextureMode[0] = ctx->textureMode[0];
	ctx->prevTextureMode[1] = ctx->textureMode[1];
	ctx->prevNumLightUsed = ctx->numLightUsed ;
	ctx->prevFogshaderMode = ctx->fogshaderMode;
	ctx->prevIsMipmapEnabled[0] = ctx->isMipmapEnabled[0];
	ctx->prevIsMipmapEnabled[1] = ctx->isMipmapEnabled[1];
	ctx->prevUserClipEnabled = ctx->userClipEnabled;
	ctx->prevMatrixPaletteEnabled = ctx->matrixPaletteEnabled;
	ctx->prevPointSpriteEnabled[0] = ctx->pointSpriteEnabled[0];
	ctx->prevPointSpriteEnabled[1] = ctx->pointSpriteEnabled[1];
	ctx->prevColorAsUniform = ctx->colorAsUniform;
	ctx->prevPointEnable  = ctx->pointEnable;
	ctx->prevEnableDrawTexture = ctx->enableDrawTexture;
	ctx->uniformsDirtyState.index = GL_TRUE;

	index = ctx->prevIndex = selectShaderIndex(ctx);

	OGL2_glUseProgram(ctx->progInfo[index].programId);
	return index;

}

GL_API void GL_APIENTRY glFinish (void)
{
	////GL11LOGMSG(LOG_GLES11_API,("\n=================glFinish()=================\n"));

	OGL2_glFinish();

	//DBGFUNCTION_EXIT("glFinish ")
}

GL_API void GL_APIENTRY glFlush (void)
{

	////GL11LOGMSG(LOG_GLES11_API,("glFlush()\n"));

	OGL2_glFlush();

	//DBGFUNCTION_EXIT("glFlush ")
}

void FreeState(GLState *ctx)
{
	//  free(ctx);

	//DBGFUNCTION_EXIT("FreeState")
}

void CopyToFloat(void *dst, void * src, int count )
{
	GLfloat *fdst = (GLfloat *)dst ;
	GLfloat *fsrc = (GLfloat *)src;
	switch( count  )
	{
	case 4:
		fdst[3] = fsrc[3] ;
	case 3:
		fdst[2] = fsrc[2] ;
	case 2:
		fdst[1] = fsrc[1] ;
	case 1:
		fdst[0] = fsrc[0] ;
	default:
		break ;
	}
}

void CopyToFixed(void *dst, void * src, int count )
{
	GLfixed*fdst = (GLfixed *)dst ;
	GLfloat *fsrc = (GLfloat *)src;
	switch( count  )
	{
	case 4:
		fdst[3] = FLOAT_TO_FIXED(fsrc[3]) ;
	case 3:
		fdst[2] = FLOAT_TO_FIXED(fsrc[2] );
	case 2:
		fdst[1] = FLOAT_TO_FIXED(fsrc[1] );
	case 1:
		fdst[0] = FLOAT_TO_FIXED(fsrc[0] );
	default:
		break ;
	}
}

void CopyToInt(void *dst, void * src, int count )
{
	GLint *fdst = (GLint *)dst ;
	GLfloat *fsrc = (GLfloat *)src;
	switch( count  )
	{
	case 4:
		fdst[3] = (GLint) (fsrc[3]) ;
	case 3:
		fdst[2] = (GLint) (fsrc[2] );
	case 2:
		fdst[1] = (GLint) (fsrc[1] );
	case 1:
		fdst[0] = (GLint) (fsrc[0] );
	default:
		break ;
	}
}

void CopyToBool(void *dst, void * src, int count )
{
	GLboolean *fdst = (GLboolean  *)dst ;
	GLfloat *fsrc = (GLfloat *)src;
	switch( count  )
	{
	case 4:
		fdst[3] =(fsrc[3] ? GL_TRUE : GL_FALSE )  ;
	case 3:
		fdst[2] =(fsrc[2] ? GL_TRUE : GL_FALSE )  ;
	case 2:
		fdst[1] =(fsrc[1] ? GL_TRUE : GL_FALSE )  ;
	case 1:
		fdst[0] =(fsrc[0] ? GL_TRUE : GL_FALSE )  ;
	default:
		break ;
	}
}

void CopyToFloatS(void *dst, GLfloat src, int count )
{
	GLfloat *fdst = (GLfloat *)dst ;
	fdst[0] = src ;
}

void CopyToFixedS(void *dst, GLfloat src, int count )
{
	GLfixed*fdst = (GLfixed *)dst ;
	fdst[0] = FLOAT_TO_FIXED(src );
}

void CopyToIntS(void *dst, GLfloat src, int count )
{
	GLint *fdst = (GLint *)dst ;
	fdst[0] = src ;
}

void CopyToBoolS(void *dst, GLfloat src, int count )
{
	GLboolean *fdst = (GLboolean  *)dst ;
	fdst[0] = (src ? GL_TRUE : GL_FALSE )  ;
}

void CopyToFloatE(void *dst, GLint src, int count )
{
	GLfloat *fdst = (GLfloat *)dst ;
	fdst[0] = src ;
}

void CopyToFixedE(void *dst, GLint src, int count )
{
	GLfixed*fdst = (GLfixed *)dst ;
	fdst[0] = src ;
}

void CopyToIntE(void *dst, GLint src, int count )
{
	GLint *fdst = (GLint *)dst ;
	fdst[0] = src ;
}

void CopyToBoolE(void *dst, GLint src, int count )
{
	GLboolean *fdst = (GLboolean  *)dst ;
	fdst[0] = src ;
}

void GetStateAttrib ( GLState *ctx, GLenum pname , GLint index , pfnCopyData copy, pfnCopyDataEnum  copye , pfnCopyDataVal  copys , GLvoid *params )
{
	int i , j ;
	float tmp[4];
	int tmpi[4];
	if ( index ==GL_TEXTURE_ENV )
	{
		switch (pname)
		{
		case GL_TEXTURE_ENV_MODE:
			copye(params, ctx->texEnvMode[ctx->activeTexture] , 1 ) ;
			break ;
		case GL_COMBINE_RGB:
			copys(params, ctx->texEnvCombineRGB[ctx->activeTexture] , 1 ) ;
			break ;
		case GL_COMBINE_ALPHA:
			copys(params, ctx->texEnvCombineAlpha[ctx->activeTexture] , 1 ) ;
			break ;
		case GL_TEXTURE_ENV_COLOR:
			copy(params, ctx->texEnvColor[ctx->activeTexture] , 4 ) ;
			break ;
		case GL_SRC0_RGB:
			copye(params, ctx->mSrcRGB[ctx->activeTexture][0] , 1 ) ;
			break ;
		case GL_SRC1_RGB:
			copye(params, ctx->mSrcRGB[ctx->activeTexture][1] , 1 ) ;
			break ;
		case GL_SRC2_RGB:
			copye(params, ctx->mSrcRGB[ctx->activeTexture][2] , 1 ) ;
			break ;
		case GL_SRC0_ALPHA:
			copye(params, ctx->mSrcAlpha[ctx->activeTexture][0] , 1 ) ;
			break ;
		case GL_SRC1_ALPHA:
			copye(params, ctx->mSrcAlpha[ctx->activeTexture][1] , 1 ) ;
			break ;
		case GL_SRC2_ALPHA:
			copye(params, ctx->mSrcAlpha[ctx->activeTexture][2] , 1 ) ;
			break ;
		case GL_OPERAND0_RGB:
			copye(params, ctx->mOperandRGB[ctx->activeTexture][0] , 1 ) ;
			break ;
		case GL_OPERAND1_RGB:
			copye(params, ctx->mOperandRGB[ctx->activeTexture][1] , 1 ) ;
			break ;
		case GL_OPERAND2_RGB:
			copye(params, ctx->mOperandRGB[ctx->activeTexture][2] , 1 ) ;
			break ;
		case GL_OPERAND0_ALPHA:
			copye(params, ctx->mOperandAlpha[ctx->activeTexture][0]  , 1 ) ;
			break ;
		case GL_OPERAND1_ALPHA:
			copye(params, ctx->mOperandAlpha[ctx->activeTexture][1] , 1 ) ;
			break ;
		case GL_OPERAND2_ALPHA:
			copye(params, ctx->mOperandAlpha[ctx->activeTexture][2]  , 1 ) ;
			break ;
		case GL_RGB_SCALE:
			copys(params, ctx->mRGBScale[ctx->activeTexture] , 1 ) ;
			break ;
		case GL_ALPHA_SCALE:
			copys(params, ctx->mAlphaScale[ctx->activeTexture] , 1 ) ;
			break ;
		}
	}
	else if (index == GL_POINT_SPRITE_OES)
	{
		switch (pname)
		{
		case GL_COORD_REPLACE_OES :
			copys(params, ctx->texReplaceCoord[ctx->activeTexture] , 1 ) ;
			break;
			default :
			break;
		}
	}
	else
	{
		switch (pname)
		{
		case GL_GENERATE_MIPMAP_HINT:
			OGL2_glGetIntegerv(GL_GENERATE_MIPMAP_HINT,params);
			break ;
		case GL_FOG_HINT:
			copye(params, ctx->hFog, 1 ) ;
			break ;
		case GL_LINE_SMOOTH_HINT:
			copye(params, ctx->hLineSmooth, 1 ) ;
			break ;
		case GL_POINT_SMOOTH_HINT:
			copye(params, ctx->hPointSmooth, 1 ) ;
			break ;
		case GL_PERSPECTIVE_CORRECTION_HINT:
			copye(params, ctx->hPerspective, 1 ) ;
			break ;
		case GL_FOG:
			copye(params, (ctx->flags & F_FOG )?GL_TRUE: GL_FALSE , 1 ) ;
			break ;
		case GL_FOG_START                    :
			copys(params, ctx->fogStart, 1 ) ;
			break ;
		case GL_FOG_END                    :
			copys(params, ctx->fogEnd, 1 ) ;
			break ;
		case GL_FOG_MODE                    :
			copye(params, ctx->fogMode, 1 ) ;
			break ;
		case GL_FOG_COLOR                    :
			copy(params, ctx->fogColor, 4) ;
			break ;
		case GL_FOG_DENSITY                    :
			copys(params, ctx->fogDensity , 1 ) ;
			break ;
		case GL_POINT_SIZE_MIN:
			copys(params, ctx->pointSizeMin , 1 ) ;
			break ;
		case GL_POINT_SIZE_MAX:
			copys(params, ctx->pointSizeMax , 1 ) ;
			break ;
		case GL_POINT_FADE_THRESHOLD_SIZE:
			copys(params, ctx->pointFadeThresh , 1 ) ;
			break ;
		case GL_POINT_DISTANCE_ATTENUATION:
			copy(params, ctx->pointDistAtten , 3 ) ;
			break ;
		case GL_LIGHT_MODEL_TWO_SIDE:
			copys(params, ctx->tbs , 1 ) ;
			break ;
		case GL_LIGHT_MODEL_AMBIENT:
			copy(params, ctx->acs, 4 ) ;
			break ;
		case GL_ACTIVE_TEXTURE:
			OGL2_glGetIntegerv(GL_ACTIVE_TEXTURE,tmpi);
			copys(params, (tmpi[0] + GL_TEXTURE0), 1 ) ;
			break ;
		case GL_CLIENT_ACTIVE_TEXTURE:
			copys(params, ctx->clientActiveTex + GL_TEXTURE0, 1 ) ;
			break ;
		case GL_CURRENT_COLOR                  :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Color, GL_CURRENT_VERTEX_ATTRIB, params);
			//    copy(params, ctx->color, 4 ) ;
			break ;
		case GL_CURRENT_NORMAL                 :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Normal, GL_CURRENT_VERTEX_ATTRIB, params);
			//    copy(params, ctx->normal, 3 ) ;
			break ;
		case GL_CURRENT_TEXTURE_COORDS         :
			if (ctx->activeTexture == 0)
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_MultiTexCoord[0], GL_CURRENT_VERTEX_ATTRIB, params);
			else if (ctx->activeTexture == 1)
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_MultiTexCoord[1], GL_CURRENT_VERTEX_ATTRIB, params);
			//    copy(params, ctx->texCoords[ctx->activeTexture], 4 ) ;
			break ;
		case GL_POINT_SIZE                     :
			//  OGL2_glGetFloatv(GL_POINT_SIZE,params); //pointsize is now sent as vertex attribute to FIMG.
			copys(params, ctx->pointSize, 1);
			break ;
		case GL_LINE_WIDTH                     :
			OGL2_glGetFloatv(GL_LINE_WIDTH,params);
			break ;
		case GL_CULL_FACE_MODE                 :
			OGL2_glGetIntegerv(GL_CULL_FACE_MODE,params);
			break ;
		case GL_FRONT_FACE                     :
			OGL2_glGetIntegerv(GL_FRONT_FACE,params);
			break ;
		case GL_SHADE_MODEL                    :
			copye(params, ctx->shadeModel, 1 ) ;
			break ;
		case GL_DEPTH_RANGE                    :
			OGL2_glGetFloatv(GL_DEPTH_RANGE,params);
			break ;
		case GL_DEPTH_WRITEMASK                :
			OGL2_glGetBooleanv(GL_DEPTH_WRITEMASK,params);
			break ;
		case GL_DEPTH_CLEAR_VALUE              :
			//        OGL2_glGetIntegerv(GL_DEPTH_CLEAR_VALUE,params);
			OGL2_glGetFloatv(GL_DEPTH_CLEAR_VALUE,params);
			break ;
		case GL_DEPTH_FUNC                     :
			OGL2_glGetIntegerv(GL_DEPTH_FUNC,params);
			break ;
		case GL_STENCIL_CLEAR_VALUE            :
			OGL2_glGetIntegerv(GL_STENCIL_CLEAR_VALUE,params);
			break ;
		case GL_STENCIL_FUNC                   :
			OGL2_glGetIntegerv(GL_STENCIL_FUNC,params);
			break ;
		case GL_STENCIL_VALUE_MASK      :
			OGL2_glGetIntegerv(GL_STENCIL_VALUE_MASK,params);
			break ;
		case GL_STENCIL_FAIL                   :
			OGL2_glGetIntegerv(GL_STENCIL_FAIL,params);
			break ;
		case GL_STENCIL_PASS_DEPTH_FAIL        :
			OGL2_glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL,params);
			break ;
		case GL_STENCIL_PASS_DEPTH_PASS        :
			OGL2_glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS,params);
			break ;
		case GL_STENCIL_REF                    :
			OGL2_glGetIntegerv(GL_STENCIL_REF,params);
			break ;
		case GL_STENCIL_WRITEMASK              :
			OGL2_glGetIntegerv(GL_STENCIL_WRITEMASK,params);
			break ;
		case GL_ALPHA_TEST_FUNC                :
			OGL2_glGetIntegerv(GL_ALPHA_TEST_FUNC,params);
			break ;
		case GL_ALPHA_TEST_REF                 :
			OGL2_glGetIntegerv(GL_ALPHA_TEST_REF,params);
			break ;
		case GL_BLEND_DST                      :
			OGL2_glGetIntegerv(GL_BLEND_DST_RGB,params);
			break ;
		case GL_BLEND_SRC                      :
			OGL2_glGetIntegerv(GL_BLEND_SRC_RGB,params);
			break ;
		case GL_LOGIC_OP_MODE                  :
			OGL2_glGetIntegerv(GL_LOGIC_OP_MODE,params);
			break ;
		case GL_SCISSOR_BOX                    :
			OGL2_glGetIntegerv(GL_SCISSOR_BOX,params);
			break ;
		case GL_SCISSOR_TEST                   :
			copye(params,  (ctx->flags & F_SCISSOR_TEST)? GL_TRUE : GL_FALSE , 1 ) ;
			break ;
		case GL_COLOR_CLEAR_VALUE              :
			OGL2_glGetFloatv(GL_COLOR_CLEAR_VALUE,params);
			break ;
		case GL_COLOR_WRITEMASK                :
			OGL2_glGetBooleanv(GL_COLOR_WRITEMASK,params);
			break ;
		case GL_UNPACK_ALIGNMENT               :
			OGL2_glGetIntegerv(GL_UNPACK_ALIGNMENT,params);
			break ;
		case GL_PACK_ALIGNMENT                 :
			OGL2_glGetIntegerv(GL_PACK_ALIGNMENT,params);
			break ;
		case GL_POLYGON_OFFSET_UNITS           :
			OGL2_glGetFloatv(GL_POLYGON_OFFSET_UNITS,params);
			break ;
		case GL_POLYGON_OFFSET_FACTOR          :
			OGL2_glGetFloatv(GL_POLYGON_OFFSET_FACTOR,params);
			break ;
		case GL_TEXTURE_BINDING_2D             :
			OGL2_glGetIntegerv(GL_TEXTURE_BINDING_2D,params);
			//    copys(params,ctx->textureList.textureBindings[ctx->activeTexture], 1 ) ;
			break ;
			//ATTRIBUTE ARRAY SIZE
		case GL_VERTEX_ARRAY_SIZE              :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Vertex, GL_VERTEX_ATTRIB_ARRAY_SIZE, tmp);
			copys(params,(int ) tmp[0], 1 ) ;
			break ;
		case GL_COLOR_ARRAY_SIZE               :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Color, GL_VERTEX_ATTRIB_ARRAY_SIZE, tmp);
			copys(params,(int ) tmp[0], 1 ) ;
			break ;
		case GL_TEXTURE_COORD_ARRAY_SIZE       :
			if (ctx->clientActiveTex == 0)
			{
				OGL2_glGetVertexAttribfv(ATTRIB_LOC_MultiTexCoord[0], GL_VERTEX_ATTRIB_ARRAY_SIZE, tmp);
				copys(params,(int ) tmp[0], 1 ) ;
			}
			else if (ctx->clientActiveTex == 1)
			{
				OGL2_glGetVertexAttribfv(ATTRIB_LOC_MultiTexCoord[1], GL_VERTEX_ATTRIB_ARRAY_SIZE, tmp);
				copys(params,(int ) tmp[0], 1 ) ;
			}
			break ;
		case GL_MATRIX_INDEX_ARRAY_SIZE_OES   :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_MatrixIndices, GL_VERTEX_ATTRIB_ARRAY_SIZE, tmp);
			copys(params,(int ) tmp[0], 1 ) ;
			break;
		case GL_WEIGHT_ARRAY_SIZE_OES     :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Weights, GL_VERTEX_ATTRIB_ARRAY_SIZE, tmp);
			copys(params,(int ) tmp[0], 1 ) ;
			break;
			//ATTRIBUTE ARRAY TYPE
		case GL_VERTEX_ARRAY_TYPE              :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Vertex, GL_VERTEX_ATTRIB_ARRAY_TYPE, tmp);
			copye(params,(int)tmp[0], 1 ) ;
			break ;
		case GL_POINT_SIZE_ARRAY_TYPE_OES              :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_PointSize, GL_VERTEX_ATTRIB_ARRAY_TYPE, tmp);
			copye(params,(int)tmp[0], 1 ) ;
			break ;
		case GL_NORMAL_ARRAY_TYPE              :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Normal, GL_VERTEX_ATTRIB_ARRAY_TYPE, tmp);
			copye(params,(int)tmp[0], 1 ) ;
			break ;
		case GL_COLOR_ARRAY_TYPE               :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Color, GL_VERTEX_ATTRIB_ARRAY_TYPE, tmp);
			copye(params,(int)tmp[0], 1 ) ;
			break ;
		case GL_TEXTURE_COORD_ARRAY_TYPE       :
			if (ctx->clientActiveTex == 0)
			{
				OGL2_glGetVertexAttribfv(ATTRIB_LOC_MultiTexCoord[0], GL_VERTEX_ATTRIB_ARRAY_TYPE, tmp);
				copye(params,(int)tmp[0], 1 ) ;
			}
			else if (ctx->clientActiveTex == 1)
			{
				OGL2_glGetVertexAttribfv(ATTRIB_LOC_MultiTexCoord[1], GL_VERTEX_ATTRIB_ARRAY_TYPE, tmp);
				copye(params,(int)tmp[0], 1 ) ;
			}
			break ;
		case GL_MATRIX_INDEX_ARRAY_TYPE_OES   :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_MatrixIndices, GL_VERTEX_ATTRIB_ARRAY_TYPE, tmp);
			copye(params,(int)tmp[0], 1 ) ;
			break;
		case GL_WEIGHT_ARRAY_TYPE_OES     :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Weights, GL_VERTEX_ATTRIB_ARRAY_TYPE, tmp);
			copye(params,(int)tmp[0], 1 ) ;
			break;
			//ATTRIBUTE ARRAY STRIDE
		case GL_VERTEX_ARRAY_STRIDE            :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Vertex, GL_VERTEX_ATTRIB_ARRAY_STRIDE, tmp);
			copys(params,(int)tmp[0], 1 ) ;
			break ;
		case GL_POINT_SIZE_ARRAY_STRIDE_OES              :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_PointSize, GL_VERTEX_ATTRIB_ARRAY_STRIDE, tmp);
			copys(params,(int)tmp[0], 1 ) ;
			break ;
		case GL_NORMAL_ARRAY_STRIDE            :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Normal, GL_VERTEX_ATTRIB_ARRAY_STRIDE, tmp);
			copys(params,(int)tmp[0], 1 ) ;
			break ;
		case GL_COLOR_ARRAY_STRIDE             :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Color, GL_VERTEX_ATTRIB_ARRAY_STRIDE, tmp);
			copys(params,(int)tmp[0], 1 ) ;
			break ;
		case GL_TEXTURE_COORD_ARRAY_STRIDE     :
			if (ctx->clientActiveTex == 0)
			{
				OGL2_glGetVertexAttribfv(ATTRIB_LOC_MultiTexCoord[0], GL_VERTEX_ATTRIB_ARRAY_STRIDE, tmp);
				copys(params,(int)tmp[0], 1 ) ;
			}
			else if (ctx->clientActiveTex == 1)
			{
				OGL2_glGetVertexAttribfv(ATTRIB_LOC_MultiTexCoord[1], GL_VERTEX_ATTRIB_ARRAY_STRIDE, tmp);
				copys(params,(int)tmp[0], 1 ) ;
			}
			break ;
		case GL_MATRIX_INDEX_ARRAY_STRIDE_OES   :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_MatrixIndices, GL_VERTEX_ATTRIB_ARRAY_STRIDE, tmp);
			copys(params,(int)tmp[0], 1 ) ;
			break;
		case GL_WEIGHT_ARRAY_STRIDE_OES       :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Weights, GL_VERTEX_ATTRIB_ARRAY_STRIDE, tmp);
			copys(params,(int)tmp[0], 1 ) ;
			break;
			//ATTRIBUTE ARRAY BUFFER BINDING
		case GL_ARRAY_BUFFER_BINDING           :
			OGL2_glGetIntegerv(GL_ARRAY_BUFFER_BINDING, params);
			break ;
		case GL_ELEMENT_ARRAY_BUFFER_BINDING :
			OGL2_glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, params);
			break ;
		case GL_VERTEX_ARRAY_BUFFER_BINDING   :
			OGL2_glGetVertexAttribiv(ATTRIB_LOC_Vertex, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, params);
			break ;
		case GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES   :
			OGL2_glGetVertexAttribiv(ATTRIB_LOC_PointSize, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, params);
			break ;
		case GL_NORMAL_ARRAY_BUFFER_BINDING  :
			OGL2_glGetVertexAttribiv(ATTRIB_LOC_Normal, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, params);
			break ;
		case GL_COLOR_ARRAY_BUFFER_BINDING    :
			OGL2_glGetVertexAttribiv(ATTRIB_LOC_Color, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, params);
			break ;
		case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING :
			if (ctx->clientActiveTex == 0)
			{
				OGL2_glGetVertexAttribiv(ATTRIB_LOC_MultiTexCoord[0], GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, params);
			}
			else if  (ctx->clientActiveTex == 1)
			{
				OGL2_glGetVertexAttribiv(ATTRIB_LOC_MultiTexCoord[1], GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, params);
			}
			break ;
		case GL_MATRIX_INDEX_ARRAY_BUFFER_BINDING_OES :
			OGL2_glGetVertexAttribiv(ATTRIB_LOC_MatrixIndices, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, params);
			break;
		case GL_WEIGHT_ARRAY_BUFFER_BINDING_OES :
			OGL2_glGetVertexAttribiv(ATTRIB_LOC_Weights, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, params);
			break;
			//ATTRIBUTE ARRAY POINTER
		case GL_VERTEX_ARRAY_POINTER           :
			OGL2_glGetVertexAttribPointerv(ATTRIB_LOC_Vertex, GL_VERTEX_ATTRIB_ARRAY_POINTER, params);
			break ;
		case GL_POINT_SIZE_ARRAY_POINTER_OES           :
			OGL2_glGetVertexAttribPointerv(ATTRIB_LOC_PointSize, GL_VERTEX_ATTRIB_ARRAY_POINTER, params);
			break ;
		case GL_NORMAL_ARRAY_POINTER           :
			OGL2_glGetVertexAttribPointerv(ATTRIB_LOC_Normal, GL_VERTEX_ATTRIB_ARRAY_POINTER, params);
			break ;
		case GL_COLOR_ARRAY_POINTER            :
			OGL2_glGetVertexAttribPointerv(ATTRIB_LOC_Color, GL_VERTEX_ATTRIB_ARRAY_POINTER, params);
			break ;
		case GL_TEXTURE_COORD_ARRAY_POINTER    :
			if (ctx->clientActiveTex == 0)
			{
				OGL2_glGetVertexAttribPointerv(ATTRIB_LOC_MultiTexCoord[0], GL_VERTEX_ATTRIB_ARRAY_POINTER, params);
			}
			else if  (ctx->clientActiveTex == 1)
			{
				OGL2_glGetVertexAttribPointerv(ATTRIB_LOC_MultiTexCoord[1], GL_VERTEX_ATTRIB_ARRAY_POINTER, params);
			}
			break ;
		case GL_MATRIX_INDEX_ARRAY_POINTER_OES  :
			OGL2_glGetVertexAttribPointerv(ATTRIB_LOC_MatrixIndices, GL_VERTEX_ATTRIB_ARRAY_POINTER, params);
			break;
		case GL_WEIGHT_ARRAY_POINTER_OES    :
			OGL2_glGetVertexAttribPointerv(ATTRIB_LOC_Weights, GL_VERTEX_ATTRIB_ARRAY_POINTER, params);
			break;
			//ATTRIBUTE ARRAY ENABLED/DISABLED
		case GL_VERTEX_ARRAY           :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Vertex, GL_VERTEX_ATTRIB_ARRAY_ENABLED, tmp);
			copye(params, (int)tmp[0], 1 ) ;
			break ;
		case GL_POINT_SIZE_ARRAY_OES           :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_PointSize, GL_VERTEX_ATTRIB_ARRAY_ENABLED, tmp);
			copye(params, (int)tmp[0], 1 ) ;
			break ;
		case GL_NORMAL_ARRAY          :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Normal, GL_VERTEX_ATTRIB_ARRAY_ENABLED, tmp);
			copye(params, (int)tmp[0], 1 ) ;
			break ;
		case GL_COLOR_ARRAY           :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Color, GL_VERTEX_ATTRIB_ARRAY_ENABLED, tmp);
			copye(params, (int)tmp[0], 1 ) ;
			break ;
		case GL_TEXTURE_COORD_ARRAY    :
			if (ctx->clientActiveTex == 0)
			{
				OGL2_glGetVertexAttribfv(ATTRIB_LOC_MultiTexCoord[0], GL_VERTEX_ATTRIB_ARRAY_ENABLED, tmp);
				copye(params, (int)tmp[0], 1 ) ;
			}
			else if (ctx->clientActiveTex == 1)
			{
				OGL2_glGetVertexAttribfv(ATTRIB_LOC_MultiTexCoord[1], GL_VERTEX_ATTRIB_ARRAY_ENABLED, tmp);
				copye(params, (int)tmp[0], 1 ) ;
			}
			break ;
		case GL_MATRIX_INDEX_ARRAY_OES    :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_MatrixIndices, GL_VERTEX_ATTRIB_ARRAY_ENABLED, tmp);
			copye(params, (int)tmp[0], 1 ) ;
			break;
		case GL_WEIGHT_ARRAY_OES      :
			OGL2_glGetVertexAttribfv(ATTRIB_LOC_Weights, GL_VERTEX_ATTRIB_ARRAY_ENABLED, tmp);
			copye(params, (int)tmp[0], 1 ) ;
			break;
		case GL_SAMPLE_BUFFERS                 :
			copys(params,C_SAMPLE_BUFFERS, 1 ) ;
			break ;
		case GL_SAMPLES                        :
			copys(params,C_SAMPLES, 1 ) ;
			break ;
		case GL_SAMPLE_COVERAGE_VALUE          :
			OGL2_glGetFloatv(GL_SAMPLE_COVERAGE_VALUE,params);
			break ;
		case GL_SAMPLE_COVERAGE_INVERT         :
			OGL2_glGetBooleanv(GL_SAMPLE_COVERAGE_INVERT,params);
			break ;
		case GL_MAX_CLIP_PLANES :
			copys(params, C_MAX_CLIP_PLANES, 1 ) ;
			break ;
		case GL_MAX_PALETTE_MATRICES_OES:
			copys(params, C_MAX_PALETTE_MATRICES_OES, 1 ) ;
			break ;
		case GL_MAX_VERTEX_UNITS_OES:
			copys(params, C_MAX_VERTEX_UNITS_OES, 1 ) ;
			break ;
		case GL_VIEWPORT:
			OGL2_glGetIntegerv(GL_VIEWPORT,params);
			break;
		case GL_MATRIX_MODE:
			if ( GL_MATRIX_PALETTE_OES == ctx->matrixMode)
			{
				copye(params, GL_MATRIX_PALETTE_OES, 1 ) ;
			}
			else
			{
				copye(params, ctx->matrixMode, 1 ) ;
			}
			break ;
		case GL_ALIASED_LINE_WIDTH_RANGE :
			//OGL2_glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE,params);
			//break;
		case GL_ALIASED_POINT_SIZE_RANGE :
		case GL_SMOOTH_POINT_SIZE_RANGE :
		case GL_SMOOTH_LINE_WIDTH_RANGE :
			{
				GLfloat tmp[2] ;
				tmp[0] =  C_ALIASED_POINT_SIZE_MIN;
				tmp[1] =  C_ALIASED_POINT_SIZE_MAX;
				copy(params,tmp, 2 ) ;
			}
			break;
		case GL_ALPHA_BITS:
			OGL2_glGetIntegerv(GL_ALPHA_BITS, params);
			return;
		case GL_BLUE_BITS:
			OGL2_glGetIntegerv(GL_BLUE_BITS, params);
			return;
		case GL_DEPTH_BITS :
			OGL2_glGetIntegerv(GL_DEPTH_BITS, params);
			break ;
		case GL_GREEN_BITS:
			OGL2_glGetIntegerv(GL_GREEN_BITS, params);
			return;
		case GL_RED_BITS:
			OGL2_glGetIntegerv(GL_RED_BITS, params);
			return;
		case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES :
			copye(params, GL_RGBA , 1);
			break ;
		case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES :
			copye(params, GL_UNSIGNED_BYTE , 1);
			break ;
		case GL_MAX_ELEMENTS_INDICES :
		case GL_MAX_ELEMENTS_VERTICES :
			copys(params, 0xFFFF - 1 , 1);
			break ;
		case GL_MAX_LIGHTS:
			copys(params, C_MAX_LIGHTS , 1);
			return;
		case GL_MODELVIEW_STACK_DEPTH:
			copys(params, ctx->stacks[MAT_MODELVIEW].pos + 1, 1);
			break ;
		case GL_PROJECTION_STACK_DEPTH :
			copys(params, ctx->stacks[MAT_PROJECTION].pos + 1, 1);
			break ;
		case GL_TEXTURE_STACK_DEPTH :
			copys(params, ctx->stacks[MAT_TEXTURE0 + ctx->activeTexture].pos + 1, 1);
			break ;
		case GL_MAX_MODELVIEW_STACK_DEPTH :
		case GL_MAX_PROJECTION_STACK_DEPTH :
		case GL_MAX_TEXTURE_STACK_DEPTH :
			copys(params, C_MAX_MODELVIEW_STACK_DEPTH/2 , 1);
			break ;
		case GL_MAX_TEXTURE_SIZE :
			OGL2_glGetIntegerv(GL_MAX_TEXTURE_SIZE,params);
			break ;
		case GL_MAX_TEXTURE_UNITS :
			copys(params, C_MAX_TEXTURE_UNITS , 1);
			break ;
		case GL_MAX_VIEWPORT_DIMS:
			{
				GLfloat tmp[2] ;
				OGL2_glGetFloatv(GL_MAX_VIEWPORT_DIMS, tmp);
				copy(params,tmp, 2 ) ;
			}
			return;
		case GL_NUM_COMPRESSED_TEXTURE_FORMATS :
			OGL2_glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS,params);
			break ;
		case GL_STENCIL_BITS :
			OGL2_glGetIntegerv(GL_STENCIL_BITS, params);
			break ;
		case GL_SUBPIXEL_BITS :
			copys(params, C_SUBPIXEL_BITS , 1);
			break ;
		case GL_PROJECTION_MATRIX:
			{
				GLfloat *data = ctx->stacks[MAT_PROJECTION].matrices[ctx->stacks[MAT_PROJECTION].pos ]->data;
				for (  i = 0 ; i < 4 ; i++)
				{
					for (  j = 0 ; j < 4 ; j++)
					{
						copys ((GLfloat *)params + i + j * 4  , (data[i *4 + j ]) , 1 ) ;
					}
				}
				break;
			}
		case GL_MODELVIEW_MATRIX:
			{
				GLfloat *data = ctx->stacks[MAT_MODELVIEW].matrices[ctx->stacks[MAT_MODELVIEW].pos ]->data;
				for (  i = 0 ; i < 4 ; i++)
				{
					for (  j = 0 ; j < 4 ; j++)
					{
						copys ((GLfloat *)params + i + j * 4 , (data[i *4 + j ]) , 1 ) ;
					}
				}
				break;
			}
		case GL_TEXTURE_MATRIX:
			{
				GLfloat *data = ctx->stacks[MAT_TEXTURE0+ctx->activeTexture ].matrices[ctx->stacks[MAT_TEXTURE0 + ctx->activeTexture].pos]->data;
				for (  i = 0 ; i < 4 ; i++)
				{
					for (  j = 0 ; j < 4 ; j++)
					{
						copys ((GLfloat *)params + i + j * 4  , (data[i *4 + j ]) , 1 ) ;
					}
				}
				break;
			}
		default:
			break;
		}
	}
}
GLState * CreateStateGLES11(GLES11Context sharedctx)
{

#ifndef MULTI_CONTEXT
	if(sharedctx != GLES_NO_CONTEXT){
		printf("ERROR: Sharing texture object withour using multi-context \n");
		sharedctx = GLES_NO_CONTEXT;
	}
#endif

	//Create the 2.0 state first so any 2.0 function calls can be made during the creation of this 1.1 state
	//However currently the gl20 context is attached to this state at the end of this function.
	GLES2Context sharedctx20 = GLES_NO_CONTEXT;

	//get 2.0 context of the sharedctx11 and pass it to gles2createContext
	if (sharedctx != GLES_NO_CONTEXT) {
		sharedctx20  =    ( (GLState *)sharedctx)->gl20context ;
		assert(sharedctx20 != 0 && "The gl20 context should not be null");
	}

	GLES2Context ctxgl20 = GLES2CreateContext(sharedctx20);

	//SET_GLES20_STATE(ctxgl20) //ramdev


	GLState *ctx = (GLState *)ALLOC_MEM(sizeof(GLState));
	int i = 0 ;
	if ( !ctx )
	{
		return ctx ;
	}

	memset(ctx, 0 , sizeof(GLState));
	SET_GLES11_STATE(ctx )
	ctx->gl20context = ctxgl20;

	ctx->version = 11;
	ctx->magicNumber = 0xDEADBEEF;
	ctx->id      = gGL11ContextIDCounter++;

	InitializeMatrixState(ctx) ;
	ctx->color [0] = ctx->color [1]  = ctx->color [2]  = ctx->color [3] = 1.0f ;


	ctx->matrixMode = GL_MODELVIEW ;

	ctx->pointSize = 1.0f;

	InitMaterial(&ctx->frontNback) ;
	ctx->acs[0] = ctx->acs[1] = ctx->acs[2] = 0.2f;
	ctx->acs[3] = 1.0f ;

	for ( i = 0 ; i < C_MAX_LIGHTS ; i++ )
	{
		InitLight(i, &ctx->lights[i]);
	}
	for ( i = 0 ; i < C_MAX_TEXTURE_UNITS ; i++)
	{
		ctx->mOperandRGB[i][0] = GL_SRC_COLOR ;
		ctx->mOperandRGB[i][1] = GL_SRC_COLOR ;
		//ctx->mOperandRGB[i][2] = GL_SRC_ALPHA;
		//ctx->mOperandAlpha[i][0] = GL_SRC_ALPHA ;
		//ctx->mOperandAlpha[i][1] = GL_SRC_ALPHA ;
		//ctx->mOperandAlpha[i][2] = GL_SRC_ALPHA ;

		ctx->mSrcRGB[i][0] = GL_TEXTURE;
		ctx->mSrcRGB[i][1] = GL_PREVIOUS;
		//ctx->mSrcRGB[i][2] = GL_CONSTANT;
		//ctx->mSrcAlpha[i][0] = GL_TEXTURE ;
		//ctx->mSrcAlpha[i][1] = GL_PREVIOUS ;
		//ctx->mSrcAlpha[i][2] = GL_CONSTANT ;

		ctx->texEnvCombineRGB[i] = GL_MODULATE ;
		//ctx->texEnvCombineAlpha[i] = GL_MODULATE ;

		ctx->texEnvMode[i] = GL_MODULATE; // imbumoh
	}

	ctx->wa.lightChange = 1 ;

	//ctx->texEnvMode[i] = GL_MODULATE;

	ctx->fogStart = 0.0f ;
	ctx->fogEnd = 1.0f;
	ctx->fogDensity = 1.0f ;
	ctx->fogMode = GL_EXP ;

	ctx->pointEnable = 0;
	ctx->pointSizeArrayEnable = 0;
	ctx->pointSizeMin = C_ALIASED_POINT_SIZE_MIN ;
	ctx->pointSizeMax = C_ALIASED_POINT_SIZE_MAX ;
	ctx->pointFadeThresh = C_POINT_FADE_THRESHOLD ;
	ctx->pointDistAtten [0] = C_POINT_DISTANCE_ATTENUATION ;
	ctx->pointDistAtten [1] = C_POINT_DISTANCE_ATTENUATION ;
	ctx->pointDistAtten [2] = C_POINT_DISTANCE_ATTENUATION ;

	//added
	//ctx->isTexCoordDirty = 0;
	ctx->isFlatShading = GL_FALSE;
	ctx->shadeModel = GL_SMOOTH;

	ctx->textureMode[0] = 0;
	ctx->textureMode[1] = 0;
	ctx->numLightUsed = 0;
	ctx->fogshaderMode = 0;
	ctx->userClipEnabled = 0;
	ctx->matrixPaletteEnabled = 0;
	ctx->pointSpriteEnabled[0] = 0;
	ctx->pointSpriteEnabled[1] = 0;
	ctx->isMipmapEnabled[0] = 0;
	ctx->isMipmapEnabled[1] = 0;
	ctx->colorAsUniform = 0;

	ctx->progInfo = (progUnifInfo* )malloc(sizeof(progUnifInfo) * totalShaderCount);
	memset(ctx->progInfo, 0x0 , sizeof(progUnifInfo) * totalShaderCount);

	ShaderInit();
	//resetShaderInfo();

	// memset(ctx->texCoordInfo, 0, sizeof(ctx->texCoordInfo));
	ctx->extension = (unsigned char *)"GL_OES_matrix_palette GL_OES_draw_texture GL_OES_framebuffer_object GL_OES_matrix_get GL_ARB_texture_non_power_of_two";

	//point sprite flag is disabled  in GLES 2.0 library
	OGL2_glDisable(GL_POINT_SPRITE_OES);

#ifdef MATRIX_DIRTY_STATE
	ctx->mvprojDirtyState = GL_TRUE;
#endif
	
	ctx->colorEnabled=0;
	ctx->glcolor=0;
	ctx->AlphaorLuminance=0;
	//cashing shader programs
	loadShaders();
	return ctx;
}

void DestroyStateGLES11(GLState * ctx)
{

	int i =0;

	for(i=0; i < totalShaderCount ; i++){
		FREE_MEM_CHKRST(ctx->progInfo[i].pUniformLocation);

	}

	FREE_MEM_CHKRST(ctx->progInfo);

	DeInitializeMatrixState(ctx);

	FREE_MEM_CHKRST(ctx);
}
