#include "includes.h"

void SetIdentity(Matrix44* matrix)
{
	GLfloat *mat = matrix->data ;
	mat[0]=mat[5]=mat[10]=mat[15]=1.0f ;
	mat[1]=mat[2]=mat[3]=mat[4]=mat[6]=mat[7]=mat[8]=mat[9]=mat[11]=mat[12]=mat[13]=mat[14]=0.0f;
	matrix->state = IDENTITY ; 
}

void InitializeMatrixState(GLState *ctx)
{
	int i = 0 ; 
	for ( i = 0 ; i < 2+C_MAX_TEXTURE_UNITS; i++)
	{
		Matrix44 *mat = (Matrix44 *)ALLOC_MEM(sizeof(Matrix44)); 
		if ( mat )
		{
			SetIdentity(mat) ; 
			ctx->stacks[i].pos = 0 ; 
			ctx->stacks[i].matrices[0] = mat ;
		}
		else
		{
			GL_ERROR(GL_OUT_OF_MEMORY) 
		}
	}
}


void DeInitializeMatrixState(GLState *ctx)
{  
	int i =0; 
	int j =0;
	for ( i = 0 ; i < 2+C_MAX_TEXTURE_UNITS ; i++){ 
		for( j =0 ; j <= ctx->stacks[i].pos  ; j++){    

			FREE_MEM_CHKRST(ctx->stacks[i].matrices[j]);
		}    
	}
}

MatrixStack *GetCurrentMatrixStack(GLState *ctx)
{
	switch ( ctx->matrixMode )
	{
	case GL_MODELVIEW:
		return &ctx->stacks[MAT_MODELVIEW];
	case GL_PROJECTION:
		return &ctx->stacks[MAT_PROJECTION];
	case GL_TEXTURE:
		if ( 0 == ctx->activeTexture ) 
		return &ctx->stacks[MAT_TEXTURE0];
		if ( 1 == ctx->activeTexture ) 
		return &ctx->stacks[MAT_TEXTURE1];
	}
	return NULL ; 
}

Matrix44 *GetCurrentMatrix(GLState *ctx)
{
	switch ( ctx->matrixMode )
	{
	case GL_MODELVIEW:
		return ctx->stacks[MAT_MODELVIEW].matrices[ctx->stacks[MAT_MODELVIEW].pos ];
	case GL_PROJECTION:
		return ctx->stacks[MAT_PROJECTION].matrices[ctx->stacks[MAT_PROJECTION].pos ];
	case GL_TEXTURE:
		if ( 0 == ctx->activeTexture ) 
		return ctx->stacks[MAT_TEXTURE0].matrices[ctx->stacks[MAT_TEXTURE0].pos ];
		if ( 1 == ctx->activeTexture ) 
		return ctx->stacks[MAT_TEXTURE1].matrices[ctx->stacks[MAT_TEXTURE1].pos ];
		break ;
	case GL_MATRIX_PALETTE_OES:
		return &ctx->paletteMatrix[ ctx->paletteIndex ] ; 
	}
	return NULL ; 
}

/* lhs *= rhs */
void MultMatrices(Matrix44* lhs, Matrix44 *rhs )
{
	int i = 0 ; 
	int j = 0 ; 
	int k = 0 ; 
	GLfloat * C1 = lhs->data ;
	GLfloat * M = rhs->data ;
	GLfloat tmp = 0 ; 
	GLfloat C[16] ;
	memcpy(C, C1,16*sizeof(float));

	for ( i = 0 ; i < 4 ; i ++ )
	for ( j= 0 ; j < 4 ; j++ )
	{
		tmp = 0 ;
		for ( k = 0 ; k < 4 ; k ++ )
		tmp += C[i*4+k] * M[k*4+j] ;
		C1[i*4+j] = tmp ; 
	}
}

GL_API void GL_APIENTRY glMatrixMode (GLenum mode)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glMatrixMode(0x%x)\n",mode));

	//DBGFUNCTION_INIT("glMatrixMode ")
	//RETAILMSG(1,(TEXT("glMatrixMode(0x%x);  \r\n"),mode));

	CHECK_GL_ENUM(((mode != GL_MODELVIEW)&&(mode != GL_PROJECTION)&&(mode != GL_TEXTURE)&&(mode != GL_MATRIX_PALETTE_OES)))
	
	ctx->matrixMode = mode ; 

	//DBGFUNCTION_EXIT("glMatrixMode ")
}

