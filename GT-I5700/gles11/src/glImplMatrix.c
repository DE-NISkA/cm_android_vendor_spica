#include "includes.h"

#define xprintf(x,y) printf(x,y); fflush(stdout)
#define MULTFUNC(s,t) MultMatrix##s##t
#define MAT(m,r,c) (m)[(r)*4+(c)]
#define SWAP_ROWS(a, b) { GLfloat *_tmp = a; (a)=(b); (b)=_tmp; }

void InverseTranspose(GLfloat *src,GLfloat *m )
{
	GLfloat m2233 =  src[5] * src[10] ; 
	GLfloat m2332 =  src[9] * src[6] ; 
	GLfloat m2133 =  src[4] * src[10] ; 
	GLfloat m3123 =  src[8] * src[6] ; 
	GLfloat m2132 =  src[4] * src[9] ; 
	GLfloat m3122 =  src[8] * src[5] ; 
	GLfloat m22332332 = m2233 - m2332 ; 
	GLfloat m31232133 = m3123 - m2133 ; 
	GLfloat m21323122 = m2132 - m3122 ; 

	GLfloat ndet = src[0] * ( m22332332 ) +
	src[1] * ( m31232133 ) + 
	src[2] * ( m21323122) ;
	m[0] =m[5] = m[10] = m[15] = 1.0f ; 
	m[1] =m[2] = m[3] = m[4] = 0.0f ; 
	m[6] =m[7] = m[8] = m[9] = 0.0f ; 
	m[11] =m[12] = m[13] = m[14] = 0.0f ; 
	
	if ( ndet == 0.0f) 
	{
		return ; 
	}
	else
	{
		
		GLfloat det = 1.0f /ndet; 
		m[0] =  m22332332 * det ;
		m[4] = (src[2]* src[9] - src[10] * src[1]) * det;
		m[8] = (src[1]* src[6] - src[2] * src[5]) * det;

		m[1] =  m31232133 * det ;
		m[5] = (src[0]* src[10] - src[2] * src[8]) * det;
		m[9] = (src[2]* src[4] - src[0] * src[6]) * det;

		m[2] =  m21323122 * det ;
		m[6] = (src[1]* src[8] - src[0] * src[9]) * det;
		m[10] = (src[0]* src[5] - src[1] * src[4]) * det;
	}
}

