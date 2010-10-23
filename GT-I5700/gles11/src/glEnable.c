#include "includes.h"
#include "attribs.h"
#include "OGL2Func_for_gles11.h"

extern GLuint shaderProgram;

GL_API void GL_APIENTRY glDisable (GLenum cap)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glDisable(0x%x)\n",cap));

	//DBGFUNCTION_INIT("glDisable ")
	//RETAILMSG(1,(TEXT("glDisable  \r\n")));
	switch (cap)
	{
	case GL_FOG                           :
		ctx->flags &= ~ F_FOG             ;
		break ;
	case GL_LIGHTING                      :
		ctx->flags &= ~ F_LIGHTING        ;
		break;
	case GL_TEXTURE_2D                    :
		if ( ctx->activeTexture == 0 )
		{
			ctx->flags &= ~ F_TEXTURE_2D_UNIT0;
		}
		else
		{
			ctx->flags &= ~ F_TEXTURE_2D_UNIT1;
		}
		break;
	case GL_CULL_FACE                     :
		ctx->flags &= ~ F_CULL_FACE         ;
		OGL2_glDisable(GL_CULL_FACE);
		break;
	case GL_ALPHA_TEST                    :
		ctx->flags &= ~ F_ALPHA_TEST        ;
		OGL2_glDisable(GL_ALPHA_TEST);
		break;
	case GL_BLEND                         :
		ctx->flags &= ~ F_BLEND             ;
		OGL2_glDisable(GL_BLEND);
		break;
	case GL_COLOR_LOGIC_OP                :
		ctx->flags &= ~ F_COLOR_LOGIC_OP    ;
		OGL2_glDisable(GL_COLOR_LOGIC_OP);
		break;
	case GL_DITHER                        :
		ctx->flags &= ~ F_DITHER            ;
		OGL2_glDisable(GL_DITHER);
		break;
	case GL_STENCIL_TEST                  :
		ctx->flags &= ~ F_STENCIL_TEST      ;
		OGL2_glDisable(GL_STENCIL_TEST);
		break;
	case GL_DEPTH_TEST                    :
		ctx->flags &= ~ F_DEPTH_TEST        ;
		OGL2_glDisable(GL_DEPTH_TEST);
		break;
	case GL_LIGHT0 :
	case GL_LIGHT1 :
	case GL_LIGHT2 :
	case GL_LIGHT3 :
	case GL_LIGHT4 :
	case GL_LIGHT5 :
	case GL_LIGHT6 :
	case GL_LIGHT7 :
		ctx->lflags[cap - GL_LIGHT0] = 0;
		break;
	case GL_CLIP_PLANE0 :
	case GL_CLIP_PLANE1 :
	case GL_CLIP_PLANE2 :
	case GL_CLIP_PLANE3 :
	case GL_CLIP_PLANE4 :
	case GL_CLIP_PLANE5 :
		ctx->cflags[cap - GL_CLIP_PLANE0] = 0 ;
		break ;
	case GL_POINT_SMOOTH                  :
		ctx->flags &= ~ F_POINT_SMOOTH      ;
		break;
	case GL_LINE_SMOOTH                   :
		ctx->flags &= ~ F_LINE_SMOOTH       ;
		break;
	case GL_SCISSOR_TEST                  :
		ctx->flags &= ~ F_SCISSOR_TEST      ;
		OGL2_glDisable(GL_SCISSOR_TEST);
		break;
	case GL_COLOR_MATERIAL                :
		ctx->flags &= ~ F_COLOR_MATERIAL    ;
		break;
	case GL_NORMALIZE                     :
		ctx->flags &= ~ F_NORMALIZE         ;
		break;
	case GL_RESCALE_NORMAL                :
		ctx->flags &= ~ F_RESCALE_NORMAL    ;
		break;
	case GL_POLYGON_OFFSET_FILL           :
		ctx->flags &= ~ F_POLYGON_OFFSET_FILL;
		OGL2_glDisable(GL_POLYGON_OFFSET_FILL);
		break;
	case GL_MULTISAMPLE                   :
		ctx->flags &= ~ F_MULTISAMPLE       ;
		break;
	case GL_SAMPLE_ALPHA_TO_COVERAGE      :
		ctx->flags &= ~ F_SAMPLE_ALPHA_TO_COVERAGE;
		break;
	case GL_SAMPLE_ALPHA_TO_ONE           :
		ctx->flags &= ~ F_SAMPLE_ALPHA_TO_ONE;
		break;
	case GL_SAMPLE_COVERAGE               :
		ctx->flags &= ~ F_SAMPLE_COVERAGE   ;
		OGL2_glDisable(GL_SAMPLE_COVERAGE);
		break;
	case GL_MATRIX_PALETTE_OES       :
		ctx->flags &= ~ F_MATRIX_PALETTE_OES ;
		break;
	case GL_POINT_SPRITE_OES              :
		ctx->flagspt &= ~ F_POINT_SPRITE_OES;
		OGL2_glDisable(GL_POINT_SPRITE_OES);
		break;
	}

	//DBGFUNCTION_EXIT("glDisable ")
}

