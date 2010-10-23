#include "includes.h"

#define MULTFUNC(s,t) MultMatrixNor##s##t
#define MAT(m,r,c) (m)[(r)*4+(c)]

void MultMatrixNor3f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
        /* x y z - FLOAT 3 */
        GLfloat *src = (GLfloat *)src1; 
        
        int i = 0 ;
        for ( i = 0 ; i < count ; i++ ) 
        {
                /* dst = Mat * src */
                dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2];
                dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2];
                dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2];
                        
                src += 3 + stride ;
                dst += 3; 
        }
}

void MultMatrixNor3b(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
        /* x y z - BYTE 3 */
        GLbyte *src = (GLbyte *)src1; 
        
        int i = 0 ;
        for ( i = 0 ; i < count ; i++ ) 
        {
                /* dst = Mat * src */
                dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2];
                dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2];
                dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2];
                
                src += 3 + stride ;
                dst += 3; 
        }
}

void MultMatrixNor3ub(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
        /* x y z - UBYTE 3 */
        GLubyte *src = (GLubyte *)src1; 
        
        int i = 0 ;
        for ( i = 0 ; i < count ; i++ ) 
        {
                /* dst = Mat * src */
                dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2];
                dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2];
                dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2];
                
                src += 3 + stride ;
                dst += 3; 
        }
}

void MultMatrixNor3s(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
        /* x y z - SHORT 3 */
        GLshort *src = (GLshort *)src1; 
        
        int i = 0 ;
        for ( i = 0 ; i < count ; i++ ) 
        {
                /* dst = Mat * src */
                dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2];
                dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2];
                dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2];
                
                src += 3 + stride ;
                dst += 3; 
        }
}

void MultMatrixNor3us(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
        /* x y z - SHORT 3 */
        GLushort *src = (GLushort *)src1; 
        
        int i = 0 ;
        for ( i = 0 ; i < count ; i++ ) 
        {
                /* dst = Mat * src */
                dst[0] = m[ 0]* src[0]+ m[ 1]* src[1]+m[ 2]* src[2];
                dst[1] = m[ 4]* src[0]+ m[ 5]* src[1]+m[ 6]* src[2];
                dst[2] = m[ 8]* src[0]+ m[ 9]* src[1]+m[10]* src[2];
                
                src += 3 + stride ;
                dst += 3; 
        }
}

#define F2F FIXED_TO_FLOAT
void MultMatrixNor3x(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) 
{
        /* x y z - FIXED 3 */
        GLfixed *src = (GLfixed *)src1; 
        
        int i = 0 ;
        for ( i = 0 ; i < count ; i++ ) 
        {
                /* dst = Mat * src */
                dst[0] = m[ 0]* F2F(src[0])+ m[ 1]* F2F(src[1])+m[ 2]* F2F(src[2]);
                dst[1] = m[ 4]* F2F(src[0])+ m[ 5]* F2F(src[1])+m[ 6]* F2F(src[2]);
                dst[2] = m[ 8]* F2F(src[0])+ m[ 9]* F2F(src[1])+m[10]* F2F(src[2]);
                
                src += 3 + stride ;
                dst += 3; 
        }
}

#undef F2F

pFNMultMatrix GetMultMatrixNorFunc(int size, GLenum type)
{
        switch (type)
        {
        case GL_BYTE:
                        return MULTFUNC(3,b);
        case GL_UNSIGNED_BYTE:
                        return MULTFUNC(3,ub);
        case GL_SHORT:
                        return MULTFUNC(3,s);
        case GL_UNSIGNED_SHORT:
                        return MULTFUNC(3,us);
        case GL_FLOAT:
                        return MULTFUNC(3,f);
        case GL_FIXED:
                        return MULTFUNC(3,x);
        }
        return 0;
}
