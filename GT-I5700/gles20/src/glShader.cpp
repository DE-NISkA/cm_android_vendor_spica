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
* \file         glShader.cpp
* \author       Sandeep Kakarlapudi (s.kakarla@samsung.com)
* \brief        Implementation of all shader and program related functionality
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*       07.08.2006      Sandeep Kakarlapudi             Initial version from ogl2_fimg.c
*
*******************************************************************************
*/

#include "glState.h"
#include "platform.h"


#include "ustl.h"

using namespace ustl;

static const char VSMagicNum[] = {'V', 'S', ' ', ' '};
static const char FSMagicNum[] = {'P', 'S', ' ', ' '};

int  InitShaderState(OGLState* ctx)
{
        //GET_GL_STATE(ctx);
        
        ctx->current_program = 0;
        ctx->current_program_ptr = NULL;
        
        ctx->cur_exe_own_memory = GL_FALSE;
        ctx->current_executable = NULL;
        

//      ctx->current_executable.clear();
//      ctx->shaderProgNames.cleanup();

        return 0;
}

int  DeInitShaderState(OGLState* ctx)
{

        ctx->current_program = 0;
        ctx->current_program_ptr = NULL;
        
        if(ctx->cur_exe_own_memory == GL_TRUE){
                ctx->current_executable->clear();
                delete ctx->current_executable;
        //Plat::safe_free(ctx->current_executable);
                ctx->cur_exe_own_memory = GL_FALSE;
        }
        ctx->current_executable = NULL; // whether true or false for both cases
        ctx->shaderProgNames.cleanup();

        return 0;
}


GLenum translateDclType(DclDataType dclType)
{
    const int NUM_TYPES = 19;
    static const GLenum glTypes[NUM_TYPES] =
    {
          GL_FLOAT, //DCL_TYPE_FLOAT,
          GL_INT,   //DCL_TYPE_INT,
          GL_BOOL,  //DCL_TYPE_BOOL,

          GL_FLOAT_VEC2,    //DCL_TYPE_VEC2_FLOAT,
          GL_INT_VEC2,      //DCL_TYPE_VEC2_INT,
          GL_BOOL_VEC2,     //DCL_TYPE_VEC2_BOOL,

          GL_FLOAT_VEC3,    //DCL_TYPE_VEC3_FLOAT,
          GL_INT_VEC3,      //DCL_TYPE_VEC3_INT,
          GL_BOOL_VEC3,     //DCL_TYPE_VEC3_BOOL,

          GL_FLOAT_VEC4,    //DCL_TYPE_VEC4_FLOAT,
          GL_INT_VEC4,      //DCL_TYPE_VEC4_INT,
          GL_BOOL_VEC4,     //DCL_TYPE_VEC4_BOOL,

          GL_FLOAT_MAT2,    //DCL_TYPE_MAT2_FLOAT,
          GL_FLOAT_MAT3,    //DCL_TYPE_MAT3_FLOAT,

          GL_FLOAT_MAT4,    //DCL_TYPE_MAT4_FLOAT,

          GL_SAMPLER_2D,    //DCL_TYPE_SAMPLER_2D,
          GL_SAMPLER_3D,    //DCL_TYPE_SAMPLER_3D,
          GL_SAMPLER_CUBE,  //DCL_TYPE_SAMPLER_CUBE,

          GL_INVALID_ENUM,  //DCL_TYPE_NONE
    };

    if(int(dclType) < NUM_TYPES)
    {
        return glTypes[dclType];
    }

    return GL_INVALID_ENUM;
}

const char* getGLTypeString(GLenum type)
{
    switch(type)
    {
          case GL_FLOAT:        return "GL_FLOAT";
          case GL_INT:          return "GL_INT";
          case GL_BOOL:         return "GL_BOOL";

          case GL_FLOAT_VEC2:   return "GL_FLOAT_VEC2";
          case GL_INT_VEC2:     return "GL_INT_VEC2";
          case GL_BOOL_VEC2:    return "GL_BOOL_VEC2";

          case GL_FLOAT_VEC3:   return "GL_FLOAT_VEC3";
          case GL_INT_VEC3:     return "GL_INT_VEC3";
          case GL_BOOL_VEC3:    return "GL_BOOL_VEC3";

          case GL_FLOAT_VEC4:   return "GL_FLOAT_VEC4";
          case GL_INT_VEC4:     return "GL_INT_VEC4";
          case GL_BOOL_VEC4:    return "GL_BOOL_VEC4";

          case GL_FLOAT_MAT2:   return "GL_FLOAT_MAT2";
          case GL_FLOAT_MAT3:   return "GL_FLOAT_MAT3";
          case GL_FLOAT_MAT4:   return "GL_FLOAT_MAT4";

          case GL_SAMPLER_2D:   return "GL_SAMPLER_2D";
          case GL_SAMPLER_3D:   return "GL_SAMPLER_3D";
          case GL_SAMPLER_CUBE: return "GL_SAMPLER_CUBE";
        
          default: return "";
    }
}
int getNumComponents(GLenum type)
{
    switch(type)
    {
    case GL_FLOAT:
    case GL_INT:
    case GL_BOOL:
        return 1;

        break;

    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
    case GL_BOOL_VEC2:
        return 2;

        break;

    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
    case GL_BOOL_VEC3:
        return 3;

        break;

    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
    case GL_BOOL_VEC4:
        return 4;

    case GL_FLOAT_MAT2:
        return 4;

    case GL_FLOAT_MAT3:
        return 9;

    case GL_FLOAT_MAT4:
        return 16;

        break;
    default:
        return 0;
    }
}

unsigned int getNumSlots(GLenum type)
{
    switch(type)
    {
    case GL_FLOAT:
    case GL_INT:
    case GL_BOOL:
        return 1;

        break;

    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
    case GL_BOOL_VEC2:
        return 1;

        break;

    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
    case GL_BOOL_VEC3:
        return 1;

        break;

    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
    case GL_BOOL_VEC4:
        return 1;

    case GL_FLOAT_MAT2:
        return 1;

    case GL_FLOAT_MAT3:
        return 3;

    case GL_FLOAT_MAT4:
        return 4;

        break;
    default:
        return 0;
    }
}

bool isVector(GLenum type)
{
    return (type == GL_FLOAT) || (type == GL_FLOAT_VEC2) || (type == GL_FLOAT_VEC3) || (type == GL_FLOAT_VEC4)
            || (type == GL_INT) || (type == GL_INT_VEC2) || (type == GL_INT_VEC3) || (type == GL_INT_VEC4)
            || (type == GL_BOOL) || (type == GL_BOOL_VEC2) || (type == GL_BOOL_VEC3) || (type == GL_BOOL_VEC4);
}

bool isMatrix(GLenum type)
{
    return (type == GL_FLOAT_MAT2) || (type == GL_FLOAT_MAT3) || (type == GL_FLOAT_MAT4);
}

bool isSampler(GLenum type)
{
    return (type == GL_SAMPLER_2D) || (type == GL_SAMPLER_3D) || (type == GL_SAMPLER_CUBE);
}

GLenum getBaseType(GLenum type)
{
    switch(type)
    {
    case GL_FLOAT:
    case GL_FLOAT_VEC2:
    case GL_FLOAT_VEC3:
    case GL_FLOAT_VEC4:
    case GL_FLOAT_MAT2:
    case GL_FLOAT_MAT3:
    case GL_FLOAT_MAT4:
        return GL_FLOAT;

    case GL_INT:
    case GL_INT_VEC2:
    case GL_INT_VEC3:
    case GL_INT_VEC4:

        return GL_INT;

    case GL_BOOL:
    case GL_BOOL_VEC2:
    case GL_BOOL_VEC3:
    case GL_BOOL_VEC4:

        return GL_BOOL;

    default:
        return GL_INVALID_ENUM; //TODO: (ab)using GL_INVALID_ENUM!
    }
}

ShaderType shaderHeaderType(const void* bin)
{
    if(Plat::memcmp(bin, VSMagicNum, 4)==0)
    {
        return VERTEX_SHADER;
    }
    else if(Plat::memcmp(bin, FSMagicNum, 4)==0)
    {
        return FRAGMENT_SHADER;
    }

    return INVALID_SHADER;
}




//---------------  UniformVar ---------------

UniformVar::UniformVar()
{
    setDefaults();
}

