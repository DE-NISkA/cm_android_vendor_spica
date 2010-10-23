#ifndef __glstate_h__
#define __glstate_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "gles11Context.h"
#include "gles20Context.h"

#define C_TEX_MAX_LEVELS 16
#define MATRIX_DIRTY_STATE

	/*  TEXTURE OBJECT STATE */
	typedef struct TTextureLevel
	{
		GLvoid*  data; 
		GLuint  format ;
		GLuint  type ;
		GLuint  width ;
		GLuint  height ;
	}TextureLevel;

	typedef struct TTextureUnit
	{
		GLenum  minFilter; 
		GLenum  magFilter;
		GLenum  wraps;
		GLenum  wrapt;
		GLboolean genMipmap ; 
	}TextureUnit;

	typedef struct TTextureObject
	{
		GLvoid *data; /* Store the MBX handle here */
		GLuint  index;
		GLuint  format ;
		GLuint  type ;
		GLuint  width ;
		GLuint  height ;
		TextureLevel levels[C_TEX_MAX_LEVELS];
		TextureUnit tu ; 
	}TextureObject;

	typedef struct TTextureList
	{
		GLvoid*         textures ; 
		GLuint          maxTextureIndex ; 
		GLuint          textureBindings[2] ;/* TEXTURE0, TEXTURE1 */
	}TextureList;


	/*  BUFFER OBJECT STATE */

	typedef struct TBufferObject
	{
		GLenum usage ;
		GLenum access; 
		GLuint index ; 
		GLvoid *data ;
		GLsizeiptr size ; 
		GLsizeiptr allocSize ; 
	}BufferObject ;
	typedef enum EPointerType
	{
		VERTEXPTR, NORMALPTR, COLORPTR, MATRIXINDPTR , WEIGHTPTR, TEXCOORDPTR0, TEXCOORDPTR1, POINTARRAY
	}PointerType ; 
	typedef struct TBufferList
	{
		GLvoid*         buffers ; 
		GLuint          maxBufferIndex ; 
		GLuint          bufferBindings[2] ;
		BufferObject*   pointerBindings[8 + C_MAX_TEXTURE_UNITS] ; 
		GLuint          pointerBuffer[8 + C_MAX_TEXTURE_UNITS] ;        
	}BufferList ; 

	/*  BUFFER OBJECT STATE END */
	typedef enum EMatrixState 
	{
		IDENTITY, TRANSLATE, SCALE, ARBITRARY
	}MatrixState ;
	typedef struct TMatrix44 
	{
		GLfloat         data[16];
		GLint           state ; 
	}Matrix44;
	typedef struct TMatrixStack
	{
		Matrix44        *matrices[C_MAX_MODELVIEW_STACK_DEPTH];
		int                     pos ; /* Min 1 matrix required */
	}MatrixStack;
	typedef enum EMatrixType
	{
		MAT_MODELVIEW, MAT_PROJECTION, MAT_TEXTURE0, MAT_TEXTURE1
	}MatrixType ; 
	typedef struct TMaterial 
	{
		GLfloat acm [4];
		GLfloat dcm [4];
		GLfloat scm [4];
		GLfloat ecm [4];
		GLfloat srm ; 

	}Material ;
	typedef struct TLight
	{
		GLfloat acli [4];
		GLfloat dcli [4];
		GLfloat scli [4];
		GLfloat ppli [4];
		GLfloat sdli [3]; 
		GLfloat oppli [4];
		GLfloat osdli [3]; 
		GLfloat srli;
		GLfloat crli;
		GLfloat k0i ;
		GLfloat k1i ;
		GLfloat k2i ; 
		
	}Light ;
	typedef struct TESVertex
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLuint  c;
	}ESVertex;
	typedef struct TESVertexFog
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLuint  c;
		GLuint  s;
	}ESVertexFog;

	typedef struct TESVertexTex1
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLuint  c;
		GLfloat u;
		GLfloat v;
	}ESVertexTex1;
	typedef struct TESVertexFogTex1
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLuint  c;
		GLuint  s;
		GLfloat u;
		GLfloat v;
	}ESVertexFogTex1;


	typedef struct TESVertexTex2
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLuint  c;
		GLfloat u;
		GLfloat v;
		GLfloat u1;
		GLfloat v1;
	}ESVertexTex2;
	typedef struct TESVertexFogTex2
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLuint  c;
		GLuint  s;
		GLfloat u;
		GLfloat v;
		GLfloat u1;
		GLfloat v1;
	}ESVertexFogTex2;

	typedef GLfloat * (*pfMoveVertex)(GLfloat *);

	typedef struct TWorkArea
	{       
		GLint indexAllocSize;
		GLvoid * indexAllocPtr ; 
		GLvoid * pntszPointer ; 
		GLint   cpStride;
		GLvoid *colorPointer ; 
		GLvoid *texCoordPointer[2] ;    
		GLint   vertexArrayStride ; 
		GLint   vwords ; 

		GLfloat *normalInvEyecoords; 
		GLvoid * texCoords ; 
		GLvoid * objcoords;
		GLfloat * eyecoords;
		GLfloat * clipcoords;
		GLfloat * vertexStream ; 
		GLfloat * triangleStream ;
		GLfloat * frontColor ; 
		GLuint * backColorStream ; 
		int             vertexCount ; 
		int             oldVertCount ; 
		int oldvwords ; 
		int             texCount;
		pfMoveVertex move ; 

		int             eyeRequired ; 
		GLfloat ecm[4] ;
		GLfloat acmPacs [4];
		GLfloat ecmAacmPacs[4] ;
		GLfloat acmPacli[C_MAX_LIGHTS][4] ; 
		GLfloat dcmPdcli[C_MAX_LIGHTS][4] ; 
		GLfloat scmPscli[C_MAX_LIGHTS][4]; 
		GLboolean       lightChange ; 
		GLuint          curLight ; 

		GLfloat *modelView;/* Points to MV pointer for non palette case and modelViewPal
										for paletted case */
		GLfloat *proj;
		
		GLfloat *texMatrix;

		GLfloat *modelViewPal;

		GLfloat projModelView[ 16 ];
		GLfloat invModelView[16 ];
		
	}WorkArea ;

	typedef struct TPointerInfo
	{
		GLint size;
		GLenum type;
		GLsizei stride;
		GLvoid *pointer;
	}PointerInfo ;

	//Texture OES API - 2009.05.20
	// Used for saving opengl state for Draw texture OES 
	typedef struct 
	{
		GLint viewport[4];
		GLint activeTexture;
		GLint clipplane[6];
		GLint Lighting;
		GLint ClientState[3];
		PointerInfo AttributeArray[3];
		GLuint arrayBind;
		GLuint elementBind;
	}GlesStateDrawTex;

	typedef struct TUniformsDirtyState
	{
		GLboolean index;
		GLboolean matrixState;
		GLboolean mormalMatrixState;
		GLboolean tex0State;
		GLboolean tex1State;
		GLboolean tex0MatrixState;
		GLboolean tex1MatrixState;
		GLboolean matrixPaletteState;
		GLboolean fogState;
		GLboolean clipPlaneState;
		GLboolean pointSpriteState;
		GLboolean pointState;
		
	}UniformsDirtyState;

	typedef struct TGLState
	{
		//Version and id must be the first two fields in the struct
		GLuint              version;    //OpenGL version
		GLuint              id;         //context id
		GLuint magicNumber;
		GLint glcolor;
		GLint colorEnabled;
		GLint AlphaorLuminance;
		
		void * m_hMaterial;
		void * m_hSurface;
		void * m_hDepth ; 
		void * m_pDisplay;
		GLenum  errors[C_MAX_ERRORS];
		GLint   error ; /* Init this to 0 */

		/* glbuffer */
		BufferList bufferList ; 

		/* glclip */
		GLfloat clipPlanes[C_MAX_CLIP_PLANES][4] ; 

		/* gltexenv */
		GLenum texCoordsEnabled[C_MAX_TEXTURE_UNITS];
		GLenum texEnvMode [C_MAX_TEXTURE_UNITS];
		GLfloat texEnvColor[C_MAX_TEXTURE_UNITS][4] ;
		GLenum texEnvCombineRGB[C_MAX_TEXTURE_UNITS];
		GLenum texEnvCombineAlpha[C_MAX_TEXTURE_UNITS];
		GLboolean       texReplaceCoord[C_MAX_TEXTURE_UNITS]; 

		GLenum mSrcRGB[C_MAX_TEXTURE_UNITS][3];
		GLenum mSrcAlpha[C_MAX_TEXTURE_UNITS][3];
		GLenum  mOperandRGB[C_MAX_TEXTURE_UNITS][3];
		GLenum  mOperandAlpha[C_MAX_TEXTURE_UNITS][3] ;
		GLfloat mRGBScale[C_MAX_TEXTURE_UNITS];
		GLfloat mAlphaScale[C_MAX_TEXTURE_UNITS] ;



		/* glmatrix */
		GLint   matrixMode;
		MatrixStack stacks[2+C_MAX_TEXTURE_UNITS];/* MV, PROJ, TEX */
		Matrix44        paletteMatrix[C_MAX_PALETTE_MATRICES_OES];
		GLint           paletteIndex ; 
		
		GLenum          shadeModel;             
		/* gltex */
		GLint           activeTexture;

		TextureList     textureList ; 

		GLfloat pointSize ; 

		GLclampf        sampleValue;
		GLboolean       sampleInvert;

		GLenum hGenMipmap ; 
		GLenum hFog ; 
		GLenum hLineSmooth ; 
		GLenum hPointSmooth ; 
		GLenum hPerspective ; 

		/* glcolor */
		Material    front;
		Material    back;
		Material        frontNback ;

		Light           lights[C_MAX_LIGHTS];
		
		GLfloat         acs[4]; /* Ambient Scene. Init to .2,.2,.2,0) */
		GLboolean       vbs ;   /* FALSE */
		GLenum          ces     ;       /* SINGLE_COLOR */
		GLboolean       tbs ;   /* FALSE */

		GLenum          fogMode;
		GLfloat         fogDensity;
		GLfloat         fogStart;
		GLfloat         fogEnd;
		GLfloat         fogColor[4];

		GLfloat         pointSizeMin ; 
		GLfloat         pointSizeMax ; 
		GLfloat         pointFadeThresh ; 
		GLfloat         pointDistAtten[3] ;     

		/* glvertex */
		GLenum          mode ;
		GLint           first ;
		GLint           count ; /* vertex count */
		GLenum          type;
		const GLvoid *indices ;
		GLboolean   isIndexed ; 

		/* glenable */
		unsigned long           flags; 
		unsigned long           flagspt; 
		unsigned long           lflags[C_MAX_LIGHTS];
		unsigned long           cflags[C_MAX_CLIP_PLANES];

		/* glvertex */
		PointerInfo pntszPointer;
		PointerInfo vertexPointer;
		PointerInfo colorPointer;
		PointerInfo texCoordPointer[C_MAX_TEXTURE_UNITS];
		PointerInfo matrixIndexPointer;
		PointerInfo weightPointer;
		GLuint          clientActiveTex ;
		GLfloat color[4] ;

		GLfloat texCoords[C_MAX_TEXTURE_UNITS][4];

		WorkArea        wa ; 

		GLuint InitDone;
		GLboolean pointEnable;

		//added for glShadeModel
		GLuint  isFlatShading;

		unsigned long  textureMode[2];
		unsigned long  numLightUsed;
		unsigned long  fogshaderMode;
		unsigned long isMipmapEnabled[2];
		unsigned long userClipEnabled;
		unsigned long matrixPaletteEnabled;
		unsigned long pointSpriteEnabled[2];
		unsigned long colorAsUniform;

		//storing the previous state of the shader.
		unsigned long  prevTextureMode[2];
		unsigned long  prevNumLightUsed;
		unsigned long  prevFogshaderMode;
		unsigned long  prevIsMipmapEnabled[2];
		unsigned long  prevUserClipEnabled;
		unsigned long  prevMatrixPaletteEnabled;
		unsigned long  prevPointSpriteEnabled[2];
		unsigned long  prevColorAsUniform;
		GLuint         prevEnableDrawTexture;
		GLboolean      prevPointEnable;

		//tracking whether color array is enabled or not
		GLboolean      colorArrayEnabled;

		GLint         prevIndex;

		struct _progUnifInfo* progInfo;

		GLboolean pointSizeArrayEnable; // 1 = point size array is enabled 0 = draw point defined by glPointsize
		GLubyte *extension;

		GLES2Context    gl20context;

		GLfloat noOfVertexUnit;         // This variable says how many vertex units are enabled in shader in case of
		// matrix palette. This value is given by user with help of 2 APIs 
		// (glMatrixIndexPointerOES and glWeightPointerOES) size parameter. In ideal 
		// case size value in these 2 APIs should be same, but user can give any value. 
		// It is not clear in GLES spec to consider which value. But it is safe to take  
		// smaller of these 2 values. Its default value is 0;

		GLboolean genMipmap; // this parameter is set if automatic mipmap generation is enabled by
		// glTexparameter*(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE/GL_FALSE) API

#ifdef MATRIX_DIRTY_STATE
		GLboolean mvprojDirtyState;
#endif  
		//Texture OES API - 2009.05.20
		GLuint enableDrawTexture;
		UniformsDirtyState uniformsDirtyState;


	}GLState;


#ifdef __cplusplus
}       
#endif

#endif /* __glstate_h__ */
