void MultMatrixArray4f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray3f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray2f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray4b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray3b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray2b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray4ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray3ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray2ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray4s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray3s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray2s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray4us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray3us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray2us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray4x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray3x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

void MultMatrixArray2x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint dstride, GLint count ) ;

pFNMultMatrixT GetMultMatrixArrayFunc(int size, GLenum type);

void MultMatrixPal4f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal3f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal2f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal4b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal3b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal2b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal4ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal3ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal2ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal4s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal3s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal2s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal4us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal3us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal2us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal4x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal3x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixPal2x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

pFNMultMatrix GetMultMatrixPalFunc(int size, GLenum type);

int InverseMatrix44(GLfloat *m , GLfloat *out ); 

int InverseMatrix33(GLfloat *in , GLfloat *out ); 

void MultMatrix44(GLfloat *m1, GLfloat *m2, GLfloat *dst);

void WeightedSum(GLfloat * dst, GLfloat *src, GLfloat wt );

void MultMatrix4f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix3f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix2f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix4b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix3b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix2b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix4ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix3ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix2ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix4s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix3s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix2s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix4us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix3us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix2us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix4x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix3x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrix2x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

pFNMultMatrix GetMultMatrixFunc(int size, GLenum type);

void InitLighting(GLState *ctx);

int DoLighting(GLState *ctx);

void DoLightingTwoSidedCM (GLState *ctx );

void DoLightingOneSidedCM (GLState *ctx );

void DoLightingTwoSided (GLState *ctx );

void DoLightingOneSided (GLState *ctx );

void  DoLightingTwoSidedCMSpot(GLState *ctx , int i);

void  DoLightingTwoSidedCMAtti(GLState *ctx , int i);

void DoLightingTwoSidedCMAll(GLState *ctx , int i);

void  DoLightingTwoSidedCMNone(GLState *ctx , int i);

void  DoLightingOneSidedCMSpot(GLState *ctx , int i);

void  DoLightingOneSidedCMAtti(GLState *ctx , int i);

void DoLightingOneSidedCMAll(GLState *ctx , int i);

void  DoLightingOneSidedCMNone(GLState *ctx , int i);

void  DoLightingTwoSidedSpot(GLState *ctx , int i);

void  DoLightingTwoSidedAtti(GLState *ctx , int i);

void DoLightingTwoSidedAll(GLState *ctx , int i);

void  DoLightingTwoSidedNone(GLState *ctx , int i);

void  DoLightingOneSidedSpot(GLState *ctx , int i);

void  DoLightingOneSidedAtti(GLState *ctx , int i);

void DoLightingOneSidedAll(GLState *ctx , int i);

void  DoLightingOneSidedNone(GLState *ctx , int i);

int GetTypeSize(GLenum type );

void InitTextureUnit( TextureUnit *to );

void SetIdentity(Matrix44* matrix);

void InitializeMatrixState(GLState *ctx);

MatrixStack *GetCurrentMatrixStack(GLState *ctx);

Matrix44 *GetCurrentMatrix(GLState *ctx);

/* lhs *= rhs */
void MultMatrices(Matrix44* lhs, Matrix44 *rhs );

void InitLight(int i, Light *l);

void InitMaterial(Material *mat );

void FlushStatePersistent(GLState *ctx)/* Call this before Start of frame , set mgl properties */;

void MultMatrixNorPal4f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal3f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal2f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal4b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal3b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal2b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal4ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal3ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal2ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal4s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal3s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal2s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal4us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal3us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal2us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 


void MultMatrixNorPal4x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal3x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNorPal2x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

pFNMultMatrix GetMultMatrixNorPalFunc(int size, GLenum type);

void MultMatrixNor3f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNor3b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNor3ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNor3s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNor3us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

void MultMatrixNor3x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ); 

pFNMultMatrix GetMultMatrixNorFunc(int size, GLenum type);

int     GetVertexWords(GLState *ctx);

int FreeTnL(GLState *ctx,WorkArea *wa );

void InitMatrices(GLState *ctx);

int GetStride(GLenum type );

int DoTnL(GLState *ctx);

void FreeState(GLState *ctx);

long StateReset(void);
//Note: the 1.1 context creation func takes in a shared 2.0 context because 1.1 is implemented on top 2.0
GLState * CreateStateGLES11(GLES11Context sharedctx);

void DestroyStateGLES11(GLState * ctx);

int DoTexturing1U1(GLState *ctx);

int DoTexturing1U2(GLState *ctx);

int DoTexturing2(GLState *ctx);

void CopyColor4f(GLvoid *src1, GLuint *dst, GLint sstride, GLint dstride, GLint count ); 

void CopyColor4ub(GLvoid *src1, GLuint *dst, GLint sstride, GLint dstride, GLint count ); 

void CopyColor4x(GLvoid *src1, GLuint *dst, GLint sstride, GLint dstride, GLint count ); 

pFNCopyColor GetCopyColorFunc(int size, GLenum type);

void CopyToFloat(void *dst, void * src, int count ) ;

void CopyToFixed(void *dst, void * src, int count ) ;

void CopyToInt(void *dst, void * src, int count ) ;

void CopyToBool(void *dst, void * src, int count ) ;

void CopyToFloatS(void *dst, GLfloat src, int count ) ;

void CopyToFixedS(void *dst, GLfloat src, int count ) ;

void CopyToIntS(void *dst, GLfloat src, int count ) ;

void CopyToBoolS(void *dst, GLfloat src, int count ) ;

void CopyToFloatE(void *dst, GLint src, int count ) ;

void CopyToFixedE(void *dst, GLint src, int count ) ;

void CopyToIntE(void *dst, GLint src, int count ) ;

void CopyToBoolE(void *dst, GLint src, int count ) ;

void GetStateAttrib ( GLState *ctx, GLenum pname , GLint index , pfnCopyData copy ,  pfnCopyDataEnum copye  ,pfnCopyDataVal copys  ,GLvoid *params) ;

int DoFog(GLState *ctx);

void ClipXYZW(GLfloat *clipin1, GLfloat *clipin2, GLfloat *ndcout, GLfloat t);

void ClipBKColor(GLuint *clipin1, GLuint *clipin2, GLuint *bkcout, GLfloat t);

void ClipES(GLvoid *v1, GLvoid *v2,GLvoid *vout, GLfloat t);

void ClipESFog(GLvoid *v1, GLvoid *v2,GLvoid *vout, GLfloat t);

void ClipESTex1(GLvoid *v1, GLvoid *v2,GLvoid *vout, GLfloat t);

void ClipESFogTex1(GLvoid *v1, GLvoid *v2,GLvoid *vout, GLfloat t);

void ClipESTex2(GLvoid *v1, GLvoid *v2,GLvoid *vout, GLfloat t);

void ClipESFogTex2(GLvoid *v1, GLvoid *v2,GLvoid *vout, GLfloat t);

pFNClipFn GetClipFn(GLState *ctx);

int AssemblePrimitivesClip(GLState *ctx, GLfloat *clipPlane);

int AssemblePrimitivesClipBk(GLState *ctx, GLfloat *clipPlane);

int AssemblePrimitivesClipIndexs(GLState *ctx, GLfloat *clipPlane);

int AssemblePrimitivesClipIndexb(GLState *ctx, GLfloat *clipPlane);

int AssemblePrimitivesClipBkIndexs(GLState *ctx, GLfloat *clipPlane);

int AssemblePrimitivesClipBkIndexb(GLState *ctx, GLfloat *clipPlane);

void SendVertexData(GLState *ctx);

void FlushStatePersistent(GLState *ctx);

void FlushState(GLState *ctx);
