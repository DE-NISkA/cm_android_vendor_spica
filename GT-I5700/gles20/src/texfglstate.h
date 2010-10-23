
#ifndef __TEXFGLSTATE_H__
#define __TEXFGLSTATE_H__


#include "register.h"
#include "macros.h"
#include "fgl.h"

#ifdef __DEBUG
#define _DEBUG printf
#else
#define _DEBUG (void)
#endif


//offset of the various texture registers for a given unit.

#define FGTO_CTRL                       ((FGTU_TEX0_CTRL - FGTU_TEX0_CTRL)/4)
#define FGTO_USIZE                      ((FGTU_TEX0_USIZE - FGTU_TEX0_CTRL)/4)     
#define FGTO_VSIZE              ((FGTU_TEX0_VSIZE - FGTU_TEX0_CTRL)/4)     
#define FGTO_PSIZE                      ((FGTU_TEX0_PSIZE - FGTU_TEX0_CTRL) /4)     
#define FGTO_L1_OFFSET          ((FGTU_TEX0_L1_OFFSET -FGTU_TEX0_CTRL)/4)      
/*

#define FGTO_L2_OFFSET          (FGTU_TEX0_L1_OFFSET - FGTU_TEX0_CTRL)   

#define FGTO_L3_OFFSET          (FGTU_TEX0_L1_OFFSET - FGTU_TEX0_CTRL)     

#define FGTO_L4_OFFSET          (FGTU_TEX0_L1_OFFSET - FGTU_TEX0_CTRL)      

#define FGTO_L5_OFFSET          (FGTU_TEX0_L1_OFFSET - FGTU_TEX0_CTRL)      

#define FGTO_L6_OFFSET          (FGTU_TEX0_L1_OFFSET - FGTU_TEX0_CTRL)   

#define FGTO_L7_OFFSET          (FGTU_TEX0_L1_OFFSET - FGTU_TEX0_CTRL)     

#define FGTO_L8_OFFSET          (FGTU_TEX0_L1_OFFSET - FGTU_TEX0_CTRL)     

#define FGTO_L9_OFFSET          (FGTU_TEX0_L1_OFFSET - FGTU_TEX0_CTRL)    

#define FGTO_L10_OFFSET         (FGTU_TEX0_L1_OFFSET - FGTU_TEX0_CTRL)   

#define FGTO_L11_OFFSET (FGTU_TEX0_L1_OFFSET - FGTU_TEX0_CTRL)  */
#define FGTO_MIN_LEVEL          ((FGTU_TEX0_MIN_LEVEL - FGTU_TEX0_CTRL)/4) 
/*

#define FGTO_MAX_LEVEL          (FGTU_TEX0_MAX_LEVEL - FGTU_TEX0_CTRL)  */   
#define FGTO_BASE_ADDR          ((FGTU_TEX0_BASE_ADDR - FGTU_TEX0_CTRL)/4)      



/*****************************************************************************

  DEFINES

 *****************************************************************************/

#define FGL_TEXTURE_TYPE_VALIDATOR  (                                                           \
                                                                                (1 << FGL_TEX_2D)           |   \
                                                                                (1 << FGL_TEX_CUBE)         |   \
                                                                                (1 << FGL_TEX_3D)               \
                                                                        )

#define FGL_COLOR_KEY_VALIDATOR (                                                                       \
                                                                        (1 << FGL_CKEY_DISABLE)     |   \
                                                                        (1 << FGL_CKEY_SEL1)        |   \
                                                                        (1 << FGL_CKEY_DISABLE2)    |   \
                                                                        (1 << FGL_CKEY_SEL2)                \
                                                                )

#define FGL_PALETTE_FORMAT_VALIDATOR (                                                                  \
                                                                                (1 << FGL_PALETTE_ARGB1555)     |       \
                                                                                (1 << FGL_PALETTE_RGB565  )     |       \
                                                                                (1 << FGL_PALETTE_ARGB4444)     |       \
                                                                                (1 << FGL_PALETTE_ARGB8888)             \
                                                                         )

