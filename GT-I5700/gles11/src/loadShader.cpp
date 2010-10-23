#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "gles.h"
#include "attribs.h"
#include "OGL2Func_for_gles11.h"
#include "shaderValues.h"
#include "shaderSplitting.h"
#include "includeShaders.h"




#define LOAD_SOURCE_FILES

#ifdef __cplusplus
extern "C" {
#endif

const int ATTRIB_LOC_Vertex = 0;
const int ATTRIB_LOC_Color = 1;
const int ATTRIB_LOC_Normal = 2;
const int ATTRIB_LOC_MultiTexCoord[2]={3,4};
const int ATTRIB_LOC_PointSize = 5;
const int ATTRIB_LOC_MatrixIndices = 6;
const int ATTRIB_LOC_Weights = 7;

#ifdef __cplusplus
}
#endif

	void matIdentity(float *m)
{
	for (int i=0; i<16; i++)
		m[i] = (i%5==0) ? 1.0f : 0.0f;  //The first and every fifth element after that is 1.0 other are 0.0
}

extern "C" int selectShaderIndex(GLState *ctx)
{
	#if 0
	return 2;
	#endif
	bool tex=0;
	int index=2;
	float texUnity[16];
	matIdentity(texUnity);	

	//-------------------------------------------------------------------------------------------
	if (ctx->matrixPaletteEnabled)
	{
//		LOGE("Selecting shader %u",15);
		return 15;
	}

	if (ctx->flags & F_LIGHTING)
	{
		if (!(ctx->flags & F_TEXTURE_2D_UNIT0) && !(ctx->flags & F_FOG))
		{
			index=14;
		}
		else if (!(ctx->flags & F_TEXTURE_2D_UNIT1) && !(ctx->flags & F_FOG) && (ctx->texEnvMode[0] == TEX_ENV_MODE_MODULATE))
		{
			index=13;
		}
		else
		{
			index=12;
		}
//		LOGE("Selecting shader %u",index);
		return index;
	}


	if(ctx->flags & F_TEXTURE_2D_UNIT1)
	{
		if(memcmp((ctx->stacks[MAT_TEXTURE0].matrices[ctx->stacks[MAT_TEXTURE0].pos ])->data, texUnity, sizeof(texUnity))
		||memcmp((ctx->stacks[MAT_TEXTURE1].matrices[ctx->stacks[MAT_TEXTURE1].pos ])->data, texUnity, sizeof(texUnity)))
		{
			if (ctx->texEnvMode[1] == TEX_ENV_MODE_ADD)
			index=11;
			else
			index=10;
		}
		else
		{
			if (ctx->texEnvMode[1] == TEX_ENV_MODE_ADD)
			index=9;
			else
			index=8;
		}
		
	}
	else
	if(ctx->flags & F_TEXTURE_2D_UNIT0)
	{
		tex=memcmp((ctx->stacks[MAT_TEXTURE0].matrices[ctx->stacks[MAT_TEXTURE0].pos ])->data, texUnity, sizeof(texUnity))!=0;

		if (ctx->colorEnabled == 1)
		{
			if (ctx->mSrcRGB[0][1] == TEX_ENV_COMBINER_SRC_CONSTANT)
			{
				index=6;
				tex = false;
			}
			else if (ctx->flags & F_FOG)
			{
				switch (ctx->fogMode)
				{
				case GL_LINEAR:	index=16;	break;
				case GL_EXP:	index=17;	break;
				case GL_EXP2:	index=18;	break;
				}

			}
			else{
				tex ? index=7 : index=5;
			}
		}else
		if(tex)
		index=4;
		else
		{
			if (ctx->glcolor == 1)// && ctx->AlphaorLuminance == 0)
			index=3;
			else
			index=2;
		}
	}
	else
	if (ctx->colorEnabled == 1)
	index=1;
	else
	index=0;
	
//	LOGE("Selecting shader %u",index);
	return index;
}


static int UniformLocation(int program, const char *name)
{
  int location = 0;

  location = OGL2_glGetUniformLocation(program, name);

  return location;
}


