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
* \file         debugHelper.h
* \author       Sandeep Kakarlapudi (s.kakarla@samsung.com)
* \brief        Debugging helper routiens
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*       03.05.2007      Sandeep Kakarlapudi             Initial version
*
*******************************************************************************
*/


#include "debugHelper.h"
#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
//needed to get the IsDebuggerPresent func
#define _WIN32_WINNT 0x0400

#include <Windows.h>
#include <Winbase.h>
#endif

struct ___Tracker
{
    const char* lastCalledFunc;
    const char* lastCalledGLFunc;
};

___Tracker tracker;


const char* getLastGLFunc()
{
    return tracker.lastCalledGLFunc;
}
const char* getLastFunc()
{
    return tracker.lastCalledFunc;
}

const char* getGLErrorString(GLenum err)
{
    const char* errString;

    switch(err)
    {
        case GL_NO_ERROR:           errString = "GL_NO_ERROR"; break;
        case GL_INVALID_ENUM:       errString = "GL_INVALID_ENUM"; break;
        case GL_INVALID_VALUE:      errString = "GL_INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:  errString = "GL_INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:     errString = "GL_STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:    errString = "GL_STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:      errString = "GL_OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: errString = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
        default:                    errString = "UNKNOWN ERROR!!"; break;
    }

    return errString;
}

bool isGLFunc(const char* funcName)
{
    if(strlen(funcName)>3)
    {
        if(funcName[0]=='g' && funcName[1]=='l' && funcName[2]!='f')
            return true;
    }

    return false;
}

void ___trackFunc(const char* funcName, const char* fileName)
{

    tracker.lastCalledFunc = funcName;
    
    if(isGLFunc(funcName))
        tracker.lastCalledGLFunc = funcName;

    //LOGMSG("\n~~~~ %s\n",funcName);
}

int ___funcPrint(const char* format, ...)
{

    va_list args;
    va_start(args, format);
    int res = 0;
#if 0
    LOGMSG("\n~~~~");
    res = vprintf(format, args);
    LOGMSG("\n");
    va_end(args);
#endif
    return res;
}

/*
bool ___funcBreak(const char* funcName, const char* fileName)
{
    FILE* fp = fopen("C:/GLES2Config/config.txt","r");
    if(!fp)
    {
        return false;
    }
#ifdef _WIN32    
    if(!IsDebuggerPresent())
        return false;
#endif
    return false;
}*/