#define FGL_TEXTURE_FORMAT_VALIDATOR (                                                                  \
                                                                                (1 << FGL_TEXEL_ARGB1555)       |       \
                                                                                (1 << FGL_TEXEL_RGB565  )       |       \
                                                                                (1 << FGL_TEXEL_ARGB4444)       |       \
                                                                                (1 << FGL_TEXEL_DEPTH24 )       |       \
                                                                                (1 << FGL_TEXEL_IA88    )       |       \
                                                                                (1 << FGL_TEXEL_I8      )       |       \
                                                                                (1 << FGL_TEXEL_ARGB8888)       |       \
                                                                                (1 << FGL_TEXEL_1BPP    )       |       \
                                                                                (1 << FGL_TEXEL_2BPP    )       |       \
                                                                                (1 << FGL_TEXEL_4BPP    )       |       \
                                                                                (1 << FGL_TEXEL_8BPP    )       |       \
                                                                                (1 << FGL_TEXEL_S3TC    )       |       \
                                                                                (1 << FGL_TEXEL_Y1VY0U  )       |       \
                                                                                (1 << FGL_TEXEL_VY1UY0  )       |       \
                                                                                (1 << FGL_TEXEL_Y1UY0V  )       |       \
                                                                                (1 << FGL_TEXEL_UY1VY0  )               \
                                                                        )

#define FGL_WRAP_MODE_VALIDATOR (                                                                       \
                                                                        (1 << FGL_TEX_WRAP_REPEAT )     |       \
                                                                        (1 << FGL_TEX_WRAP_FLIP   )     |       \
                                                                        (1 << FGL_TEX_WRAP_CLAMP  )             \
                                                                )

#define FGL_MIPMAP_FILTER_VALIDATOR     (                                                                   \
                                                                            (1 << FGL_FILTER_DISABLE)   |       \
                                                                            (1 << FGL_FILTER_NEAREST)   |       \
                                                                                (1 << FGL_FILTER_LINEAR)                \
                                                                        )


#define TEXTURE_TYPE_VALID(type)                            \
                FGL_ENUM_IN_SET(type, FGL_TEXTURE_TYPE_VALIDATOR)

#define COLOR_KEY_VALID(key)                                                            \
                FGL_ENUM_IN_SET(key, FGL_COLOR_KEY_VALIDATOR)

#define PALETTE_FORMAT_VALID(format)                                            \
                FGL_ENUM_IN_SET(format, FGL_PALETTE_FORMAT_VALIDATOR)

#define TEXTURE_FORMAT_VALID(format)                                            \
                FGL_ENUM_IN_SET(format, FGL_TEXTURE_FORMAT_VALIDATOR)

#define WRAP_MODE_VALID(mode)                                                           \
                FGL_ENUM_IN_SET(mode, FGL_WRAP_MODE_VALIDATOR)

#define MIPMAP_FILTER_VALID(filter)                         \
                FGL_ENUM_IN_SET(filter, FGL_MIPMAP_FILTER_VALIDATOR)

typedef  unsigned int * wptr;
#define WRITEMEM(Port, X)       (*((wptr ) (Port)) = (unsigned int) (X))


inline FGL_Error
setTexStatusParam(unsigned int* pTexObjStateBaseAddr,  FGL_TexStatusParams *params)
{
        unsigned int texStatusR;
        unsigned int *pTexRegBaseAddr;

        pTexRegBaseAddr = (unsigned int *) (pTexObjStateBaseAddr + FGTO_CTRL);


#ifdef _FGL_VALUE_CHECK
        if (!TEXTURE_TYPE_VALID(params->eType)
                                                || !COLOR_KEY_VALID(params->eColorKey)
                                                || !PALETTE_FORMAT_VALID(params->ePaletteFormat)
                                                || !TEXTURE_FORMAT_VALID(params->eFormat)
                                                || !WRAP_MODE_VALID(params->eUMode)
                                                || !WRAP_MODE_VALID(params->eVMode)
                                                || !MIPMAP_FILTER_VALID(params->eMipMapFilter))
        {
                return FGL_ERR_INVALID_VALUE;
        }
#endif
        texStatusR = ((params->eType) << TEXCTRL_TEXTURE_TYPE_SHIFT)
                                | ((params->eColorKey) << TEXCTRL_COLOR_KEY_SHIFT)
                                | ((params->bUseExpansion) << TEXCTRL_EXPANSION_SHIFT)
                                | ((params->ePaletteFormat) << TEXCTRL_PALETTE_FORMAT_SHIFT)
                                | ((params->eFormat) << TEXCTRL_TEXTURE_FORMAT_SHIFT)
                                | ((params->eUMode) << TEXCTRL_U_WRAPMODE_SHIFT)
                                | ((params->eVMode) << TEXCTRL_V_WRAPMODE_SHIFT)
                                | ((params->bIsNonparametric) << TEXCTRL_NONPARAMETRIC_SHIFT)
                                | ((params->bUseMagFilter) << TEXCTRL_MAG_FILTER_SHIFT)
                                | ((params->bUseMinFilter) << TEXCTRL_MIN_FILTER_SHIFT)
                                | (params->eMipMapFilter);

        WRITEMEM(pTexRegBaseAddr, texStatusR);
                
        return FGL_ERR_NO_ERROR;
}