static void GetUniformsLocation(int index, unsigned int program)
{
  //caching the uniform location and setting the pointer in the array
  GET_GLES_CONTEXT
  UniformLoc* pULoc  = ctx->progInfo[index].pUniformLocation = (UniformLoc *)malloc(sizeof(UniformLoc));
  if(pULoc== NULL){
    LOGE("unable to allocate memeory");
    return;
  }

    pULoc->UNIFORM_LOC_ModelViewMatrix = UniformLocation(program, "ModelViewMatrix");
    pULoc->UNIFORM_LOC_ModelViewProjectionMatrix = UniformLocation(program, "ModelViewProjectionMatrix");
    pULoc->UNIFORM_LOC_TextureMatrix0 = UniformLocation(program, "TextureMatrix[0]");
    pULoc->UNIFORM_LOC_TextureMatrix1 = UniformLocation(program, "TextureMatrix[1]");
    pULoc->UNIFORM_LOC_NormalMatrix = UniformLocation(program, "NormalMatrix");

    pULoc->UNIFORM_LOC_colorValue = UniformLocation(program, "Colorunif");

    pULoc->UNIFORM_LOC_paletteMatrices0 = UniformLocation(program, "paletteMatrices[0]");
    pULoc->UNIFORM_LOC_paletteMatrices1 = UniformLocation(program, "paletteMatrices[1]");
    pULoc->UNIFORM_LOC_paletteMatrices2 = UniformLocation(program, "paletteMatrices[2]");
    pULoc->UNIFORM_LOC_paletteMatrices3 = UniformLocation(program, "paletteMatrices[3]");
    pULoc->UNIFORM_LOC_paletteMatrices4 = UniformLocation(program, "paletteMatrices[4]");
    pULoc->UNIFORM_LOC_paletteMatrices5 = UniformLocation(program, "paletteMatrices[5]");
    pULoc->UNIFORM_LOC_paletteMatrices6 = UniformLocation(program, "paletteMatrices[6]");
    pULoc->UNIFORM_LOC_paletteMatrices7 = UniformLocation(program, "paletteMatrices[7]");
    pULoc->UNIFORM_LOC_paletteMatrices8 = UniformLocation(program, "paletteMatrices[8]");
    pULoc->UNIFORM_LOC_paletteMatrices9 = UniformLocation(program, "paletteMatrices[9]");

    pULoc->UNIFORM_LOC_paletteNormalMatrices0 = UniformLocation(program, "paletteNormalMatrices[0]");
    pULoc->UNIFORM_LOC_paletteNormalMatrices1 = UniformLocation(program, "paletteNormalMatrices[1]");
    pULoc->UNIFORM_LOC_paletteNormalMatrices2 = UniformLocation(program, "paletteNormalMatrices[2]");
    pULoc->UNIFORM_LOC_paletteNormalMatrices3 = UniformLocation(program, "paletteNormalMatrices[3]");
    pULoc->UNIFORM_LOC_paletteNormalMatrices4 = UniformLocation(program, "paletteNormalMatrices[4]");
    pULoc->UNIFORM_LOC_paletteNormalMatrices5 = UniformLocation(program, "paletteNormalMatrices[5]");
    pULoc->UNIFORM_LOC_paletteNormalMatrices6 = UniformLocation(program, "paletteNormalMatrices[6]");
    pULoc->UNIFORM_LOC_paletteNormalMatrices7 = UniformLocation(program, "paletteNormalMatrices[7]");
    pULoc->UNIFORM_LOC_paletteNormalMatrices8 = UniformLocation(program, "paletteNormalMatrices[8]");
    pULoc->UNIFORM_LOC_paletteNormalMatrices9 = UniformLocation(program, "paletteNormalMatrices[9]");

    pULoc->UNIFORM_LOC_matrixPaletteEnable = UniformLocation(program, "matrixPaletteEnable");
    pULoc->UNIFORM_LOC_noOfVertexUnit = UniformLocation(program, "noOfVertexUnit");


    pULoc->UNIFORM_LOC_PointEnabled = UniformLocation(program, "PointEnabled");
    pULoc->UNIFORM_LOC_Point_size = UniformLocation(program, "Point.size");
    pULoc->UNIFORM_LOC_Point_sizeMin = UniformLocation(program, "Point.sizeMin");
    pULoc->UNIFORM_LOC_Point_sizeMax = UniformLocation(program, "Point.sizeMax");
    pULoc->UNIFORM_LOC_Point_fadeThresholdSize = UniformLocation(program, "Point.fadeThresholdSize");
    pULoc->UNIFORM_LOC_Point_distanceConstantAttenuation = UniformLocation(program, "Point.distanceConstantAttenuation");
    pULoc->UNIFORM_LOC_Point_distanceLinearAttenuation = UniformLocation(program, "Point.distanceLinearAttenuation");
    pULoc->UNIFORM_LOC_Point_distanceQuadraticAttenuation = UniformLocation(program, "Point.distanceQuadraticAttenuation");

    pULoc->UNIFORM_LOC_PointSpriteEnabled0 =  UniformLocation(program, "PointSpriteEnabled[0]");
    pULoc->UNIFORM_LOC_PointSpriteEnabled1 =  UniformLocation(program, "PointSpriteEnabled[1]");


    pULoc->UNIFORM_LOC_FrontNBackMaterial_emission = UniformLocation(program, "FrontNBackMaterial.emission");
    pULoc->UNIFORM_LOC_FrontNBackMaterial_ambient = UniformLocation(program, "FrontNBackMaterial.ambient");
    pULoc->UNIFORM_LOC_FrontNBackMaterial_diffuse = UniformLocation(program, "FrontNBackMaterial.diffuse");
    pULoc->UNIFORM_LOC_FrontNBackMaterial_specular = UniformLocation(program, "FrontNBackMaterial.specular");
    pULoc->UNIFORM_LOC_FrontNBackMaterial_shininess  = UniformLocation(program, "FrontNBackMaterial.specular");
    pULoc->UNIFORM_LOC_colorMaterialEnabled = UniformLocation(program, "colorMaterialEnabled");


    pULoc->UNIFORM_LOC_FrontNBackMaterial_shininess = UniformLocation(program, "FrontNBackMaterial.shininess");
    pULoc->UNIFORM_LOC_LightSource[0].ambient = UniformLocation(program, "LightSource[0].ambient");
    pULoc->UNIFORM_LOC_LightSource[0].diffuse = UniformLocation(program, "LightSource[0].diffuse");
    pULoc->UNIFORM_LOC_LightSource[0].specular = UniformLocation(program, "LightSource[0].specular");
    pULoc->UNIFORM_LOC_LightSource[0].position = UniformLocation(program, "LightSource[0].position");
    pULoc->UNIFORM_LOC_LightSource[0].halfVector = UniformLocation(program, "LightSource[0].halfVector");
    pULoc->UNIFORM_LOC_LightSource[0].spotDirection = UniformLocation(program, "LightSource[0].spotDirection");
    pULoc->UNIFORM_LOC_LightSource[0].spotExponent = UniformLocation(program, "LightSource[0].spotExponent");
    pULoc->UNIFORM_LOC_LightSource[0].spotCutoff = UniformLocation(program, "LightSource[0].spotCutoff");
    pULoc->UNIFORM_LOC_LightSource[0].spotCosCutoff = UniformLocation(program, "LightSource[0].spotCosCutoff");
    pULoc->UNIFORM_LOC_LightSource[0].constantAttenuation = UniformLocation(program, "LightSource[0].constantAttenuation");
    pULoc->UNIFORM_LOC_LightSource[0].linearAttenuation = UniformLocation(program, "LightSource[0].linearAttenuation");
    pULoc->UNIFORM_LOC_LightSource[0].quadraticAttenuation = UniformLocation(program, "LightSource[0].quadraticAttenuation");
    pULoc->UNIFORM_LOC_LightSource[0].isDirectional = UniformLocation(program, "LightSource[0].isDirectional");
    pULoc->UNIFORM_LOC_LightSource[1].ambient = UniformLocation(program, "LightSource[1].ambient");
    pULoc->UNIFORM_LOC_LightSource[1].diffuse = UniformLocation(program, "LightSource[1].diffuse");
    pULoc->UNIFORM_LOC_LightSource[1].specular = UniformLocation(program, "LightSource[1].specular");
    pULoc->UNIFORM_LOC_LightSource[1].position = UniformLocation(program, "LightSource[1].position");
    pULoc->UNIFORM_LOC_LightSource[1].halfVector = UniformLocation(program, "LightSource[1].halfVector");
    pULoc->UNIFORM_LOC_LightSource[1].spotDirection = UniformLocation(program, "LightSource[1].spotDirection");
    pULoc->UNIFORM_LOC_LightSource[1].spotExponent = UniformLocation(program, "LightSource[1].spotExponent");
    pULoc->UNIFORM_LOC_LightSource[1].spotCutoff = UniformLocation(program, "LightSource[1].spotCutoff");
    pULoc->UNIFORM_LOC_LightSource[1].spotCosCutoff = UniformLocation(program, "LightSource[1].spotCosCutoff");
    pULoc->UNIFORM_LOC_LightSource[1].constantAttenuation = UniformLocation(program, "LightSource[1].constantAttenuation");
    pULoc->UNIFORM_LOC_LightSource[1].linearAttenuation = UniformLocation(program, "LightSource[1].linearAttenuation");
    pULoc->UNIFORM_LOC_LightSource[1].quadraticAttenuation = UniformLocation(program, "LightSource[1].quadraticAttenuation");
    pULoc->UNIFORM_LOC_LightSource[1].isDirectional = UniformLocation(program, "LightSource[1].isDirectional");
    pULoc->UNIFORM_LOC_LightSource[2].ambient = UniformLocation(program, "LightSource[2].ambient");
    pULoc->UNIFORM_LOC_LightSource[2].diffuse = UniformLocation(program, "LightSource[2].diffuse");
    pULoc->UNIFORM_LOC_LightSource[2].specular = UniformLocation(program, "LightSource[2].specular");
    pULoc->UNIFORM_LOC_LightSource[2].position = UniformLocation(program, "LightSource[2].position");
    pULoc->UNIFORM_LOC_LightSource[2].halfVector = UniformLocation(program, "LightSource[2].halfVector");
    pULoc->UNIFORM_LOC_LightSource[2].spotDirection = UniformLocation(program, "LightSource[2].spotDirection");
    pULoc->UNIFORM_LOC_LightSource[2].spotExponent = UniformLocation(program, "LightSource[2].spotExponent");
    pULoc->UNIFORM_LOC_LightSource[2].spotCutoff = UniformLocation(program, "LightSource[2].spotCutoff");
    pULoc->UNIFORM_LOC_LightSource[2].spotCosCutoff = UniformLocation(program, "LightSource[2].spotCosCutoff");
    pULoc->UNIFORM_LOC_LightSource[2].constantAttenuation = UniformLocation(program, "LightSource[2].constantAttenuation");
    pULoc->UNIFORM_LOC_LightSource[2].linearAttenuation = UniformLocation(program, "LightSource[2].linearAttenuation");
    pULoc->UNIFORM_LOC_LightSource[2].quadraticAttenuation = UniformLocation(program, "LightSource[2].quadraticAttenuation");
    pULoc->UNIFORM_LOC_LightSource[2].isDirectional = UniformLocation(program, "LightSource[2].isDirectional");
    pULoc->UNIFORM_LOC_LightSource[3].ambient = UniformLocation(program, "LightSource[3].ambient");
    pULoc->UNIFORM_LOC_LightSource[3].diffuse = UniformLocation(program, "LightSource[3].diffuse");
    pULoc->UNIFORM_LOC_LightSource[3].specular = UniformLocation(program, "LightSource[3].specular");
    pULoc->UNIFORM_LOC_LightSource[3].position = UniformLocation(program, "LightSource[3].position");
    pULoc->UNIFORM_LOC_LightSource[3].halfVector = UniformLocation(program, "LightSource[3].halfVector");
    pULoc->UNIFORM_LOC_LightSource[3].spotDirection = UniformLocation(program, "LightSource[3].spotDirection");
    pULoc->UNIFORM_LOC_LightSource[3].spotExponent = UniformLocation(program, "LightSource[3].spotExponent");
    pULoc->UNIFORM_LOC_LightSource[3].spotCutoff = UniformLocation(program, "LightSource[3].spotCutoff");
    pULoc->UNIFORM_LOC_LightSource[3].spotCosCutoff = UniformLocation(program, "LightSource[3].spotCosCutoff");
    pULoc->UNIFORM_LOC_LightSource[3].constantAttenuation = UniformLocation(program, "LightSource[3].constantAttenuation");
    pULoc->UNIFORM_LOC_LightSource[3].linearAttenuation = UniformLocation(program, "LightSource[3].linearAttenuation");
    pULoc->UNIFORM_LOC_LightSource[3].quadraticAttenuation = UniformLocation(program, "LightSource[3].quadraticAttenuation");
    pULoc->UNIFORM_LOC_LightSource[3].isDirectional = UniformLocation(program, "LightSource[3].isDirectional");
    pULoc->UNIFORM_LOC_LightSource[4].ambient = UniformLocation(program, "LightSource[4].ambient");
    pULoc->UNIFORM_LOC_LightSource[4].diffuse = UniformLocation(program, "LightSource[4].diffuse");
    pULoc->UNIFORM_LOC_LightSource[4].specular = UniformLocation(program, "LightSource[4].specular");
    pULoc->UNIFORM_LOC_LightSource[4].position = UniformLocation(program, "LightSource[4].position");
    pULoc->UNIFORM_LOC_LightSource[4].halfVector = UniformLocation(program, "LightSource[4].halfVector");
    pULoc->UNIFORM_LOC_LightSource[4].spotDirection = UniformLocation(program, "LightSource[4].spotDirection");
    pULoc->UNIFORM_LOC_LightSource[4].spotExponent = UniformLocation(program, "LightSource[4].spotExponent");
    pULoc->UNIFORM_LOC_LightSource[4].spotCutoff = UniformLocation(program, "LightSource[4].spotCutoff");
    pULoc->UNIFORM_LOC_LightSource[4].spotCosCutoff = UniformLocation(program, "LightSource[4].spotCosCutoff");
    pULoc->UNIFORM_LOC_LightSource[4].constantAttenuation = UniformLocation(program, "LightSource[4].constantAttenuation");
    pULoc->UNIFORM_LOC_LightSource[4].linearAttenuation = UniformLocation(program, "LightSource[4].linearAttenuation");
    pULoc->UNIFORM_LOC_LightSource[4].quadraticAttenuation = UniformLocation(program, "LightSource[4].quadraticAttenuation");
    pULoc->UNIFORM_LOC_LightSource[4].isDirectional = UniformLocation(program, "LightSource[4].isDirectional");
    pULoc->UNIFORM_LOC_LightSource[5].ambient = UniformLocation(program, "LightSource[5].ambient");
    pULoc->UNIFORM_LOC_LightSource[5].diffuse = UniformLocation(program, "LightSource[5].diffuse");
    pULoc->UNIFORM_LOC_LightSource[5].specular = UniformLocation(program, "LightSource[5].specular");
    pULoc->UNIFORM_LOC_LightSource[5].position = UniformLocation(program, "LightSource[5].position");
    pULoc->UNIFORM_LOC_LightSource[5].halfVector = UniformLocation(program, "LightSource[5].halfVector");
    pULoc->UNIFORM_LOC_LightSource[5].spotDirection = UniformLocation(program, "LightSource[5].spotDirection");
    pULoc->UNIFORM_LOC_LightSource[5].spotExponent = UniformLocation(program, "LightSource[5].spotExponent");
    pULoc->UNIFORM_LOC_LightSource[5].spotCutoff = UniformLocation(program, "LightSource[5].spotCutoff");
    pULoc->UNIFORM_LOC_LightSource[5].spotCosCutoff = UniformLocation(program, "LightSource[5].spotCosCutoff");
    pULoc->UNIFORM_LOC_LightSource[5].constantAttenuation = UniformLocation(program, "LightSource[5].constantAttenuation");
    pULoc->UNIFORM_LOC_LightSource[5].linearAttenuation = UniformLocation(program, "LightSource[5].linearAttenuation");
    pULoc->UNIFORM_LOC_LightSource[5].quadraticAttenuation = UniformLocation(program, "LightSource[5].quadraticAttenuation");
    pULoc->UNIFORM_LOC_LightSource[5].isDirectional = UniformLocation(program, "LightSource[5].isDirectional");
    pULoc->UNIFORM_LOC_LightSource[6].ambient = UniformLocation(program, "LightSource[6].ambient");
    pULoc->UNIFORM_LOC_LightSource[6].diffuse = UniformLocation(program, "LightSource[6].diffuse");
    pULoc->UNIFORM_LOC_LightSource[6].specular = UniformLocation(program, "LightSource[6].specular");
    pULoc->UNIFORM_LOC_LightSource[6].position = UniformLocation(program, "LightSource[6].position");
    pULoc->UNIFORM_LOC_LightSource[6].halfVector = UniformLocation(program, "LightSource[6].halfVector");
    pULoc->UNIFORM_LOC_LightSource[6].spotDirection = UniformLocation(program, "LightSource[6].spotDirection");
    pULoc->UNIFORM_LOC_LightSource[6].spotExponent = UniformLocation(program, "LightSource[6].spotExponent");
    pULoc->UNIFORM_LOC_LightSource[6].spotCutoff = UniformLocation(program, "LightSource[6].spotCutoff");
    pULoc->UNIFORM_LOC_LightSource[6].spotCosCutoff = UniformLocation(program, "LightSource[6].spotCosCutoff");
    pULoc->UNIFORM_LOC_LightSource[6].constantAttenuation = UniformLocation(program, "LightSource[6].constantAttenuation");
    pULoc->UNIFORM_LOC_LightSource[6].linearAttenuation = UniformLocation(program, "LightSource[6].linearAttenuation");
    pULoc->UNIFORM_LOC_LightSource[6].quadraticAttenuation = UniformLocation(program, "LightSource[6].quadraticAttenuation");
    pULoc->UNIFORM_LOC_LightSource[6].isDirectional = UniformLocation(program, "LightSource[6].isDirectional");
    pULoc->UNIFORM_LOC_LightSource[7].ambient = UniformLocation(program, "LightSource[7].ambient");
    pULoc->UNIFORM_LOC_LightSource[7].diffuse = UniformLocation(program, "LightSource[7].diffuse");
    pULoc->UNIFORM_LOC_LightSource[7].specular = UniformLocation(program, "LightSource[7].specular");
    pULoc->UNIFORM_LOC_LightSource[7].position = UniformLocation(program, "LightSource[7].position");
    pULoc->UNIFORM_LOC_LightSource[7].halfVector = UniformLocation(program, "LightSource[7].halfVector");
    pULoc->UNIFORM_LOC_LightSource[7].spotDirection = UniformLocation(program, "LightSource[7].spotDirection");
    pULoc->UNIFORM_LOC_LightSource[7].spotExponent = UniformLocation(program, "LightSource[7].spotExponent");
    pULoc->UNIFORM_LOC_LightSource[7].spotCutoff = UniformLocation(program, "LightSource[7].spotCutoff");
    pULoc->UNIFORM_LOC_LightSource[7].spotCosCutoff = UniformLocation(program, "LightSource[7].spotCosCutoff");
    pULoc->UNIFORM_LOC_LightSource[7].constantAttenuation = UniformLocation(program, "LightSource[7].constantAttenuation");
    pULoc->UNIFORM_LOC_LightSource[7].linearAttenuation = UniformLocation(program, "LightSource[7].linearAttenuation");
    pULoc->UNIFORM_LOC_LightSource[7].quadraticAttenuation = UniformLocation(program, "LightSource[7].quadraticAttenuation");
    pULoc->UNIFORM_LOC_LightSource[7].isDirectional = UniformLocation(program, "LightSource[7].isDirectional");
    pULoc->UNIFORM_LOC_LightModel_ambient = UniformLocation(program, "LightModel.ambient");
    pULoc->UNIFORM_LOC_LightModel_isTwoSided = UniformLocation(program, "LightModel.isTwoSided");
    pULoc->UNIFORM_LOC_LightingEnabled = UniformLocation(program, "LightingEnabled");
    pULoc->UNIFORM_LOC_NumLightUsed = UniformLocation(program, "numLightUsed");
    pULoc->UNIFORM_LOC_FrontNBackLightModelProduct_sceneColor = UniformLocation(program, "FrontNBackLightModelProduct.sceneColor");

    pULoc->UNIFORM_LOC_FrontNBackLightProduct0_ambient = UniformLocation(program, "FrontNBackLightProduct[0].ambient");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct0_diffuse = UniformLocation(program, "FrontNBackLightProduct[0].diffuse");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct0_specular = UniformLocation(program, "FrontNBackLightProduct[0].specular");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct1_ambient = UniformLocation(program, "FrontNBackLightProduct[1].ambient");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct1_diffuse = UniformLocation(program, "FrontNBackLightProduct[1].diffuse");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct1_specular = UniformLocation(program, "FrontNBackLightProduct[1].specular");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct2_ambient = UniformLocation(program, "FrontNBackLightProduct[2].ambient");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct2_diffuse = UniformLocation(program, "FrontNBackLightProduct[2].diffuse");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct2_specular = UniformLocation(program, "FrontNBackLightProduct[2].specular");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct3_ambient = UniformLocation(program, "FrontNBackLightProduct[3].ambient");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct3_diffuse = UniformLocation(program, "FrontNBackLightProduct[3].diffuse");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct3_specular = UniformLocation(program, "FrontNBackLightProduct[3].specular");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct4_ambient = UniformLocation(program, "FrontNBackLightProduct[4].ambient");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct4_diffuse = UniformLocation(program, "FrontNBackLightProduct[4].diffuse");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct4_specular = UniformLocation(program, "FrontNBackLightProduct[4].specular");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct5_ambient = UniformLocation(program, "FrontNBackLightProduct[5].ambient");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct5_diffuse = UniformLocation(program, "FrontNBackLightProduct[5].diffuse");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct5_specular = UniformLocation(program, "FrontNBackLightProduct[5].specular");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct6_ambient = UniformLocation(program, "FrontNBackLightProduct[6].ambient");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct6_diffuse = UniformLocation(program, "FrontNBackLightProduct[6].diffuse");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct6_specular = UniformLocation(program, "FrontNBackLightProduct[6].specular");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct7_ambient = UniformLocation(program, "FrontNBackLightProduct[7].ambient");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct7_diffuse = UniformLocation(program, "FrontNBackLightProduct[7].diffuse");
    pULoc->UNIFORM_LOC_FrontNBackLightProduct7_specular = UniformLocation(program, "FrontNBackLightProduct[7].specular");

    pULoc->UNIFORM_LOC_TexEnv[0].color = UniformLocation(program, "TexEnv[0].color");
    pULoc->UNIFORM_LOC_TexEnv[0].mode = UniformLocation(program, "TexEnv[0].mode");
    pULoc->UNIFORM_LOC_TexEnv[0].combineRgb = UniformLocation(program, "TexEnv[0].combineRgb");
    pULoc->UNIFORM_LOC_TexEnv[0].combineAlpha = UniformLocation(program, "TexEnv[0].combineAlpha");
    pULoc->UNIFORM_LOC_TexEnv[0].srcRgb0 = UniformLocation(program, "TexEnv[0].srcRgb[0]");
    pULoc->UNIFORM_LOC_TexEnv[0].srcRgb1 = UniformLocation(program, "TexEnv[0].srcRgb[1]");
    pULoc->UNIFORM_LOC_TexEnv[0].srcRgb2 = UniformLocation(program, "TexEnv[0].srcRgb[2]");
    pULoc->UNIFORM_LOC_TexEnv[0].srcAlpha0 = UniformLocation(program, "TexEnv[0].srcAlpha[0]");
    pULoc->UNIFORM_LOC_TexEnv[0].srcAlpha1 = UniformLocation(program, "TexEnv[0].srcAlpha[1]");
    pULoc->UNIFORM_LOC_TexEnv[0].srcAlpha2 = UniformLocation(program, "TexEnv[0].srcAlpha[2]");
    pULoc->UNIFORM_LOC_TexEnv[0].operandRgb0 = UniformLocation(program, "TexEnv[0].operandRgb[0]");
    pULoc->UNIFORM_LOC_TexEnv[0].operandRgb1 = UniformLocation(program, "TexEnv[0].operandRgb[1]");
    pULoc->UNIFORM_LOC_TexEnv[0].operandRgb2 = UniformLocation(program, "TexEnv[0].operandRgb[2]");
    pULoc->UNIFORM_LOC_TexEnv[0].operandAlpha0 = UniformLocation(program, "TexEnv[0].operandAlpha[0]");
    pULoc->UNIFORM_LOC_TexEnv[0].operandAlpha1 = UniformLocation(program, "TexEnv[0].operandAlpha[1]");
    pULoc->UNIFORM_LOC_TexEnv[0].operandAlpha2 = UniformLocation(program, "TexEnv[0].operandAlpha[2]");
    pULoc->UNIFORM_LOC_TexEnv[0].rgbScale = UniformLocation(program, "TexEnv[0].rgbScale");
    pULoc->UNIFORM_LOC_TexEnv[0].alphaScale = UniformLocation(program, "TexEnv[0].alphaScale");

    pULoc->UNIFORM_LOC_TexEnv[1].color = UniformLocation(program, "TexEnv[1].color");
    pULoc->UNIFORM_LOC_TexEnv[1].mode = UniformLocation(program, "TexEnv[1].mode");
    pULoc->UNIFORM_LOC_TexEnv[1].combineRgb = UniformLocation(program, "TexEnv[1].combineRgb");
    pULoc->UNIFORM_LOC_TexEnv[1].combineAlpha = UniformLocation(program, "TexEnv[1].combineAlpha");
    pULoc->UNIFORM_LOC_TexEnv[1].srcRgb0 = UniformLocation(program, "TexEnv[1].srcRgb[0]");
    pULoc->UNIFORM_LOC_TexEnv[1].srcRgb1 = UniformLocation(program, "TexEnv[1].srcRgb[1]");
    pULoc->UNIFORM_LOC_TexEnv[1].srcRgb2 = UniformLocation(program, "TexEnv[1].srcRgb[2]");
    pULoc->UNIFORM_LOC_TexEnv[1].srcAlpha0 = UniformLocation(program, "TexEnv[1].srcAlpha[0]");
    pULoc->UNIFORM_LOC_TexEnv[1].srcAlpha1 = UniformLocation(program, "TexEnv[1].srcAlpha[1]");
    pULoc->UNIFORM_LOC_TexEnv[1].srcAlpha2 = UniformLocation(program, "TexEnv[1].srcAlpha[2]");
    pULoc->UNIFORM_LOC_TexEnv[1].operandRgb0 = UniformLocation(program, "TexEnv[1].operandRgb[0]");
    pULoc->UNIFORM_LOC_TexEnv[1].operandRgb1 = UniformLocation(program, "TexEnv[1].operandRgb[1]");
    pULoc->UNIFORM_LOC_TexEnv[1].operandRgb2 = UniformLocation(program, "TexEnv[1].operandRgb[2]");
    pULoc->UNIFORM_LOC_TexEnv[1].operandAlpha0 = UniformLocation(program, "TexEnv[1].operandAlpha[0]");
    pULoc->UNIFORM_LOC_TexEnv[1].operandAlpha1 = UniformLocation(program, "TexEnv[1].operandAlpha[1]");
    pULoc->UNIFORM_LOC_TexEnv[1].operandAlpha2 = UniformLocation(program, "TexEnv[1].operandAlpha[2]");
    pULoc->UNIFORM_LOC_TexEnv[1].rgbScale = UniformLocation(program, "TexEnv[1].rgbScale");
    pULoc->UNIFORM_LOC_TexEnv[1].alphaScale = UniformLocation(program, "TexEnv[1].alphaScale");

    pULoc->UNIFORM_LOC_Texture2DEnabled0 = UniformLocation(program, "Texture2DEnabled[0]");
    pULoc->UNIFORM_LOC_Texture2DEnabled1 = UniformLocation(program, "Texture2DEnabled[1]");

    pULoc->UNIFORM_LOC_FS_alphaTextureEnable0 = UniformLocation(program, "alphaTextureEnable[0]");
    pULoc->UNIFORM_LOC_FS_noAlphaChannelTextureEnable0 = UniformLocation(program, "noAlphaChannelTextureEnable[0]");
    pULoc->UNIFORM_LOC_FS_alphaTextureEnable1 = UniformLocation(program, "alphaTextureEnable[1]");
    pULoc->UNIFORM_LOC_FS_noAlphaChannelTextureEnable1 = UniformLocation(program, "noAlphaChannelTextureEnable[1]");

  // added for multi texture implementation
    pULoc->UNIFORM_LOC_TexImages[0] =  UniformLocation(program, "TexImages0");
    pULoc->UNIFORM_LOC_TexImages[1] =  UniformLocation(program, "TexImages1");

    //user clip plane
    pULoc->UNIFORM_LOC_UserClippingEnabled =  UniformLocation(program, "UserClippingEnabled");
    pULoc->UNIFORM_LOC_ClipPlane0 = UniformLocation(program, "ClipPlane[0]");
    pULoc->UNIFORM_LOC_ClipPlane1 = UniformLocation(program, "ClipPlane[1]");
    pULoc->UNIFORM_LOC_ClipPlane2 = UniformLocation(program, "ClipPlane[2]");
    pULoc->UNIFORM_LOC_ClipPlane3 = UniformLocation(program, "ClipPlane[3]");
    pULoc->UNIFORM_LOC_ClipPlane4 = UniformLocation(program, "ClipPlane[4]");
    pULoc->UNIFORM_LOC_ClipPlane5 = UniformLocation(program, "ClipPlane[5]");
    pULoc->UNIFORM_LOC_ClipPlaneEnabled0 = UniformLocation(program, "ClipPlaneEnabled[0]");
    pULoc->UNIFORM_LOC_ClipPlaneEnabled1 = UniformLocation(program, "ClipPlaneEnabled[1]");
    pULoc->UNIFORM_LOC_ClipPlaneEnabled2 = UniformLocation(program, "ClipPlaneEnabled[2]");
    pULoc->UNIFORM_LOC_ClipPlaneEnabled3 = UniformLocation(program, "ClipPlaneEnabled[3]");
    pULoc->UNIFORM_LOC_ClipPlaneEnabled4 = UniformLocation(program, "ClipPlaneEnabled[4]");
    pULoc->UNIFORM_LOC_ClipPlaneEnabled5 = UniformLocation(program, "ClipPlaneEnabled[5]");


    pULoc->UNIFORM_LOC_Fog_density = UniformLocation(program, "Fog.density");
//    pULoc->UNIFORM_LOC_Fog_start = UniformLocation(program, "Fog.start");
    pULoc->UNIFORM_LOC_Fog_end = UniformLocation(program, "Fog.end");
    pULoc->UNIFORM_LOC_Fog_color = UniformLocation(program, "Fog.color");
    pULoc->UNIFORM_LOC_Fog_scale = UniformLocation(program, "Fog.scale");
    pULoc->UNIFORM_LOC_Fog_mode = UniformLocation(program, "Fog.mode");
    pULoc->UNIFORM_LOC_FogEnabled = UniformLocation(program, "FogEnabled");
	pULoc->UNIFORM_LOC_AlphaorLuminance = UniformLocation(program, "AlphaorLuminance");
}



