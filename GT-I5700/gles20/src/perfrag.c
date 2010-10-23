/**
 * Samsung Project
 * Copyright (c) 2007 Mobile XG, Samsung Electronics, Inc.
 * All right reserved.
 *
 * This software is the confidential and proprietary information
 * of Samsung Electronics Inc. ("Confidential Information"). You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung Electronics.
 */

/**
 * @file        perfrag.c
 * @brief       This is the perfragment-unit-setting file.
 * @author      Cheolkyoo Kim
 * @version     1.5
 */

/*****************************************************************************
  INCLUDES 
 *****************************************************************************/
#include "fgl.h"
#include "macros.h"
#include "register.h"

#ifdef __DEBUG
#define _DEBUG printf
#else
#define _DEBUG (void)
#endif


/*****************************************************************************
  DEFINES 
 *****************************************************************************/

#define FGL_PF_UNIT_VALIDATOR           (                                                               \
                                                                                FGL_PF_SCISSOR                  |       \
                                                                                FGL_PF_ALPHA                    |       \
                                                                                FGL_PF_STENCIL                  |       \
                                                                                FGL_PF_DEPTH                    |       \
                                                                                FGL_PF_BLENDING                 |       \
                                                                                FGL_PF_LOGICALOP                        \
                                                                        )

#define FGL_COMPARE_FUNC_VALIDATOR      (                                                                   \
                                                                            (1 << FGL_COMP_NEVER)               |       \
                                                                                (1 << FGL_COMP_ALWAYS)      |   \
                                                                                (1 << FGL_COMP_LESS)        |   \
                                                                                (1 << FGL_COMP_LEQUAL)          |       \
                                                                                (1 << FGL_COMP_EQUAL)           |       \
                                                                                (1 << FGL_COMP_GREATER)         |       \
                                                                                (1 << FGL_COMP_GEQUAL)          |       \
                                                                                (1 << FGL_COMP_NOTEQUAL)                \
                                                                        )
/* STENCIL */
#define FGL_FACE_VALIDATOR              (                                                                   \
                                                                            (1 << FGL_FACE_BACK)        |       \
                                                                                (1 << FGL_FACE_FRONT)           |       \
                                                                                (1 << FGL_FACE_FRONT_AND_BACK)  \
                                                                        )


#define FGL_STENCIL_OP_VALIDATOR        (                                                                   \
                                                                            (1 << FGL_ACT_KEEP)                 |       \
                                                                                (1 << FGL_ACT_ZERO)             |       \
                                                                                (1 << FGL_ACT_REPLACE)      |   \
                                                                                (1 << FGL_ACT_INCR)                     |       \
                                                                                (1 << FGL_ACT_DECR)                     |       \
                                                                                (1 << FGL_ACT_INVERT)           |       \
                                                                                (1 << FGL_ACT_INCRWRAP)         |       \
                                                                                (1 << FGL_ACT_DECRWRAP)                 \
                                                                        )

/* BLEND */
#define FGL_BLEND_FUNC_VALIDATOR        (                                                                                                       \
                                                                                (1 << FGL_BLEND_ZERO)                                   |       \
                                                                                (1 << FGL_BLEND_ONE)                        |   \
                                                                                (1 << FGL_BLEND_SRC_COLOR)                      |       \
                                                                                (1 << FGL_BLEND_ONE_MINUS_SRC_COLOR)            |       \
                                                                                (1 << FGL_BLEND_DST_COLOR)                              |       \
                                                                                (1 << FGL_BLEND_ONE_MINUS_DST_COLOR)            |   \
                                                                                (1 << FGL_BLEND_SRC_ALPHA)                              |       \
                                                                                (1 << FGL_BLEND_ONE_MINUS_SRC_ALPHA)            |       \
                                                                                (1 << FGL_BLEND_DST_ALPHA)                              |   \
                                                                                (1 << FGL_BLEND_ONE_MINUS_DST_ALPHA)            |       \
                                                                                (1 << FGL_BLEND_CONSTANT_COLOR)                 |       \
                                                                                (1 << FGL_BLEND_ONE_MINUS_CONSTANT_COLOR)       |   \
                                                                                (1 << FGL_BLEND_CONSTANT_ALPHA)                 |       \
                                                                                (1 << FGL_BLEND_ONE_MINUS_CONSTANT_ALPHA)       |   \
                                                                                (1 << FGL_BLEND_SRC_ALPHA_SATURATE)                             \
                                                                        )

#define FGL_BLEND_EQUA_VALIDATOR        (                                                                               \
                                                                                (1 << FGL_EQ_ADD)                       |       \
                                                                                (1 << FGL_EQ_SUBTRACT)          |       \
                                                                                (1 << FGL_EQ_REV_SUBTRACT)              |       \
                                                                                (1 << FGL_EQ_MIN)                       |       \
                                                                                (1 << FGL_EQ_MAX)                               \
                                                                        )

/* LOGICAL OP */                                    
#define FGL_LOGIC_OP_VALIDATOR          (                                                                   \
                                            (1 << FGL_OP_CLEAR)           |     \
                                            (1 << FGL_OP_AND)             |     \
                                            (1 << FGL_OP_AND_REVERSE)     |     \
                                            (1 << FGL_OP_COPY)            |     \
                                            (1 << FGL_OP_AND_INVERTED)    |     \
                                            (1 << FGL_OP_NOOP)            |     \
                                            (1 << FGL_OP_XOR)             |     \
                                            (1 << FGL_OP_OR)              |     \
                                            (1 << FGL_OP_NOR)             |     \
                                            (1 << FGL_OP_EQUIV)           |     \
                                            (1 << FGL_OP_INVERT)          |     \
                                            (1 << FGL_OP_OR_REVERSE)      |     \
                                            (1 << FGL_OP_COPY_INVERTED)   |     \
                                            (1 << FGL_OP_OR_INVERTED)     |     \
                                            (1 << FGL_OP_NAND)            |     \
                                            (1 << FGL_OP_SET)               \
                                                                        )

#define FGL_PIXEL_FORMAT_VALIDATOR      (                                                                       \
                                                                                (1 << FGL_PIXEL_RGB555)         |       \
                                                                                (1 << FGL_PIXEL_RGB565)         |       \
                                                                                (1 << FGL_PIXEL_ARGB4444)       |       \
                                                                                (1 << FGL_PIXEL_ARGB1555)       |       \
                                                                                (1 << FGL_PIXEL_ARGB0888)       |       \
                                                                                (1 << FGL_PIXEL_ARGB8888)               \
                                                                        ) 
 
/* Definitions used to detect enum ranges for expression validation */
/* Per-fragment */
#define PF_UNIT_VALID(enable)                                                                           \
                FGL_VALID_BITFIELD(enable, FGL_PF_UNIT_VALIDATOR)

/* Scissor reference value is an 11 bit field */
#define SCISSOR_REF_VALUE_VALID(scissorref)     (scissorref < SCISSOR_MAX_SIZE)

#define ALPHA_COMPARE_FUNC_VALID(compfunc)                          \
                FGL_ENUM_IN_SET(compfunc, FGL_COMPARE_FUNC_VALIDATOR)

/* Alpha reference value is an 8 bit field */
#define ALPHA_REF_VALUE_VALID(alpharef) (alpharef < 0x100)

/* Stencil reference value is an 8 bit field */
#define STENCIL_REF_VALUE_VALID(stencilref)     (stencilref < 0x100)

#define STENCIL_FACE_VALID(face)                                                                        \
                FGL_ENUM_IN_SET(face, FGL_FACE_VALIDATOR)

#define STENCIL_OP_VALID(stencilop)                                                                     \
                FGL_ENUM_IN_SET(stencilop, FGL_STENCIL_OP_VALIDATOR)

#define STENCIL_COMPARE_FUNC_VALID(stencilfunc)                     \
        FGL_ENUM_IN_SET(stencilfunc, FGL_COMPARE_FUNC_VALIDATOR)

/* Stencil mask value is an 8 bit field */
#define STENCIL_MASK_VALID(stencilmask) (stencilmask < 0x100)

#define DEPTH_COMPARE_FUNC_VALID(compfunc)                                                      \
                FGL_ENUM_IN_SET(compfunc, FGL_COMPARE_FUNC_VALIDATOR)

#define BLEND_EQUATION_VALID(equa)                                  \
        FGL_ENUM_IN_SET(equa, FGL_BLEND_EQUA_VALIDATOR)

#define BLEND_SOURCE_FUNC_VALID(srcfunc)                                                        \
                FGL_ENUM_IN_SET(srcfunc, FGL_BLEND_FUNC_VALIDATOR)

#define BLEND_DEST_FUNC_VALID(dstfunc)                                                          \
                FGL_ENUM_IN_SET(dstfunc, FGL_BLEND_FUNC_VALIDATOR)

#define BLEND_SOURCE_ALPHA_FUNC_VALID(srcalphafunc)                                     \
                FGL_ENUM_IN_SET(srcalphafunc, FGL_BLEND_FUNC_VALIDATOR)

#define BLEND_DEST_ALPHA_FUNC_VALID(dstalphafunc)                       \
        FGL_ENUM_IN_SET(dstalphafunc, FGL_BLEND_FUNC_VALIDATOR)

#define LOGIC_OP_FUNC_VALID(opcode)                                                                     \
        FGL_ENUM_IN_SET(opcode, FGL_LOGIC_OP_VALIDATOR)

#define PIXEL_FORMAT_VALID(format)                                                                      \
        FGL_ENUM_IN_SET(format, FGL_PIXEL_FORMAT_VALIDATOR)