inline FGL_Error
setTexUSize(unsigned int* pTexObjStateBaseAddr, unsigned int uSize)
{
        unsigned int *pTexRegBaseAddr;

#ifdef _FGL_VALUE_CHECK
        if (uSize > MAXVALUE_TU_TEX_SIZE)
        {
                return FGL_ERR_INVALID_SIZE;
        }
#endif

        pTexRegBaseAddr = (unsigned int *)( pTexObjStateBaseAddr + FGTO_USIZE);
        
        WRITEMEM(pTexRegBaseAddr, uSize);
                
        return FGL_ERR_NO_ERROR;
}




inline FGL_Error
setTexVSize(unsigned int* pTexObjStateBaseAddr, unsigned int vSize)
{
        unsigned int *pTexRegBaseAddr;

#ifdef _FGL_VALUE_CHECK
        if (vSize > MAXVALUE_TU_TEX_SIZE)
        {
                return FGL_ERR_INVALID_SIZE;
        }
#endif

        pTexRegBaseAddr = (unsigned int *)(pTexObjStateBaseAddr + FGTO_VSIZE) ;
        
        WRITEMEM(pTexRegBaseAddr, vSize);
                
        return FGL_ERR_NO_ERROR;
}



inline FGL_Error
setTexPSize(unsigned int* pTexObjStateBaseAddr, unsigned int pSize)
{
        unsigned int *pTexRegBaseAddr;

#ifdef _FGL_VALUE_CHECK
        if (pSize > MAXVALUE_TU_TEX_SIZE)
        {
                return FGL_ERR_INVALID_SIZE;
        }
#endif

    pTexRegBaseAddr = (unsigned int *)(pTexObjStateBaseAddr + FGTO_PSIZE);


        WRITEMEM(pTexRegBaseAddr, pSize);
                
        return FGL_ERR_NO_ERROR;
}




static FGL_TexWrapMode
TexWrapMode (GLenum wrap)
{
        FGL_TexWrapMode result = (FGL_TexWrapMode) 0;

        switch(wrap) {

        case GL_REPEAT:
                result = FGL_TEX_WRAP_REPEAT;
                break;

        case GL_CLAMP_TO_EDGE:
                result = FGL_TEX_WRAP_CLAMP;
                break;

        case GL_MIRRORED_REPEAT:
                result = FGL_TEX_WRAP_FLIP;
                break;

        default:
                _DEBUG("ERROR: unexpected wrap mode!\n");
        }

        return result;
}