static void checkGLerror(const char* location)
{
    GLenum err = glGetError();

    const char* errString = 0;
    switch(err)
    {
        case GL_NO_ERROR:           errString = "GL_NO_ERROR"; break;
        case GL_INVALID_ENUM:       errString = "GL_INVALID_ENUM"; break;
        case GL_INVALID_VALUE:      errString = "GL_INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:  errString = "GL_INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:     errString = "GL_STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:    errString = "GL_STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:      errString = "GL_OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: errString = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
        default:                    errString = "UNKNOWN ERROR!!"; break;
    }

    if(err != GL_NO_ERROR)
    {
        LOGE("%s: %s\n", location,errString);
    }
}

static void tswap(float& a, float& b)
{
  float temp;
        temp = a;
        a = b;
        b = temp;
}

extern "C" void matTranspose(float* m)
{
        tswap(m[1],m[4]);       tswap(m[2],m[8]);       tswap(m[3],m[12]);
        tswap(m[6],m[9]);      tswap(m[7],m[13]);       tswap(m[11],m[14]);
}

#ifdef WIN32
static char *textFileRead(const char *fn) {


  FILE *fp;
  char *content = NULL;

  int count=0;

    if (fn != NULL) {
    fp = fopen(fn,"rt");

        if (fp != NULL) {

      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);

      if (count > 0) {
        content = (char *)malloc(sizeof(char) * (count+1));
        count = fread(content,sizeof(char),count,fp);
        content[count] = '\0';
      }
            //printf("SOURCE:%s\n",content);
      fclose(fp);
    }
        else
        {
            LOGE("\nERROR: unable to open file %s for reading\n", fn);
        }
  }

  return content;
}
#endif

