
/*
*******************************************************************************
*
*                        SAMSUNG INDIA SOFTWARE OPERATIONS
*                               Copyright(C) 2006
*                              ALL RIGHTS RESERVED
*
* This program is proprietary  to Samsung India  Software Operations Pvt. Ltd.,
* and is protected under International Copyright Act as an unpublished work.Its
* use and disclosure is limited by the terms and conditions of a license agree-
* -ment. It may not be  copied or otherwise  reproduced or disclosed to persons
* outside the licensee's  organization except in accordance  with the terms and
* conditions of  such an agreement. All copies and  reproductions  shall be the
* property of  Samsung  India Software Operations Pvt. Ltd.  and must bear this
* notice in its entirety.
*
*******************************************************************************
*/

/*
***************************************************************************//*!
*
* \file         binaryShaderFormat.h
* \author       Sandeep Kakarlapudi (s.kakarla@samsung.com)
*                       Anurag Ved (anuragv@samsung.com)
* \brief        Structures and constant definitions for binary shader format
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*******************************************************************************
*/

#ifndef __BINARYSHADERFORMAT_H__
#define __BINARYSHADERFORMAT_H__

/*
*******************************************************************************
* Macro definitions and enumerations
*******************************************************************************
*/

typedef unsigned int    uint32;
typedef unsigned short  uint16;
typedef unsigned char   ubyte;


#define SH_ALIGN_PACKED 

/*
#if defined( _MSC_VER )
#define SH_ALIGN_PACKED __declspec(align(1)) 
#elif defined( __CC_ARM )
#define SH_ALIGN_PACKED __packed
#elif defined( __GNUC__ )
#define SH_ALIGN_PACKED __attribute__((packed))
#else
#error SH_ALIGN_PACKED is not defined for this compiler!
#endif
*/

enum
{
    BINARY_SHADER_VERSION = 0xFFFF0008,
};


/* NOTE: Do not change the ordering of members of this enum. 
 *       Changing the order will break the asembler code (see fn ReadDclDataType)
 */
typedef enum
{
  DCL_TYPE_FLOAT,
  DCL_TYPE_INT,
  DCL_TYPE_BOOL,

  DCL_TYPE_VEC2_FLOAT,
  DCL_TYPE_VEC2_INT,
  DCL_TYPE_VEC2_BOOL,

  DCL_TYPE_VEC3_FLOAT,
  DCL_TYPE_VEC3_INT,
  DCL_TYPE_VEC3_BOOL,

  DCL_TYPE_VEC4_FLOAT,
  DCL_TYPE_VEC4_INT,
  DCL_TYPE_VEC4_BOOL,

  DCL_TYPE_MAT2_FLOAT,
  /* there is no support for 2x2 matrices of types int and bool */

  DCL_TYPE_MAT3_FLOAT,
  /* there is no support for 3x3 matrices of types int and bool */

  DCL_TYPE_MAT4_FLOAT,
  /* there is no support for 4x4 matrices of types int and bool */

  DCL_TYPE_SAMPLER_2D,
  DCL_TYPE_SAMPLER_3D,
  DCL_TYPE_SAMPLER_CUBE,

  DCL_TYPE_NONE

} DclDataType;

typedef enum
{
  DCL_REGCLASS_INPUT,
  DCL_REGCLASS_OUTPUT,
  DCL_REGCLASS_UNIFORM,
  DCL_REGCLASS_SAMPLER
} DclRegClass;

typedef enum
{
  DCL_REGSET_INPUT,
  DCL_REGSET_CONST_FLOAT,
  DCL_REGSET_CONST_INT,
  DCL_REGSET_CONST_BOOL,
  DCL_REGSET_OUTPUT,
  DCL_REGSET_SAMPLER
} DclRegSet;

enum
{
    NR_DCL_REG_CLASSES =  4,
};

/*
*******************************************************************************
* Type, Structure & Class Definitions
*******************************************************************************
*/

SH_ALIGN_PACKED typedef struct DclBaseType_t
{
  uint32      name_offset;      /* name offset relative to the start of the string-block */
        uint32      name_length;                  /* length of the name */
        
        uint32      type;                               /* data type (DclDataType) */

  uint16      regset;           /* register set (DclRegSet) */
  uint16      regclass;         /* register class (DclRegClass) */

  uint32      start_reg_index;  /* index of the register (including its component) */
                                                                /* The lsb 2 bits indicate where the first component  \
                                                                is located in the register. If the first component of \
                                                                this variable is accessed by c0.y then the lsb is set \
                                                                to 01b .x .y .z .w correspond to 00b 01b 10b 11b    */

} DclBaseType;

