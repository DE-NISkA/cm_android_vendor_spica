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
* \file         glConfig.h
* \author       Prashant Singh (prashant@samsung.com)
* \brief        Configuration file
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*******************************************************************************
*/

#ifndef __GL_CONFIG_H__
#define __GL_CONFIG_H__

/*
*******************************************************************************
* Includes
*******************************************************************************
*/

/*
*******************************************************************************
* Macro definitions and enumerations
*******************************************************************************
*/

#ifdef _MSC_VER

#define FIMG_PLATFORM 1

#else

#define FIMG_PLATFORM 1

#endif

//-----------------------------------------------------------------------------
// Hardware constraints
//-----------------------------------------------------------------------------

enum {
	MAX_TEXTURE_UNITS           = 8,                                            //!< No. of texture units
	MAX_VERTEX_TEXTURE_UNITS    = 4,
	MAX_MIPMAP_LEVELS           = 12,                                           //!< Max mipmap levels
	MAX_TEXTURE_SIZE            = 2048,
	MAX_CUBEMAP_TEXTURE_SIZE    = 2048,
	
	
	MAX_VERTEX_ATTRIBS          = 10,
	MAX_VARYING_VECTORS         = 8,
	MAX_UNIFORMS                = 256,
};

//-----------------------------------------------------------------------------
// Lib configuration
//-----------------------------------------------------------------------------

enum {
	
	MAX_BUFFER_OBJECTS              = 1024,//100,
	
	NUM_COMPRESSED_TEXTURE_FORMATS  = 12,

	MAX_TEXTURE_OBJECTS             = 1024,                                         //!< Maximum number of texture objects
	MAX_VARYING_VARS                = MAX_VARYING_VECTORS*4,
	MAX_SAMPLER_VARS                = 8,
	MAX_VERTEX_ATTRIB_VARS          = 10,

	MAX_VERTEX_ATTRIB_BINDINGS      = 20,
	MAX_VARYING_MAPPINGS            = MAX_VARYING_VECTORS + 1, //8 float4s + 1 position
	
	MAX_RENDERBUFFER_SIZE           = 2048,
};

enum {
	GLF_TEMP_STRING_BUFFER_LENGTH   = 1024,
};

enum{
	MAX_VIEWPORT_DIMS               = 4096,
	MAX_ELEMENTS_INDICES            = 4096,
	MAX_ELEMENTS_VERTICES           = 4096,
	NO_OF_SAMPLES                   = 0, //multi sampling is not supported by the hardware
	MULTISAMPLING                   = 0,
	NUM_SHADER_BINARY_FORMATS       = 1,
};



enum{
	
	GLF_SHADERRANGE_FLOATMIN        = -126,
	GLF_SHADERRANGE_FLOATMAX        = 126,
	GLF_SHADERPRECISION_FLOAT       = 23,

	GLF_SHADERRANGE_INTMIN          = -16,
	GLF_SHADERRANGE_INTMAX          =  16,
	GLF_SHADERPRECISION_INT         = 16,


};

#define GLES2_LIBRARY_VERSION "(lib version 20.02.70.b)"
#define GLES2_VERSION_STRING "2.0 " GLES2_LIBRARY_VERSION


//-----------------------------------------------------------------------------
// JIT optimizer
//-----------------------------------------------------------------------------
//#define FSO_JITO


//-----------------------------------------------------------------------------
// Extensions
//-----------------------------------------------------------------------------

#define ENABLE                                  1
#define DISABLE                                 0
//!< Non power of 2 texture
#define OES_TEXTURE_NPOT                ENABLE                                          

#define EXT_ALPHA_TEST_EXP              ENABLE                      

//-----------------------------------------------------------------------------
// GLF options
//-----------------------------------------------------------------------------



#define GLF_DRAW                        ENABLE       

#define GLF_SCISSOR_IN_RA               ENABLE  //this will do scissor test in rasterization instead of per fragment unit

#if  defined(MULTI_CONTEXT) || defined(MULTI_PROCESS)
#define TRACE_DIRTY_STATE               DISABLE
#else
#define TRACE_DIRTY_STATE               ENABLE  // for enabling dirty state tracking of fimg register.
#endif

#define  STORE_TEX_OBJ_POINTER          ENABLE

#define RESET_UNUSED_TEXTURE_UNITS      DISABLE

//cglee
#define INTERRUPT_MODE_ENABLED
#define G3D_IOCTL_MAGIC          'S'
#define WAIT_FOR_FLUSH           _IO(G3D_IOCTL_MAGIC, 2) //dsdsdadadwerwer
#define GET_CONFIG               _IO(G3D_IOCTL_MAGIC, 101)
#define START_DMA_BLOCK          _IO(G3D_IOCTL_MAGIC, 102)


/*
*******************************************************************************
* Type, Structure & Class Definitions
*******************************************************************************
*/

/*
*******************************************************************************
* Global Variable Declarations
*******************************************************************************
*/

/*
*******************************************************************************
* Function Declarations
*******************************************************************************
*/

#endif /*__GL_CONFIG_H__*/

