#ifndef __SHADERSPLITTING_H__
#define __SHADERSPLITTING_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAXCOUNT 10             //max value that the uniform can have

#define MAXSHADER 1000  //maximum no of shader


typedef struct{
        int ambient;
        int diffuse;
        int specular;
        int position;
		int halfVector;

        int spotDirection;
        int spotExponent;
        int spotCutoff;
        int spotCosCutoff;
        int constantAttenuation;
        int linearAttenuation;
        int quadraticAttenuation;
        int isDirectional;

} lightSourceParameters;



typedef struct{
    int color;
        int mode;
        int combineRgb;
        int combineAlpha;
        int srcRgb0;
        int srcRgb1;
        int srcRgb2;
        int srcAlpha0;
        int srcAlpha1;
        int srcAlpha2;
        int operandRgb0;
        int operandRgb1;
        int operandRgb2;
        int operandAlpha0;
        int operandAlpha1;
        int operandAlpha2;
        int rgbScale;
        int alphaScale;
        
}texureEnvParameter;



typedef struct {
        int UNIFORM_LOC_ModelViewMatrix;
        int UNIFORM_LOC_ModelViewProjectionMatrix;
        int UNIFORM_LOC_TextureMatrix0;
        int UNIFORM_LOC_TextureMatrix1;
        int UNIFORM_LOC_NormalMatrix;

//		int UNIFORM_LOC_Color;
        int UNIFORM_LOC_colorValue;
    
        int UNIFORM_LOC_paletteMatrices0;
        int UNIFORM_LOC_paletteMatrices1;
        int UNIFORM_LOC_paletteMatrices2;
        int UNIFORM_LOC_paletteMatrices3;
        int UNIFORM_LOC_paletteMatrices4;
        int UNIFORM_LOC_paletteMatrices5;
        int UNIFORM_LOC_paletteMatrices6;
        int UNIFORM_LOC_paletteMatrices7;
        int UNIFORM_LOC_paletteMatrices8;
        int UNIFORM_LOC_paletteMatrices9;

        int UNIFORM_LOC_paletteNormalMatrices0;
        int UNIFORM_LOC_paletteNormalMatrices1;
        int UNIFORM_LOC_paletteNormalMatrices2;
        int UNIFORM_LOC_paletteNormalMatrices3;
        int UNIFORM_LOC_paletteNormalMatrices4;
        int UNIFORM_LOC_paletteNormalMatrices5;
        int UNIFORM_LOC_paletteNormalMatrices6;
        int UNIFORM_LOC_paletteNormalMatrices7;
        int UNIFORM_LOC_paletteNormalMatrices8;
        int UNIFORM_LOC_paletteNormalMatrices9;
        int UNIFORM_LOC_matrixPaletteEnable;
        int UNIFORM_LOC_noOfVertexUnit;

        
        int UNIFORM_LOC_PointEnabled;
        int UNIFORM_LOC_Point_size;
        int UNIFORM_LOC_Point_sizeMin;
        int UNIFORM_LOC_Point_sizeMax;
        int UNIFORM_LOC_Point_fadeThresholdSize;
        int UNIFORM_LOC_Point_distanceConstantAttenuation;
        int UNIFORM_LOC_Point_distanceLinearAttenuation;
        int UNIFORM_LOC_Point_distanceQuadraticAttenuation;

        int UNIFORM_LOC_PointSpriteEnabled0;
        int UNIFORM_LOC_PointSpriteEnabled1;

        int UNIFORM_LOC_FrontNBackMaterial_emission;

        int UNIFORM_LOC_FrontNBackMaterial_ambient;
        int UNIFORM_LOC_FrontNBackMaterial_diffuse;
        int UNIFORM_LOC_FrontNBackMaterial_specular;
        int UNIFORM_LOC_FrontNBackMaterial_shininess;
        int UNIFORM_LOC_colorMaterialEnabled;

        lightSourceParameters UNIFORM_LOC_LightSource[8];
        
        int UNIFORM_LOC_LightModel_ambient;

        int UNIFORM_LOC_LightModel_isTwoSided;


        int UNIFORM_LOC_LightingEnabled;
        int UNIFORM_LOC_FrontNBackLightModelProduct_sceneColor;
        int UNIFORM_LOC_NumLightUsed;
        

        int UNIFORM_LOC_FrontNBackLightProduct0_ambient;

        int UNIFORM_LOC_FrontNBackLightProduct0_diffuse;

        int UNIFORM_LOC_FrontNBackLightProduct0_specular;

        int UNIFORM_LOC_FrontNBackLightProduct1_ambient;

        int UNIFORM_LOC_FrontNBackLightProduct1_diffuse;

        int UNIFORM_LOC_FrontNBackLightProduct1_specular;

        int UNIFORM_LOC_FrontNBackLightProduct2_ambient;

        int UNIFORM_LOC_FrontNBackLightProduct2_diffuse;

        int UNIFORM_LOC_FrontNBackLightProduct2_specular;

        int UNIFORM_LOC_FrontNBackLightProduct3_ambient;

        int UNIFORM_LOC_FrontNBackLightProduct3_diffuse;

        int UNIFORM_LOC_FrontNBackLightProduct3_specular;

        int UNIFORM_LOC_FrontNBackLightProduct4_ambient;

        int UNIFORM_LOC_FrontNBackLightProduct4_diffuse;

        int UNIFORM_LOC_FrontNBackLightProduct4_specular;

        int UNIFORM_LOC_FrontNBackLightProduct5_ambient;

        int UNIFORM_LOC_FrontNBackLightProduct5_diffuse;

        int UNIFORM_LOC_FrontNBackLightProduct5_specular;

        int UNIFORM_LOC_FrontNBackLightProduct6_ambient;

        int UNIFORM_LOC_FrontNBackLightProduct6_diffuse;

        int UNIFORM_LOC_FrontNBackLightProduct6_specular;

        int UNIFORM_LOC_FrontNBackLightProduct7_ambient;

        int UNIFORM_LOC_FrontNBackLightProduct7_diffuse;

        int UNIFORM_LOC_FrontNBackLightProduct7_specular;

        

        texureEnvParameter UNIFORM_LOC_TexEnv[2] ;
        int UNIFORM_LOC_TexImages[2];
        int UNIFORM_LOC_Texture2DEnabled0;
        int UNIFORM_LOC_Texture2DEnabled1;

        int UNIFORM_LOC_FS_alphaTextureEnable0;
        int UNIFORM_LOC_FS_noAlphaChannelTextureEnable0;
        int UNIFORM_LOC_FS_alphaTextureEnable1;
        int UNIFORM_LOC_FS_noAlphaChannelTextureEnable1;
    
    //user clip plane

        int UNIFORM_LOC_UserClippingEnabled;
        int UNIFORM_LOC_ClipPlane0;
        int UNIFORM_LOC_ClipPlane1;
        int UNIFORM_LOC_ClipPlane2;
        int UNIFORM_LOC_ClipPlane3;
        int UNIFORM_LOC_ClipPlane4;
        int UNIFORM_LOC_ClipPlane5;
        int UNIFORM_LOC_ClipPlaneEnabled0;
        int UNIFORM_LOC_ClipPlaneEnabled1;
        int UNIFORM_LOC_ClipPlaneEnabled2;
        int UNIFORM_LOC_ClipPlaneEnabled3;
        int UNIFORM_LOC_ClipPlaneEnabled4;
        int UNIFORM_LOC_ClipPlaneEnabled5;


        
        int UNIFORM_LOC_Fog_density;
        int UNIFORM_LOC_Fog_start;

        int UNIFORM_LOC_Fog_end;
        int UNIFORM_LOC_Fog_color;
        int UNIFORM_LOC_Fog_scale;
        int UNIFORM_LOC_Fog_mode;
        int UNIFORM_LOC_FogEnabled;
		int UNIFORM_LOC_AlphaorLuminance;
}UniformLoc;



typedef struct uniformNode uniformDecision;

typedef struct _shaderUniformInfo shaderUniformInfo;
typedef struct _progUnifInfo progUnifInfo;


//structure of the array containing info about the shader and uniform

struct _shaderUniformInfo{
        const   void* pFragmentShader;                          //containg the address of the FS to load

        const int   FSLength;

        const void* pVertexShader;                              //containing the address of the VS to load

        const int   VSLength;
        
};

struct _progUnifInfo{
        UniformLoc*  pUniformLocation;          //pointer to the uniform location

        unsigned int   programId;                       //containing progrma id to be set when linked successfully

};

/* array containing info about the shader to use and the uniform location 

corresponding to the differnet shader */



//structure for the shader selection

struct uniformNode {

      unsigned long* pUniformValue;                              //the pointer to the uniform present in the context 

          //int mask;                                                    

          uniformDecision*  uniformInfo[MAXCOUNT];      //pointer to the corresponding uniform value

      int shaderInfo;                           //index to the array containing the shader info                  

};


#ifdef __cplusplus 
}
#endif


#endif