GL_API void GL_APIENTRY glPopMatrix (void)
{
	GET_GLES_CONTEXT
	MatrixStack *st = (MatrixStack *)0 ; 

	//GL11LOGMSG(LOG_GLES11_API,("glPopMatrix()\n"));

	st = GetCurrentMatrixStack(ctx) ; 
	if ( st )
	{
		Matrix44 *mt = st->matrices[st->pos] ;
		if (st->pos >=1)
		{
			FREE_MEM_CHKRST(mt);
			st->matrices[st->pos] = (Matrix44 *) 0 ;
			st->pos--;

			#ifdef MATRIX_DIRTY_STATE
			// iF ANY ONE OF MODELVIEW OR PROJECTION MATRIX IS MODIFIED THEN DIRTY BIT IS SET 
			if((ctx->matrixMode == GL_MODELVIEW) ||(ctx->matrixMode == GL_PROJECTION)) 
			ctx->mvprojDirtyState = GL_TRUE;
			#endif  
		}
		else
		{
			GL_ERROR( GL_STACK_UNDERFLOW )
		}
	}

}

GL_API void GL_APIENTRY glPushMatrix (void)
{
	GET_GLES_CONTEXT
	MatrixStack *st = (MatrixStack *)0 ; 

	//GL11LOGMSG(LOG_GLES11_API,("glPushMatrix()\n"));

	st = GetCurrentMatrixStack(ctx) ; 
	if ( st )
	{
		if (st->pos == (C_MAX_MODELVIEW_STACK_DEPTH-1))
		{
			GL_ERROR( GL_STACK_OVERFLOW )   
		}
		else
		{
			Matrix44 *mat = (Matrix44 *)ALLOC_MEM(sizeof(Matrix44)); 
			if ( mat )
			{
				memcpy(mat, st->matrices[st->pos], sizeof(Matrix44)); 
				st->pos++;
				st->matrices[st->pos] = mat ;

				#ifdef MATRIX_DIRTY_STATE
				// iF ANY ONE OF MODELVIEW OR PROJECTION MATRIX IS MODIFIED THEN DIRTY BIT IS SET 
				if((ctx->matrixMode == GL_MODELVIEW) ||(ctx->matrixMode == GL_PROJECTION)) 
				ctx->mvprojDirtyState = GL_TRUE;
				#endif  
			}
			else
			{
				GL_ERROR(GL_OUT_OF_MEMORY) 
			}
		}
	}
	
}

GL_API void GL_APIENTRY glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	GET_GLES_CONTEXT
	Matrix44 *curMat = (Matrix44 *) 0 ; 
	Matrix44 rot ; 
	double rad = angle * C_PI  / 180.0 ; 
	float c = cos(rad);
	float ci = 1.0f - c;
	float s = sin(rad);
	float modval = (x* x + y* y + z* z); 
	float x1,y1,z1, x2,y2,z2 , xs, ys, zs , xy , yz, xz;
	float invmod ; 
	float * mat ; 

	//GL11LOGMSG(LOG_GLES11_API,("glRotatef(%ff\t%ff\t%ff\t%ff)\n",angle,x,y,z));

	curMat = GetCurrentMatrix(ctx);
	ASSERT(curMat != 0 ) ; 
	SetIdentity(&rot) ; 
	mat = rot.data;
	
	if ( modval == 0.0f ) 
	{
		return ; /* No error */
	}
	modval = sqrt(modval) ; 
	
	invmod = 1.0f / modval ; 
	x1 = x * invmod ; 
	y1 = y * invmod ; 
	z1 = z * invmod ; 

	xs = x1 * s ;
	ys = y1 * s ;
	zs = z1 * s ;

	x2 = x1 * x1 ;
	y2 = y1 * y1 ;
	z2 = z1 * z1 ;

	xy = x1 * y1 ;
	yz = y1 * z1 ;
	xz = x1 * z1 ; 
	mat[ 0] = x2 * ci + c  ; mat[ 1] = xy * ci - zs ; mat[ 2] = xz * ci + ys ; 
	mat[ 4] = xy * ci + zs ; mat[ 5] = y2 * ci + c  ; mat[ 6] = yz * ci - xs ; 
	mat[ 8] = xz * ci - ys ; mat[ 9] = yz * ci + xs ; mat[10] = z2 * ci + c  ; 

	#ifdef MATRIX_DIRTY_STATE
	// iF ANY ONE OF MODELVIEW OR PROJECTION MATRIX IS MODIFIED THEN DIRTY BIT IS SET 
	if((ctx->matrixMode == GL_MODELVIEW) ||(ctx->matrixMode == GL_PROJECTION)) 
	ctx->mvprojDirtyState = GL_TRUE;
	#endif
	MultMatrices(curMat , &rot ) ; 
	curMat->state = ARBITRARY ;     
	
}