inline FGL_Error
calculateMipmapOffset(
                                        unsigned int* pTexObjStateBaseAddr,
                                        unsigned int uUSize,
                                        unsigned int uVSize,
                    unsigned int *maxLev
                )
{
        unsigned int uTexMipMapSize;
        unsigned int uCheckSize;
        
        unsigned int* pTexRegBaseAddr;
        
        unsigned int i;

#ifdef _FGL_VALUE_CHECK
        if ((uUSize > MAXVALUE_TU_TEX_SIZE) || (uVSize > MAXVALUE_TU_TEX_SIZE))
        {
                return FGL_ERR_INVALID_SIZE;
        }
#endif


        pTexRegBaseAddr = (unsigned int *)(pTexObjStateBaseAddr + FGTO_L1_OFFSET);

        uTexMipMapSize = (unsigned int)(uUSize * uVSize);
        WRITEMEM(pTexRegBaseAddr++, uTexMipMapSize);

        uCheckSize = uUSize > uVSize ? uUSize : uVSize;
        uCheckSize >>= 1;
        for(i = 0; uCheckSize > 0 && i < 10; i++) 
        {
                uUSize = (uUSize <= 1) ? 1 : (uUSize >> 1);
                uVSize = (uVSize <= 1) ? 1 : (uVSize >> 1);
                uCheckSize >>= 1;
                uTexMipMapSize += uUSize * uVSize;
                WRITEMEM(pTexRegBaseAddr++, uTexMipMapSize);
        }
    *maxLev = i;

        return FGL_ERR_NO_ERROR;
}




inline FGL_Error
calculateMipmapOffsetS3TC(
                                        unsigned int* pTexObjStateBaseAddr,
                                        unsigned int uUSize,
                                        unsigned int uVSize,
                    unsigned int *maxLev
                )
{
        unsigned int uTexMipMapSize;
        unsigned int uCheckSize;
        unsigned int offset;
        
        unsigned int* pTexRegBaseAddr;
        
        unsigned int i;

#ifdef _FGL_VALUE_CHECK
        if ((uUSize > MAXVALUE_TU_TEX_SIZE) || (uVSize > MAXVALUE_TU_TEX_SIZE))
        {
                return FGL_ERR_INVALID_SIZE;
        }
#endif

        pTexRegBaseAddr = (unsigned int *)(pTexObjStateBaseAddr + FGTO_L1_OFFSET);


        uTexMipMapSize = (unsigned int)(uUSize * uVSize);

        if((uTexMipMapSize == 4) || (uTexMipMapSize == 1))
                WRITEMEM(pTexRegBaseAddr++, 16);
        else
                WRITEMEM(pTexRegBaseAddr++, uTexMipMapSize);


        uCheckSize = uUSize > uVSize ? uUSize : uVSize;
        uCheckSize >>= 1;

        for(i = 0; uCheckSize > 0 && i < 10; i++) 
        {
                uUSize = (uUSize <= 1) ? 1 : (uUSize >> 1);
                uVSize = (uVSize <= 1) ? 1 : (uVSize >> 1);
                uCheckSize >>= 1;
                offset = uUSize * uVSize;

                if((offset == 4) || (offset == 1))
                        uTexMipMapSize += 16;
                else
                        uTexMipMapSize += offset;

                WRITEMEM(pTexRegBaseAddr++, uTexMipMapSize);
        }
    *maxLev = i;

        return FGL_ERR_NO_ERROR;
}

/*****************************************************************************

 * FUNCTIONS: fglCalculateMipmapOffsetYUV

 *

 *                      refer to

 *                      drvsys_write_reg(FGTU_TEX#_L#_OFFSET, unsigned int)

 *

 * SYNOPSIS: This function sets a texture mipmap offset

 *                       for each texture unit.

 * PARAMETERS: [IN]     unsigned int unit: texture unit ( 0~7 )

 *                         [IN] unsigned int uSize: texture u size

 *                         [IN] unsigned int vSize: texture v size

 * RETURNS: FGL_ERR_NO_ERROR - if successful.

 *          FGL_ERR_INVALID_SIZE - if texture size is invalid

 *          FGL_ERR_INVALID_VALUE - if specified unit is invalid

 * ERRNO: FGL_ERR_INVALID_SIZE = 6

 *        FGL_ERR_INVALID_VALUE = 7

 *****************************************************************************/