void UniformVar::setDefaults() 
{
    name.clear();
    size = 0;
    type = (GLenum)0;

    vsLocation = -1;
    fsLocation = -1;

    vsRegSet = DCL_REGSET_CONST_FLOAT;
    fsRegSet = DCL_REGSET_CONST_FLOAT;

    nextElement = 0;

    isArray = false;
    isInitialized = false;
}

//---------------  UniformTable ---------------


//If name is of the form <name[123]> baseName is <name> else its just name
//If the name is invalid in anyway, then it returns name itself.
static const char* extractBaseNameAndOffset(char* baseName, int maxSize, int* offset, int* isArrayType, const char* name)
{

        *offset = 0;
        *isArrayType = 0;

        int length = strlen(name);
        if( length > (maxSize-1))
        {
                //do nothing, just return the original name
                return name;
        }

        int i=0;

        if( name[length-1] != ']' )
        {
                //no ']' at the end, so return the source string
                return name;
        }

        int endPos = length-2; //endPos is one position before ]

        for( i=endPos; i>=0; i--)
        {
                if( name[i] == '[')
                        break;
        }

        if( (i == -1) || (i == (endPos))) // '[' doesnt exist in string or if we
                                                                                // have '[]' return name
        {
                return name;
        }

        int startPos = i+1;
        
        int lengthToCopy = i;
        strncpy(baseName, name, lengthToCopy);
        baseName[lengthToCopy] = '\0';

        int val =0;

        for(i= startPos; i<=endPos; i++)
        {
                char c = name[i];

                if( (c >= '0')&&(c <= '9') ) //digit
                {
                        val = c - '0' + 10*val;
                }
                else
                {
                        //not a number
                        return name;
                }
        }

        *offset = val;
        *isArrayType = 1;

        return baseName;
}

UniformTable::UniformTable()
:uniformNameMap()
{
    //Empty
}

void UniformTable::clear()
{
    uniformNameMap.clear();
    uniforms.clear();
}

int UniformTable::getLocation(const char* uniformName)
{
    GET_GL_STATE(ctx);
        //static char buff[MAX_UNIFORM_NAME_LENGTH];
    char* buff = ctx->tempStrBuff;

        int offset = 0;
        int isArrayType = 0;

    if(!uniformName)
        return -1;

        const char* baseName = extractBaseNameAndOffset(buff, GLF_TEMP_STRING_BUFFER_LENGTH, &offset, &isArrayType, uniformName);   

    NameMap::const_iterator it = uniformNameMap.find(NameString(baseName));

    if(it == uniformNameMap.end())
    {
        return -1;
    }
    //Need to check if this is really an array type, should not allow <nonArrayName>[123]!
        if(isArrayType)
        {
        if(!uniforms[it->second.index].isArray)
                {
                        //Using array operator on non array uniform
                        return -1;
                }
                

        if(uniforms[it->second.index].name != NameString(baseName))
        {
            return -1;
        }

        if( offset >= it->second.size) //out of range
        {
            return -1;
        }

        /*
        int i=0;
        for(i=0; i<=offset; i++)
        {
            if( uniforms[it->second.index+i].name != NameString(baseName))
                return -1;
            if( uniforms[it->second.index+i].nextElement == 0)
                break;
        }

        if(i<offset)
        {
            //out of range! i.e. actual array length is L and requested is element r where r>=L
            return -1;
        }
        */

    }

        // Note that array types' first element location can also be got from just arrayName

    return it->second.index + offset;
}

//TODO: This function assumes that the vertex shader's uniforms are inserted first.
bool UniformTable::insertOrUpdateBaseEntry(const DclBaseType* bt, ShaderType sType, const char* name, ustl::string& errString , const uint32 constFloatSize)
{
    GLenum varType = translateDclType(DclDataType(bt->type));

    if(GL_INVALID_ENUM == varType)
    {
        errString.append("INTERNAL ERROR: unknown variable type!");
        return false;
    }

    static const char * regClassStr[]=
    {
        "INPUT", "OUTPUT", "UNIFORM", "SAMPLER"
    };

    gAssert( (bt->regclass == DCL_REGSET_CONST_FLOAT) || (bt->regclass == DCL_REGSET_CONST_INT) || (bt->regclass == DCL_REGSET_CONST_BOOL));

    int startRidx = bt->start_reg_index;

        unsigned int val3 = startRidx>>2;
        int val4 = startRidx&0x3;

        //TODO: mask out and extract the constregset index
        int offset = startRidx;

        if(isSampler(varType)) 
        {
                //Sampler registers are virtual registers used to identify the texture unit
                // so the components are unused.
                offset >>=2;
        }

        //LOGMSG("Var: %s, regClass: %s, regIndex: %d, comp: %d,Type: %s\n",name, regClassStr[bt->regclass], val3, val4, getGLTypeString(varType));

        //to check that the const_float_size present in the shader is correct.Sampler are not part of constant float register.
    if(isSampler(varType) == false){
        if(constFloatSize < val3 + getNumSlots(varType)){
            //LOGMSG("\n const float size = %u, location =%d", constFloatSize , val3 + getNumSlots(varType) );
            errString.append("LINK FAILED: Internal Error. The register value is greater than the size of const float register ");
            return false;
        }
    }
        
    ustl::string nameStr(name);
    NameMap::iterator it = this->uniformNameMap.find(nameStr);

    int index = -1;

    if( it != uniformNameMap.end())//Entry Already exits
    {
        if(sType == VERTEX_SHADER)
        {
            gAssert(false && "Duplicate var name!");
            errString.append("LINK FAILED: Duplicate uniform name in VS");
            return false;
        }

        index = it->second.index;
        if(it->second.size != 1)
        {
            errString.append("LINK FAILED: Uniform name for base entry exists but its size != 1");
            return false;
        }
    }
    else
    {
        index = uniforms.size();
        uniformNameMap[nameStr] = NameInfo(index,1);
        uniforms.push_back(UniformVar());
    }

    if(!(uniforms[index].isInitialized))
    {
        uniforms[index].name = nameStr;
        uniforms[index].size = 1;
        uniforms[index].type = varType;

        if(sType == VERTEX_SHADER)
        {
            uniforms[index].vsLocation = offset;
            uniforms[index].vsRegSet = (unsigned char)(bt->regset);
        }
        else
        {
            uniforms[index].fsLocation = offset;
            uniforms[index].fsRegSet = (unsigned char)(bt->regset);
        }

        uniforms[index].isArray = false;
        uniforms[index].isInitialized = true;
        
        uniforms[index].nextElement = 0;
    }
    else
    {
        //Check if the data matches
        if(uniforms[index].name != nameStr)
        {
            gAssert(false && "Internal error: var string not as expected!");
            return false;
        }

        if( (uniforms[index].size != 1) || (uniforms[index].type != varType)
            || (uniforms[index].isArray != false))
        {
            errString.append("LINK FAILED: Mismatched uniform data");
            return false;
        }

        //It matches, so update the vs location or fs location
        if(sType == VERTEX_SHADER)
        {
            uniforms[index].vsLocation = offset;
            uniforms[index].vsRegSet = (unsigned char)(bt->regset);
        }
        else
        {
            uniforms[index].fsLocation = offset;
            uniforms[index].fsRegSet = (unsigned char)(bt->regset);
        }
    }

    return true;
}