/* Alpha constant alpha value is an 8 bit field */
#define ALPHA_THRESHOLD_VALUE_VALID(threshold)  (threshold < 0x100)

/* Alpha constant alpha value is an 8 bit field */
#define ALPHA_CONSTANT_VALUE_VALID(constalpha)  (constalpha < 0x100)


/*****************************************************************************
  TYPEDEFS 
 *****************************************************************************/

/*****************************************************************************
  CONSTANT MACRO 
 *****************************************************************************/

/*****************************************************************************
  FUNCTION MACRO 
 *****************************************************************************/

/*****************************************************************************
  TYPEDEFS 
 *****************************************************************************/

/*****************************************************************************
  ENUMS 
 *****************************************************************************/

/*****************************************************************************
 EXTERN 
 *****************************************************************************/

/*****************************************************************************
  NON_STATIC GLOBALS 
 *****************************************************************************/


/*****************************************************************************
  STATIC GLOBALS 
 *****************************************************************************/

/* The single FIMG device */

/* The FIMG device global variable */

/* structure declarations */
/* in some sort of meaningful order but, If defining large numbers of 
   essentially-independent utility functions, consider alphabetical order */

/* Internal Functions */ 

/* Functions */

// Per-fragment Unit Low-level API 

/***************************************************************************** 
 * FUNCTIONS: fglEnablePerFragUnit
 * SYNOPSIS: This function enables and disables various per-fragment capabilities: 
 *           scissor test, alpha test, stencil test, depth test, blending 
 *           and logical operation.
 * PARAMETERS: [IN] unit: a enum value indicating an per-fragment capability. 
 *             [IN] enable: a boolean value to enable or disable.
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_PARAMETER - unit was not one of the values listed 
 *                                      in the FGL_PerFragUnit
 * ERRNO:   FGL_ERR_NO_ERROR            1           
 *          FGL_ERR_INVALID_PARAMETER   3
 *****************************************************************************/
FGL_Error
fglEnablePerFragUnit( FGL_PerFragUnit unit, FGL_BOOL enable )
{
        FGL_Error ret = FGL_ERR_UNKNOWN;
#ifdef _FGL_VALUE_CHECK
        FGL_BOOL bValidUnit = FGL_FALSE;
#endif
        unsigned int nUnitEnableVal = 0;
        
#ifdef _FGL_VALUE_CHECK
        bValidUnit = PF_UNIT_VALID(unit);

        if(bValidUnit)
        {
#endif
                switch(unit)
                {
                default:
                case FGL_PF_SCISSOR: 
                        READREGP(FGPF_SCISSOR_XCORD, nUnitEnableVal);
                        FGL_SET_BITFIELD(nUnitEnableVal, 31:31, enable);
                        WRITEREG(FGPF_SCISSOR_XCORD, nUnitEnableVal);
                        break;
                case FGL_PF_ALPHA: 
                        READREGP(FGPF_ALPHA, nUnitEnableVal);
                        FGL_SET_BITFIELD(nUnitEnableVal, 0:0, enable);
                        WRITEREG(FGPF_ALPHA, nUnitEnableVal);
                        break;
                case FGL_PF_STENCIL: 
                        READREGP(FGPF_FRONTFACE_STENCIL, nUnitEnableVal);
                        FGL_SET_BITFIELD(nUnitEnableVal, 0:0, enable);
                        WRITEREG(FGPF_FRONTFACE_STENCIL, nUnitEnableVal);
                        break;
                case FGL_PF_DEPTH: 
                        READREGP(FGPF_DEPTH, nUnitEnableVal);
                        FGL_SET_BITFIELD(nUnitEnableVal, 0:0, enable);
                        WRITEREG(FGPF_DEPTH, nUnitEnableVal);
                        break;
                case FGL_PF_BLENDING: 
                        READREGP(FGPF_BLEND, nUnitEnableVal);
                        FGL_SET_BITFIELD(nUnitEnableVal, 0:0, enable);
#if TARGET_FIMG_VERSION != _FIMG3DSE_VER_1_2
                        /// CCache must be empty before blend-enable
                        fglFlush(FGL_PIPESTATE_ALL_WITH_CCACHE);
#endif
                        WRITEREG(FGPF_BLEND, nUnitEnableVal);
                        break;
                case FGL_PF_LOGICALOP: 
                        READREGP(FGPF_LOGIC_OP, nUnitEnableVal);
                        FGL_SET_BITFIELD(nUnitEnableVal, 0:0, enable);
                        WRITEREG(FGPF_LOGIC_OP, nUnitEnableVal);
                        break;
                }
                
                ret = FGL_ERR_NO_ERROR;
#ifdef _FGL_VALUE_CHECK
        }
        else
        {
                //_DEBUG((DBG_ERROR, "Cannot set enable per-fragment units - invalid unit."));
                ret = FGL_ERR_INVALID_PARAMETER;
        }
#endif
        
        return ret;     
}

/***************************************************************************** 
 * FUNCTIONS: fglSetScissorParams
 * SYNOPSIS: This function specifies an arbitary screen-aligned rectangle 
 *           outside of which fragments will be discarded.  
 * PARAMETERS: [IN] xMax: the maximum x coordinates of scissor box.
 *             [IN] xMin: the minimum x coordinates of scissor box.
 *             [IN] yMax: the maximum y coordiantes of scissor box.
 *             [IN] yMin: the minimum y coordinates of scissor box.
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - either x or y coordinates was not an 
 *                                  accepted value.
 * ERRNO:   FGL_ERR_NO_ERROR            1           
 *          FGL_ERR_INVALID_VALUE       7       
 *****************************************************************************/