inline FGL_Error
calculateMipmapOffsetYUV(
                                        unsigned int* pTexObjStateBaseAddr,
                                        unsigned int uUSize,
                                        unsigned int uVSize,
                    unsigned int *maxLev
                )
{
        unsigned int uTexMipMapSize;
        unsigned int uCheckSize;
        unsigned int offset;
        
        unsigned int* pTexRegBaseAddr;
        
        unsigned int i;

#ifdef _FGL_VALUE_CHECK
        if ((uUSize > MAXVALUE_TU_TEX_SIZE) || (uVSize > MAXVALUE_TU_TEX_SIZE))
        {
                return FGL_ERR_INVALID_SIZE;
        }
#endif


         pTexRegBaseAddr = (unsigned int *)(pTexObjStateBaseAddr + FGTO_L1_OFFSET);

        uTexMipMapSize = (unsigned int)(uUSize * uVSize);

        if(uTexMipMapSize == 1)
                WRITEMEM(pTexRegBaseAddr++, 2);
        else
                WRITEMEM(pTexRegBaseAddr++, uTexMipMapSize);

        uCheckSize = uUSize > uVSize ? uUSize : uVSize;
        uCheckSize >>= 1;
        for(i = 0; uCheckSize > 0 && i < 10; i++) 
        {
                uUSize = (uUSize <= 1) ? 1 : (uUSize >> 1);
                uVSize = (uVSize <= 1) ? 1 : (uVSize >> 1);
                uCheckSize >>= 1;
                offset = uUSize * uVSize;

                if(offset == 1)
                        uTexMipMapSize += 2;
                else
                        uTexMipMapSize += offset;

                WRITEMEM(pTexRegBaseAddr++, uTexMipMapSize);
        }
    *maxLev = i;

        return FGL_ERR_NO_ERROR;
}




inline FGL_Error
setTexMipmapLevel (
                                  unsigned int* pTexObjStateBaseAddr,
                                                    FGL_MipmapLevel level, 
                                                    unsigned int    value
                                               )
{
    unsigned int *pRegAddr = 0;

#ifdef _FGL_VALUE_CHECK
        if (((unsigned int)level > FGL_MIPMAP_MAX_LEVEL)
                        || (value > MAXVALUE_TU_MIPMAP_LEVEL))
        {
                return FGL_ERR_INVALID_VALUE;
        }
#endif

    pRegAddr = (unsigned int *)(pTexObjStateBaseAddr
                                    + FGTO_MIN_LEVEL
                                    +  level);
#endif

        WRITEMEM(pRegAddr, value);        
        return FGL_ERR_NO_ERROR;
       
}



inline FGL_Error
setTexBaseAddr(unsigned int* pTexObjStateBaseAddr, unsigned int addr)
{
    unsigned int *pTexRegBaseAddr;// = 0;



    pTexRegBaseAddr = (unsigned int *)(pTexObjStateBaseAddr + FGTO_BASE_ADDR);


        WRITEMEM(pTexRegBaseAddr, addr);        
        return FGL_ERR_NO_ERROR;
}




/*****************************************************************************

 * FUNCTIONS: fglSetTexPaletteAddr

 *

 *                      refer to

 *                      drvsys_write_reg(FGTU_PALETTE_ADDR, unsigned int)

 *

 * SYNOPSIS: This function sets palette address for texturing

 * PARAMETERS: [IN]     unsigned int addr:  8-bit palette address

 * RETURNS: FGL_ERR_NO_ERROR - if successful

 *          FGL_ERR_INVALID_VALUE - if specified address is invalid

 * ERRNO: FGL_ERR_INVALID_VALUE = 7

 *****************************************************************************/
inline FGL_Error
setTexPaletteAddr(unsigned int addr)
{
#ifdef _FGL_VALUE_CHECK
        if (addr > MAXVALUE_TU_PAL_ADDR)
        {
                return FGL_ERR_INVALID_VALUE;
        }
#endif

        WRITEREG(FGTU_PALETTE_ADDR, addr);

        return FGL_ERR_NO_ERROR;
}

/*****************************************************************************

 * FUNCTIONS: fglSetTexPaletteEntry

 *

 *                      refer to

 *                      drvsys_write_reg(FGTU_PALETTE_ENTRY, unsigned int)

 *

 * SYNOPSIS: This function sets palette entry for texturing

 * PARAMETERS: [IN]     unsigned int entry:  palette data in

 * RETURNS: FGL_ERR_NO_ERROR - It'll always succeed

 * ERRNO:

 *****************************************************************************/
inline FGL_Error
setTexPaletteEntry(unsigned int entry)
{
        WRITEREG(FGTU_PALETTE_ENTRY, entry);

        return FGL_ERR_NO_ERROR;
}