bool UniformTable::insertOrUpdateArrayEntry(const DclArrayType* at, ShaderType sType, const char* name, ustl::string& errString, const uint32 constFloatSize)
{
    GLenum varType = translateDclType(DclDataType(at->type));

    if(GL_INVALID_ENUM == varType)
    {
        errString.append("INTERNAL ERROR: unknown variable type!");
        return false;
    }

    static const char * regClassStr[]=
    {
        "INPUT", "OUTPUT", "UNIFORM", "SAMPLER"
    };

    gAssert( (at->regclass == DCL_REGSET_CONST_FLOAT) || (at->regclass == DCL_REGSET_CONST_INT) || (at->regclass == DCL_REGSET_CONST_BOOL));


    int startRidx = at->start_reg_index;

        int val3 = startRidx>>2;
        int val4 = startRidx&0x3;

        //TODO: mask out and extract the constregset index
        int offset = startRidx;

    int stride = at->stride;
        if(isSampler(varType)) 
        {
                //Sampler registers are virtual registers used to identify the texture unit
                // so the components are unused.
                offset >>=2;
        gAssert( ((stride & 0x3)==0) && "Sampler var has non multiple of 4 stride!");
        stride >>=2;
        }

        //LOGMSG("ArrayVar: %s[%d], regClass: %s, regIndex: %d, comp: %d, Type: %s\n",name,at->num_elements, regClassStr[at->regclass], val3, val4, getGLTypeString(varType));

        //to check that the const_float_size present in the shader is correct.Sampler are not part of constant float register.
    if(isSampler(varType) == false){
        if(constFloatSize < val3 + (at->num_elements - 1) * (stride/4)  + getNumSlots(varType)){
            //LOGMSG("\n const float size 1 = %u, location  1=%d", constFloatSize ,val3 + (at->num_elements - 1) * stride  + getNumSlots(varType) );
            errString.append("LINK FAILED: Internal Error. The register value is greater than the size of const float register ");
            return false;
        }
    }
        
  
    ustl::string nameStr(name);
    NameMap::iterator it = this->uniformNameMap.find(nameStr);

    int index = -1;

    if( it != uniformNameMap.end()) //Entry exists
    {
        if(sType == VERTEX_SHADER)
        {
            gAssert(false && "Duplicate var name!");
            errString.append("LINK FAILED: Duplicate uniform name in VS");
            return false;
        }

        index = it->second.index;
        gAssert(it->second.size >= 0);
        if(static_cast<unsigned int>(it->second.size) != at->num_elements)
        {
            errString.append("LINK FAILED: Uniform name for array entry exists but its size != array size");
            return false;
        }
    }
    else
    {
        //Create entry
        index = uniforms.size();
        uniformNameMap[nameStr] = NameInfo(index,at->num_elements);
        
        for(unsigned int i=0; i< at->num_elements; i++)
        {
            uniforms.push_back(UniformVar());
        }
        
    }

    for(unsigned int i=0; i<at->num_elements; i++)
    {
        if(!(uniforms[index+i].isInitialized))
        {
            uniforms[index+i].name = nameStr;
            uniforms[index+i].size = 1;
            uniforms[index+i].type = varType;

            if(sType == VERTEX_SHADER)
            {
                uniforms[index+i].vsLocation = offset+i*stride;
                uniforms[index+i].vsRegSet = (unsigned char)(at->regset);
            }
            else
            {
                uniforms[index+i].fsLocation = offset+i*stride;
                uniforms[index+i].fsRegSet = (unsigned char)(at->regset);
            }

            uniforms[index+i].isArray = true;
            uniforms[index+i].isInitialized = true;
            
            uniforms[index+i].nextElement = 0;
        }
        else
        {
            //Check if the data matches
            if(uniforms[index+i].name != nameStr)
            {
                gAssert(false && "Internal error: var string not as expected!");
                return false;
            }

            if( (uniforms[index+i].size != 1) || (uniforms[index+i].type != varType)
                || (uniforms[index+i].isArray != true))
            {
                errString.append("LINK FAILED: Mismatched uniform data");
                return false;
            }

            //It matches, so update the vs location or fs location
            if(sType == VERTEX_SHADER)
            {
                uniforms[index+i].vsLocation = offset+i*stride;
                uniforms[index+i].vsRegSet = (unsigned char)(at->regset);
            }
            else
            {
                uniforms[index+i].fsLocation = offset+i*stride;
                uniforms[index+i].fsRegSet = (unsigned char)(at->regset);
            }
        }
    }

    
    if(uniforms[index].nextElement == 0)
    {
        //The pointers to the next element in the array is not initialized
        
        for(unsigned int i=0; i<at->num_elements; i++)
        {
            if(i==(at->num_elements-1))//last element of array
            {
                uniforms[index+i].nextElement=0;
            }
            else
            {
                uniforms[index+i].nextElement = &(uniforms[index+i+1]);
            }
        }
    }

    return true;

}

//--------------- VaryingVar ---------------
VaryingVar::VaryingVar()
{
    setDefaults();
}

void VaryingVar::setDefaults()
{
    name.clear();
    type = (GLenum)0;

    vsRegIndex = -1;
    fsRegIndex = -1;
}

//--------------- Sampler map Entry ---------------

SamplerMapEntry::SamplerMapEntry()
{
        setDefaults();
}

void SamplerMapEntry::setDefaults()
{
       name.clear();
        type = (GLenum)(0);

        glTexUnit = 0;  // glTexUnit initialized to 0 (since all uniforms are
                                                                                //!< initialized to 0
        isUsed = false;
}

//--------------- AttributeVar ---------------
AttributeVar::AttributeVar()
{
    setDefaults();
}

void AttributeVar::setDefaults()
{
    name.clear();
    type = (GLenum)(0);

    vsRegIndex = -1;
    attributeIndex = -1;
}

//---------------  ProgramVars ---------------

ProgramVars::ProgramVars()
{
    //TODO: not really required here now since ProgramVars does
    //      own the mem blocks
    vsFloatMemBlock = 0;
    fsFloatMemBlock = 0;

    vsIntMemBlock = 0;
    fsIntMemBlock = 0;

    vsBoolMemBlock = 0;
    fsBoolMemBlock = 0;

    clear();
}

void ProgramVars::clear()
{
    uniformTbl.clear();

    //TODO: Clearing of the these arrays can be removed in final code

    for(int i=0; i<MAX_VARYING_VARS; i++)
    {
        varyingArray[i].setDefaults();
    }
    numVaryingVars = 0;

    for(int i=0; i<MAX_VERTEX_ATTRIB_VARS; i++)
    {
        attributeArray[i].setDefaults();
    }
    numAttributeVars = 0;
    
    vsFloatMemBlock = 0;
    fsFloatMemBlock = 0;

    vsIntMemBlock = 0;
    fsIntMemBlock = 0;

    vsBoolMemBlock = 0;
    fsBoolMemBlock = 0;
}

