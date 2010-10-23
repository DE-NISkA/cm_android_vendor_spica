#include "includes.h"

void MultMatrix4f(GLfloat * m, GLvoid *src1, GLfloat *dst, GLint stride, GLint count ) ;

GL_API void GL_APIENTRY glClipPlanef (GLenum plane, const GLfloat *equation)
{
        GET_GLES_CONTEXT

        //DBGFUNCTION_INIT("glClipPlanef ")
        //RETAILMSG(1,(TEXT("glClipPlanef  \r\n")));

        CHECK_GL_ENUM(((plane < GL_CLIP_PLANE0) ||(plane > GL_CLIP_PLANE5)))
        InitMatrices(ctx); 
        MultMatrix4f(ctx->wa.invModelView, (GLfloat *)equation,  ctx->clipPlanes[plane - GL_CLIP_PLANE0] ,0, 1);

        //DBGFUNCTION_EXIT("glClipPlanef ")
}

GL_API void GL_APIENTRY glGetClipPlanef (GLenum pname, GLfloat eqn[4])
{
        GET_GLES_CONTEXT

        //DBGFUNCTION_INIT("glGetClipPlanef ")
        //RETAILMSG(1,(TEXT("glGetClipPlanef  \r\n")));

        CHECK_GL_ENUM(((pname < GL_CLIP_PLANE0) ||(pname > GL_CLIP_PLANE5)))
        memcpy(eqn , ctx->clipPlanes[pname - GL_CLIP_PLANE0], sizeof(GLfloat )* 4 ) ;
        
        //DBGFUNCTION_EXIT("glGetClipPlanef ")
}

GL_API void GL_APIENTRY glClipPlanex (GLenum plane, const GLfixed *equation)
{
        GET_GLES_CONTEXT

        //DBGFUNCTION_INIT("glClipPlanex ")
        //RETAILMSG(1,(TEXT("glClipPlanex  \r\n")));

        CHECK_GL_ENUM(((plane < GL_CLIP_PLANE0) ||(plane > GL_CLIP_PLANE5)))

        ctx->clipPlanes[plane - GL_CLIP_PLANE0][0]=FIXED_TO_FLOAT( equation [0] );      
        ctx->clipPlanes[plane - GL_CLIP_PLANE0][1]=FIXED_TO_FLOAT( equation [1] );      
        ctx->clipPlanes[plane - GL_CLIP_PLANE0][2]=FIXED_TO_FLOAT( equation [2] );      
        ctx->clipPlanes[plane - GL_CLIP_PLANE0][3]=FIXED_TO_FLOAT( equation [3] );      

        //DBGFUNCTION_EXIT("glClipPlanex ")
}

GL_API void GL_APIENTRY glGetClipPlanex (GLenum pname, GLfixed eqn[4])
{
        GET_GLES_CONTEXT

        //DBGFUNCTION_INIT("glGetClipPlanex ")
        //RETAILMSG(1,(TEXT("glGetClipPlanex  \r\n")));
        CHECK_GL_ENUM(((pname < GL_CLIP_PLANE0) ||(pname > GL_CLIP_PLANE5)))
        
        eqn [0] =FLOAT_TO_FIXED(ctx->clipPlanes[pname - GL_CLIP_PLANE0][0]);    
        eqn [1] =FLOAT_TO_FIXED(ctx->clipPlanes[pname - GL_CLIP_PLANE0][1]);    
        eqn [2] =FLOAT_TO_FIXED(ctx->clipPlanes[pname - GL_CLIP_PLANE0][2]);    
        eqn [3] =FLOAT_TO_FIXED(ctx->clipPlanes[pname - GL_CLIP_PLANE0][3]);    

        //DBGFUNCTION_EXIT("glGetClipPlanex ")
}