GL_API void GL_APIENTRY glScalef (GLfloat x, GLfloat y, GLfloat z)
{
	GET_GLES_CONTEXT
	Matrix44 *curMat = (Matrix44 *) 0 ; 
	Matrix44 mat ; 
	
	//GL11LOGMSG(LOG_GLES11_API,("glScalef(%ff\t%ff\t%ff)\n",x,y,z));

	curMat = GetCurrentMatrix(ctx);
	ASSERT(curMat != 0 ) ; 
	SetIdentity(&mat) ; 

	mat.data[0] = x ;
	mat.data[5] = y ;
	mat.data[10] = z ;

	
	#ifdef MATRIX_DIRTY_STATE
	// iF ANY ONE OF MODELVIEW OR PROJECTION MATRIX IS MODIFIED THEN DIRTY BIT IS SET 
	if((ctx->matrixMode == GL_MODELVIEW) ||(ctx->matrixMode == GL_PROJECTION)) 
	ctx->mvprojDirtyState = GL_TRUE;
	
	#endif

	MultMatrices(curMat , &mat  ) ;         
	if (( curMat->state == IDENTITY ) || (curMat->state == SCALE))
	{
		curMat->state = SCALE ; 
	}
	else
	{
		curMat->state = ARBITRARY ;             
	}
}

GL_API void GL_APIENTRY glTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
	GET_GLES_CONTEXT
	Matrix44 *curMat = (Matrix44 *) 0 ; 
	Matrix44 mat ; 

	//GL11LOGMSG(LOG_GLES11_API,("glTranslatef(%ff\t%ff\t%ffx)\n",x,y,z));

	curMat = GetCurrentMatrix(ctx);
	ASSERT(curMat != 0 ) ; 

	SetIdentity(&mat) ; 

	mat.data[3] = x ;
	mat.data[7] = y ;
	mat.data[11] = z ;

	#ifdef MATRIX_DIRTY_STATE
	// iF ANY ONE OF MODELVIEW OR PROJECTION MATRIX IS MODIFIED THEN DIRTY BIT IS SET 
	if((ctx->matrixMode == GL_MODELVIEW) ||(ctx->matrixMode == GL_PROJECTION)){ 
		ctx->mvprojDirtyState = GL_TRUE;
	}
	#endif
	
	MultMatrices(curMat , &mat ) ;  
	if (( curMat->state == IDENTITY ) || (curMat->state == TRANSLATE))
	{
		curMat->state = TRANSLATE ; 
	}
	else
	{
		curMat->state = ARBITRARY ;             
	}
}