SH_ALIGN_PACKED typedef struct DclArrayType_t
{
        uint32      name_offset;                  /* name offset relative to the start of the string-block */
        uint32      name_length;                  /* length of the name */

        uint32      type;                               /* data type (DclDataType) */

        uint16      regset;           /* register set (DclRegSet) */
        uint16      regclass;           /* register class (DclRegClass) */

        uint32      start_reg_index;    /* index of the register (including its component) */
                                                                        /* The lsb 2 bits indicate where the first component  \
                                                                is located in the register. If the first component of \
                                                                this variable is accessed by c0.y then the lsb is set \
                                                                to 01b .x .y .z .w correspond to 00b 01b 10b 11b    */

        uint32      num_elements;                 /* number of elements in the array */
        uint32      stride;                           /* num of components to skip to get to the next element */

} DclArrayType;

SH_ALIGN_PACKED typedef struct ShaderHeader_t
{
  uint32    magic;
  uint32    version;
  uint32    header_sz;
  uint32    fimg_version;

  uint32    instruction_sz;             /* Number of instructions */
  uint32    const_float_sz;
  uint32    const_int_sz;
  uint32    const_bool_sz;

  uint32    dcl_base_type_dcls_table_sz[NR_DCL_REG_CLASSES];
  uint32    dcl_array_type_dcls_table_sz[NR_DCL_REG_CLASSES];
  uint32    dcl_string_table_sz;

  uint32    start_address;
} ShaderHeader;

/*
*******************************************************************************
* BINARY SHADER LAYOUT
*******************************************************************************


  // ---------------------------------------------
  // Shader Object Header
  // ---------------------------------------------
  // MAGIC (PS  ) or (VS  )
  // VERSION
  // HEADER SIZE (64 Bytes)
  // INSTRUCTION SIZE
  // CONSTANT FLOAT SIZE
  // CONSTANT INTEGER SIZE
  // CONSTANT BOOLEAN SIZE
  // DCL - BASE TYPE - INPUTS - COUNT
  // DCL - BASE TYPE - OUTPUTS - COUNT
  // DCL - BASE TYPE - UNIFORMS - COUNT
  // DCL - BASE TYPE - SAMPLERS - COUNT
  // DCL - ARRAY TYPE - INPUTS - COUNT
  // DCL - ARRAY TYPE - OUTPUTS - COUNT
  // DCL - ARRAY TYPE - UNIFORMS - COUNT
  // DCL - ARRAY TYPE - SAMPLERS - COUNT
  // DCL - STRING TABLE SIZE

  // ---------------------------------------------
  // Instruction Area
  // - 0x0, 0x4, 0x8, 0xc (Data at 0xC will not be written.)
  // ...
  // ---------------------------------------------

  // ---------------------------------------------
  // Constant Float Registers (0 ~ 255)
  // - x , y , z , w
  // ...
  // ---------------------------------------------

  // ---------------------------------------------
  // Constant Integer Registers (0 ~ 15)
  // - w/z/y/x ( w channel will not be written. )
  // - each channel has 8-bit width
  // ...
  // ---------------------------------------------

  // ---------------------------------------------
  // Constant Boolean Registers (0 ~ 15)
  // ...
  // ---------------------------------------------

  // ---------------------------------------------
  // DCL Table
  // ---------------------------------------------
  // DCL - BASE TYPE - INPUTS
  // ...
  // ---------------------------------------------
  // DCL - BASE TYPE - OUTPUTS
  // ...
  // ---------------------------------------------
  // DCL - BASE TYPE - UNIFORMS
  // ...
  // ---------------------------------------------
  // DCL - BASE TYPE - SAMPLERS
  // ...
  // ---------------------------------------------
  // DCL - ARRAY TYPE - INPUTS
  // ...
  // ---------------------------------------------
  // DCL - ARRAY TYPE - OUTPUTS
  // ...
  // ---------------------------------------------
  // DCL - ARRAY TYPE - UNIFORMS
  // ...
  // ---------------------------------------------
  // DCL - ARRAY TYPE - SAMPLERS
  // ...
  // ---------------------------------------------
  // STRING TABLE
  // This is stored as a list of null terminated
  // strings. [0] gives the first character of the 
  // first string.
  // ...
  // ---------------------------------------------
*/

#endif //__BINARYSHADERFORMAT_H__