const char* GLenum2Str(GLenum x)
{ 
        //Pray that the implementation supports this big a switch :D
        
        switch(x)
        {
        

        case 16384 :            return "GL_COLOR_BUFFER_BIT";
        case 0 :                return "GL_FALSE or GL_POINTS or GL_ZERO or GL_NO_ERROR or GL_NONE";
        case 2 :                return "GL_LINE_LOOP";
        case 3 :                return "GL_LINE_STRIP";
        case 4 :                return "GL_TRIANGLES";
        case 5 :                return "GL_TRIANGLE_STRIP";
        case 6 :                return "GL_TRIANGLE_FAN";
        case 1 :                return "GL_TRUE or GL_LINES or GL_ONE";
        case 32777 :            return "GL_BLEND_EQUATION or GL_BLEND_EQUATION_RGB";
        case 32778 :            return "GL_FUNC_SUBTRACT";
        case 32779 :            return "GL_FUNC_REVERSE_SUBTRACT";
        case 32770 :            return "GL_ONE_MINUS_CONSTANT_COLOR";
        case 32771 :            return "GL_CONSTANT_ALPHA";
        case 32772 :            return "GL_ONE_MINUS_CONSTANT_ALPHA";
        case 32773 :            return "GL_BLEND_COLOR";
        case 36048 :            return "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE";
        case 32774 :            return "GL_FUNC_ADD";
        case 32819 :            return "GL_UNSIGNED_SHORT_4_4_4_4";
        case 32820 :            return "GL_UNSIGNED_SHORT_5_5_5_1";
        case 32823 :            return "GL_POLYGON_OFFSET_FILL";
        case 32824 :            return "GL_POLYGON_OFFSET_FACTOR";
        case 34877 :            return "GL_BLEND_EQUATION_ALPHA";
        case 32849 :            return "GL_RGB8";
        case 32854 :            return "GL_RGBA4";
        case 32855 :            return "GL_RGB5_A1";
        case 32856 :            return "GL_RGBA8";
        case 32873 :            return "GL_TEXTURE_BINDING_2D";
        case 32874 :            return "GL_TEXTURE_BINDING_3D";
        case 32879 :            return "GL_TEXTURE_3D";
        case 34930 :            return "GL_MAX_TEXTURE_IMAGE_UNITS";
        case 32883 :            return "GL_MAX_3D_TEXTURE_SIZE";
        case 34962 :            return "GL_ARRAY_BUFFER";
        case 34963 :            return "GL_ELEMENT_ARRAY_BUFFER";
        case 34964 :            return "GL_ARRAY_BUFFER_BINDING";
        case 34965 :            return "GL_ELEMENT_ARRAY_BUFFER_BINDING";
        case 32926 :            return "GL_SAMPLE_ALPHA_TO_COVERAGE";
        case 32928 :            return "GL_SAMPLE_COVERAGE";
        case 5120 :             return "GL_BYTE";
        case 32936 :            return "GL_SAMPLE_BUFFERS";
        case 32937 :            return "GL_SAMPLES";
        case 32938 :            return "GL_SAMPLE_COVERAGE_VALUE";
        case 32939 :            return "GL_SAMPLE_COVERAGE_INVERT";
        case 35001 :            return "GL_WRITE_ONLY";
        case 35003 :            return "GL_BUFFER_ACCESS";
        case 35004 :            return "GL_BUFFER_MAPPED";
        case 35005 :            return "GL_BUFFER_MAP_POINTER";
        case 5121 :             return "GL_UNSIGNED_BYTE";
        case 32968 :            return "GL_BLEND_DST_RGB";
        case 32969 :            return "GL_BLEND_SRC_RGB";
        case 32970 :            return "GL_BLEND_DST_ALPHA";
        case 32971 :            return "GL_BLEND_SRC_ALPHA";
        case 35040 :            return "GL_STREAM_DRAW";
        case 35044 :            return "GL_STATIC_DRAW";
        case 35048 :            return "GL_DYNAMIC_DRAW";
        case 33001 :            return "GL_MAX_ELEMENTS_INDICES";
        case 2304 :             return "GL_CW";
        case 2305 :             return "GL_CCW";
        case 4354 :             return "GL_NICEST";
        case 6406 :             return "GL_ALPHA";
        case 6407 :             return "GL_RGB";
        case 6408 :             return "GL_RGBA";
        case 6409 :             return "GL_LUMINANCE";
        case 6410 :             return "GL_LUMINANCE_ALPHA";
        case 5124 :             return "GL_INT";
        case 33062 :            return "GL_POINT_SIZE_MIN";
        case 33063 :            return "GL_POINT_SIZE_MAX";
        case 33071 :            return "GL_CLAMP_TO_EDGE";
        case 5125 :             return "GL_UNSIGNED_INT";
        case 10240 :            return "GL_TEXTURE_MAG_FILTER";
        case 10241 :            return "GL_TEXTURE_MIN_FILTER";
        case 7681 :             return "GL_REPLACE";
        case 10242 :            return "GL_TEXTURE_WRAP_S";
        case 33170 :            return "GL_GENERATE_MIPMAP_HINT";
        case 10243 :            return "GL_TEXTURE_WRAP_T";
        case 33189 :            return "GL_DEPTH_COMPONENT16";
        case 33190 :            return "GL_DEPTH_COMPONENT24";
        case 33191 :            return "GL_DEPTH_COMPONENT32";
        case 512 :              return "GL_NEVER";
        case 513 :              return "GL_LESS";
        case 514 :              return "GL_EQUAL";
        case 515 :              return "GL_LEQUAL";
        case 516 :              return "GL_GREATER";
        case 517 :              return "GL_NOTEQUAL";
        case 518 :              return "GL_GEQUAL";
        case 519 :              return "GL_ALWAYS";
        case 9986 :             return "GL_NEAREST_MIPMAP_LINEAR";
        case 9987 :             return "GL_LINEAR_MIPMAP_LINEAR";
        case 34921 :            return "GL_MAX_VERTEX_ATTRIBS";
        case 34922 :            return "GL_VERTEX_ATTRIB_ARRAY_NORMALIZED";
        case 32882 :            return "GL_TEXTURE_WRAP_R";
        case 10752 :            return "GL_POLYGON_OFFSET_UNITS";
        case 5890 :             return "GL_TEXTURE";
        case 768 :              return "GL_SRC_COLOR";
        case 769 :              return "GL_ONE_MINUS_SRC_COLOR";
        case 770 :              return "GL_SRC_ALPHA";
        case 771 :              return "GL_ONE_MINUS_SRC_ALPHA";
        case 772 :              return "GL_DST_ALPHA";
        case 773 :              return "GL_ONE_MINUS_DST_ALPHA";
        case 774 :              return "GL_DST_COLOR";
        case 775 :              return "GL_ONE_MINUS_DST_COLOR";
        case 776 :              return "GL_SRC_ALPHA_SATURATE";
        case 2833 :             return "GL_POINT_SIZE";
        case 7937 :             return "GL_RENDERER";
        case 2849 :             return "GL_LINE_WIDTH";
        case 35632 :            return "GL_FRAGMENT_SHADER";
        case 35633 :            return "GL_VERTEX_SHADER";
        case 9984 :             return "GL_NEAREST_MIPMAP_NEAREST";
        case 2960 :             return "GL_STENCIL_TEST";
        case 2884 :             return "GL_CULL_FACE";
        case 2885 :             return "GL_CULL_FACE_MODE";
        case 2886 :             return "GL_FRONT_FACE";
        case 35657 :            return "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS";
        case 35658 :            return "GL_MAX_VERTEX_UNIFORM_COMPONENTS";
        case 35659 :            return "GL_MAX_VARYING_FLOATS";
        case 35660 :            return "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS";
        case 35661 :            return "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS";
        case 35663 :            return "GL_SHADER_TYPE";
        case 35664 :            return "GL_FLOAT_VEC2";
        case 35665 :            return "GL_FLOAT_VEC3";
        case 35666 :            return "GL_FLOAT_VEC4";
        case 35667 :            return "GL_INT_VEC2";
        case 35668 :            return "GL_INT_VEC3";
        case 35669 :            return "GL_INT_VEC4";
        case 35670 :            return "GL_BOOL";
        case 35671 :            return "GL_BOOL_VEC2";
        case 35672 :            return "GL_BOOL_VEC3";
        case 35673 :            return "GL_BOOL_VEC4";
        case 35674 :            return "GL_FLOAT_MAT2";
        case 35675 :            return "GL_FLOAT_MAT3";
        case 35676 :            return "GL_FLOAT_MAT4";
        case 35678 :            return "GL_SAMPLER_2D";
        case 35679 :            return "GL_SAMPLER_3D";
        case 35680 :            return "GL_SAMPLER_CUBE";
        case 33635 :            return "GL_UNSIGNED_SHORT_5_6_5";
        case 33648 :            return "GL_MIRRORED_REPEAT";
        case 2929 :             return "GL_DEPTH_TEST";
        case 2930 :             return "GL_DEPTH_WRITEMASK";
        case 2931 :             return "GL_DEPTH_CLEAR_VALUE";
        case 2932 :             return "GL_DEPTH_FUNC";
        case 35712 :            return "GL_DELETE_STATUS";
        case 35713 :            return "GL_COMPILE_STATUS";
        case 35714 :            return "GL_LINK_STATUS";
        case 35715 :            return "GL_VALIDATE_STATUS";
        case 35716 :            return "GL_INFO_LOG_LENGTH";
        case 35717 :            return "GL_ATTACHED_SHADERS";
        case 35718 :            return "GL_ACTIVE_UNIFORMS";
        case 35719 :            return "GL_ACTIVE_UNIFORM_MAX_LENGTH";
        case 35720 :            return "GL_SHADER_SOURCE_LENGTH";
        case 35721 :            return "GL_ACTIVE_ATTRIBUTES";
        case 35722 :            return "GL_ACTIVE_ATTRIBUTE_MAX_LENGTH";
        case 35723 :            return "GL_FRAGMENT_SHADER_DERIVATIVE_HINT";
        case 35724 :            return "GL_SHADING_LANGUAGE_VERSION";
        case 35725 :            return "GL_CURRENT_PROGRAM";
        case 35728 :            return "GL_PALETTE4_RGB8_OES";
        case 35729 :            return "GL_PALETTE4_RGBA8_OES";
        case 35730 :            return "GL_PALETTE4_R5_G6_B5_OES";
        case 2963 :             return "GL_STENCIL_VALUE_MASK";
        case 35732 :            return "GL_PALETTE4_RGB5_A1_OES";
        case 35733 :            return "GL_PALETTE8_RGB8_OES";
        case 2966 :             return "GL_STENCIL_PASS_DEPTH_PASS";
        case 35735 :            return "GL_PALETTE8_R5_G6_B5_OES";
        case 35736 :            return "GL_PALETTE8_RGBA4_OES";
        case 35737 :            return "GL_PALETTE8_RGB5_A1_OES";
        case 35738 :            return "GL_IMPLEMENTATION_COLOR_READ_TYPE_OES";
        case 35739 :            return "GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES";
        case 2978 :             return "GL_VIEWPORT";
        case 3024 :             return "GL_DITHER";
        case 3040 :             return "GL_BLEND_DST";
        case 3041 :             return "GL_BLEND_SRC";
        case 3042 :             return "GL_BLEND";
        case 3056 :             return "GL_LOGIC_OP_MODE";
        case 3058 :             return "GL_COLOR_LOGIC_OP";
        case 2900 :             return "GL_SHADE_MODEL";
        case 1024 :             return "GL_STENCIL_BUFFER_BIT";
        case 9728 :             return "GL_NEAREST";
        case 5122 :             return "GL_SHORT";
        case 5123 :             return "GL_UNSIGNED_SHORT";
        case 1028 :             return "GL_FRONT";
        case 1029 :             return "GL_BACK";
        case 5126 :             return "GL_FLOAT";
        case 9729 :             return "GL_LINEAR";
        case 1032 :             return "GL_FRONT_AND_BACK";
        case 5132 :             return "GL_FIXED";
        case 3088 :             return "GL_SCISSOR_BOX";
        case 3089 :             return "GL_SCISSOR_TEST";
        case 3106 :             return "GL_COLOR_CLEAR_VALUE";
        case 3107 :             return "GL_COLOR_WRITEMASK";
        case 32769 :            return "GL_CONSTANT_COLOR";
        case 34816 :            return "GL_STENCIL_BACK_FUNC";
        case 34817 :            return "GL_STENCIL_BACK_FAIL";
        case 33901 :            return "GL_ALIASED_POINT_SIZE_RANGE";
        case 33902 :            return "GL_ALIASED_LINE_WIDTH_RANGE";
        case 34818 :            return "GL_STENCIL_BACK_PASS_DEPTH_FAIL";
        case 34819 :            return "GL_STENCIL_BACK_PASS_DEPTH_PASS";
        case 2928 :             return "GL_DEPTH_RANGE";
        case 36003 :            return "GL_STENCIL_BACK_REF";
        case 36004 :            return "GL_STENCIL_BACK_VALUE_MASK";
        case 36005 :            return "GL_STENCIL_BACK_WRITEMASK";
        case 36006 :            return "GL_FRAMEBUFFER_BINDING";
        case 36007 :            return "GL_RENDERBUFFER_BINDING";
        case 33984 :            return "GL_TEXTURE0";
        case 33985 :            return "GL_TEXTURE1";
        case 33986 :            return "GL_TEXTURE2";
        case 33987 :            return "GL_TEXTURE3";
        case 33988 :            return "GL_TEXTURE4";
        case 33989 :            return "GL_TEXTURE5";
        case 33990 :            return "GL_TEXTURE6";
        case 33991 :            return "GL_TEXTURE7";
        case 33992 :            return "GL_TEXTURE8";
        case 33993 :            return "GL_TEXTURE9";
        case 33994 :            return "GL_TEXTURE10";
        case 33995 :            return "GL_TEXTURE11";
        case 33996 :            return "GL_TEXTURE12";
        case 33997 :            return "GL_TEXTURE13";
        case 33998 :            return "GL_TEXTURE14";
        case 33999 :            return "GL_TEXTURE15";
        case 34000 :            return "GL_TEXTURE16";
        case 34001 :            return "GL_TEXTURE17";
        case 34002 :            return "GL_TEXTURE18";
        case 34003 :            return "GL_TEXTURE19";
        case 34004 :            return "GL_TEXTURE20";
        case 34005 :            return "GL_TEXTURE21";
        case 34006 :            return "GL_TEXTURE22";
        case 34007 :            return "GL_TEXTURE23";
        case 34008 :            return "GL_TEXTURE24";
        case 34009 :            return "GL_TEXTURE25";
        case 34010 :            return "GL_TEXTURE26";
        case 34011 :            return "GL_TEXTURE27";
        case 34012 :            return "GL_TEXTURE28";
        case 36061 :            return "GL_FRAMEBUFFER_UNSUPPORTED";
        case 34014 :            return "GL_TEXTURE30";
        case 36063 :            return "GL_MAX_COLOR_ATTACHMENTS";
        case 36064 :            return "GL_COLOR_ATTACHMENT0";
        case 36065 :            return "GL_COLOR_ATTACHMENT1";
        case 34018 :            return "GL_MAX_TEXTURE_UNITS";
        case 36067 :            return "GL_COLOR_ATTACHMENT3";
        case 36068 :            return "GL_COLOR_ATTACHMENT4";
        case 36069 :            return "GL_COLOR_ATTACHMENT5";
        case 36070 :            return "GL_COLOR_ATTACHMENT6";
        case 36049 :            return "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME";
        case 34024 :            return "GL_MAX_RENDERBUFFER_SIZE";
        case 36073 :            return "GL_COLOR_ATTACHMENT9";
        case 36074 :            return "GL_COLOR_ATTACHMENT10";
        case 36075 :            return "GL_COLOR_ATTACHMENT11";
        case 36076 :            return "GL_COLOR_ATTACHMENT12";
        case 36050 :            return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL";
        case 36078 :            return "GL_COLOR_ATTACHMENT14";
        case 36079 :            return "GL_COLOR_ATTACHMENT15";
        case 36051 :            return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE";
        case 3317 :             return "GL_UNPACK_ALIGNMENT";
        case 36052 :            return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET";
        case 36053 :            return "GL_FRAMEBUFFER_COMPLETE";
        case 1280 :             return "GL_INVALID_ENUM";
        case 1281 :             return "GL_INVALID_VALUE";
        case 1282 :             return "GL_INVALID_OPERATION";
        case 1283 :             return "GL_STACK_OVERFLOW";
        case 1284 :             return "GL_STACK_UNDERFLOW";
        case 1285 :             return "GL_OUT_OF_MEMORY";
        case 1286 :             return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case 34055 :            return "GL_INCR_WRAP";
        case 34056 :            return "GL_DECR_WRAP";
        case 5386 :             return "GL_INVERT";
        case 36055 :            return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        case 36056 :            return "GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT";
        case 34067 :            return "GL_TEXTURE_CUBE_MAP";
        case 34068 :            return "GL_TEXTURE_BINDING_CUBE_MAP";
        case 34069 :            return "GL_TEXTURE_CUBE_MAP_POSITIVE_X";
        case 34070 :            return "GL_TEXTURE_CUBE_MAP_NEGATIVE_X";
        case 34071 :            return "GL_TEXTURE_CUBE_MAP_POSITIVE_Y";
        case 34072 :            return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";
        case 34073 :            return "GL_TEXTURE_CUBE_MAP_POSITIVE_Z";
        case 34074 :            return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";
        case 34076 :            return "GL_MAX_CUBE_MAP_TEXTURE_SIZE";
        case 36058 :            return "GL_FRAMEBUFFER_INCOMPLETE_FORMATS";
        case 36128 :            return "GL_STENCIL_ATTACHMENT";
        case 34013 :            return "GL_TEXTURE29";
        case 3379 :             return "GL_MAX_TEXTURE_SIZE";
        case 36062 :            return "GL_FRAMEBUFFER_STATUS_ERROR";
        case 3386 :             return "GL_MAX_VIEWPORT_DIMS";
        case 34015 :            return "GL_TEXTURE31";
        case 36160 :            return "GL_FRAMEBUFFER";
        case 36161 :            return "GL_RENDERBUFFER";
        case 36162 :            return "GL_RENDERBUFFER_WIDTH";
        case 36163 :            return "GL_RENDERBUFFER_HEIGHT";
        case 36164 :            return "GL_RENDERBUFFER_INTERNAL_FORMAT";
        case 36165 :            return "GL_STENCIL_INDEX";
        case 36166 :            return "GL_STENCIL_INDEX1_OES";
        case 36167 :            return "GL_STENCIL_INDEX4_OES";
        case 36168 :            return "GL_STENCIL_INDEX8_OES";
        case 36066 :            return "GL_COLOR_ATTACHMENT2";
        case 3408 :             return "GL_SUBPIXEL_BITS";
        case 36177 :            return "GL_RENDERBUFFER_GREEN_SIZE";
        case 3410 :             return "GL_RED_BITS";
        case 3411 :             return "GL_GREEN_BITS";
        case 3412 :             return "GL_BLUE_BITS";
        case 3413 :             return "GL_ALPHA_BITS";
        case 3414 :             return "GL_DEPTH_BITS";
        case 3415 :             return "GL_STENCIL_BITS";
        case 36193 :            return "GL_HALF_FLOAT_OES";
        case 36194 :            return "GL_RGB565";
        case 36195 :            return "GL_PLATFORM_BINARY_OES";
        case 36196 :            return "GL_ETC1_RGB8_OES";
        case 36197 :            return "GL_RGB_S3TC_OES";
        case 36198 :            return "GL_RGBA_S3TC_OES";
        case 2961 :             return "GL_STENCIL_CLEAR_VALUE";
        case 36071 :            return "GL_COLOR_ATTACHMENT7";
        case 2962 :             return "GL_STENCIL_FUNC";
        case 33000 :            return "GL_MAX_ELEMENTS_VERTICES";
        case 35731 :            return "GL_PALETTE4_RGBA4_OES";
        case 2964 :             return "GL_STENCIL_FAIL";
        case 2965 :             return "GL_STENCIL_PASS_DEPTH_FAIL";
        case 36054 :            return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
        case 35734 :            return "GL_PALETTE8_RGBA8_OES";
        case 2967 :             return "GL_STENCIL_REF";
        case 36077 :            return "GL_COLOR_ATTACHMENT13";
        case 2968 :             return "GL_STENCIL_WRITEMASK";
        case 36057 :            return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
        case 3553 :             return "GL_TEXTURE_2D";
        case 7680 :             return "GL_KEEP";
        case 36096 :            return "GL_DEPTH_ATTACHMENT";
        case 7682 :             return "GL_INCR";
        case 7683 :             return "GL_DECR";
        case 4353 :             return "GL_FASTEST";
        case 256 :              return "GL_DEPTH_BUFFER_BIT";
        case 3333 :             return "GL_PACK_ALIGNMENT";
        case 34338 :            return "GL_VERTEX_ATTRIB_ARRAY_ENABLED";
        case 34339 :            return "GL_VERTEX_ATTRIB_ARRAY_SIZE";
        case 34340 :            return "GL_VERTEX_ATTRIB_ARRAY_STRIDE";
        case 34341 :            return "GL_VERTEX_ATTRIB_ARRAY_TYPE";
        case 34342 :            return "GL_CURRENT_VERTEX_ATTRIB";
        case 34370 :            return "GL_VERTEX_PROGRAM_POINT_SIZE";
        case 34373 :            return "GL_VERTEX_ATTRIB_ARRAY_POINTER";
        case 34016 :            return "GL_ACTIVE_TEXTURE";
        case 4352 :             return "GL_DONT_CARE";
        case 34466 :            return "GL_NUM_COMPRESSED_TEXTURE_FORMATS";
        case 34467 :            return "GL_COMPRESSED_TEXTURE_FORMATS";
        case 7936 :             return "GL_VENDOR";
        case 9985 :             return "GL_LINEAR_MIPMAP_NEAREST";
        case 7938 :             return "GL_VERSION";
        case 7939 :             return "GL_EXTENSIONS";
        case 34660 :            return "GL_BUFFER_SIZE";
        case 34661 :            return "GL_BUFFER_USAGE";
        case 10497 :            return "GL_REPEAT";
        case 36072 :            return "GL_COLOR_ATTACHMENT8";
        case 36176 :            return "GL_RENDERBUFFER_RED_SIZE";
        case 36178 :            return "GL_RENDERBUFFER_BLUE_SIZE";
        case 36179 :            return "GL_RENDERBUFFER_ALPHA_SIZE";
        case 36180 :            return "GL_RENDERBUFFER_DEPTH_SIZE";
        case 36181 :            return "GL_RENDERBUFFER_STENCIL_SIZE";

        }
        
return "UNKNOWN";

}