FGL_Error 
fglSetScissorParams ( 
                                                unsigned int xMax, 
                                                unsigned int xMin,
                                                unsigned int yMax, 
                                                unsigned int yMin
                                        )
{
        FGL_Error ret = FGL_ERR_UNKNOWN;
#ifdef _FGL_VALUE_CHECK
        FGL_BOOL bValidXCoord = FGL_FALSE;
        FGL_BOOL bValidYCoord = FGL_FALSE;
#endif
        
        unsigned int nScissorXVal =0;
        unsigned int nScissorYVal =0;
        
#ifdef _FGL_VALUE_CHECK
        bValidXCoord = SCISSOR_REF_VALUE_VALID(xMax) && SCISSOR_REF_VALUE_VALID(xMin) && (xMax >= xMin);
        bValidYCoord = SCISSOR_REF_VALUE_VALID(yMax) && SCISSOR_REF_VALUE_VALID(yMin) && (yMax >= yMin);

        if (bValidXCoord && bValidYCoord)
        {
#endif
                READREGP(FGPF_SCISSOR_XCORD, nScissorXVal);
                FGL_SET_BITFIELD(nScissorXVal, 27:16, xMax);
                FGL_SET_BITFIELD(nScissorXVal, 11:0, xMin);
                WRITEREG(FGPF_SCISSOR_XCORD, nScissorXVal);
                
                FGL_SET_BITFIELD(nScissorYVal, 27:16, yMax);
                FGL_SET_BITFIELD(nScissorYVal, 11:0, yMin);
                WRITEREG(FGPF_SCISSOR_YCORD, nScissorYVal);

                ret = FGL_ERR_NO_ERROR;
#ifdef _FGL_VALUE_CHECK
        }
        else
        {
                //if(!bValidXCoord)
                //{
                        //_DEBUG((DBG_ERROR, "Cannot set scissor test value - invalid x coordinate value"));
                //}
                
                //if(!bValidYCoord)
                //{
                        //_DEBUG((DBG_ERROR, "Cannot set scissor test value - invalid Y coordinate value"));
                //}

                ret = FGL_ERR_INVALID_VALUE;
                
        }
#endif

        return ret;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetScissor
 * SYNOPSIS: This function specifies an arbitary screen-aligned rectangle 
 *           outside of which fragments will be discarded.  
 * PARAMETERS: [IN] enable: if FGL_TRUE, scissor is enabled
 *             [IN] xMax: the maximum x coordinates of scissor box.
 *             [IN] xMin: the minimum x coordinates of scissor box.
 *             [IN] yMax: the maximum y coordiantes of scissor box.
 *             [IN] yMin: the minimum y coordinates of scissor box.
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - either x or y coordinates was not an 
 *                                  accepted value.
 * ERRNO:   FGL_ERR_NO_ERROR            1           
 *          FGL_ERR_INVALID_VALUE       7       
 *****************************************************************************/
FGL_Error 
fglSetScissor ( 
                        FGL_BOOL enable,
                                                unsigned int xMax, 
                                                unsigned int xMin,
                                                unsigned int yMax, 
                                                unsigned int yMin
                                        )
{
        FGL_Error ret = FGL_ERR_UNKNOWN;
#ifdef _FGL_VALUE_CHECK
        FGL_BOOL bValidXCoord = FGL_FALSE;
        FGL_BOOL bValidYCoord = FGL_FALSE;
#endif
        
        unsigned int nScissorXVal =0;
        unsigned int nScissorYVal =0;
        
#ifdef _FGL_VALUE_CHECK
        bValidXCoord = SCISSOR_REF_VALUE_VALID(xMax) && SCISSOR_REF_VALUE_VALID(xMin) && (xMax >= xMin);
        bValidYCoord = SCISSOR_REF_VALUE_VALID(yMax) && SCISSOR_REF_VALUE_VALID(yMin) && (yMax >= yMin);

        if (bValidXCoord && bValidYCoord)
        {
#endif
        nScissorXVal = (enable << 31) | ((xMax << 16) | xMin);
                //FGL_SET_BITFIELD(nScissorXVal, 27:16, xMax);
                //FGL_SET_BITFIELD(nScissorXVal, 11:0, xMin);
                WRITEREG(FGPF_SCISSOR_XCORD, nScissorXVal);
                
        nScissorYVal = (yMax << 16) | yMin;
                //FGL_SET_BITFIELD(nScissorYVal, 27:16, yMax);
                //FGL_SET_BITFIELD(nScissorYVal, 11:0, yMin);
                WRITEREG(FGPF_SCISSOR_YCORD, nScissorYVal);

                ret = FGL_ERR_NO_ERROR;
#ifdef _FGL_VALUE_CHECK
        }
        else
        {
                ret = FGL_ERR_INVALID_VALUE;
        }
#endif

        return ret;
}


/***************************************************************************** 
 * FUNCTIONS: fglSetAlphaParams
 * SYNOPSIS: This function discards a fragment depending on the outcome of a 
 *           comparison between the fragment's alpha value and a constant 
 *           reference value.
 * PARAMETERS:[IN] refAlpha: The reference value to which incoming alpha values
 *                 are compared. This value is clamped to the range 8bit value.
 *            [IN] mode: The alpha comparison function.
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - refAlpha was not an accepted value
 *          FGL_ERR_INVALID_PARAMETER - mode was not an accepted enum value
 * ERRNO:   FGL_ERR_NO_ERROR            1           
 *          FGL_ERR_INVALID_PARAMETER   2
 *          FGL_ERR_INVALID_VALUE       7       
 *****************************************************************************/
FGL_Error 
fglSetAlphaParams ( unsigned int refAlpha, FGL_CompareFunc mode )
{
#ifdef _FGL_VALUE_CHECK
        FGL_BOOL bValidAlphaRef = FGL_FALSE;
        FGL_BOOL bValidCompareFunc = FGL_FALSE;
#endif
        unsigned int nAlphaVal =0;
        
#ifdef _FGL_VALUE_CHECK
        bValidAlphaRef = ALPHA_REF_VALUE_VALID(refAlpha);
        bValidCompareFunc = ALPHA_COMPARE_FUNC_VALID(mode);
                
        if (bValidAlphaRef && bValidCompareFunc)
        {
#endif
                READREGP(FGPF_ALPHA, nAlphaVal);
                FGL_SET_BITFIELD(nAlphaVal, 11:4, refAlpha);
                FGL_SET_BITFIELD(nAlphaVal, 3:1, mode);
                WRITEREG(FGPF_ALPHA, nAlphaVal);

                return FGL_ERR_NO_ERROR;
#ifdef _FGL_VALUE_CHECK
        }
        else
        {
                if(!bValidAlphaRef)
                {
                        //_DEBUG((DBG_ERROR, "Cannot set alpha test control register - invalid reference value."));
                        return FGL_ERR_INVALID_VALUE;
                }
                
                if(!bValidCompareFunc)
                {
                        //_DEBUG((DBG_ERROR, "Cannot set alpha test control register - invalid parameters."));
                        return FGL_ERR_INVALID_PARAMETER;
                }
        }

        return FGL_ERR_NO_ERROR;
#endif
}

/***************************************************************************** 
 * FUNCTIONS: fglSetAlpha
 * SYNOPSIS: This function discards a fragment depending on the outcome of a 
 *           comparison between the fragment's alpha value and a constant 
 *           reference value.
 * PARAMETERS:[IN] enable: if FGL_TRUE, alpha test is enabled
 *            [IN] refAlpha: The reference value to which incoming alpha values
 *                 are compared. This value is clamped to the range 8bit value.
 *            [IN] mode: The alpha comparison function.
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - refAlpha was not an accepted value
 *          FGL_ERR_INVALID_PARAMETER - mode was not an accepted enum value
 * ERRNO:   FGL_ERR_NO_ERROR            1           
 *          FGL_ERR_INVALID_PARAMETER   2
 *          FGL_ERR_INVALID_VALUE       7       
 *****************************************************************************/
FGL_Error 
fglSetAlpha (
            FGL_BOOL enable,
            unsigned int refAlpha,
            FGL_CompareFunc mode
            )
{
#ifdef _FGL_VALUE_CHECK
        FGL_BOOL bValidAlphaRef = FGL_FALSE;
        FGL_BOOL bValidCompareFunc = FGL_FALSE;
#endif
        unsigned int nAlphaVal =0;
        
#ifdef _FGL_VALUE_CHECK
        bValidAlphaRef = ALPHA_REF_VALUE_VALID(refAlpha);
        bValidCompareFunc = ALPHA_COMPARE_FUNC_VALID(mode);
                
        if (bValidAlphaRef && bValidCompareFunc)
        {
#endif
//              READREGP(FGPF_ALPHA, nAlphaVal);
//              FGL_SET_BITFIELD(nAlphaVal, 11:4, refAlpha);
//              FGL_SET_BITFIELD(nAlphaVal, 3:1, mode);
        nAlphaVal = (refAlpha << 4) | (mode << 1) | enable;
                WRITEREG(FGPF_ALPHA, nAlphaVal);

                return FGL_ERR_NO_ERROR;
#ifdef _FGL_VALUE_CHECK
        }
        else
        {
                if(!bValidAlphaRef)
                {
                        return FGL_ERR_INVALID_VALUE;
                }
                
                if(!bValidCompareFunc)
                {
                        return FGL_ERR_INVALID_PARAMETER;
                }
        }

        return FGL_ERR_NO_ERROR;
#endif
}

/***************************************************************************** 
 * FUNCTIONS: fglSetStencilParams
 * SYNOPSIS: The stencil test conditionally discards a fragment based on the 
 *           outcome of a comparison between the value in the stencil buffer 
 *           and a reference value.
 * PARAMETERS: [IN] stencilParam: The point argument of FGL_StencilParam
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - The mask and ref of FGL_StencilParam was 
 *                                  not an accepted value .
 *          FGL_ERR_INVALID_PARAMETER - The zpass, zfail, sfail and mode of 
 *                                      FGL_StencilParam was not an accepted 
 *                                      enum value.
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2
 *          FGL_ERR_INVALID_VALUE       7
 *****************************************************************************/
FGL_Error 
fglSetStencilParams ( pFGL_StencilParam stencilParam )
{
#ifdef _FGL_VALUE_CHECK
        FGL_BOOL bValidParam = FGL_FALSE;
#endif
        unsigned int nStencilVal = 0;
        unsigned int nFrontRegVal = 0;
        FGL_CompareFunc comparefunc;

#ifdef _FGL_VALUE_CHECK
    bValidParam = ( STENCIL_FACE_VALID(stencilParam->face) & STENCIL_OP_VALID(stencilParam->zpass) & 
                    STENCIL_OP_VALID(stencilParam->zfail) & STENCIL_OP_VALID(stencilParam->sfail) & 
                    STENCIL_MASK_VALID(stencilParam->mask) & STENCIL_REF_VALUE_VALID(stencilParam->ref) & 
                    STENCIL_COMPARE_FUNC_VALID(stencilParam->mode));

    if(bValidParam)
    {
#endif
        // Software workaround for bug ID 81 (v1.21), 31 (v1.51)
        switch((FGL_CompareFunc)stencilParam->mode)
        {
        case FGL_COMP_LESS:
            comparefunc = FGL_COMP_GREATER;
            break;
        case FGL_COMP_LEQUAL:
            comparefunc = FGL_COMP_GEQUAL;
            break;
        case FGL_COMP_GREATER:
            comparefunc = FGL_COMP_LESS;
            break;
        case FGL_COMP_GEQUAL:
            comparefunc = FGL_COMP_LEQUAL;
            break;
        default:
            comparefunc = stencilParam->mode;
                break;
        }

        nStencilVal = (stencilParam->zpass << 29)
                        | (stencilParam->zfail << 26)
                        | (stencilParam->sfail << 23)
                        | (stencilParam->mask << 12)
                        | (stencilParam->ref << 4)
                        | (comparefunc << 1);
        switch((FGL_Face)(stencilParam->face))
        {
        case FGL_FACE_BACK:
                    WRITEREG(FGPF_BACKFACE_STENCIL, nStencilVal);
            break;
        case FGL_FACE_FRONT:
                    READREGP(FGPF_FRONTFACE_STENCIL, nFrontRegVal);
                    nStencilVal |= (nFrontRegVal & STENCIL_ENABLE_MASK);
                    WRITEREG(FGPF_FRONTFACE_STENCIL, nStencilVal);
            break;
        case FGL_FACE_FRONT_AND_BACK:
                    WRITEREG(FGPF_BACKFACE_STENCIL, nStencilVal);
                    READREGP(FGPF_FRONTFACE_STENCIL, nFrontRegVal);
                    nStencilVal |= (nFrontRegVal & STENCIL_ENABLE_MASK);
                    WRITEREG(FGPF_FRONTFACE_STENCIL, nStencilVal);
            break;
        default:
            return FGL_ERR_INVALID_PARAMETER;
        }
        
                return FGL_ERR_NO_ERROR;
#ifdef _FGL_VALUE_CHECK
        }
        else
        {
        return FGL_ERR_INVALID_PARAMETER;
        }
#endif
}

/***************************************************************************** 
 * FUNCTIONS: fglSetFrontStencil
 * SYNOPSIS: The stencil test conditionally discards a fragment based on the 
 *           outcome of a comparison between the value in the stencil buffer 
 *           and a reference value.
 * PARAMETERS: [IN] enable: if FGL_TRUE, stencil test is enable
 *             [IN] stencilParam: The point argument of FGL_Stencil
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - The mask and ref of FGL_StencilParam was 
 *                                  not an accepted value .
 *          FGL_ERR_INVALID_PARAMETER - The zpass, zfail, sfail and mode of 
 *                                      FGL_StencilParam was not an accepted 
 *                                      enum value.
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2
 *          FGL_ERR_INVALID_VALUE       7
 *****************************************************************************/
FGL_Error 
fglSetFrontStencil( FGL_BOOL enable, pFGL_Stencil stencilParam )
{
#ifdef _FGL_VALUE_CHECK
        FGL_BOOL bValidParam = FGL_FALSE;
#endif
        unsigned int nStencilVal = 0;
        FGL_CompareFunc comparefunc;

#ifdef _FGL_VALUE_CHECK
    bValidParam = ( STENCIL_OP_VALID(stencilParam->zpass)
                    & STENCIL_OP_VALID(stencilParam->zfail)
                    & STENCIL_OP_VALID(stencilParam->sfail)
                    & STENCIL_MASK_VALID(stencilParam->mask)
                    & STENCIL_REF_VALUE_VALID(stencilParam->ref)
                    & STENCIL_COMPARE_FUNC_VALID(stencilParam->mode));

    if(bValidParam)
    {
#endif
        // Software workaround for bug ID 81 (v1.21), 31 (v1.51)
        switch((FGL_CompareFunc)stencilParam->mode)
        {
        case FGL_COMP_LESS:
            comparefunc = FGL_COMP_GREATER;
            break;
        case FGL_COMP_LEQUAL:
            comparefunc = FGL_COMP_GEQUAL;
            break;
        case FGL_COMP_GREATER:
            comparefunc = FGL_COMP_LESS;
            break;
        case FGL_COMP_GEQUAL:
            comparefunc = FGL_COMP_LEQUAL;
            break;
        default:
            comparefunc = stencilParam->mode;
                break;
        }

        nStencilVal = (stencilParam->zpass << 29)
                    | (stencilParam->zfail << 26)
                    | (stencilParam->sfail << 23)
                    | (stencilParam->mask << 12)
                    | (stencilParam->ref << 4)
                    | (comparefunc << 1)
                    | enable;

//              FGL_SET_BITFIELD(nStencilVal, 31:29, stencilParam->zpass);
//              FGL_SET_BITFIELD(nStencilVal, 28:26, stencilParam->zfail);
//              FGL_SET_BITFIELD(nStencilVal, 25:23, stencilParam->sfail);
//              FGL_SET_BITFIELD(nStencilVal, 19:12, stencilParam->mask);
//              FGL_SET_BITFIELD(nStencilVal, 11:4, stencilParam->ref);
//              FGL_SET_BITFIELD(nStencilVal, 3:1, stencilParam->mode);

                WRITEREG(FGPF_FRONTFACE_STENCIL, nStencilVal);
                return FGL_ERR_NO_ERROR;
#ifdef _FGL_VALUE_CHECK
        }
        else
        {
        return FGL_ERR_INVALID_PARAMETER;
        }
#endif
}

/***************************************************************************** 
 * FUNCTIONS: fglSetBaskStencil
 * SYNOPSIS: The stencil test conditionally discards a fragment based on the 
 *           outcome of a comparison between the value in the stencil buffer 
 *           and a reference value.
 * PARAMETERS: [IN] stencilParam: The point argument of FGL_Stencil
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - The mask and ref of FGL_StencilParam was 
 *                                  not an accepted value .
 *          FGL_ERR_INVALID_PARAMETER - The zpass, zfail, sfail and mode of 
 *                                      FGL_StencilParam was not an accepted 
 *                                      enum value.
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2
 *          FGL_ERR_INVALID_VALUE       7
 *****************************************************************************/
FGL_Error 
fglSetBackStencil( pFGL_Stencil stencilParam )
{
#ifdef _FGL_VALUE_CHECK
        FGL_BOOL bValidParam = FGL_FALSE;
#endif
        unsigned int nStencilVal = 0;
        FGL_CompareFunc comparefunc;

#ifdef _FGL_VALUE_CHECK
    bValidParam = ( STENCIL_OP_VALID(stencilParam->zpass)
                    & STENCIL_OP_VALID(stencilParam->zfail)
                    & STENCIL_OP_VALID(stencilParam->sfail)
                    & STENCIL_MASK_VALID(stencilParam->mask)
                    & STENCIL_REF_VALUE_VALID(stencilParam->ref)
                    & STENCIL_COMPARE_FUNC_VALID(stencilParam->mode));

    if(bValidParam)
    {
#endif
        
        // Software workaround for bug ID 81 (v1.21), 31 (v1.51)
        switch((FGL_CompareFunc)stencilParam->mode)
        {
        case FGL_COMP_LESS:
            comparefunc = FGL_COMP_GREATER;
            break;
        case FGL_COMP_LEQUAL:
            comparefunc = FGL_COMP_GEQUAL;
            break;
        case FGL_COMP_GREATER:
            comparefunc = FGL_COMP_LESS;
            break;
        case FGL_COMP_GEQUAL:
            comparefunc = FGL_COMP_LEQUAL;
            break;
        default:
            comparefunc = stencilParam->mode;
                break;
        }
        
        nStencilVal = (stencilParam->zpass << 29)
                    | (stencilParam->zfail << 26)
                    | (stencilParam->sfail << 23)
                    | (stencilParam->mask << 12)
                    | (stencilParam->ref << 4)
                    | (comparefunc << 1);

//              FGL_SET_BITFIELD(nStencilVal, 31:29, stencilParam->zpass);
//              FGL_SET_BITFIELD(nStencilVal, 28:26, stencilParam->zfail);
//              FGL_SET_BITFIELD(nStencilVal, 25:23, stencilParam->sfail);
//              FGL_SET_BITFIELD(nStencilVal, 19:12, stencilParam->mask);
//              FGL_SET_BITFIELD(nStencilVal, 11:4, stencilParam->ref);
//              FGL_SET_BITFIELD(nStencilVal, 3:1, stencilParam->mode);

                WRITEREG(FGPF_BACKFACE_STENCIL, nStencilVal);
                return FGL_ERR_NO_ERROR;
#ifdef _FGL_VALUE_CHECK
        }
        else
        {
        return FGL_ERR_INVALID_PARAMETER;
        }
#endif
}

/***************************************************************************** 
 * FUNCTIONS: fglSetDepthParams
 * SYNOPSIS: This function specifies the value used for depth-buffer comparisons.
 * PARAMETERS: [IN] mode: Specifies the depth-comparison function
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_PARAMETER - mode was not an accepted enum value.
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2
 *****************************************************************************/
FGL_Error 
fglSetDepthParams ( FGL_CompareFunc mode )
{
#ifdef _FGL_VALUE_CHECK
        FGL_BOOL bValidCompFunc = FGL_FALSE;
#endif
        unsigned int nDepthVal =0;
        
#ifdef _FGL_VALUE_CHECK
        bValidCompFunc = DEPTH_COMPARE_FUNC_VALID(mode);
                
        if (bValidCompFunc)
        {
#endif
                READREGP(FGPF_DEPTH, nDepthVal);
                FGL_SET_BITFIELD(nDepthVal, 3:1, mode);
                WRITEREG(FGPF_DEPTH, nDepthVal);

                return FGL_ERR_NO_ERROR;
#ifdef _FGL_VALUE_CHECK
        }
        else
        {
                //_DEBUG((DBG_ERROR, "Cannot set depth test register - invalid parameters."));
                return FGL_ERR_INVALID_PARAMETER;
        }
#endif
}

/***************************************************************************** 
 * FUNCTIONS: fglSetDepth
 * SYNOPSIS: This function specifies the value used for depth-buffer comparisons.
 * PARAMETERS: [IN] enable: if FGL_TRUE, depth test is enabled
 *             [IN] mode: Specifies the depth-comparison function
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_PARAMETER - mode was not an accepted enum value.
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2
 *****************************************************************************/
FGL_Error 
fglSetDepth( FGL_BOOL enable, FGL_CompareFunc mode )
{
#ifdef _FGL_VALUE_CHECK
        FGL_BOOL bValidCompFunc = FGL_FALSE;
#endif
        unsigned int nDepthVal =0;
        
#ifdef _FGL_VALUE_CHECK
        bValidCompFunc = DEPTH_COMPARE_FUNC_VALID(mode);
                
        if (bValidCompFunc)
        {
#endif
//              FGL_SET_BITFIELD(nDepthVal, 3:1, mode);
        nDepthVal = (mode << 1) | enable;
                WRITEREG(FGPF_DEPTH, nDepthVal);

                return FGL_ERR_NO_ERROR;
#ifdef _FGL_VALUE_CHECK
        }
        else
        {
                //_DEBUG((DBG_ERROR, "Cannot set depth test register - invalid parameters."));
                return FGL_ERR_INVALID_PARAMETER;
        }
#endif
}

/***************************************************************************** 
 * FUNCTIONS: fglSetBlendParams
 * SYNOPSIS: In RGB mode, pixels can be drawn using a function that blends 
 *           the incoming (source) RGBA values with the RGBA values that are 
 *           already in the framebuffer (the destination values). 
 * PARAMETERS: [in] blendParam - the pointer parameter of FGL_BlendParam
 * RETURNS: FGL_ERR_NO_ERROR - if successful
 *          FGL_ERR_INVALID_PARAMETER - the member of struct FGL_BlendParam  
 *                                      was not an accepted enum value.
 * ERRNO:   FGL_ERR_NO_ERROR            1
 *          FGL_ERR_INVALID_PARAMETER   2
 *****************************************************************************/
FGL_Error 
fglSetBlendParams ( pFGL_BlendParam blendParam )
{
#ifdef _FGL_VALUE_CHECK
    FGL_BOOL bValidParam = FGL_FALSE;
#endif
        unsigned int nBlendVal =0;
        unsigned int nBufCtrlVal =0;
        
#ifdef _FGL_VALUE_CHECK
        bValidParam = ( BLEND_EQUATION_VALID(blendParam->colorEqua) &
                        BLEND_EQUATION_VALID(blendParam->alphaEqua) &
                        BLEND_DEST_ALPHA_FUNC_VALID(blendParam->dstAlpha) &
                        BLEND_DEST_FUNC_VALID(blendParam->dstColor) &
                        BLEND_SOURCE_ALPHA_FUNC_VALID(blendParam->srcAlpha) &
                        BLEND_SOURCE_FUNC_VALID(blendParam->srcColor));

        if(bValidParam)
        {
#endif

        

    FGL_BOOL bAlphaSaturateMode = FGL_FALSE;
    READREGP(FGPF_COLORBUF_CTRL, nBufCtrlVal);

//      _DEBUG ("\n@@@@@@@@@@@@@@@ fglSetBlendParams \n");
    
    if((nBufCtrlVal & 0x7) == FGL_PIXEL_RGB565)
    {
        if(blendParam->srcColor == FGL_BLEND_SRC_ALPHA_SATURATE)
        {
                blendParam->srcColor = FGL_BLEND_CONSTANT_COLOR;
                blendParam->srcAlpha = FGL_BLEND_CONSTANT_COLOR;
                WRITEREG(FGPF_BLEND_COLOR, 0x000000FF);
                bAlphaSaturateMode = FGL_TRUE;          
        }

        if(blendParam->dstColor == FGL_BLEND_SRC_ALPHA_SATURATE)
        {
                blendParam->dstColor = FGL_BLEND_CONSTANT_COLOR;
                blendParam->dstAlpha = FGL_BLEND_CONSTANT_COLOR;
                WRITEREG(FGPF_BLEND_COLOR, 0x000000FF);
                bAlphaSaturateMode = FGL_TRUE;          
        }
    }


#if TARGET_FIMG_VERSION != _FIMG3DSE_VER_1_2
        // check for blend
                fglFlush(FGL_PIPESTATE_ALL_WITH_CCACHE);
#endif
                READREGP(FGPF_BLEND, nBlendVal);
                FGL_SET_BITFIELD(nBlendVal, 22:20, blendParam->alphaEqua);
                FGL_SET_BITFIELD(nBlendVal, 19:17, blendParam->colorEqua);
                FGL_SET_BITFIELD(nBlendVal, 16:13, blendParam->dstAlpha);
                FGL_SET_BITFIELD(nBlendVal, 12:9, blendParam->dstColor);
                FGL_SET_BITFIELD(nBlendVal, 8:5, blendParam->srcAlpha);
                FGL_SET_BITFIELD(nBlendVal, 4:1, blendParam->srcColor);


        if(!bAlphaSaturateMode) 
        {
            WRITEREG(FGPF_BLEND_COLOR, blendParam->constColor);
        }

                WRITEREG(FGPF_BLEND, nBlendVal);

                return FGL_ERR_NO_ERROR;
#ifdef _FGL_VALUE_CHECK
        }
        else
        {
            return FGL_ERR_INVALID_PARAMETER;
        }
#endif
}

// Software workaround for bug ID 82 (v1.21), 32 (v1.51)
FGL_Error 
fglSetBlendParamsWorkAround ( pFGL_BlendParam blendParam )
{
#ifdef _FGL_VALUE_CHECK
     FGL_BOOL bValidParam = FGL_FALSE;
 #endif
         unsigned int  nBlendVal     = 0;
         unsigned int  nBufCtrlVal   = 0;
         FGL_BlendFunc eSrcColorFunc = 0;
         FGL_BlendFunc eDstColorFunc = 0;
         
 #ifdef _FGL_VALUE_CHECK
         bValidParam = ( BLEND_EQUATION_VALID(blendParam->colorEqua) &
                         BLEND_EQUATION_VALID(blendParam->alphaEqua) &
                         BLEND_DEST_ALPHA_FUNC_VALID(blendParam->dstAlpha) &
                         BLEND_DEST_FUNC_VALID(blendParam->dstColor) &
                         BLEND_SOURCE_ALPHA_FUNC_VALID(blendParam->srcAlpha) &
                         BLEND_SOURCE_FUNC_VALID(blendParam->srcColor));
 
         if(bValidParam)
         {
 #endif
 
     READREGP(FGPF_COLORBUF_CTRL, nBufCtrlVal);
     
     if((nBufCtrlVal & 0x7) == FGL_PIXEL_RGB565)
     {
         switch(blendParam->srcColor)
         {
         case FGL_BLEND_DST_ALPHA:
                 eSrcColorFunc = FGL_BLEND_ONE ;
             break;
         case FGL_BLEND_ONE_MINUS_DST_ALPHA :
                 eSrcColorFunc = FGL_BLEND_ZERO;
             break;
         case FGL_BLEND_SRC_ALPHA_SATURATE :
                 eSrcColorFunc = FGL_BLEND_ONE;
             break;
         default :
             eSrcColorFunc = blendParam->srcColor;
             break;
         }
 
         switch(blendParam->dstColor)
         {
         case FGL_BLEND_DST_ALPHA :
                 eDstColorFunc = FGL_BLEND_ONE;
             break;
         case FGL_BLEND_ONE_MINUS_DST_ALPHA :
                 eDstColorFunc = FGL_BLEND_ZERO;
             break;
         default :
             eDstColorFunc = blendParam->dstColor;
             break;
         }
     }
 
 
 #if TARGET_FIMG_VERSION != _FIMG3DSE_VER_1_2
         // check for blend
                 fglFlush(FGL_PIPESTATE_ALL_WITH_CCACHE);
 #endif
                 READREGP(FGPF_BLEND, nBlendVal);
                 FGL_SET_BITFIELD(nBlendVal, 22:20, blendParam->alphaEqua);
                 FGL_SET_BITFIELD(nBlendVal, 19:17, blendParam->colorEqua);
                 FGL_SET_BITFIELD(nBlendVal, 16:13, blendParam->dstAlpha);
                 FGL_SET_BITFIELD(nBlendVal, 12:9,  eDstColorFunc);
                 FGL_SET_BITFIELD(nBlendVal, 8:5,   blendParam->srcAlpha);
                 FGL_SET_BITFIELD(nBlendVal, 4:1,   eSrcColorFunc);
 
                 WRITEREG(FGPF_BLEND, nBlendVal);
 
                 return FGL_ERR_NO_ERROR;
 #ifdef _FGL_VALUE_CHECK
         }
         else
         {
             return FGL_ERR_INVALID_PARAMETER;
         }
 #endif
 }
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetBlend
  * SYNOPSIS: In RGB mode, pixels can be drawn using a function that blends 
  *           the incoming (source) RGBA values with the RGBA values that are 
  *           already in the framebuffer (the destination values). 
  * PARAMETERS: [IN] enable: if FGL_TRUE, blend is enabled
  *             [IN] blendParam: the pointer parameter of FGL_Blend
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  *          FGL_ERR_INVALID_PARAMETER - the member of struct FGL_BlendParam  
  *                                      was not an accepted enum value.
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *          FGL_ERR_INVALID_PARAMETER   2
  *****************************************************************************/
 FGL_Error 
 fglSetBlend ( FGL_BOOL enable, pFGL_Blend blendParam )
 {
 #ifdef _FGL_VALUE_CHECK
     FGL_BOOL bValidParam = FGL_FALSE;
 #endif
         unsigned int nBlendVal =0;
 
 #ifdef _FGL_VALUE_CHECK
         bValidParam = ( BLEND_EQUATION_VALID(blendParam->colorEqua) &
                         BLEND_EQUATION_VALID(blendParam->alphaEqua) &
                         BLEND_DEST_ALPHA_FUNC_VALID(blendParam->dstAlpha) &
                         BLEND_DEST_FUNC_VALID(blendParam->dstColor) &
                         BLEND_SOURCE_ALPHA_FUNC_VALID(blendParam->srcAlpha) &
                         BLEND_SOURCE_FUNC_VALID(blendParam->srcColor));
 
         if(bValidParam)
         {
 #endif
 
 //      _DEBUG ("\n@@@@@@@@@@@@@@@ fglSetBlend \n");
 //              FGL_SET_BITFIELD(nBlendVal, 22:20, blendParam->alphaEqua);
 //              FGL_SET_BITFIELD(nBlendVal, 19:17, blendParam->colorEqua);
 //              FGL_SET_BITFIELD(nBlendVal, 16:13, blendParam->dstAlpha);
 //              FGL_SET_BITFIELD(nBlendVal, 12:9, blendParam->dstColor);
 //              FGL_SET_BITFIELD(nBlendVal, 8:5, blendParam->srcAlpha);
 //              FGL_SET_BITFIELD(nBlendVal, 4:1, blendParam->srcColor);
 
         nBlendVal = (blendParam->alphaEqua << 20)
                     | (blendParam->colorEqua << 17)
                     | (blendParam->dstAlpha << 13)
                     | (blendParam->dstColor << 9)
                     | (blendParam->srcAlpha << 5)
                     | (blendParam->srcColor << 1)
                     | enable;
 
 #if TARGET_FIMG_VERSION != _FIMG3DSE_VER_1_2
         // check for blend
                 fglFlush(FGL_PIPESTATE_ALL_WITH_CCACHE);
 #endif
 
                 WRITEREG(FGPF_BLEND, nBlendVal);
 
                 return FGL_ERR_NO_ERROR;
 #ifdef _FGL_VALUE_CHECK
         }
         else
         {
             return FGL_ERR_INVALID_PARAMETER;
         }
 #endif
 }
 
 
 // Software workaround for bug ID 82 (v1.21), 32 (v1.51)
 FGL_Error 
 fglSetBlendWorkAround ( FGL_BOOL enable, pFGL_Blend blendParam )
 {
 #ifdef _FGL_VALUE_CHECK
     FGL_BOOL bValidParam = FGL_FALSE;
 #endif
         unsigned int nBlendVal =0;
         FGL_BlendFunc eSrcColorFunc = 0;
         FGL_BlendFunc eDstColorFunc = 0;
         
 #ifdef _FGL_VALUE_CHECK
         bValidParam = ( BLEND_EQUATION_VALID(blendParam->colorEqua) &
                         BLEND_EQUATION_VALID(blendParam->alphaEqua) &
                         BLEND_DEST_ALPHA_FUNC_VALID(blendParam->dstAlpha) &
                         BLEND_DEST_FUNC_VALID(blendParam->dstColor) &
                         BLEND_SOURCE_ALPHA_FUNC_VALID(blendParam->srcAlpha) &
                         BLEND_SOURCE_FUNC_VALID(blendParam->srcColor));
         if(bValidParam)
         {
 #endif
 //      _DEBUG ("\n@@@@@@@@@@@@@@@ fglSetBlendWorkAround \n");
         switch(blendParam->srcColor)
         {
         case FGL_BLEND_DST_ALPHA:
                 eSrcColorFunc = FGL_BLEND_ONE ;
             break;
         case FGL_BLEND_ONE_MINUS_DST_ALPHA :
                 eSrcColorFunc = FGL_BLEND_ZERO;
             break;
         case FGL_BLEND_SRC_ALPHA_SATURATE :
                 eSrcColorFunc = FGL_BLEND_ZERO;
             break;
         default :
             eSrcColorFunc = blendParam->srcColor;
             break;
         }
 
         switch(blendParam->dstColor)
         {
         case FGL_BLEND_DST_ALPHA :
                 eDstColorFunc = FGL_BLEND_ONE;
             break;
         case FGL_BLEND_ONE_MINUS_DST_ALPHA :
                 eDstColorFunc = FGL_BLEND_ZERO;
             break;
         default :
             eDstColorFunc = blendParam->dstColor;
             break;
         }
 
 //              FGL_SET_BITFIELD(nBlendVal, 22:20, blendParam->alphaEqua);
 //              FGL_SET_BITFIELD(nBlendVal, 19:17, blendParam->colorEqua);
 //              FGL_SET_BITFIELD(nBlendVal, 16:13, blendParam->dstAlpha);
 //              FGL_SET_BITFIELD(nBlendVal, 12:9, blendParam->dstColor);
 //              FGL_SET_BITFIELD(nBlendVal, 8:5, blendParam->srcAlpha);
 //              FGL_SET_BITFIELD(nBlendVal, 4:1, blendParam->srcColor);
 
         nBlendVal = (blendParam->alphaEqua << 20)
                     | (blendParam->colorEqua << 17)
                     | (blendParam->dstAlpha << 13)
                     | (eDstColorFunc << 9)
                     | (blendParam->srcAlpha << 5)
                     | (eSrcColorFunc << 1)
                     | enable;
 
 #if TARGET_FIMG_VERSION != _FIMG3DSE_VER_1_2
         // check for blend
                 fglFlush(FGL_PIPESTATE_ALL_WITH_CCACHE);
 #endif
 
                 WRITEREG(FGPF_BLEND, nBlendVal);
 
                 return FGL_ERR_NO_ERROR;
 #ifdef _FGL_VALUE_CHECK
         }
         else
         {
             return FGL_ERR_INVALID_PARAMETER;
         }
 #endif
 }
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetBlendColor
  * SYNOPSIS: This function set constant blend color.
  *           This value can be used in blending.
  * PARAMETERS: [IN] blendColor - RGBA-order 32bit color
  * RETURNS: FGL_ERR_NO_ERROR - always
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *****************************************************************************/
 FGL_Error 
 fglSetBlendColor ( unsigned int  blendColor )
 {
     WRITEREG(FGPF_BLEND_COLOR, blendColor);
         return FGL_ERR_NO_ERROR;
 }
 
 /* thomas-20050921@comment: opengl|es 2.0 not supported */
 /***************************************************************************** 
  * FUNCTIONS: fglSetLogicalOpParams
  * SYNOPSIS: A logical operation can be applied the fragment and value stored 
  *           at the corresponding location in the framebuffer; the result 
  *           replaces the current framebuffer value. 
  * PARAMETERS: [in] colorOp - specifies an enumeration value that selects 
  *                            a logical operation.
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  *          FGL_ERR_INVALID_PARAMETER - The colorOp was not an accepted value.
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *          FGL_ERR_INVALID_PARAMETER   2
  *****************************************************************************/
 FGL_Error 
 fglSetLogicalOpParams ( FGL_LogicalOp colorOp )
 {
         unsigned int nLogicOpVal = 0;
         
 #ifdef _FGL_VALUE_CHECK
         if (LOGIC_OP_FUNC_VALID(colorOp))
         {
 #endif
                 READREGP(FGPF_LOGIC_OP, nLogicOpVal);
                 FGL_SET_BITFIELD(nLogicOpVal, 8:5, colorOp);
                 FGL_SET_BITFIELD(nLogicOpVal, 4:1, colorOp);
                 WRITEREG(FGPF_LOGIC_OP, nLogicOpVal);
                 return FGL_ERR_NO_ERROR;
 #ifdef _FGL_VALUE_CHECK
         }
         else
         {
                 //_DEBUG((DBG_ERROR, "Cannot set logical operation color function - invalid colorOp."));
                 return FGL_ERR_INVALID_PARAMETER;
         }
 #endif
 }
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetLogicalOp
  * SYNOPSIS: A logical operation can be applied the fragment and value stored 
  *           at the corresponding location in the framebuffer; the result 
  *           replaces the current framebuffer value. 
  * PARAMETERS: [IN] enable: if FGL_TRUE, logicalOperation is enabled
  *             [IN] alphaOp: specifies an enumeration value that selects 
  *                            a logical operation for alpha value.
  *             [IN] colorOp: specifies an enumeration value that selects 
  *                            a logical operation for color value.
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  *          FGL_ERR_INVALID_PARAMETER - The colorOp was not an accepted value.
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *          FGL_ERR_INVALID_PARAMETER   2
  *****************************************************************************/
 FGL_Error 
 fglSetLogicalOp(FGL_BOOL enable, FGL_LogicalOp alphaOp, FGL_LogicalOp colorOp)
 {
         unsigned int nLogicOpVal = 0;
         
 #ifdef _FGL_VALUE_CHECK
         if (LOGIC_OP_FUNC_VALID(alphaOp) & LOGIC_OP_FUNC_VALID(colorOp))
         {
 #endif
 //              FGL_SET_BITFIELD(nLogicOpVal, 8:5, alphaOp);
 //              FGL_SET_BITFIELD(nLogicOpVal, 4:1, colorOp);
         nLogicOpVal = (alphaOp << 5) | (colorOp << 1) | enable;
                 WRITEREG(FGPF_LOGIC_OP, nLogicOpVal);
                 return FGL_ERR_NO_ERROR;
 #ifdef _FGL_VALUE_CHECK
         }
         else
         {
                 return FGL_ERR_INVALID_PARAMETER;
         }
 #endif
 }
 
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetColorBufWriteMask
  * SYNOPSIS: enable and disable writing of frame(color) buffer color components
  * PARAMETERS: [in] r - whether red can or cannot be written into the frame buffer.
  *             [in] g - whether green can or cannot be written into the frame buffer.
  *             [in] b - whether blue can or cannot be written into the frame buffer.
  *             [in] a - whether alpha can or cannot be written into the frame buffer.
  * RETURNS: FGL_ERR_NO_ERROR, if successful
  * ERRNO:   FGL_ERR_NO_ERROR        1
  *****************************************************************************/
 FGL_Error 
 fglSetColorBufWriteMask ( 
                             FGL_BOOL    r, 
                             FGL_BOOL    g, 
                             FGL_BOOL    b, 
                             FGL_BOOL    a 
                         )
 {
         unsigned int nWriteMaskVal = 0; 
 
 //      if(r) nWriteMaskVal |= SET_BIT(3);              
 //      if(g) nWriteMaskVal |= SET_BIT(2);
 //      if(b) nWriteMaskVal |= SET_BIT(1);      
 //      if(a) nWriteMaskVal |= SET_BIT(0);
     nWriteMaskVal = (r << 3) | (g << 2) | (b << 1) | a;
         
         WRITEREG(FGPF_COLOR_MASK, nWriteMaskVal);
                         
     return FGL_ERR_NO_ERROR;
 }
 
 
 #if TARGET_FIMG_VERSION != _FIMG3DSE_VER_1_2
 /***************************************************************************** 
  * FUNCTIONS: fglSetStencilBufWriteMask
  * SYNOPSIS: control the front and/or back writing of individual bits 
  *           in the stencil buffer.
  * PARAMETERS: [in] face - specifies whether the front and/or back stencil 
  *                         writemask is updated.
  *             [in] mask - A bit mask to enable and disable writing of individual
  *                         bits in the stencil buffer.
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  *          FGL_ERR_INVALID_PARAMETER - The face and mask were not an accepted value.
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *          FGL_ERR_INVALID_PARAMETER   2
  *****************************************************************************/
 FGL_Error 
 fglSetStencilBufWriteMask ( FGL_Face face, unsigned int mask )
 {
 #ifdef _FGL_VALUE_CHECK
         FGL_BOOL bValidParam = (STENCIL_FACE_VALID(face) & STENCIL_MASK_VALID(mask));
 #endif
         
     //unsigned int nStencilMaskVal = READREG(FGPF_STENCIL_DEPTH_MASK);
     unsigned int nStencilMaskVal;
     READREGP(FGPF_STENCIL_DEPTH_MASK, nStencilMaskVal);
         
 #ifdef _FGL_VALUE_CHECK
         if(bValidParam)
         {
 #endif
         switch((FGL_Face)face)
         {
         case FGL_FACE_BACK:
             FGL_SET_BITFIELD(nStencilMaskVal, 31:24, mask);
                     WRITEREG(FGPF_STENCIL_DEPTH_MASK, nStencilMaskVal);
             break;
         case FGL_FACE_FRONT:
             FGL_SET_BITFIELD(nStencilMaskVal, 23:16, mask);
                     WRITEREG(FGPF_STENCIL_DEPTH_MASK, nStencilMaskVal);
             break;
         case FGL_FACE_FRONT_AND_BACK:
             FGL_SET_BITFIELD(nStencilMaskVal, 23:16, mask);
             FGL_SET_BITFIELD(nStencilMaskVal, 31:24, mask);
                     WRITEREG(FGPF_STENCIL_DEPTH_MASK, nStencilMaskVal);
             break;
         default:
             return FGL_ERR_INVALID_PARAMETER;
         }
         
             return FGL_ERR_NO_ERROR;    
 #ifdef _FGL_VALUE_CHECK
         }
         else
         {
             return FGL_ERR_INVALID_PARAMETER;
         }
 #endif
 }
 #endif
 /***************************************************************************** 
  * FUNCTIONS: fglSetZBufWriteMask
  * SYNOPSIS: enables or disables writing into the depth buffer.
  * PARAMETERS: [in] enable - specifies whether the depth buffer is enabled 
  *                           for writing.
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *****************************************************************************/
 FGL_Error 
 fglSetZBufWriteMask ( FGL_BOOL enable )
 {
         //unsigned int nDepthMaskVal = READREG(FGPF_STENCIL_DEPTH_MASK);
         unsigned int nDepthMaskVal;
         READREGP(FGPF_STENCIL_DEPTH_MASK, nDepthMaskVal);
         FGL_SET_BITFIELD(nDepthMaskVal, 0:0, enable);
         WRITEREG(FGPF_STENCIL_DEPTH_MASK, nDepthMaskVal);
         return FGL_ERR_NO_ERROR;        
 }
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetDepthStencilWriteMask
  * SYNOPSIS: enables or disables writing into the depth buffer.
  * PARAMETERS: [IN]backStencilMask: a 8-bit mask to enable and disable writing.
  *             [IN]frontStencilMask: a 8-bit mask to enable and disable writing.
  *             [IN] bDepthBufferMask: if FGL_TRUE,
  *                                     depth buffer writing is not disabled
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *****************************************************************************/
 FGL_Error 
 fglSetDepthStencilWriteMask (
                 unsigned int    backStencilMask, 
                 unsigned int    frontStencilMask,
                 FGL_BOOL                bDepthBufferMask 
             )
 {
         unsigned int nMaskVal = 0;
     // TODO: value checking!!
     nMaskVal = (backStencilMask << 24)
                 | (frontStencilMask << 16)
                 | bDepthBufferMask;
 
         WRITEREG(FGPF_STENCIL_DEPTH_MASK, nMaskVal);
         return FGL_ERR_NO_ERROR;        
 }
 
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetFrameBufParams
  * SYNOPSIS: specifies the value used for frame buffer control.
  * PARAMETERS: [in] fbctrlParam - the pointer parameter of FGL_FBCtrlParam.
  *             opaqueAlpha - after alpha blending, the alpha value is 
  *                           forced to opaque.
  *             thresholdAlpha - specifies an alpha value in the frame buffer 
  *                              ARGB1555 format. 
  *             constAlpha - specifies constant alpha value in the frame 
  *                          buffer ARGB0888 format.
  *             dither - specifies whether the dither is enable or disable.
  *             format - specifies the format used for the frame buffer.
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  *          FGL_ERR_INVALID_PARAMETER - The parameters were not an accepted value.
  * ERRNO:   FGL_ERR_NO_ERROR                1
  *          FGL_ERR_INVALID_PARAMETER       2
  *****************************************************************************/
 FGL_Error 
 fglSetFrameBufParams ( pFGL_FBCtrlParam fbctrlParam )
 {
 
 #ifdef _FGL_VALUE_CHECK
         FGL_BOOL bValidParam = FGL_FALSE;
 #endif
         unsigned int nBufCtrlVal =0 ;
 
 #ifdef _FGL_VALUE_CHECK
     bValidParam = ( ALPHA_THRESHOLD_VALUE_VALID(fbctrlParam->thresholdAlpha) &
                     ALPHA_CONSTANT_VALUE_VALID(fbctrlParam->constAlpha)      &
                     PIXEL_FORMAT_VALID(fbctrlParam->format));
 
         if(bValidParam)
         {
 #endif
                 //FGL_SET_BITFIELD(nBufCtrlVal, 20:20, fbctrlParam->opaqueAlpha);
                 
                 //if((FGL_PixelFormat)fbctrlParam->format == FGL_PIXEL_ARGB1555)
                 //    FGL_SET_BITFIELD(nBufCtrlVal, 19:12, fbctrlParam->thresholdAlpha);
                 
                 //if((FGL_PixelFormat)fbctrlParam->format == FGL_PIXEL_ARGB0888)
                 //    FGL_SET_BITFIELD(nBufCtrlVal, 11:4, fbctrlParam->constAlpha);
                 
                 //FGL_SET_BITFIELD(nBufCtrlVal, 3:3, fbctrlParam->dither);
                 //FGL_SET_BITFIELD(nBufCtrlVal, 2:0, fbctrlParam->format);
 
         nBufCtrlVal = (fbctrlParam->opaqueAlpha << 20)
                         | (fbctrlParam->thresholdAlpha << 12)
                         | (fbctrlParam->constAlpha << 4)
                         | (fbctrlParam->dither << 3)
                         | fbctrlParam->format;
 
                 WRITEREG(FGPF_COLORBUF_CTRL, nBufCtrlVal);
                 return FGL_ERR_NO_ERROR;
 #ifdef _FGL_VALUE_CHECK
         }
         else
         {
             return FGL_ERR_INVALID_PARAMETER;
         }
 #endif
 }
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetZBufBaseAddr
  * SYNOPSIS: Depth and Stencil buffer base address
  * PARAMETERS: [in] addr - specifies the value used for stencil/depth buffer 
  *                         address.
  * RETURNS: FGL_ERR_NO_ERROR, if successful
  * ERRNO:   FGL_ERR_NO_ERROR    1
  *****************************************************************************/
 FGL_Error 
 fglSetZBufBaseAddr ( unsigned int addr )
 {
         WRITEREG(FGPF_DEPTHBUF_ADDR, addr);
         return FGL_ERR_NO_ERROR;        
 }
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetColorBufBaseAddr
  * SYNOPSIS: color buffer base address
  * PARAMETERS: [in] addr - specifies the value used for frame buffer address.
  * RETURNS: FGL_ERR_NO_ERROR, if successful
  * ERRNO:   FGL_ERR_NO_ERROR    1
  *****************************************************************************/
 FGL_Error 
 fglSetColorBufBaseAddr ( unsigned int addr )
 {
         WRITEREG(FGPF_COLORBUF_ADDR, addr);
         return FGL_ERR_NO_ERROR;        
 }
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetFrameBufWidth
  * SYNOPSIS: frame buffer width
  * PARAMETERS: [in] width - specifies the value used for frame buffer width.
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  *          FGL_ERR_INVALID_VALUE - 
  * ERRNO:   FGL_ERR_NO_ERROR        1
  *          FGL_ERR_INVALID_VALUE   7
  *****************************************************************************/
 FGL_Error 
 fglSetFrameBufWidth ( unsigned int width )
 {
 #ifdef _FGL_VALUE_CHECK
         if (width > FGL_ZERO && width <= FGPF_FB_MAX_STRIDE)
         {
 #endif
                 WRITEREG(FGPF_COLORBUF_WIDTH, width);
                 return FGL_ERR_NO_ERROR;
 #ifdef _FGL_VALUE_CHECK
         }
         else
         {
                 //_DEBUG((DBG_ERROR, "Cannot set frame buffer width - invalid width value."));
                 return FGL_ERR_INVALID_VALUE;
         }
 #endif
 }
 
 #if TARGET_FIMG_VERSION == _FIMG3DSE_VER_2_0
 /***************************************************************************** 
  * 
  *  Pre-Depth Test Unit Register-level API 
  * 
  *****************************************************************************/
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetPreStencilTest
  * SYNOPSIS: enables or disables pre-stencil test in PDT.
  * PARAMETERS: [in] enable - specifies whether the pre-stencil test is enabled 
  *                           for writing.
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *****************************************************************************/
 FGL_Error 
 fglSetPreStencilTest ( FGL_BOOL enable )
 {
         unsigned int nStencil;
         READREGP(FGPDT_FFACE_STENCIL, nStencil);
         FGL_SET_BITFIELD(nStencil, 0:0, enable);
         WRITEREG(FGPDT_FFACE_STENCIL, nStencil);
         return FGL_ERR_NO_ERROR;        
 }
 
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetPreDepthTest
  * SYNOPSIS: enables or disables pre-depth test in PDT.
  * PARAMETERS: [in] enable - specifies whether the pre-depth test is enabled 
  *                           for writing.
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *****************************************************************************/
 FGL_Error 
 fglSetPreDepthTest ( FGL_BOOL enable )
 {
         unsigned int nDepth;
         READREGP(FGPDT_DEPTH_TEST, nDepth);
         FGL_SET_BITFIELD(nDepth, 0:0, enable);
         WRITEREG(FGPDT_DEPTH_TEST, nDepth);
         return FGL_ERR_NO_ERROR;        
 }
 
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetPDTStencilParams
  * SYNOPSIS: The stencil test conditionally discards a fragment based on the 
  *           outcome of a comparison between the value in the stencil buffer 
  *           and a reference value.
  * PARAMETERS: [IN] stencilParam: The point argument of FGL_StencilParam
  * RETURNS: FGL_ERR_NO_ERROR - if successful.
  *          FGL_ERR_INVALID_VALUE - The mask and ref of FGL_StencilParam was 
  *                                  not an accepted value .
  *          FGL_ERR_INVALID_PARAMETER - The zpass, zfail, sfail and mode of 
  *                                      FGL_StencilParam was not an accepted 
  *                                      enum value.
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *          FGL_ERR_INVALID_PARAMETER   2
  *          FGL_ERR_INVALID_VALUE       7
  *****************************************************************************/
 FGL_Error 
 fglSetPDTStencilParams (pFGL_StencilParam stencilParam )
 {
         unsigned int nStencil = 0;
         unsigned int nEnable = 0;
 #ifdef __STRICT_CHECK_CORRECT__
     if( !( STENCIL_FACE_VALID (stencilParam->face)      && 
            STENCIL_OP_VALID (stencilParam->zpass)       &&
            STENCIL_OP_VALID (stencilParam->zfail)       && 
            STENCIL_OP_VALID (stencilParam->sfail)       &&
            STENCIL_MASK_VALID (stencilParam->mask)      && 
            STENCIL_REF_VALUE_VALID(stencilParam->ref)   &&
            STENCIL_COMPARE_FUNC_VALID(stencilParam->mode)) )
     {
         return FGL_ERR_INVALID_PARAMETER;
         }
 #endif    
         FGL_SET_BITFIELD(nStencil, 31:29, stencilParam->zpass);
         FGL_SET_BITFIELD(nStencil, 28:26, stencilParam->zfail);
         FGL_SET_BITFIELD(nStencil, 25:23, stencilParam->sfail);
         FGL_SET_BITFIELD(nStencil, 19:12, stencilParam->mask);
         FGL_SET_BITFIELD(nStencil, 11:4, stencilParam->ref);
         FGL_SET_BITFIELD(nStencil, 3:1, stencilParam->mode);
 
     switch((FGL_Face)(stencilParam->face))
     {
     case FGL_FACE_BACK:
             WRITEREG(FGPDT_BFACE_STENCIL, nStencil);
         break;
     case FGL_FACE_FRONT:
             READREGP(FGPDT_FFACE_STENCIL, nEnable);
             nStencil |= (nEnable & STENCIL_ENABLE_MASK);
             WRITEREG(FGPDT_FFACE_STENCIL, nStencil);
         break;
     case FGL_FACE_FRONT_AND_BACK:
             READREGP(FGPDT_FFACE_STENCIL, nEnable);
             nStencil |= (nEnable & STENCIL_ENABLE_MASK);
             WRITEREG(FGPDT_FFACE_STENCIL, nStencil);
             WRITEREG(FGPDT_BFACE_STENCIL, nStencil);
         break;
     default:
         return FGL_ERR_INVALID_PARAMETER;
     }
     
         return FGL_ERR_NO_ERROR;
 }
 
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetPDTDepthParams
  * SYNOPSIS: This function specifies the value used for depth-buffer comparisons.
  * PARAMETERS: [IN] mode: Specifies the depth-comparison function
  * RETURNS: FGL_ERR_NO_ERROR - if successful.
  *          FGL_ERR_INVALID_PARAMETER - mode was not an accepted enum value.
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *          FGL_ERR_INVALID_PARAMETER   2
  *****************************************************************************/
 FGL_Error 
 fglSetPDTDepthParams ( FGL_CompareFunc mode )
 {
         unsigned int nDepth;
 #ifdef __STRICT_CHECK_CORRECT__
         if (!DEPTH_COMPARE_FUNC_VALID(mode))
         {
                 //_DEBUG((DBG_ERROR, "Cannot set depth test register - invalid parameters."));
                 return FGL_ERR_INVALID_PARAMETER;
         }
 #endif
         READREGP(FGPDT_DEPTH_TEST, nDepth);
         FGL_SET_BITFIELD(nDepth, 3:1, mode);
         WRITEREG(FGPDT_DEPTH_TEST, nDepth);
 
         return FGL_ERR_NO_ERROR;
 }
 
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetPDTStencilBufWriteMask
  * SYNOPSIS: control the front and/or back writing of individual bits 
  *           in the stencil buffer.
  * PARAMETERS: [in] face - specifies whether the front and/or back stencil 
  *                         writemask is updated.
  *             [in] mask - A bit mask to enable and disable writing of individual
  *                         bits in the stencil buffer.
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  *          FGL_ERR_INVALID_PARAMETER - The face and mask were not an accepted value.
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *          FGL_ERR_INVALID_PARAMETER   2
  *****************************************************************************/
 FGL_Error 
 fglSetPDTStencilBufWriteMask( FGL_Face face, unsigned int mask )
 {
 #ifdef __STRICT_CHECK_CORRECT__
         if(!(STENCIL_FACE_VALID(face) && STENCIL_MASK_VALID(mask)))
         {
             return FGL_ERR_INVALID_PARAMETER;
         }
 #endif  
     switch((FGL_Face)face)
     {
     case FGL_FACE_BACK:
             WRITEREG(FGPDT_BFSTENCIL_WMASK, mask);
         break;
     case FGL_FACE_FRONT:
             WRITEREG(FGPDT_FFSTENCIL_WMASK, mask);
         break;
     case FGL_FACE_FRONT_AND_BACK:
         WRITEREG(FGPDT_FFSTENCIL_WMASK, mask);
             WRITEREG(FGPDT_BFSTENCIL_WMASK, mask);
         break;
     default:
         return FGL_ERR_INVALID_PARAMETER;
     }
     
     return FGL_ERR_NO_ERROR;    
 }
 
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetPDTZBufWriteMask
  * SYNOPSIS: enables or disables writing into the depth buffer.
  * PARAMETERS: [in] enable - specifies whether the depth buffer is enabled 
  *                           for writing.
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  * ERRNO:   FGL_ERR_NO_ERROR            1
  *****************************************************************************/
 FGL_Error 
 fglSetPDTZBufWriteMask ( FGL_BOOL enable )
 {
         unsigned int nWriteMask;
         READREGP(FGPDT_DEPTH_TEST, nWriteMask);
         FGL_SET_BITFIELD(nWriteMask, 4:4, enable);
         WRITEREG(FGPDT_DEPTH_TEST, nWriteMask);
         return FGL_ERR_NO_ERROR;        
 }
 
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetPDTZBufBaseAddr
  * SYNOPSIS: Depth and Stencil buffer base address for PDT feature
  * PARAMETERS: [in] addr - specifies the value used for stencil/depth buffer 
  *                         address.
  * RETURNS: FGL_ERR_NO_ERROR, if successful
  * ERRNO:   FGL_ERR_NO_ERROR    1
  *****************************************************************************/
 FGL_Error 
 fglSetPDTZBufBaseAddr ( unsigned int addr )
 {
         WRITEREG(FGPDT_DEPTHBUF_BASE, addr);
         return FGL_ERR_NO_ERROR;        
 }
 
 
 /***************************************************************************** 
  * FUNCTIONS: fglSetPDTFrameBufWidth
  * SYNOPSIS: frame buffer width for PDT feature
  * PARAMETERS: [in] width - specifies the value used for frame buffer width.
  * RETURNS: FGL_ERR_NO_ERROR - if successful
  *          FGL_ERR_INVALID_VALUE - 
  * ERRNO:   FGL_ERR_NO_ERROR        1
  *          FGL_ERR_INVALID_VALUE   7
  *****************************************************************************/
 FGL_Error 
 fglSetPDTFrameBufWidth ( unsigned int width )
 {
 #ifdef __STRICT_CHECK_CORRECT__
         if ((width < FGL_ONE) || (width > FGPF_FB_MAX_STRIDE))
         {
                 //_DEBUG((DBG_ERROR, "Cannot set frame buffer width - invalid width value."));
                 return FGL_ERR_INVALID_VALUE;
         }
         
 #endif
         WRITEREG(FGPDT_DEPTHBUF_WIDTH, width);
         return FGL_ERR_NO_ERROR;
 }
 #endif /* TARGET_FIMG_VERSION == _FIMG3DSE_VER_2_0 */
 
 /*-----------------------< End of file >----------------------------- */