int InverseMatrix44(GLfloat *m , GLfloat *out ) 
{
	GLfloat wtmp[4][8];
	GLfloat m0, m1, m2, m3, s;
	GLfloat *r0, *r1, *r2, *r3;
	//GLfloat temp;
	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

	r0[0] = MAT(m,0,0), r0[1] = MAT(m,0,1),
	r0[2] = MAT(m,0,2), r0[3] = MAT(m,0,3),
	r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,

	r1[0] = MAT(m,1,0), r1[1] = MAT(m,1,1),
	r1[2] = MAT(m,1,2), r1[3] = MAT(m,1,3),
	r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,

	r2[0] = MAT(m,2,0), r2[1] = MAT(m,2,1),
	r2[2] = MAT(m,2,2), r2[3] = MAT(m,2,3),
	r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,

	r3[0] = MAT(m,3,0), r3[1] = MAT(m,3,1),
	r3[2] = MAT(m,3,2), r3[3] = MAT(m,3,3),
	r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

	/* choose pivot - or die */
	if (fabsf(r3[0])>fabsf(r2[0])) SWAP_ROWS(r3, r2);
	if (fabsf(r2[0])>fabsf(r1[0])) SWAP_ROWS(r2, r1);
	if (fabsf(r1[0])>fabsf(r0[0])) SWAP_ROWS(r1, r0);
	//   temp = r0[0];
	if (0.0 == r0[0]) 
	return GL_FALSE;

	/* eliminate first variable     */
	m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
	s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
	s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
	s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
	s = r0[4];
	if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r0[5];
	if (s != 0.0) 
	{ r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r0[6];
	if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r0[7];
	if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

	/* choose pivot - or die */
	if (fabsf(r3[1])>fabsf(r2[1])) SWAP_ROWS(r3, r2);
	if (fabsf(r2[1])>fabsf(r1[1])) SWAP_ROWS(r2, r1);
	if (0.0 == r1[1])  return GL_FALSE;

	/* eliminate second variable */
	m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
	r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
	s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

	/* choose pivot - or die */
	if (fabsf(r3[2])>fabsf(r2[2])) SWAP_ROWS(r3, r2);
	if (0.0 == r2[2])  return GL_FALSE;

	/* eliminate third variable */
	m3 = r3[2]/r2[2];
	r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
	r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
	r3[7] -= m3 * r2[7];

	/* last check */
	if (0.0 == r3[3]) return GL_FALSE;

	s = 1.0F/r3[3];             /* now back substitute row 3 */
	r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

	m2 = r2[3];                 /* now back substitute row 2 */
	s  = 1.0F/r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
	r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
	r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
	m0 = r0[3];
	r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
	r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

	m1 = r1[2];                 /* now back substitute row 1 */
	s  = 1.0F/r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
	r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
	r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

	m0 = r0[1];                 /* now back substitute row 0 */
	s  = 1.0F/r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
	r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

	MAT(out,0,0) = r0[4]; MAT(out,0,1) = r0[5],
	MAT(out,0,2) = r0[6]; MAT(out,0,3) = r0[7],
	MAT(out,1,0) = r1[4]; MAT(out,1,1) = r1[5],
	MAT(out,1,2) = r1[6]; MAT(out,1,3) = r1[7],
	MAT(out,2,0) = r2[4]; MAT(out,2,1) = r2[5],
	MAT(out,2,2) = r2[6]; MAT(out,2,3) = r2[7],
	MAT(out,3,0) = r3[4]; MAT(out,3,1) = r3[5],
	MAT(out,3,2) = r3[6]; MAT(out,3,3) = r3[7];

	return GL_TRUE;
}

#undef SWAP_ROWS

int InverseMatrix33(GLfloat *in , GLfloat *out ) 
{
	GLfloat pos, neg, t;
	GLfloat det;

	/* Calculate the determinant of upper left 3x3 submatrix and
	* determine if the matrix is singular.
	*/

	pos = neg = 0.0;
	t =  MAT(in,0,0) * MAT(in,1,1) * MAT(in,2,2);
	if (t >= 0.0) pos += t; else neg += t;

	t =  MAT(in,1,0) * MAT(in,2,1) * MAT(in,0,2);
	if (t >= 0.0) pos += t; else neg += t;

	t =  MAT(in,2,0) * MAT(in,0,1) * MAT(in,1,2);
	if (t >= 0.0) pos += t; else neg += t;

	t = -MAT(in,2,0) * MAT(in,1,1) * MAT(in,0,2);
	if (t >= 0.0) pos += t; else neg += t;

	t = -MAT(in,1,0) * MAT(in,0,1) * MAT(in,2,2);
	if (t >= 0.0) pos += t; else neg += t;

	t = -MAT(in,0,0) * MAT(in,2,1) * MAT(in,1,2);
	if (t >= 0.0) pos += t; else neg += t;

	det = pos + neg;

	if (det*det < 1e-25)
	return GL_FALSE;

	det = 1.0F / det;
	MAT(out,0,0) = (  (MAT(in,1,1)*MAT(in,2,2) - MAT(in,2,1)*MAT(in,1,2) )*det);
	MAT(out,0,1) = (- (MAT(in,0,1)*MAT(in,2,2) - MAT(in,2,1)*MAT(in,0,2) )*det);
	MAT(out,0,2) = (  (MAT(in,0,1)*MAT(in,1,2) - MAT(in,1,1)*MAT(in,0,2) )*det);
	MAT(out,1,0) = (- (MAT(in,1,0)*MAT(in,2,2) - MAT(in,2,0)*MAT(in,1,2) )*det);
	MAT(out,1,1) = (  (MAT(in,0,0)*MAT(in,2,2) - MAT(in,2,0)*MAT(in,0,2) )*det);
	MAT(out,1,2) = (- (MAT(in,0,0)*MAT(in,1,2) - MAT(in,1,0)*MAT(in,0,2) )*det);
	MAT(out,2,0) = (  (MAT(in,1,0)*MAT(in,2,1) - MAT(in,2,0)*MAT(in,1,1) )*det);
	MAT(out,2,1) = (- (MAT(in,0,0)*MAT(in,2,1) - MAT(in,2,0)*MAT(in,0,1) )*det);
	MAT(out,2,2) = (  (MAT(in,0,0)*MAT(in,1,1) - MAT(in,1,0)*MAT(in,0,1) )*det);

	/* Do the translation part */
	MAT(out,0,3) = - (MAT(in,0,3) * MAT(out,0,0) +
	MAT(in,1,3) * MAT(out,0,1) +
	MAT(in,2,3) * MAT(out,0,2) );
	MAT(out,1,3) = - (MAT(in,0,3) * MAT(out,1,0) +
	MAT(in,1,3) * MAT(out,1,1) +
	MAT(in,2,3) * MAT(out,1,2) );
	MAT(out,2,3) = - (MAT(in,0,3) * MAT(out,2,0) +
	MAT(in,1,3) * MAT(out,2,1) +
	MAT(in,2,3) * MAT(out,2,2) );

	return GL_TRUE;
}

void MultMatrix44(GLfloat *m1, GLfloat *m2, GLfloat *dst)
{
	int i , j, k ;
	for ( i = 0 ; i < 4 ; i++)
	for ( j = 0 ; j < 4 ; j++)
	{
		dst[i *4 + j  ] = 0.0f ;
		for ( k = 0 ; k < 4 ; k++)
		{
			dst[i *4 + j ] += m1[i *4 + k ] * m2[k * 4 + j ] ;
		}
	}
}

void WeightedSum(GLfloat * dst, GLfloat *src, GLfloat wt )
{
	int i = 0 ; 
	for ( i = 0 ; i < 16 ; i++)
	{
		dst[i] += src[i] * wt ; 
	}
}

void MultMatrix4f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y z w - FLOAT 4 */
	GLfloat *src = (GLfloat *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2]+m[ 3]* src[3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2]+m[ 7]* src[3];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2]+m[11]* src[3];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[14]* src[2]+m[15]* src[3];

		src += 4 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix3f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y z - FLOAT 3 */
	GLfloat *src = (GLfloat *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2]+m[ 3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2]+m[ 7];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2]+m[11];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[14]* src[2]+m[15];

		src += 3 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix2f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y - FLOAT 2 */
	GLfloat *src = (GLfloat *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 7];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[11];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[15];

		src += 2 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix4b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y z w - BYTE 4 */
	GLbyte *src = (GLbyte *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2]+m[ 3]* src[3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2]+m[ 7]* src[3];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2]+m[11]* src[3];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[14]* src[2]+m[15]* src[3];

		src += 4 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix3b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y z - BYTE 3 */
	GLbyte *src = (GLbyte *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2]+m[ 3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2]+m[ 7];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2]+m[11];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[14]* src[2]+m[15];

		src += 3 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix2b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y - BYTE 2 */
	GLbyte *src = (GLbyte *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 7];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[11];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[15];

		src += 2 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix4ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y z w - UBYTE 4 */
	GLubyte *src = (GLubyte *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2]+m[ 3]* src[3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2]+m[ 7]* src[3];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2]+m[11]* src[3];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[14]* src[2]+m[15]* src[3];

		src += 4 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix3ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y z - UBYTE 3 */
	GLubyte *src = (GLubyte *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2]+m[ 3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2]+m[ 7];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2]+m[11];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[14]* src[2]+m[15];

		src += 3 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix2ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y - UBYTE 2 */
	GLubyte *src = (GLubyte *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 7];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[11];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[15];

		src += 2 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix4s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y z w - SHORT 4 */
	GLshort *src = (GLshort*)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2]+m[ 3]* src[3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2]+m[ 7]* src[3];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2]+m[11]* src[3];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[14]* src[2]+m[15]* src[3];

		src += 4 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix3s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y z - SHORT 3 */
	GLshort *src = (GLshort *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2]+m[ 3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2]+m[ 7];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2]+m[11];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[14]* src[2]+m[15];

		src += 3 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix2s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y - SHORT 2 */
	GLshort *src = (GLshort *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 7];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[11];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[15];

		src += 2 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix4us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y z w - SHORT 4 */
	GLushort *src = (GLushort*)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2]+m[ 3]* src[3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2]+m[ 7]* src[3];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2]+m[11]* src[3];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[14]* src[2]+m[15]* src[3];

		src += 4 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix3us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y z - SHORT 3 */
	GLushort *src = (GLushort *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2]+m[ 3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2]+m[ 7];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2]+m[11];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[14]* src[2]+m[15];

		src += 3 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix2us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y - USHORT 2 */
	GLushort *src = (GLushort *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 3];
		dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 7];
		dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[11];
		dst[3] = m[12]* src[0]+ m[13]* src[1]+m[15];

		src += 2 + stride ;
		dst += 4 ; 
	}
}