bool ProgramVars::insertVars(Executable& exe, const unsigned char* bin, int size, ShaderType sType, ustl::string& errString)
{
    GET_GL_STATE(ctx);

    char* tmpNameBuff = ctx->tempStrBuff;

    const char* texDimVarPrefix = "TexDim";
    //const char* pointCoordStr = "gl_PointCoord";


    const ShaderHeader* sh = (ShaderHeader*)(bin);
    
    gAssert( bin && "Bin shaderPtr is 0!");
    gAssert( (sType == shaderHeaderType(bin)) && "shader type and shader binary type are differing!");

    //TODO: check version number (though it will already be checked while loading the binary)

    //WORKAROUND: binary shader offsets info, size in types units rather than bytes.
    int curOffset = sh->header_sz + (sh->instruction_sz)*16 + (sh->const_float_sz)*16 + sh->const_int_sz*4 + (sh->const_bool_sz ? 4 : 0);
    
    int offsetsBaseTypes[NR_DCL_REG_CLASSES];

    for(int i=0; i<NR_DCL_REG_CLASSES; i++)
    {
        offsetsBaseTypes[i] = curOffset;
        curOffset += (sh->dcl_base_type_dcls_table_sz[i])*sizeof(DclBaseType);
    }

    int offsetsArrayTypes[NR_DCL_REG_CLASSES];

    for(int i=0; i<NR_DCL_REG_CLASSES; i++)
    {
        offsetsArrayTypes[i] = curOffset;
        curOffset += (sh->dcl_array_type_dcls_table_sz[i])*sizeof(DclArrayType);
    }

    int offsetStringTable = curOffset;

    ShaderExecutable* se = 0;
    if(sType == VERTEX_SHADER)
    {
        se = &(exe.vs);
    }
    else if(sType == FRAGMENT_SHADER)
    {
        se = &(exe.fs);
    }
    else 
    {
        gAssert(false && "unexpected sType");
    }

    //TODO: Following clear is Not really required
    se->clear();

    se->fmbSize = sh->const_float_sz * 4*4;
    se->imbSize = sh->const_int_sz * 4*1;
        se->bmbSize = sh->const_bool_sz ? 4 : 0; //Max of 32 bools, the full storage is used 
                                                                                         // even if a single bool is used.
        
        se->constFloatMemBlock = (float*) Plat::malloc(se->fmbSize);
    se->constIntMemBlock = (char*) Plat::malloc(se->imbSize);
    se->constBoolMemBlock = (char*) Plat::malloc(se->bmbSize);

        //Not required
    Plat::memset(se->constFloatMemBlock, 0, se->fmbSize);
    Plat::memset(se->constIntMemBlock, 0, se->imbSize);
    Plat::memset(se->constBoolMemBlock,0, se->bmbSize);

    const int constFloatOffset = sh->header_sz + (sh->instruction_sz)*16;
    const int constIntOffset = constFloatOffset + se->fmbSize;
    const int constBoolOffset = constIntOffset + se->imbSize;
    
    Plat::memcpy(se->constFloatMemBlock, bin + constFloatOffset,se->fmbSize);
    Plat::memcpy(se->constIntMemBlock, bin + constIntOffset, se->imbSize);
        Plat::memcpy(se->constBoolMemBlock, bin+ constBoolOffset, se->bmbSize);
        
    if(sType == VERTEX_SHADER)
    {
        this->vsFloatMemBlock = se->constFloatMemBlock;
        this->vsIntMemBlock = se->constIntMemBlock;
        this->vsBoolMemBlock = se->constBoolMemBlock;
    }
    else if(sType == FRAGMENT_SHADER)
    {
        this->fsFloatMemBlock = se->constFloatMemBlock;
        this->fsIntMemBlock = se->constIntMemBlock;
        this->fsBoolMemBlock = se->constBoolMemBlock;
    }

/*    
    //Verify our offsets by printing the string table!
    //LOGMSG("String table:  ######\n");
    for(int i=0; i < sh->dcl_string_table_sz; )
    {
        int len = strlen((const char*)bin + offsetStringTable + i);
        //LOGMSG("%s\n",bin + offsetStringTable + i);

        i += len+1;
    }
    //LOGMSG("     ######     \n");
*/
    ////LOGMSG(" sizeof DclBaseType = %d   \n",sizeof(DclBaseType));

    const DclBaseType* bt = (const DclBaseType*)(bin+offsetsBaseTypes[DCL_REGCLASS_UNIFORM]);
    
    //Insert base type uniforms
    for(unsigned int j=0; j< sh->dcl_base_type_dcls_table_sz[DCL_REGCLASS_UNIFORM]; j++,bt++)
    {

        //check if this is a special compiler generated variable.
        
        int len = strlen(texDimVarPrefix);
        const char* name = (const char*)(bin+offsetStringTable+bt->name_offset);
        if(strncmp(name,texDimVarPrefix,len)==0)
        {

            if(sType == VERTEX_SHADER)
            {
                errString.append("INTERNAL ERROR: TexDim var generated for vertex shader");
                return false;
            }
            int texIndex = name[len]-'0';
            gAssert( MAX_TEXTURE_UNITS < 10 ); //Else the above code name[len]-'' breaks;
            
            if(texIndex >= MAX_TEXTURE_UNITS)
            {
                errString.append("INTERNAL WARNING: TexDim used for a tex unit > MAX_TEXTURE_UNIT");
            }
            else // add it
            {
                exe.dimensions.tex[texIndex] = bt->start_reg_index; 
            }
        }
        else if(len >3 && name[0] == 'g' && name[1] == 'l' && name[2] == '_') {
            
            //          Built-in uniforms
            
            if( strcmp(name,"gl_DepthRange.near") == 0) {
                 se->builtinUniformIndices.depthRange_near = bt->start_reg_index;
             }
             else if( strcmp(name,"gl_DepthRange.far")  == 0) {
                 se->builtinUniformIndices.depthRange_far = bt->start_reg_index;
             }
             else if( strcmp(name,"gl_DepthRange.diff") == 0) {
                 se->builtinUniformIndices.depthRange_diff = bt->start_reg_index;
             }
             else {
                 gAssert("Unrecognized built in uniform\n"); 
             }
         }
         else if(!this->uniformTbl.insertOrUpdateBaseEntry(bt, sType, name, errString ,sh->const_float_sz))
         {
             return false;
         }
     }
 
     //Insert Array type uniforms //TODO: test!!!
     const DclArrayType* at = (const DclArrayType*)(bin+offsetsArrayTypes[DCL_REGCLASS_UNIFORM]);
 
     for(unsigned int j=0; j< sh->dcl_array_type_dcls_table_sz[DCL_REGCLASS_UNIFORM]; j++,at++)
     {
         if(!this->uniformTbl.insertOrUpdateArrayEntry(at, sType, (const char*)(bin+offsetStringTable+at->name_offset), errString, sh->const_float_sz))
         {
             return false;
         }
         //at++;
     }
 
     const int maxSamplers = ((sType == VERTEX_SHADER) ? MAX_VERTEX_TEXTURE_UNITS : MAX_TEXTURE_UNITS);
     const ustl::string errExeceededSamplers = ((sType == VERTEX_SHADER) ? ustl::string("Exceeded VS samplers!\n") : "Exceeded FS samplers!\n");
     
     //Insert base type Sampler vars
     bt = (const DclBaseType*)(bin+offsetsBaseTypes[DCL_REGCLASS_SAMPLER]);
 
     for(unsigned int j=0; j< sh->dcl_base_type_dcls_table_sz[DCL_REGCLASS_SAMPLER]; j++,bt++)
     {
         ustl::string varName((const char*)(bin+offsetStringTable+bt->name_offset));
 
         GLenum  varType = translateDclType(DclDataType(bt->type));
 
         if(!isSampler(varType))
         {
             errString.append("Unsupported sampler type\n");
             return false;
         }
 
         int sReg = (bt->start_reg_index)>>2;
 
         if(sReg >= maxSamplers)
         {
             errString.append(errExeceededSamplers);
             return false;
         }
 
         if(!this->uniformTbl.insertOrUpdateBaseEntry(bt, sType, (const char*)(bin+offsetStringTable+bt->name_offset), errString, sh->const_float_sz))
         {
             return false;
         }
 
         if(sType == VERTEX_SHADER)
         {
             //ALERT: unlike the rest of the vars the type info is stored in the exe!
             exe.vsSamplerMappings[sReg].isUsed = true;
             exe.vsSamplerMappings[sReg].type    = varType;
             exe.vsSamplerMappings[sReg].name = varName;
         }
         else if(sType == FRAGMENT_SHADER)
         {
             exe.fsSamplerMappings[sReg].isUsed = true;
             exe.fsSamplerMappings[sReg].type    = varType;
             exe.fsSamplerMappings[sReg].name = varName;
         }
 
         //bt++;
     }
 
     //Insert array type samplers
     at = (const DclArrayType*)(bin+offsetsArrayTypes[DCL_REGCLASS_SAMPLER]);
 
     for(unsigned int j=0; j< sh->dcl_array_type_dcls_table_sz[DCL_REGCLASS_SAMPLER]; j++,at++)
     {
         ustl::string varName((const char*)(bin+offsetStringTable+at->name_offset));
 
         GLenum  varType = translateDclType(DclDataType(at->type));
 
         if(!isSampler(varType))
         {
             errString.append("Unsupported sampler type\n");
             return false;
         }
 
         int sReg = (at->start_reg_index)>>2;
         int stride = at->stride;
         gAssert( ((stride & 0x3)==0) && "Sampler var has non multiple of 4 stride!");
         stride >>=2;
 
         int maxSReg = (at->start_reg_index + (at->num_elements)*(at->stride))>>2;
 
         if(maxSReg >= maxSamplers)
         {
             errString.append(errExeceededSamplers);
             return false;
         }
 
         if(!this->uniformTbl.insertOrUpdateArrayEntry(at, sType, (const char*)(bin+offsetStringTable+at->name_offset), errString, sh->const_float_sz))
         {
             return false;
         }
 
         for(unsigned int i=0; i<at->num_elements; i++)
         {
             int reg = (sReg + i*(stride));
 
             if(sType == VERTEX_SHADER)
             {
                 //ALERT: unlike the rest of the vars the type info is stored in the exe!
                 exe.vsSamplerMappings[reg].isUsed = true;
                 exe.vsSamplerMappings[reg].type = varType;
                 exe.vsSamplerMappings[reg].name = varName;
             }
             else if(sType == FRAGMENT_SHADER)
             {
                 exe.fsSamplerMappings[reg].isUsed = true;
                 exe.fsSamplerMappings[reg].type = varType;
                 exe.fsSamplerMappings[reg].name = varName;
             }
         }
 
         //at++;
     }
 
     //Insert attribs
     if(sType == VERTEX_SHADER)
     {
 
                 numAttributeVars = sh->dcl_base_type_dcls_table_sz[DCL_REGCLASS_INPUT];
                 bt = (const DclBaseType*)(bin+offsetsBaseTypes[DCL_REGCLASS_INPUT]);
                 for(unsigned int i=0; i< numAttributeVars; i++,bt++)
                 {
                         attributeArray[i].name = ustl::string((const char*)(bin+offsetStringTable+bt->name_offset));
                         attributeArray[i].type = translateDclType((DclDataType)bt->type);
                         attributeArray[i].vsRegIndex = bt->start_reg_index>>2; //Discard component
                         //LOGMSG("ATTRIB: %s type: %s\n",attributeArray[i].name.c_str(), getGLTypeString(attributeArray[i].type));
                         
                         //bt += 1;
                 }
     }
         
 //TODO: test, prove, remove older code
 
     DclRegClass varyingClass;
     if(sType == VERTEX_SHADER)
     {
         varyingClass = DCL_REGCLASS_OUTPUT;
     }
     else
     {
         varyingClass = DCL_REGCLASS_INPUT;
     }
 
     //Insert base type varying vars
     bt = (const DclBaseType*)(bin+offsetsBaseTypes[varyingClass]);
     int numBtVaryings = sh->dcl_base_type_dcls_table_sz[varyingClass];
     for(int i=0; i< numBtVaryings; i++,bt++)
     {
         if( (sType == FRAGMENT_SHADER) && 
             (strcmp((const char*)(bin+offsetStringTable+bt->name_offset),"gl_FragCoord")==0))
         {
             continue;
         }
         
 #if 1
                 if( (sType == VERTEX_SHADER) && 
             (strcmp((const char*)(bin+offsetStringTable+bt->name_offset),"gl_PointSize")==0))
         {
                         //to add the Reg value
                         //exe.pointSizeIndex = 9;
                         exe.pointSizeIndex = bt->start_reg_index>>2;
                         ////LOGMSG("\n point sizer =%d",exe.pointSizeIndex);
                         continue;
         }
 
 #endif
         if(!(insertVaryingVar(sType, ustl::string((const char*)(bin+offsetStringTable+bt->name_offset)), 
             translateDclType((DclDataType)bt->type), bt->start_reg_index, errString)))
         {
             return false;
         }
         //bt+=1;
     }
 
     //Insert array type varying vars by essentially cnoverting each array's slements into base types with names <array_name>[<i>]
     at = (const DclArrayType*)(bin+offsetsArrayTypes[varyingClass]);
     int numAtVaryings = sh->dcl_array_type_dcls_table_sz[varyingClass];
     
     for(int i=0; i < numAtVaryings; i++,at++)
     {
         for(unsigned int j=0; j< at->num_elements; j++)
         {
 
             sprintf(tmpNameBuff,"[%d]",j);
             
             if(!(insertVaryingVar(sType, ustl::string((const char*)(bin+offsetStringTable+at->name_offset))+ustl::string(tmpNameBuff), 
                 translateDclType((DclDataType)at->type), at->start_reg_index+j*(at->stride), errString)))
             {
                 return false;
             }
         }
         //at+=1;
     }
 
     return true;
 }
 
 bool ProgramVars::insertVaryingVar(ShaderType sType, ustl::string name, GLenum type, int regIndex, ustl::string& errString)
 {
     if(sType == VERTEX_SHADER)
     {
         int index = numVaryingVars;
         numVaryingVars++;
 
         varyingArray[index].name = name;
         varyingArray[index].type = type;
         varyingArray[index].vsRegIndex  = regIndex;
 
         if(varyingArray[index].type == GL_INVALID_ENUM)
         {
             errString.append("LINK FAILED: unknown varying type");
             return false;
         }
     }
     else
     {
         if(name == ustl::string("gl_PointCoord"))
         {
             int index = numVaryingVars;
             numVaryingVars++;
 
             varyingArray[index].name = name;
             varyingArray[index].type = type;
             varyingArray[index].fsRegIndex = regIndex;
         }
         else
         {
             int j=0;
             for(j=0; j< numVaryingVars; j++)
             {
                 if(varyingArray[j].name == name)
                     break;
             }
 
             if(j == numVaryingVars)
             {
                 errString.append("LINK FAILED: no matching varying declaration found in VS for: ");
                 errString.append(name);
                 return false;
             }
 
             if(varyingArray[j].type != type)
             {
                 errString.append("LINK FAILED: type mismatch for varying : ");
                 errString.append(name + " VStype: "+getGLTypeString(varyingArray[j].type)+ " FStype: "
                     +getGLTypeString(type)+"\n");
                 return false;
             }
 
                 varyingArray[j].fsRegIndex = regIndex;
         }
     }
 
     return true;
 }
 
 bool ProgramVars::updateVaryingMap(Executable& exe, ustl::string& errString)
 {
 
     for(int i=0; i < MAX_VARYING_MAPPINGS; i++)
     {
         exe.varyingMap[i] = -1;
     }
 #if 1
     //TODO: Handle arrays, implemented needs testing
 
     for(int i=0; i < numVaryingVars; i++)
     {
         //discard component indices since hw only maps a full vs vec4 to a fs vec4
         const int& vsReg = (varyingArray[i].vsRegIndex)>>2; 
         const int& fsReg = (varyingArray[i].fsRegIndex)>>2; 
 
         const int numSlotsUsed = getNumSlots(varyingArray[i].type);
 
         gAssert( numSlotsUsed!= 0);
 
         if( varyingArray[i].name != ustl::string("gl_PointCoord"))
         {
             if( (0 > vsReg) || ((vsReg+numSlotsUsed-1) > (MAX_VARYING_MAPPINGS)) )
             {
                 errString.append(ustl::string("LINK ERROR: VS varying <")+varyingArray[i].name+ustl::string("> register out of range\n"));
                 return false;
             }
         }
         else
         {
             exe.pointCoordIndex = varyingArray[i].fsRegIndex>>2;
         }
                 //its okay to have a vs out which is not used by fs in so fsReg = -1 is allowed
         if( /*(0 > fsReg) || */((fsReg+numSlotsUsed-1)>= MAX_VARYING_MAPPINGS) )
         {
             errString.append(ustl::string("LINK ERROR: FS varying <")+varyingArray[i].name+ustl::string("> register out of range\n"));
             return false;
         }
         
         if( varyingArray[i].name != ustl::string("gl_PointCoord"))
         {
             for(int j = 0; j < numSlotsUsed; j++)
             {
                             int actFsReg = (vsReg == 0)? 0 : (fsReg+j+1);
                             if(fsReg == -1 && vsReg !=0 ) 
                             {
                                     actFsReg = -2;//TODO: remove this
                             }
                 gAssert((vsReg!=-1) &&"vsReg is -1");
 
                 if( exe.varyingMap[vsReg+j] == -1)
                 {
                     exe.varyingMap[vsReg+j] = actFsReg;
                 }
                 else if( exe.varyingMap[vsReg+j] != actFsReg )
                 {
                     //There is a bad mapping in the varyings. A variable in part of vsReg is mapped to exe.varyingMap[vsReg] but another
                     // variable needs it to be mapped to fsReg 
 
                     errString.append(ustl::string("LINK ERROR: two variables in a VS varying register are to be mapped to different FS varying registers"));
                     return false;
                 }
             }
         }
     }
 
     //find the number of used varyings regs (may not be equal to numVaryingVars
     // because of types like mat4 which requires 4 full vec4 registers or 
     // the number might be different due to packing.
 
     int lastActiveMapping = -1;
     int numActiveMappings = 0;
 
     int lastActiveFSReg = -1;
 
     for(int i=0; i< MAX_VARYING_MAPPINGS; i++)
     {
         if( exe.varyingMap[i] >=0)
         {
             lastActiveMapping = i;
             numActiveMappings++;
         }
     }
 
     for(int i=0; i< MAX_VARYING_MAPPINGS; i++)
     {
         if(exe.varyingMap[i]>=lastActiveFSReg)
         {
             lastActiveFSReg = exe.varyingMap[i];
         }
     }
         bool unusedVSVarying = false;
         // set all fsregs with -2 to some unused fs attrib reg. Like lastActiveMapping+1;
         for(int i=0; i<=lastActiveMapping; i++)
         {
                 if( exe.varyingMap[i] == -2)
                 {
                         //exe.varyingMap[i] = lastActiveFSReg+1;
                         exe.varyingMap[i] = ++lastActiveFSReg;
                         unusedVSVarying = true;
                 }
         }
 
         if(unusedVSVarying)
         {
                 lastActiveFSReg++;
         }
 
         //TODO: since fimg has no problem with "holes" in mapping, we disregard it
         /*
     if( numActiveMappings != (lastActiveMapping+1))
     {
         //Some intermediate regs have no mapping
         errString.append(ustl::string("LINK ERROR\n"));
         gAssert(false && "Some intermediate varying regs have no mapping!");
         return false;
     }
  */
 
         exe.numVSVarying = lastActiveMapping+1;
     exe.numFSVarying = lastActiveFSReg; //TODO check this
     if(exe.pointCoordIndex!=-1)
     {
         if((exe.pointCoordIndex+1) > lastActiveFSReg)
         {
             exe.numFSVarying = (exe.pointCoordIndex+1);
         }
 #if 0
         else
         {
         }
 
 #endif
 
     }
 
     //LOGMSG("\n\n_____VARYING MAP: [max used VS varyings = %d, FS varyings = %d]_____\ngl_PointCoord : %d\n", exe.numVSVarying, exe.numFSVarying, exe.pointCoordIndex);
         for(int i=0; i< exe.numVSVarying; i++)
         {
                 //LOGMSG(" %d => %d\n", i, exe.varyingMap[i]);
         }
         //LOGMSG("\n");
 
 
 #endif
     return true;
 }
 
 bool ProgramVars::updateAttribMap(Executable& exe, ustl::string& errString)
 {
         
         int numMappings = 0;
         int currentMap = 0;
         for(unsigned int i=0; i < numAttributeVars; i++)
         {
 
                 const int numSlotsUsed = getNumSlots(this->attributeArray[i].type);
         gAssert( numSlotsUsed!= 0);
 
                 //Check here user assigned bindings
 
                 for(int j=0; j<numSlotsUsed; j++)
                 {
                         exe.attribMap[numMappings++] = currentMap++;    
                 }
 
         }
 
         exe.numAttribs = numMappings;
 
         //LOGMSG("\n\n_____OLD ATTRIB MAPPING: [max used attribs = %d]_____\n", exe.numAttribs);
         //LOGMSG("VSReg => attrib index\n");
         for(int i=0; i< exe.numAttribs; i++)
         {
                 //LOGMSG(" %d => %d\n", i, exe.attribMap[i]);
         }
         //LOGMSG("\n");
 
         return true;
 }
 
 void ProgramVars::printVaryings() const
 {
     //LOGMSG("\nVARYINGS (actuals not mapping!):\n");
//     for(int i=0; i<numVaryingVars; i++)
//     {
         //LOGMSG("%s\t vs:%d\t fs:%d\t type:%s\n", varyingArray[i].name.c_str(),
         //    varyingArray[i].vsRegIndex, varyingArray[i].fsRegIndex, getGLTypeString(varyingArray[i].type));
//     }
     //LOGMSG("\n");
 }
 
 ProgramVars::~ProgramVars()
 {
     clear();
 }
 
 
 unsigned int ProgramVars::getNumActiveAttributes() const
 {
     return this->numAttributeVars;
 }
 
 int ProgramVars::getActiveAttributeMaxLength() const
 {
     const int numActiveAttributes = getNumActiveAttributes();
     
     int maxLength = 0;
     
     for(int i=0; i< numActiveAttributes; i++)
     {
         int curAttribLen = attributeArray[i].name.length()+1;
         if(maxLength < curAttribLen) 
             maxLength = curAttribLen;
     }
 
     return maxLength;
 }
 
 unsigned int ProgramVars::getNumActiveUniforms() const
 {
     return uniformTbl.uniformNameMap.size();
 }
 
 int ProgramVars::getActiveUniformMaxLength() const
 {
     //int numActiveUniforms = getNumActiveUniforms();
 
     int maxLength = 0;
 
     NameMap::const_iterator it = uniformTbl.uniformNameMap.begin();
     for(; it !=uniformTbl.uniformNameMap.end() ; ++it)
         {
         int curUniformLength = it->first.length()+1;
 
         if(maxLength < curUniformLength) 
             maxLength = curUniformLength;
         }
 
     return maxLength;
 }
 
 
 
 //---------------  AttribBindings ---------------
 
 AttribBindings::AttribBindings()
 {
 }
 
 AttribBindings::~AttribBindings()
 {
     clear();
 }
 
 void AttribBindings::clear()
 {
     attribMap.clear();
 }
 
 // getLocation : returns the location if an entry is made against name
 //               else returns -1;
 int AttribBindings::getLocation(const char *name)
 {
     ustl::string s(name);
     ustl::map<ustl::string, int>::const_iterator it = attribMap.find(s);
     
     if(it == attribMap.end())
     {
         return -1;
     }
 
     return it->second;
 }
 
 bool AttribBindings::setBinding(const char *name, int attribLoc)
 {
     ustl::map<ustl::string,int>::iterator it = attribMap.find(ustl::string(name));
     if( it == attribMap.end())  //New entry
     {
                 //Limit the maximum number of bindings!
         if(attribMap.size() < MAX_VERTEX_ATTRIB_BINDINGS)
         {
             attribMap[ustl::string(name)] = attribLoc;
             return true;
         }
 
         return false;
     }
 
     it->second = attribLoc;
     return true;
 }
 
 //---------------  ShaderExecutable ---------------
 ShaderExecutable::ShaderExecutable()
 {
     constFloatMemBlock = 0;
     constIntMemBlock  = 0;
     constBoolMemBlock = 0;
     
     binaryCode = 0;
 
 #ifdef FSO_JITO
 
         origBinaryCode = 0;
 #endif
 
     clear();
 }
 
 void ShaderExecutable::clear()
 {
     Plat::safe_free(constFloatMemBlock);
     Plat::safe_free(constIntMemBlock);
     Plat::safe_free(constBoolMemBlock);
 
     fmbSize = 0;
     imbSize = 0;
     bmbSize = 0;
 
     Plat::safe_free(binaryCode);
 
     binaryCodeSize  = 0;
     pcStart         = 0;
     
     builtinUniformIndices.depthRange_near = -1;
     builtinUniformIndices.depthRange_far = -1;
     builtinUniformIndices.depthRange_diff = -1;
     
 
 #ifdef FSO_JITO
 
         Plat::safe_free(origBinaryCode);
         origBinaryCodeSize      = 0;
         origPcStart                     = 0;
         jitoEnabled                     = false;
         
         jitoData.clear();
 
 #endif
 }
 
 void ShaderExecutable::copyFrom(const ShaderExecutable &se)
 {
     this->clear();
 
     void* dataPtr = 0;
 
     dataPtr = Plat::malloc(se.fmbSize);
     Plat::memcpy(dataPtr, se.constFloatMemBlock, se.fmbSize);
     constFloatMemBlock = static_cast<float*>(dataPtr);
     fmbSize = se.fmbSize;
 
     dataPtr = Plat::malloc(se.imbSize);
     Plat::memcpy(dataPtr, se.constIntMemBlock, se.imbSize);
     constIntMemBlock = static_cast<char*>(dataPtr);
     imbSize = se.imbSize;
 
     dataPtr = Plat::malloc(se.bmbSize);
     Plat::memcpy(dataPtr, se.constBoolMemBlock, se.bmbSize);
     constBoolMemBlock = static_cast<char*>(dataPtr);
     bmbSize = se.bmbSize;
 
     dataPtr = Plat::malloc(se.binaryCodeSize);
     Plat::memcpy(dataPtr, se.binaryCode, se.binaryCodeSize);
     binaryCode = static_cast<unsigned char*>(dataPtr);
     
     binaryCodeSize  = se.binaryCodeSize;
     pcStart         = se.pcStart;
     
     builtinUniformIndices = se.builtinUniformIndices;
 
 #ifdef FSO_JITO
 
         origBinaryCodeSize = se.origBinaryCodeSize;
         origPcStart             = se.origPcStart;
         origBinaryCode  = static_cast<unsigned char*>(Plat::malloc(origBinaryCodeSize));
         Plat::memcpy(origBinaryCode, se.origBinaryCode, origBinaryCodeSize);
 
         jitoEnabled             = se.jitoEnabled;
         jitoData                = se.jitoData;
 
 #endif
 
 }
 
 
 //---------------  Executable ---------------
 
 void Executable::clear()
 {
     vs.clear();
     fs.clear();
 
     numAttribs = -1;
     numVSVarying  = -1;
     numFSVarying  = -1;
 
     pointCoordIndex = -1;
         pointSizeIndex = -1;
         
         for(int i=0; i< MAX_VERTEX_TEXTURE_UNITS; i++)
         {
                 vsSamplerMappings[i].setDefaults();
         }
 
         for(int i=0; i< MAX_TEXTURE_UNITS; i++)
         {
                 fsSamplerMappings[i].setDefaults();
         }
 
         for(int i=0; i< MAX_VERTEX_ATTRIB_VARS; i++)
         {
                 
                 attribMap[i] = 0;
         }
         
         for(int i=0; i<MAX_VARYING_MAPPINGS; i++)
         {
                 varyingMap[i] = 0;
         }
 
 #ifdef EN_EXT_VARYING_INTERPOLATION_EXP
                 varyingInterpolation = 0;       //All smooth
 #endif
 
     //Careful! the indices in dimensions.tex[] are no longer valid! 
     //They should not be accessed
     for(int i=0; i<MAX_TEXTURE_UNITS; i++)
     {
         dimensions.tex[i] = -1;  
     }
 
 
 }
 
 void Executable::copyFrom(const Executable &e)
 {
     vs.copyFrom(e.vs);
     fs.copyFrom(e.fs);
 
     numAttribs = e.numAttribs;
     numVSVarying = e.numVSVarying;
     numFSVarying = e.numFSVarying;
 
     pointCoordIndex = e.pointCoordIndex;
         pointSizeIndex = e.pointSizeIndex;
                 
         for(int i=0; i< MAX_VERTEX_TEXTURE_UNITS; i++)
         {
                 vsSamplerMappings[i] = e.vsSamplerMappings[i];
         }
 
         for(int i=0; i< MAX_TEXTURE_UNITS; i++)
         {
                 fsSamplerMappings[i] = e.fsSamplerMappings[i];
         }
         
         for(int i=0; i< MAX_VERTEX_ATTRIB_VARS; i++)
         {
                 
                 attribMap[i] = e.attribMap[i];
         }
         
         for(int i=0; i<MAX_VARYING_MAPPINGS; i++)
         {
                 varyingMap[i] = e.varyingMap[i];
         }
 
 #ifdef EN_EXT_VARYING_INTERPOLATION_EXP
                 varyingInterpolation = e.varyingInterpolation;
 #endif
 
     for(int i=0; i<MAX_TEXTURE_UNITS; i++)
     {
         dimensions.tex[i] = e.dimensions.tex[i];
         }
 }
 
 void Executable::dumpConstants()
 {
     FILE* fp = fopen("VSConstants.txt","wt");
     fprintf(fp,"VS Float constants");
     
     for(int i=0; i< (vs.fmbSize/4) ; i++)
     {
         if(i%4 == 0)
             fprintf(fp,"\nC%d ",i/4);
 
         fprintf(fp," %f",vs.constFloatMemBlock[i]);
         
     }
 
     fclose(fp);
 
     fp = fopen("FSConstants.txt","wt");
     fprintf(fp,"FS Float constants");
     
     for(int i=0; i< (fs.fmbSize/4) ; i++)
     {
         if(i%4 == 0)
             fprintf(fp,"\nC%d ",i/4);
         fprintf(fp," %f",fs.constFloatMemBlock[i]);
    
     }
 
     fclose(fp);
 }
 //---------------  Shader ---------------
 
 Shader::Shader(ShaderType sType, GLuint id_)
 {
     infoLog = 0;
     shaderSourceString = 0;
     shaderBinary = 0;
 
     setDefaults();
 
     this->id    = id_;
     type        = sType;
 }
 
 void Shader::setDefaults()
 {
     id          = 0;
     type        = INVALID_SHADER;
 
     deleteStatus = false;
     compileStatus = false;
 
 
     Plat::safe_free(infoLog);
 
 
     lengthInfoLog = 0;
 
     Plat::safe_free(shaderSourceString);
 
     lengthShaderSourceString = 0;
 
     Plat::safe_free(shaderBinary);
 
     sizeShaderBinary    = 0;
     pcStart             = 0;
 
     refCount = 0;
 }
 
 void Shader::loadShaderBin(const void * bin, int binSize)
 {
     if((bin == 0) || ((unsigned int)binSize < sizeof(ShaderHeader)))
     {
         return;
     }
 
     gAssert((type != INVALID_SHADER)&&"Shader type is invalid!");
 
     if(type != shaderHeaderType(bin))
     {
         return;
     }
 
     Plat::safe_free(this->shaderBinary);
         
     this->sizeShaderBinary  = 0;
     this->pcStart           = 0;
 
     this->shaderBinary = static_cast<unsigned char*>(Plat::malloc(binSize));
         
     if(this->shaderBinary)
     {
         Plat::memcpy(this->shaderBinary, bin, binSize);
         this->sizeShaderBinary  = binSize;
 
         this->pcStart           = (static_cast<const ShaderHeader*>(bin))->start_address;
         this->compileStatus     = true;
     }
     else
     {
         set_err(GL_OUT_OF_MEMORY);
     }
     
 }
 int Shader::getInfoLogLength() const
 {
     int infoLogLength = this->lengthInfoLog;
 
     if(infoLogLength>0) infoLogLength++; //If info log is of non zero length we need to give the length + 1 for '\0'
     
     return infoLogLength;
 }
 
 Shader::~Shader()
 {
     setDefaults();  //... since this takes care of freeing too!
 }
 
 //---------------  Program ---------------
 Program::Program(GLuint id_)
 {
 
     infoLog.clear();
 
     vertexShader = 0;
     fragmentShader = 0;
     
     setDefaults();
 
     this->id = id_;
 }
 
 void Program::setDefaults()
 {
     id              = 0;
 
     deleteStatus    = false;
     linkStatus      = false;
     validationStatus    = false;
 
     detachAll();
 
     //Plat::safe_free(infoLog);
     infoLog.clear();
 
     attribBindings.clear();
 
     vars.clear();
 
     progExecutable.clear();
 
 }
 bool Program::updateAttribMap(ustl::string& errString)
 {
         //int numMappings = 0;
         //int currentMap = 0;
 
         bool locationUsed[MAX_VERTEX_ATTRIBS];
 
         for(unsigned int i=0; i < MAX_VERTEX_ATTRIBS; i++)
         {
                 locationUsed[i] = false;
         }
         for(unsigned int i=0; i < MAX_VERTEX_ATTRIB_VARS; i++)
         {
                 progExecutable.attribMap[i] = -1;
         }
 
         for(unsigned int i=0; i< vars.numAttributeVars; i++)
         {
 
                 int loc = attribBindings.getLocation(vars.attributeArray[i].name.c_str());
 
                 //If a user specified attrib mapping exists for this attrib
                 if( loc != -1)
                 {
                         int numSlots = getNumSlots(vars.attributeArray[i].type);
                         int vsRegIndex = vars.attributeArray[i].vsRegIndex;
 
                         for(int j=0; j<numSlots; j++)
                         {
 
                                 gAssert(((vsRegIndex+numSlots-1)<MAX_VERTEX_ATTRIB_VARS)&&"Attrib reg allocation is invalid");
 
                                 if((loc+j)>= MAX_VERTEX_ATTRIBS) //This test can be moved outside the loop
                                 {
                                         //LOGMSG("LINK FAILED: Overflowing attrib locations\n");
                     //We do not mention the reason the attributes are exceeded. Is it because of
                     //too many active atributes or becaue no contiguous slots could be found for matrices
                     //which otherwise would have fit in the number of attributes
                     errString.append("LINK FAILED: Number of attributes exceeded\n"); 
                                         return false; //linking fails
                                 }
 
                                 //If no mapping exists, establish one
                                 if( progExecutable.attribMap[vsRegIndex+j] == -1 )
                                 {
                                         progExecutable.attribMap[vsRegIndex+j] = loc+j;
                                         locationUsed[loc+j] = true;
                                 }
                                 //if it already exists and is what we want to set it to, do nothing
                                 else if( progExecutable.attribMap[vsRegIndex+j] == (loc+j))
                                 {
                                         //do nothing
                                 }
                                 //if it already exists and is NOT what we want to set it to, ERROR, link failure
                                 else
                                 {
                                         //collision! Two attrib regs have to be mapped to different locations
                                         //LOGMSG("FAILED: collision!\n");
                                         return false;
                                 }
                         }
                 }
         }
 
 
         for(unsigned int i=0; i< vars.numAttributeVars; i++)
         {
 
                 int loc = attribBindings.getLocation(vars.attributeArray[i].name.c_str());
 
                 //If a user specified attrib mapping doesn't exits for this attrib
                 if( loc == -1)
                 {
                         int numSlots = getNumSlots(vars.attributeArray[i].type);
                         int vsRegIndex = vars.attributeArray[i].vsRegIndex;
 
                         int validStartSlot = -1;
 
                         for(int k=0; k<(MAX_VERTEX_ATTRIBS-numSlots); k++)
                         {
                                 int j=0;
                                 for(; j< numSlots;j++)
                                 {
                                         if( locationUsed[k+j] )
                                         {
                                                 break;
                                         }
                                 }
 
                                 if(j == numSlots) //we have found contiguous unused locations!
                                 {
                                         for(j=0; j<numSlots; j++)
                                         {
 
                                                 progExecutable.attribMap[vsRegIndex+j] = k+j;
                                                 locationUsed[k+j] = true;
                                         }
                                         validStartSlot = k;
                                         break;
                                 }
 
                         }
 
                         if(validStartSlot == -1) //No mapping was possible
                         {
                                 //links fails
                                 //LOGMSG("FAILED: No free locations found of required size\n");
                                 return false;
                         }
                 }
         }
 #if 0
     progExecutable.numAttribs = vars.numAttributeVars;
 #else
     int lastActiveSlot = -1;
     for(int i=0; i<MAX_VERTEX_ATTRIB_VARS; i++)
     {
         if(progExecutable.attribMap[i]>=0) {
             lastActiveSlot = i;
         }
     }
     progExecutable.numAttribs = lastActiveSlot+1;
 #endif    
//     //LOGMSG("\n\n_____ATTRIB MAP: [max used attribs = %d]_____\n", progExecutable.numAttribs);
//         //LOGMSG("VSReg => attrib index\n");
     /*
         for(int i=0; i< progExecutable.numAttribs; i++)
         {
                 //LOGMSG(" %d => %d\n", i, progExecutable.attribMap[i]);
         }
     */
//     for(int i=0; i< MAX_VERTEX_ATTRIB_VARS; i++)
//     {
//         if(progExecutable.attribMap[i]>=0)
//             //LOGMSG(" %d => %d\n", i, progExecutable.attribMap[i]);
 //    }
 
//         //LOGMSG("\n");
 
 
         return true;
 }
 bool Program::isAttached(Shader* s)
 {
     if(s == 0)
     {
         gAssert(0 && "ShaderPtr is 0!!!");
         return false;
     }
 
     return (this->vertexShader == s) || (this->fragmentShader == s);
 }
 void Program::attachShader(Shader* s)
 {
         GET_GL_STATE(ctx);
 
     if(s == 0)
     {
         gAssert(false && "Trying to attach null shader!");
         return;
     }
 
     Shader** curShader=0;
     if(s->type == VERTEX_SHADER)
     {
         curShader = &vertexShader;
     }
     else if(s->type == FRAGMENT_SHADER)
     {
         curShader = &fragmentShader;
     }
     else
     {
         gAssert(false && "Shader type is not vertex or fragment!");
         return;
     }
 
     if(*curShader != 0)
     {
         (*curShader)->refCount--;
         if((*curShader)->deleteStatus && ((*curShader)->refCount == 0))
         {
             ctx->shaderProgNames.remove((*curShader)->id);
         }
     }
 
     s->refCount++;
     *curShader = s;
 }
 
 //Implementation Note:
 //              Do not delete the shader if its refcount is 0 because
 //              detachShader could be executed while running through
 //              shaderprogNameslist and deleting programs. This causes
 //              problems due to the looping iterator being invalidated. 
 
 
 void Program::detachShader(Shader* s)
 {
         //GET_GL_STATE(ctx);
     
         if(s == 0)
     {
         return;
     }
     
     Shader * currentShader = 0;
     
     if(s == vertexShader)
     {
         currentShader = vertexShader;
         vertexShader = 0;
     }
     else if( s == fragmentShader)
     {
         currentShader = fragmentShader;
         fragmentShader = 0;
     }
 
     if(currentShader != 0)
     {
         currentShader->refCount--;
         if(currentShader->deleteStatus && (currentShader->refCount == 0))
         {
             //Do nothing. Deleting the shader here will cause a problem if
             // this is being run under the context of program deletion.
             // This is a problem is remove is implemented such that it 
             // actually erases it from the list. Not calling remove here
             // has the benefit on working with any implementation of remove
        
             //ctx->shaderProgNames.remove(currentShader->id);
         }
     }
     
 }
 
 
 void Program::detachAll()
 {
     detachShader(vertexShader);
     detachShader(fragmentShader);
 }
 
 
 bool Program::validate()
 {
    //GET_GL_STATE(ctx);
 
    return true;
 }
 
 
 Program::~Program()
 {
     setDefaults();
 }
 
 
 int Program::getNumAttachedShaders() const
 {
     int numAttachedShaders = 0;
     if(vertexShader!=0) numAttachedShaders++;
     if(fragmentShader!=0) numAttachedShaders++;
 
     return numAttachedShaders;
 }
 
 
 int Program::getInfoLogLength() const
 {
     int infoLogLength = infoLog.length();
 
     if(infoLogLength>0) infoLogLength++; //If info log is of non zero length we need to give the length + 1 for '\0'
     
     return infoLogLength;
 }
 
 
 
 //---------------  ShaderProgNameList ---------------
 
 
 ShaderProgNameList::const_iterator ShaderProgNameList::find(GLuint id) const
 {
     ShaderProgNameList::const_iterator it = this->begin();
 
     while(it != this->end())
         {
 
         if(it->id == id)
             break;
         ++it;
         }
 
     return it;
 }
 
 
 void ShaderProgNameList::remove(GLuint id) 
 {
     ShaderProgNameList::iterator it = this->begin();
 
     while(it != this->end())
         {
         if(it->id == id)
             break;
         ++it;
         }
     it->release();
     this->erase(it);
 }
 
 //! This function will only return an unused id and will not mark it as
 //! used. An id I is treated as used only if a ShaderProgramNameMap element
 //! is added to this object with its member id set to I.
 
 GLuint  ShaderProgNameList::getUnusedName() const
 {
     GLuint  i = 1;
 
     ShaderProgNameList::const_iterator it = this->begin();
 
 #if 0
     //TODO: use a better id gen method - this would work only for an ustl::map
     while(it != this->end())
     {
         if(it->id == i) 
         {
             i++;
         }
 
         ++it;
     }
     return i;
 #else
     while(it != this->end())
         {
         i = Plat::max(it->id, i);
         ++it;
         }
     
         return i+1;
 #endif    
 
 }
 
 
 void ShaderProgNameList::cleanup()
 {
     ShaderProgNameList::iterator it = this->begin();
 
     //Delete programs first so there will be no dangling pointers
     // ... as programs store pointers to shaders
 
     while(it != this->end())
     {
         if(it->type == PROGRAM)
         {
             it->release();
         }
         ++it;
         }
 
     it = this->begin();
 
     while(it != this->end())
         {
         if(it->type == SHADER)
         {
             it->release();
         }
         ++it;
         }
 
     clear(); //delete all entries
 }
 
 
 void ShaderProgNameList::freeUnused()
 {
     GET_GL_STATE(ctx);
     ShaderProgNameList::iterator it = this->begin();
 
     //unused entries (shader or program) are of two types:
     //  1) the object still exists ands its deleteStatus is true and (refcount is 0 in case of shader)
     //  2) the object doesn't exits. so its pointer is zero. 
 
 
     while(it != this->end())
         {
         if(it->type == SHADER && it->shaderPtr && it->shaderPtr->deleteStatus && (it->shaderPtr->refCount==0))
         {
             it->release();
             it = erase(it);
         }
         else if(it->type == SHADER && (it->shaderPtr==0))
         {
             it = erase(it);
         }
         else if(it->type == PROGRAM && it->programPtr && it->programPtr->deleteStatus && (ctx->current_program != it->id) )
         {
             it->release();
             it = erase(it);
         }
         else if(it->type == PROGRAM && (it->programPtr==0))
         {
             it = erase(it);
         }
         else
         {
             ++it;
         }
     }
 }
 
 
 ShaderProgNameList::~ShaderProgNameList()
 {
     cleanup();
 }