GL_API void GL_APIENTRY glEnableClientState(GLenum array)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glEnableClientState(0x%x)\n",array));

	switch(array)
	{
	case GL_VERTEX_ARRAY:
		OGL2_glEnableVertexAttribArray( ATTRIB_LOC_Vertex );
		break;
	case GL_NORMAL_ARRAY:
	OGL2_glEnableVertexAttribArray( ATTRIB_LOC_Normal);
		break;
	case GL_COLOR_ARRAY:
		ctx->colorEnabled=1;
		OGL2_glEnableVertexAttribArray( ATTRIB_LOC_Color );
		break;
	case GL_TEXTURE_COORD_ARRAY:
		switch (ctx->clientActiveTex)
		{
		case 0:
			OGL2_glEnableVertexAttribArray( ATTRIB_LOC_MultiTexCoord[0]);
			break;
		case 1:
			OGL2_glEnableVertexAttribArray( ATTRIB_LOC_MultiTexCoord[1]);
			break;
		}
		break;
	case GL_POINT_SIZE_ARRAY_OES:
		OGL2_glEnableVertexAttribArray(ATTRIB_LOC_PointSize);
		ctx->pointSizeArrayEnable = 1;
		break;
	case GL_WEIGHT_ARRAY_OES:
		OGL2_glEnableVertexAttribArray (ATTRIB_LOC_Weights);
		break;
	case GL_MATRIX_INDEX_ARRAY_OES:
		OGL2_glEnableVertexAttribArray(ATTRIB_LOC_MatrixIndices);
		break;

	}
}

GL_API void GL_APIENTRY glDisableClientState(GLenum array)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glDisableClientState(0x%x)\n",array));

	switch(array)
	{
	case GL_VERTEX_ARRAY:
		OGL2_glDisableVertexAttribArray( ATTRIB_LOC_Vertex );
		break;
	case GL_NORMAL_ARRAY:
		OGL2_glDisableVertexAttribArray( ATTRIB_LOC_Normal);
		break;
	case GL_COLOR_ARRAY:
		ctx->colorEnabled=0;
		OGL2_glDisableVertexAttribArray( ATTRIB_LOC_Color );
		break;
	case GL_TEXTURE_COORD_ARRAY:
		switch (ctx->clientActiveTex)
		{
		case 0:
			OGL2_glDisableVertexAttribArray( ATTRIB_LOC_MultiTexCoord[0]);
			break;
		case 1:
			OGL2_glDisableVertexAttribArray( ATTRIB_LOC_MultiTexCoord[1]);
			break;
		}
		break;
	case GL_POINT_SIZE_ARRAY_OES:
		OGL2_glDisableVertexAttribArray( ATTRIB_LOC_PointSize );
		ctx->pointSizeArrayEnable = 0;
		break;
	case GL_WEIGHT_ARRAY_OES:
		OGL2_glDisableVertexAttribArray (ATTRIB_LOC_Weights);
		break;
	case GL_MATRIX_INDEX_ARRAY_OES:
		OGL2_glDisableVertexAttribArray(ATTRIB_LOC_MatrixIndices);
		break;
	}
}