GL_API void GL_APIENTRY glOrthof (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	GET_GLES_CONTEXT
	Matrix44 *curMat = (Matrix44 *) 0 ; 
	Matrix44 mat; 
	GLfloat *tmp ;

	//GL11LOGMSG(LOG_GLES11_API,("glOrthof(%ff\t%ff\t%ff\tf%f\t%ff\t%ff)\n",left,right,bottom,top,zNear,zFar));

	CHECK_GL_VALUE((left==right))
	CHECK_GL_VALUE((top==bottom))
	CHECK_GL_VALUE((zNear==zFar))

	curMat = GetCurrentMatrix(ctx);
	ASSERT(curMat != 0 ) ; 

	SetIdentity(&mat) ; 


	tmp = mat.data ;
	tmp[0] = (2.0f )/(right - left ) ; 
	tmp[3] = -(right + left )/(right - left ) ; 

	tmp[5] = (2.0f )/(top - bottom ) ; 
	tmp[7] = -(top + bottom )/(top - bottom ) ; 

	tmp[10] = (2.0f )/(zNear - zFar ) ; 
	tmp[11] = (zFar + zNear)/(zNear - zFar ) ; 

	#ifdef MATRIX_DIRTY_STATE
	// iF ANY ONE OF MODELVIEW OR PROJECTION MATRIX IS MODIFIED THEN DIRTY BIT IS SET 
	if((ctx->matrixMode == GL_MODELVIEW) ||(ctx->matrixMode == GL_PROJECTION)) 
	{ 
		ctx->mvprojDirtyState = GL_TRUE;
	}
	
	#endif

	MultMatrices(curMat , &mat ) ;  

	curMat->state = ARBITRARY ;     
}

GL_API void GL_APIENTRY glLoadMatrixf (const GLfloat *m)
{
	GET_GLES_CONTEXT
	int i = 0 ; 
	int j = 0 ; 

	Matrix44 *curMat = (Matrix44 *) 0 ; 
	GLfloat *mat ; 

	//GL11LOGMSG(LOG_GLES11_API,("glLoadMatrixf(%ff)\n",*m));

	curMat = GetCurrentMatrix(ctx);
	ASSERT(curMat != 0 ) ; 

	mat = curMat->data ;
	for (i = 0 ; i < 4 ; i++ )
	{
		for ( j = 0 ; j < 4 ; j++)
		{
			mat[i * 4 + j ] = m[j*4 + i ] ; 
		}
	}

	#ifdef MATRIX_DIRTY_STATE
	// iF ANY ONE OF MODELVIEW OR PROJECTION MATRIX IS MODIFIED THEN DIRTY BIT IS SET 
	if((ctx->matrixMode == GL_MODELVIEW) ||(ctx->matrixMode == GL_PROJECTION)){ 
		ctx->mvprojDirtyState = GL_TRUE;
	}
	
	#endif  
	curMat->state = ARBITRARY ; 
}

GL_API void GL_APIENTRY glFrustumf (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	GET_GLES_CONTEXT
	Matrix44 *curMat = (Matrix44 *) 0 ; 
	Matrix44 mat ; 
	GLfloat *tmp ;

	//GL11LOGMSG(LOG_GLES11_API,("glFrustumf(%ff\t%ff\t%ff\t%ff\t%ff\t%f)\n",left,right,bottom,top,zNear,zFar));

	CHECK_GL_VALUE((left==right))
	CHECK_GL_VALUE((top==bottom))
	CHECK_GL_VALUE((zNear<0.0f))
	CHECK_GL_VALUE((zFar<0.0f))
	CHECK_GL_VALUE((zNear==zFar))

	curMat = GetCurrentMatrix(ctx);
	ASSERT(curMat != 0 ) ; 

	SetIdentity(&mat) ; 

	tmp = mat.data ;
	tmp[0] = (2.0f * zNear )/(right - left ) ; 
	tmp[2] = (right + left )/(right - left ) ; 

	tmp[5] = (2.0f * zNear )/(top - bottom ) ; 
	tmp[6] = (top + bottom )/(top - bottom ) ; 

	tmp[10] = (zFar + zNear )/(zNear - zFar ) ; 
	tmp[11] = (2.0f * zFar * zNear)/(zNear - zFar ) ; 

	tmp[14] = -1.0f ; 
	tmp[15] = 0.0f ; 

	#ifdef MATRIX_DIRTY_STATE
	// iF ANY ONE OF MODELVIEW OR PROJECTION MATRIX IS MODIFIED THEN DIRTY BIT IS SET 
	if((ctx->matrixMode == GL_MODELVIEW) ||(ctx->matrixMode == GL_PROJECTION)) 
	ctx->mvprojDirtyState = GL_TRUE;
	
	#endif

	MultMatrices(curMat , &mat ) ;  
	
	curMat->state = ARBITRARY ; 
}