#define F2F(x) FIXED_TO_FLOAT(x)
void MultMatrix4x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y z w - FIXED 4 */
	GLfixed *src = (GLfixed*)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* F2F(src[0])+ m[ 1]* F2F(src[1])+m[ 2]* F2F(src[2])+m[ 3]* F2F(src[3]);
		dst[1] = m[ 4]* F2F(src[0])+ m[ 5]* F2F(src[1])+m[ 6]* F2F(src[2])+m[ 7]* F2F(src[3]);
		dst[2] = m[ 8]* F2F(src[0])+ m[ 9]* F2F(src[1])+m[10]* F2F(src[2])+m[11]* F2F(src[3]);
		dst[3] = m[12]* F2F(src[0])+ m[13]* F2F(src[1])+m[14]* F2F(src[2])+m[15]* F2F(src[3]);

		src += 4 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix3x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y z - FIXED 3 */
	GLfixed *src = (GLfixed *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		dst[0] = m[ 0]* F2F(src[0])+ m[ 1]* F2F(src[1])+m[ 2]* F2F(src[2])+m[ 3];
		dst[1] = m[ 4]* F2F(src[0])+ m[ 5]* F2F(src[1])+m[ 6]* F2F(src[2])+m[ 7];
		dst[2] = m[ 8]* F2F(src[0])+ m[ 9]* F2F(src[1])+m[10]* F2F(src[2])+m[11];
		dst[3] = m[12]* F2F(src[0])+ m[13]* F2F(src[1])+m[14]* F2F(src[2])+m[15];

		src += 3 + stride ;
		dst += 4 ; 
	}
}