extern "C" void loadShaders()
{
	int i;
	GLuint vs=0,fs=0,prog=0;
	static char buff[16000];
	int len;

	GET_GLES_CONTEXT

	//Cashing shader programs?

	for(int i=0;i<totalShaderCount;i++){
	
		vs = OGL2_glCreateShader(GL_VERTEX_SHADER);
		fs = OGL2_glCreateShader(GL_FRAGMENT_SHADER);
		prog = OGL2_glCreateProgram();

		OGL2_glAttachShader(prog, fs);
		checkGLerror("Post fs attach shader");

		OGL2_glAttachShader(prog, vs);
		checkGLerror("Post vs attach shader");

		OGL2_glShaderBinary(1, &fs, (GLenum)0, shaderInfoUnifInfo[i].pFragmentShader, shaderInfoUnifInfo[i].FSLength * 4);
		checkGLerror("Post fs bin load");

		OGL2_glShaderBinary(1, &vs, (GLenum)0, shaderInfoUnifInfo[i].pVertexShader, shaderInfoUnifInfo[i].VSLength * 4);
		checkGLerror("Post vs bin load");

		OGL2_glBindAttribLocation(prog,ATTRIB_LOC_Vertex,"Vertex");
		OGL2_glBindAttribLocation(prog,ATTRIB_LOC_Color,"Color");
		OGL2_glBindAttribLocation(prog,ATTRIB_LOC_Normal,"Normal");
		OGL2_glBindAttribLocation(prog,ATTRIB_LOC_MultiTexCoord[0],"MultiTexCoord0");
		OGL2_glBindAttribLocation(prog,ATTRIB_LOC_MultiTexCoord[1],"MultiTexCoord1");
		OGL2_glBindAttribLocation(prog,ATTRIB_LOC_PointSize,"PointSize");
		OGL2_glBindAttribLocation(prog,ATTRIB_LOC_Weights, "Weights");
		OGL2_glBindAttribLocation(prog,ATTRIB_LOC_MatrixIndices ,"MatrixIndices");

		OGL2_glLinkProgram(prog);
		checkGLerror("Post prog Link");

		OGL2_glUseProgram(prog);
		checkGLerror("set current program");

		//caching the uniform location
		GetUniformsLocation(i, prog);

		//storing the program id
		ctx->progInfo[i].programId = prog;
	}
}