GL_API void GL_APIENTRY glLoadIdentity (void)
{
	GET_GLES_CONTEXT
	Matrix44 *curMat = (Matrix44 *) 0 ; 

	//GL11LOGMSG(LOG_GLES11_API,("glLoadIdentity()\n"));

	curMat = GetCurrentMatrix(ctx);
	ASSERT(curMat != 0 ) ; 

	#ifdef MATRIX_DIRTY_STATE
	// iF ANY ONE OF MODELVIEW OR PROJECTION MATRIX IS MODIFIED THEN DIRTY BIT IS SET 
	if((ctx->matrixMode == GL_MODELVIEW) ||(ctx->matrixMode == GL_PROJECTION)) 
	ctx->mvprojDirtyState = GL_TRUE;
	#endif  
	
	SetIdentity( curMat ) ; 
}

GL_API void GL_APIENTRY glMultMatrixf (const GLfloat *m)
{
	GET_GLES_CONTEXT
	int i = 0 ; 
	int j = 0 ; 
	Matrix44 *curMat = (Matrix44 *) 0 ; 
	Matrix44 tmpMat ; 
	GLfloat *mat = tmpMat.data ;

	//GL11LOGMSG(LOG_GLES11_API,("glMultMatrixf(%ff)\n",*m));

	for (i = 0 ; i < 4 ; i++ )
	{
		for ( j = 0 ; j < 4 ; j++)
		{
			mat[i * 4 + j ] = m[j*4 + i ] ; 
		}
	}
	
	curMat = GetCurrentMatrix(ctx);
	ASSERT(curMat != 0 ) ; 

	#ifdef MATRIX_DIRTY_STATE
	// iF ANY ONE OF MODELVIEW OR PROJECTION MATRIX IS MODIFIED THEN DIRTY BIT IS SET 
	if((ctx->matrixMode == GL_MODELVIEW) ||(ctx->matrixMode == GL_PROJECTION)) 
	ctx->mvprojDirtyState = GL_TRUE;
	
	#endif  
	
	MultMatrices(curMat , &tmpMat ) ;       
	curMat->state = ARBITRARY ; 
}

GL_API void GL_APIENTRY glOrthox (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glOrthox(0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",left,right,bottom,top,zNear,zFar));

	//DBGFUNCTION_INIT("glOrthox ")
	//RETAILMSG(1,(TEXT("glOrthox  \r\n")));

	glOrthof(FIXED_TO_FLOAT(left),FIXED_TO_FLOAT(right),FIXED_TO_FLOAT(bottom),FIXED_TO_FLOAT(top),FIXED_TO_FLOAT(zNear),FIXED_TO_FLOAT(zFar));             
	
	//DBGFUNCTION_EXIT("glOrthox ")
}

GL_API void GL_APIENTRY glFrustumx (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glFrustumx(0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x)\n",left,right,bottom,top,zNear,zFar));

	//DBGFUNCTION_INIT("glFrustumx ")
	//RETAILMSG(1,(TEXT("glFrustumx  \r\n")));

	glFrustumf(FIXED_TO_FLOAT(left),FIXED_TO_FLOAT(right),FIXED_TO_FLOAT(bottom),FIXED_TO_FLOAT(top),FIXED_TO_FLOAT(zNear),FIXED_TO_FLOAT(zFar));           
	
	//DBGFUNCTION_EXIT("glFrustumx ")
}

GL_API void GL_APIENTRY glMultMatrixx (const GLfixed *m)
{
	int i = 0 ;
	GLfloat mat [16]; 
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glMultMatrixx(0x%x)\n",m));

	//DBGFUNCTION_INIT("glMultMatrixx ")
	//RETAILMSG(1,(TEXT("glMultMatrixx  \r\n")));

	for ( i = 0 ; i < 16 ; i ++)
	mat[i]=FIXED_TO_FLOAT(m[i]);

	glMultMatrixf(mat) ; 
	//DBGFUNCTION_EXIT("glMultMatrixx ")
}

GL_API void GL_APIENTRY glTranslatex (GLfixed x, GLfixed y, GLfixed z)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glTranslatex(0x%x\t0x%x\t0x%x)\n",x,y,z));

	//DBGFUNCTION_INIT("glTranslatex ")
	//RETAILMSG(1,(TEXT("glTranslatex  \r\n")));

	glTranslatef(FIXED_TO_FLOAT(x),FIXED_TO_FLOAT(y),FIXED_TO_FLOAT(z));    
	//DBGFUNCTION_EXIT("glTranslatex ")
}