void MultMatrix2x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
	/* x y - FIXED 2 */
	GLfixed *src = (GLfixed *)src1; 
	
	int i = 0 ;
	for ( i = 0 ; i < count ; i++ ) 
	{
		/* dst = Mat * src */
		
		dst[0] = m[ 0]* F2F(src[0])+ m[ 1]* F2F(src[1])+m[ 3];
		dst[1] = m[ 4]* F2F(src[0])+ m[ 5]* F2F(src[1])+m[ 7];
		dst[2] = m[ 8]* F2F(src[0])+ m[ 9]* F2F(src[1])+m[11];
		dst[3] = m[12]* F2F(src[0])+ m[13]* F2F(src[1])+m[15];

		src += 2 + stride ;
		dst += 4 ; 
	}
}

#undef F2F

pFNMultMatrix GetMultMatrixFunc(int size, GLenum type)
{
	switch (type)
	{
	case GL_BYTE:
		switch (size)
		{
		case 2:
			return MULTFUNC(2,b);
		case 3:
			return MULTFUNC(3,b);
		case 4:
			return MULTFUNC(4,b);
		}
	case GL_UNSIGNED_BYTE:
		switch (size)
		{
		case 2:
			return MULTFUNC(2,ub);
		case 3:
			return MULTFUNC(3,ub);
		case 4:
			return MULTFUNC(4,ub);
		}
	case GL_SHORT:
		switch (size)
		{
		case 2:
			return MULTFUNC(2,s);
		case 3:
			return MULTFUNC(3,s);
		case 4:
			return MULTFUNC(4,s);
		}
	case GL_UNSIGNED_SHORT:
		switch (size)
		{
		case 2:
			return MULTFUNC(2,us);
		case 3:
			return MULTFUNC(3,us);
		case 4:
			return MULTFUNC(4,us);
		}
	case GL_FLOAT:
		switch (size)
		{
		case 2:
			return MULTFUNC(2,f);
		case 3:
			return MULTFUNC(3,f);
		case 4:
			return MULTFUNC(4,f);
		}
	case GL_FIXED:
		switch (size)
		{
		case 2:
			return MULTFUNC(2,x);
		case 3:
			return MULTFUNC(3,x);
		case 4:
			return MULTFUNC(4,x);
		}
	}
	return 0;
}