GL_API void GL_APIENTRY glEnable (GLenum cap)
{
	GET_GLES_CONTEXT

	//GL11LOGMSG(LOG_GLES11_API,("glEnable(0x%x)\n",cap));

	//DBGFUNCTION_INIT("glEnable ")
	//RETAILMSG(1,(TEXT("glEnable  \r\n")));
	switch (cap)
	{
	case GL_FOG                           :
		ctx->flags |= F_FOG               ;
		break ;
	case GL_LIGHTING                      :
		ctx->flags |= F_LIGHTING          ;
		break;
	case GL_TEXTURE_2D                    :
		if ( ctx->activeTexture == 0 )
		{
			ctx->flags |= F_TEXTURE_2D_UNIT0;
		}
		else
		{
			ctx->flags |= F_TEXTURE_2D_UNIT1;
		}
		break;
	case GL_CULL_FACE                     :
		ctx->flags |= F_CULL_FACE         ;
		OGL2_glEnable(GL_CULL_FACE);
		break;
	case GL_ALPHA_TEST                    :
		ctx->flags |= F_ALPHA_TEST        ;
		OGL2_glEnable(GL_ALPHA_TEST);
		break;
	case GL_BLEND                         :
		ctx->flags |= F_BLEND             ;
		OGL2_glEnable(GL_BLEND);
		break;
	case GL_COLOR_LOGIC_OP                :
		ctx->flags |= F_COLOR_LOGIC_OP    ;
		OGL2_glEnable(GL_COLOR_LOGIC_OP);
		break;
	case GL_DITHER                        :
		ctx->flags |= F_DITHER            ;
		OGL2_glEnable(GL_DITHER);
		break;
	case GL_STENCIL_TEST                  :
		ctx->flags |= F_STENCIL_TEST      ;
		OGL2_glEnable(GL_STENCIL_TEST);
		break;
	case GL_DEPTH_TEST                    :
		ctx->flags |= F_DEPTH_TEST        ;
		OGL2_glEnable(GL_DEPTH_TEST);
		break;
	case GL_LIGHT0 :
	case GL_LIGHT1 :
	case GL_LIGHT2 :
	case GL_LIGHT3 :
	case GL_LIGHT4 :
	case GL_LIGHT5 :
	case GL_LIGHT6 :
	case GL_LIGHT7 :
		ctx->lflags[cap - GL_LIGHT0] = 1;
		break;
	case GL_CLIP_PLANE0 :
	case GL_CLIP_PLANE1 :
	case GL_CLIP_PLANE2 :
	case GL_CLIP_PLANE3 :
	case GL_CLIP_PLANE4 :
	case GL_CLIP_PLANE5 :
		ctx->cflags[cap - GL_CLIP_PLANE0] = 1 ;
		break ;
	case GL_POINT_SMOOTH                  :
		ctx->flags |= F_POINT_SMOOTH      ;
		break;
	case GL_LINE_SMOOTH                   :
		ctx->flags |= F_LINE_SMOOTH       ;
		break;
	case GL_SCISSOR_TEST                  :
		ctx->flags |= F_SCISSOR_TEST      ;
		OGL2_glEnable(GL_SCISSOR_TEST);
		break;
	case GL_COLOR_MATERIAL                :
		ctx->flags |= F_COLOR_MATERIAL    ;
		break;
	case GL_NORMALIZE                     :
		ctx->flags |= F_NORMALIZE         ;
		break;
	case GL_RESCALE_NORMAL                :
		ctx->flags |= F_RESCALE_NORMAL    ;
		break;
	case GL_POLYGON_OFFSET_FILL           :
		ctx->flags |= F_POLYGON_OFFSET_FILL;
		OGL2_glEnable(GL_POLYGON_OFFSET_FILL);
		break;
	case GL_MULTISAMPLE                   :
		ctx->flags |= F_MULTISAMPLE       ;
		break;
	case GL_SAMPLE_ALPHA_TO_COVERAGE      :
		ctx->flags |= F_SAMPLE_ALPHA_TO_COVERAGE;
		OGL2_glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		break;
	case GL_SAMPLE_ALPHA_TO_ONE           :
		ctx->flags |= F_SAMPLE_ALPHA_TO_ONE;
		break;
	case GL_SAMPLE_COVERAGE               :
		ctx->flags |= F_SAMPLE_COVERAGE   ;
		OGL2_glEnable(GL_SAMPLE_COVERAGE);
		break;
	case GL_MATRIX_PALETTE_OES       :
		ctx->flags |= F_MATRIX_PALETTE_OES ;
		break;
	case GL_POINT_SPRITE_OES              :
		ctx->flagspt |= F_POINT_SPRITE_OES;
		OGL2_glEnable(GL_POINT_SPRITE_OES);
		break;
	}

	//DBGFUNCTION_EXIT("glEnable ")
}

