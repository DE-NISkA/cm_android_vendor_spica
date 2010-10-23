#ifndef __INCLUDESHADERS_H__ 
#define __INCLUDESHADERS_H__ 
#include "includes.h" 
#include "Flat.vert.h"
#include "Flat.frag.h"
#include "Color.vert.h"
#include "Color.frag.h"
#include "Texture.vert.h"
#include "Texture.frag.h"
#include "TextureC.frag.h"
#include "TexCol.vert.h"
#include "TexCol.frag.h"
#include "TexColC.frag.h"
#include "TexMat.vert.h"
#include "TexMatCol.vert.h"
#include "TexMat2.vert.h"
#include "TexAlpha.vert.h"
#include "TexAlpha.frag.h"
#include "TexAdd.frag.h"
#include "Universal.vert.h"
#include "Universal.frag.h"
#include "MatPal.vert.h"
#include "Fog.vert.h"
//#include "./shader/Fog.frag.h"
#include "FogLin.frag.h"
#include "FogExp.frag.h"
#include "FogExp2.frag.h"

#ifdef __cplusplus
extern "C" {
#endif

	shaderUniformInfo shaderInfoUnifInfo[totalShaderCount] = {
		{ (void*) FlatFrag, FlatFragLength,
			(void*) FlatVert, FlatVertLength  },//0 sh_flat
		
		{ (void*) ColorFrag , ColorFragLength,
			(void*) ColorVert , ColorVertLength  },//1 sh_color
		
		{ (void*) TextureFrag, TextureFragLength,
			(void*) TextureVert, TextureVertLength  },//2 sh_texture
		
		{ (void*) TextureCFrag, TextureCFragLength,
			(void*) TextureVert , TextureVertLength  },//3 sh_texture_c
		
		{ (void*) TextureFrag, TextureFragLength,
			(void*) TexMatVert , TexMatVertLength  },//4 sh_tex_mat
		
		{ (void*) TexColFrag, TexColFragLength,
			(void*) TexColVert, TexColVertLength  },//5 sh_tex_col
		
		{ (void*) TexColCFrag, TexColCFragLength,
			(void*) TexColVert , TexColVertLength  },//6 sh_tex_col_c
		
		{ (void*) TexColFrag   , TexColFragLength,
			(void*) TexMatColVert, TexMatColVertLength  },//7 sh_tex_mat_col
		
		{ (void*) TexAlphaFrag, TexAlphaFragLength,
			(void*) TexAlphaVert, TexAlphaVertLength  },//8 sh_tex_alpha
		
		{ (void*) TexAddFrag  , TexAddFragLength,
			(void*) TexAlphaVert, TexAlphaVertLength  },//9 sh_tex_add
		
		{ (void*) TexAlphaFrag, TexAlphaFragLength,
			(void*) TexMat2Vert , TexMat2VertLength  },//10 sh_tex_alpha2
		
		{ (void*) TexAddFrag , TexAddFragLength,
			(void*) TexMat2Vert, TexMat2VertLength  },//11 sh_tex_add2
		
		{ (void*) UniversalFrag, UniversalFragLength,
		(void*) UniversalVert, UniversalVertLength  },//12 sh_universal			
		
		{ (void*) TexColFrag,    TexColFragLength,
			(void*) UniversalVert, UniversalVertLength  },//13 sh_unitexcol
		
		{ (void*) ColorFrag,     ColorFragLength,
			(void*) UniversalVert, UniversalVertLength  },//14 sh_unicol
		
		{ (void*) TexColFrag, TexColFragLength,
			(void*) MatPalVert, MatPalVertLength  },//15 sh_mat_pal
		
		{ (void*) FogLinFrag, FogLinFragLength,
			(void*) FogVert, FogVertLength  },//16 sh_fog_lin
		
		{ (void*) FogExpFrag, FogExpFragLength,
			(void*) FogVert, FogVertLength  },//17 sh_fog_exp
		
		{ (void*) FogExp2Frag, FogExp2FragLength,
			(void*) FogVert, FogVertLength  }//18 sh_fog_exp2
	};

#ifdef __cplusplus
}
#endif
#endif