GL_API void GL_APIENTRY glScalex (GLfixed x, GLfixed y, GLfixed z)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glScalex(0x%x\t0x%x\t0x%x)\n",x,y,z));

	//DBGFUNCTION_INIT("glScalex ")
	//RETAILMSG(1,(TEXT("glScalex  \r\n")));
	
	glScalef(FIXED_TO_FLOAT(x),FIXED_TO_FLOAT(y),FIXED_TO_FLOAT(z));        
	
	//DBGFUNCTION_EXIT("glScalex ")
}

GL_API void GL_APIENTRY glRotatex (GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glRotatex(0x%x\t0x%x\t0x%x\t0x%x)\n",angle,x,y,z));

	//DBGFUNCTION_INIT("glRotatex ")
	//RETAILMSG(1,(TEXT("glRotatex  \r\n")));

	glRotatef(FIXED_TO_FLOAT(angle),FIXED_TO_FLOAT(x),FIXED_TO_FLOAT(y),FIXED_TO_FLOAT(z)); 
	//DBGFUNCTION_EXIT("glRotatex ")
}

GL_API void GL_APIENTRY glLoadMatrixx (const GLfixed *m)
{
	GET_GLES_CONTEXT
	Matrix44 *curMat = (Matrix44 *) 0 ; 
	GLfloat *mat = (GLfloat * ) 0 ; 
	int i = 0 ; 
	int j = 0;

	//GL11LOGMSG(LOG_GLES11_API,("glLoadMatrixx(0x%x)\n",m));

	curMat = GetCurrentMatrix(ctx);
	ASSERT(curMat != 0 ) ; 
	mat = curMat->data;
	for (i = 0 ; i < 4 ; i++ ){
		for ( j = 0 ; j < 4 ; j++){
			mat[i * 4 + j ] = FIXED_TO_FLOAT(m[j*4 + i]); 
		}
	}

#ifdef MATRIX_DIRTY_STATE
	// iF ANY ONE OF MODELVIEW OR PROJECTION MATRIX IS MODIFIED THEN DIRTY BIT IS SET 
	if((ctx->matrixMode == GL_MODELVIEW) ||(ctx->matrixMode == GL_PROJECTION)){
		ctx->mvprojDirtyState = GL_TRUE;
	}
#endif

	curMat->state = ARBITRARY ; 
}

GL_API void GL_APIENTRY glCurrentPaletteMatrixOES (GLuint matrixpaletteindex)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glCurrentPaletteMatrixOES(0x%x)\n",matrixpaletteindex));

	//DBGFUNCTION_INIT("glCurrentPaletteMatrixOES ")
	//RETAILMSG(1,(TEXT("glCurrentPaletteMatrixOES  \r\n")));
	
	CHECK_GL_VALUE(( matrixpaletteindex>= C_MAX_PALETTE_MATRICES_OES ))
	ctx->paletteIndex = matrixpaletteindex;

}

GL_API void GL_APIENTRY glLoadPaletteFromModelViewMatrixOES (void)
{
	Matrix44 *dst, *src; 
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glLoadPaletteFromModelViewMatrixOES()\n"));

	//DBGFUNCTION_INIT("glLoadPaletteFromModelViewMatrixOES ")
	//RETAILMSG(1,(TEXT("glLoadPaletteFromModelViewMatrixOES  \r\n")));
	
	dst = &ctx->paletteMatrix[ ctx->paletteIndex ] ;
	src = ctx->stacks[MAT_MODELVIEW].matrices[ctx->stacks[MAT_MODELVIEW].pos ];
	if ( src && dst )
	{
		memcpy(dst, src, sizeof(Matrix44));
	}
}