GL_API GLboolean GL_APIENTRY glIsEnabled (GLenum cap)
{
	GLint enabled = 0 ;
	GET_GLES_CONTEXT_RETURN

	//GL11LOGMSG(LOG_GLES11_API,("glIsEnabled(0x%x)\n",cap));

	//DBGFUNCTION_INIT("glIsEnabled ")
	//RETAILMSG(1,(TEXT("glIsEnabled  \r\n")));
	switch (cap)
	{
	case GL_POINT_SIZE_ARRAY_OES:
		enabled = ctx->pointSizeArrayEnable ;
		break ;
	case GL_FOG                           :
		enabled = (ctx->flags & F_FOG               );
		break;
	case GL_LIGHTING                      :
		enabled = (ctx->flags & F_LIGHTING );
		break;
	case GL_TEXTURE_2D                    :
		if ( ctx->activeTexture == 0 )
		{
			enabled = (ctx->flags & F_TEXTURE_2D_UNIT0);
		}
		else
		{
			enabled = (ctx->flags & F_TEXTURE_2D_UNIT1);
		}
		break;
	case GL_CULL_FACE                     :
		enabled = OGL2_glIsEnabled(GL_CULL_FACE);
		break;
	case GL_ALPHA_TEST                    :
		enabled = OGL2_glIsEnabled(GL_ALPHA_TEST);
		break;
	case GL_BLEND                         :
		enabled = OGL2_glIsEnabled(GL_BLEND);
		break;
	case GL_COLOR_LOGIC_OP                :
		enabled = OGL2_glIsEnabled(GL_COLOR_LOGIC_OP);
		break;
	case GL_DITHER                        :
		enabled = OGL2_glIsEnabled(GL_DITHER);
		break;
	case GL_STENCIL_TEST                  :
		enabled = OGL2_glIsEnabled(GL_STENCIL_TEST);
		break;
	case GL_DEPTH_TEST                    :
		enabled = OGL2_glIsEnabled(GL_DEPTH_TEST);
		break;
	case GL_LIGHT0 :
	case GL_LIGHT1 :
	case GL_LIGHT2 :
	case GL_LIGHT3 :
	case GL_LIGHT4 :
	case GL_LIGHT5 :
	case GL_LIGHT6 :
	case GL_LIGHT7 :
		enabled = (ctx->lflags[cap - GL_LIGHT0] );
		break;
	case GL_CLIP_PLANE0 :
	case GL_CLIP_PLANE1 :
	case GL_CLIP_PLANE2 :
	case GL_CLIP_PLANE3 :
	case GL_CLIP_PLANE4 :
	case GL_CLIP_PLANE5 :
		enabled = (ctx->cflags[cap - GL_CLIP_PLANE0] );
		break;
	case GL_POINT_SMOOTH                  :
		enabled = (ctx->flags & F_POINT_SMOOTH      );
		break;
	case GL_LINE_SMOOTH                   :
		enabled = (ctx->flags & F_LINE_SMOOTH       );
		break;
	case GL_SCISSOR_TEST                  :
		enabled = OGL2_glIsEnabled(GL_SCISSOR_TEST);
		break;
	case GL_COLOR_MATERIAL                :
		enabled = (ctx->flags & F_COLOR_MATERIAL    );
		break;
	case GL_NORMALIZE                     :
		enabled = (ctx->flags & F_NORMALIZE         );
		break;
	case GL_RESCALE_NORMAL                :
		enabled = (ctx->flags & F_RESCALE_NORMAL    );
		break;
	case GL_POLYGON_OFFSET_FILL           :
		enabled = OGL2_glIsEnabled(GL_POLYGON_OFFSET_FILL);
		break;
	case GL_MULTISAMPLE                   :
		enabled = (ctx->flags & F_MULTISAMPLE       );
		break;
	case GL_SAMPLE_ALPHA_TO_COVERAGE      :
		enabled = OGL2_glIsEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE);
		break;
	case GL_SAMPLE_ALPHA_TO_ONE           :
		enabled = (ctx->flags & F_SAMPLE_ALPHA_TO_ONE);
		break;
	case GL_SAMPLE_COVERAGE               :
		enabled = OGL2_glIsEnabled(GL_SAMPLE_COVERAGE);
		break;
	case GL_MATRIX_PALETTE_OES  :
		enabled = (ctx->flags & F_MATRIX_PALETTE_OES  );
		break;
	case GL_POINT_SPRITE_OES              :
		enabled = OGL2_glIsEnabled(GL_POINT_SPRITE_OES);
		break;
	default:
		GetStateAttrib(ctx, cap , 0 , CopyToInt, CopyToIntE, CopyToIntS, &enabled ) ;
		break ;
	}
	//DBGFUNCTION_EXIT("glIsEnabled ")
	if ( enabled )
	{
		return GL_TRUE ;
	}
	else
	{
		return GL_FALSE ;
	}
}
