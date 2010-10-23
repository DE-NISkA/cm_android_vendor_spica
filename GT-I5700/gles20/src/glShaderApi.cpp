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
* \file         glShaderApi.cpp
* \author       Sandeep Kakarlapudi (s.kakarla@samsung.com)
*           Anurag Ved (anuragv@samsung.com)
* \brief        Implementation of GL shader and program entry points
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include <cutils/log.h>
#include <cutils/properties.h>

static const char VSMagicNum[] = {'V', 'S', ' ', ' '};
static const char FSMagicNum[] = {'P', 'S', ' ', ' '};
   

GL_API void GL_APIENTRY glAttachShader (GLuint program, GLuint shader)
{
        GET_GL_STATE(ctx);            

    if(program == 0)
    {
        set_err(GL_INVALID_VALUE);
        return;                             //TODO: Spec is not clear on this
                                            //      silently ignore or not
    }

    if(shader == 0)
    {
        set_err(GL_INVALID_VALUE);
        return;
    }

    ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);
    ShaderProgNameList::const_iterator shaderIt = ctx->shaderProgNames.find(shader);

    if((progIt == ctx->shaderProgNames.end()) || (shaderIt == ctx->shaderProgNames.end()))
    {
        set_err(GL_INVALID_VALUE);
        return;
    }
    
    if((progIt->type != PROGRAM) || (shaderIt->type != SHADER))
    {
        set_err(GL_INVALID_OPERATION);
        return;
    }
    
    gAssert((progIt->programPtr != 0) && "program ptr in name list is 0!!!");
    gAssert((shaderIt->shaderPtr != 0) && "shader ptr in name list is 0!!!");

    if((progIt->programPtr->isAttached(shaderIt->shaderPtr)))
    {
        set_err(GL_INVALID_OPERATION);
        return;
    }   

    progIt->programPtr->attachShader(shaderIt->shaderPtr);
}



GL_API void GL_APIENTRY glBindAttribLocation (GLuint program, GLuint index, const char *name)
{
    GET_GL_STATE(ctx);
   
    //-------- Validate attrib binding values --------
    if((index >= MAX_VERTEX_ATTRIBS)||(program == 0))
    {
        set_err(GL_INVALID_VALUE);
        return;
    }
    if( (name == 0) || ( (name[0] == 'g') && (name[1] == 'l') && (name[2] == '_') ) )
    {
        //TODO: Spec ambiguous on error on name == 0!
        set_err(GL_INVALID_OPERATION);
        return;
    }

    //-------- Check if the program name a valid program name --------
    ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);

    if( (progIt == ctx->shaderProgNames.end()) )
    {
        set_err(GL_INVALID_VALUE);
        return;
    }

    if( progIt->type != PROGRAM )
    {
        set_err(GL_INVALID_OPERATION);
        return; 
    }
    
    gAssert( (progIt->programPtr != 0) && "Prog ptr is 0!\n" );

    if( ! progIt->programPtr->attribBindings.setBinding(name, index) )
    {
        set_err(GL_OUT_OF_MEMORY);
        return;
    }
}

GL_API GLuint GL_APIENTRY glCreateProgram (void)   
{
        GET_GL_STATE(ctx);
        
    GLuint  i = ctx->shaderProgNames.getUnusedName();

    ctx->shaderProgNames.freeUnused();

    //create a program and add it to the list.
    ShaderProgramNameMap temp(new Program(i),i);

    ctx->shaderProgNames.push_back(temp);

    return temp.id;
}

GL_API GLuint GL_APIENTRY glCreateShader (GLenum type)
{
        GET_GL_STATE(ctx);
    
    ShaderType sType;

        switch(type)
        {
        case GL_VERTEX_SHADER:
                sType = VERTEX_SHADER;
        break;
        case GL_FRAGMENT_SHADER:
        sType = FRAGMENT_SHADER;
                break;

        default:
        set_err(GL_INVALID_ENUM);
                return 0;
        }

    GLuint  i = ctx->shaderProgNames.getUnusedName();

    ctx->shaderProgNames.freeUnused();

    //Create a shader and add it to the list
    ShaderProgramNameMap temp(new Shader(sType,i), i);
    ctx->shaderProgNames.push_back(temp);

    return temp.id;
 
}


GL_API void GL_APIENTRY glDeleteProgram (GLuint program)
{
        GET_GL_STATE(ctx);

    if(program == 0)
    {
        return;
    }

    ShaderProgNameList::const_iterator it = ctx->shaderProgNames.find(program);
    
    if(it == ctx->shaderProgNames.end())
    {
        set_err(GL_INVALID_VALUE);
        return;
    }
    else if(it->type != PROGRAM)        //TODO: Spec is ambiguous, clarify
    {
        set_err(GL_INVALID_OPERATION);
        return;
    }
    else if(it->id == ctx->current_program)
    {
        gAssert(it->programPtr != 0);

        it->programPtr->deleteStatus = true;
    }
    else
    {
        ctx->shaderProgNames.remove(it->id);
    }
}

GL_API void GL_APIENTRY glDeleteShader (GLuint shader)
{
        GET_GL_STATE(ctx);

    if(shader == 0)
    {
        return;
    }

     ShaderProgNameList::const_iterator it = ctx->shaderProgNames.find(shader);

    if(it == ctx->shaderProgNames.end())
    {
        set_err(GL_INVALID_VALUE);
        return;
    }
    else if( it->type != SHADER)    //TODO: Spec is ambiguous, clarify
    {
        set_err(GL_INVALID_OPERATION);
        return;
    }
    else if( it->shaderPtr->refCount != 0)
    {
        it->shaderPtr->deleteStatus = true;
    }
    else
    {
        ctx->shaderProgNames.remove(it->id);
    }
}

GL_API void GL_APIENTRY glDetachShader (GLuint program, GLuint shader)
{
        GET_GL_STATE(ctx);

    if((program == 0))
    {
        set_err(GL_INVALID_VALUE);                          //added by chanchal
        return;                             //TODO: Spec is not clear on this:
                                            //      silently ignore or not?
    }

    if(shader == 0)
    {
        set_err(GL_INVALID_VALUE);
        return;
    }

    ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);
    ShaderProgNameList::const_iterator shaderIt = ctx->shaderProgNames.find(shader);

    if((progIt == ctx->shaderProgNames.end()) || (shaderIt == ctx->shaderProgNames.end()))
    {
        set_err(GL_INVALID_VALUE);
        return;
    }
    
    if((progIt->type != PROGRAM) || (shaderIt->type != SHADER))
    {
        set_err(GL_INVALID_OPERATION);
        return;
    }
    
    gAssert((progIt->programPtr != 0) && "program ptr in name list is 0!!!");
    gAssert((shaderIt->shaderPtr != 0) && "shader ptr in name list is 0!!!");

    if( !(progIt->programPtr->isAttached(shaderIt->shaderPtr)) )
    {
        set_err(GL_INVALID_OPERATION);
        return;
    }

    progIt->programPtr->detachShader(shaderIt->shaderPtr);

}

GL_API void GL_APIENTRY glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, char *name)
{
        GET_GL_STATE(ctx);

    if( (!size) || (!type) || (!name))
    {
        return;
    }    

//    if(program == 0)                               //error checking added by chanchal
//    {
//        set_err(GL_INVALID_VALUE);
//        return;
//    }


    ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);

    if( (progIt == ctx->shaderProgNames.end()) )
    {
        set_err(GL_INVALID_VALUE);
        return;
    }

    if( progIt->type != PROGRAM )
    {
        set_err(GL_INVALID_OPERATION);
        return; 
    }

    gAssert((progIt->programPtr!=0) && "prog ptr is 0!!");

    Program* prog = progIt->programPtr;
    
    if(index >= prog->vars.getNumActiveAttributes())
    {
        set_err(GL_INVALID_VALUE);
        return;
    }

    if(bufsize < 0)
    {
        set_err(GL_INVALID_VALUE);
        return;
    }

    if(bufsize < 1)
    {
        return;
    }

    int len = Plat::min(bufsize-1, GLsizei(prog->vars.attributeArray[index].name.length()));
    *type = prog->vars.attributeArray[index].type;
    *size = 1;
    strncpy(name, prog->vars.attributeArray[index].name.c_str(), len);
    name[len] = '\0';
    if(length)
    {
        *length = len;
    }

}


GL_API void GL_APIENTRY glGetActiveUniform (GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, char *name)
{
        GET_GL_STATE(ctx);

    if( (!size) || (!type) || (!name))
    {
        return;
    } 

//    if(program == 0)                                              //error checking added by chanchal
//    {
//        set_err(GL_INVALID_VALUE);
//        return;
//    }    

    ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);

    if( (progIt == ctx->shaderProgNames.end()) )
    {
        set_err(GL_INVALID_VALUE);
        return;
    }

    if( progIt->type != PROGRAM )
    {
        set_err(GL_INVALID_OPERATION);
        return; 
    }

    gAssert((progIt->programPtr!=0) && "prog ptr is 0!!");

    Program* prog = progIt->programPtr;
    
    if(index >= prog->vars.getNumActiveUniforms())
    {
        set_err(GL_INVALID_VALUE);
        return;
    }

    if(bufsize < 0)
    {
        set_err(GL_INVALID_VALUE);
        return;
    }

    if(bufsize < 1)
    {
        return;
    }

    NameMap::const_iterator it = prog->vars.uniformTbl.uniformNameMap.begin();
    for(unsigned int i=0; i<index; i++)
    {
        ++it;
    }
    
    int len = Plat::min(bufsize-1, GLsizei(it->first.length()));
    *type = prog->vars.uniformTbl.uniforms[it->second.index].type;
    *size = it->second.size;
    strncpy(name, prog->vars.uniformTbl.uniforms[it->second.index].name.c_str(), len);
    name[len] = '\0';
    if(length)
    {
        *length = len;
    }


}

GL_API void GL_APIENTRY glGetAttachedShaders (GLuint program, GLsizei maxcount, GLsizei *count, GLuint *shaders)
{
    GET_GL_STATE(ctx);
    
    if(program == 0)                                            // error checking added by chanchal
    {
        set_err(GL_INVALID_VALUE);
    return;
    }

    ShaderProgNameList::const_iterator it = ctx->shaderProgNames.find(program);
    
    if(it == ctx->shaderProgNames.end())
    {
        set_err(GL_INVALID_VALUE);
        return;
    }
    else if(it->type != PROGRAM)        
    {
        set_err(GL_INVALID_OPERATION);
        return;
    }

    gAssert( (it->programPtr != 0) && "Prog ptr is 0!\n" );

    if(maxcount < 0)
    {
        set_err(GL_INVALID_VALUE);
        return;
    }

    Program* prog = it->programPtr;

    int numShaders = 0;

    if(numShaders > maxcount)
        return;

    if( prog->vertexShader!=0) 
    {
        if(shaders) //TODO: SPEC AMBIGUITY : what error on null? nv driver crashes!
        {
            shaders[numShaders] = prog->vertexShader->id;
        }
        numShaders++;
    }
    
    if(numShaders > maxcount)
        return;

    if( prog->fragmentShader!=0)
    {
        if(shaders) //TODO: SPEC AMBIGUITY : what error on null? nv driver crashes!
        {
            shaders[numShaders] = prog->fragmentShader->id;
        }
        numShaders++;
    }

    if(count)
    {
        *count = numShaders;
    }

}


GL_API int  GL_APIENTRY glGetAttribLocation (GLuint program, const char *name)
{
    GET_GL_STATE(ctx);

//    if(program == 0)                                     //error checking added by chanchal
//   {
//        set_err(GL_INVALID_VALUE);
//        return -1;
//    }

    //-------- Check if the program name a valid program name --------
    ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);

    if( (progIt == ctx->shaderProgNames.end()) )
    {
        set_err(GL_INVALID_VALUE);
        return -1;
    }

    if( progIt->type != PROGRAM )
    {
        set_err(GL_INVALID_OPERATION);
        return -1; 
    }

    gAssert((progIt->programPtr!=0) && "prog ptr is 0!!");

    
    if( progIt->programPtr->linkStatus != true )
    {
        set_err(GL_INVALID_OPERATION);
        return -1; 
    }

    const ustl::string nameStr(name);
    
    for( unsigned int i=0; i< progIt->programPtr->vars.numAttributeVars; i++)
    {
        if(nameStr == progIt->programPtr->vars.attributeArray[i].name)
        {
            int vsRegIndex = progIt->programPtr->vars.attributeArray[i].vsRegIndex;
            return static_cast<int>(progIt->programPtr->progExecutable.attribMap[vsRegIndex]);
        }
    }

    return -1;
}

GL_API void GL_APIENTRY glGetProgramiv (GLuint program, GLenum pname, GLint *params)
{
        GET_GL_STATE(ctx);

    if(!params)  //TODO: SPEC AMBIGUITY: behaviour when params is null?
    {
        return;
    }

    ShaderProgNameList::const_iterator it = ctx->shaderProgNames.find(program);
    
    if(it == ctx->shaderProgNames.end())
    {
        set_err(GL_INVALID_VALUE);
        return;
    }
    else if(it->type != PROGRAM)        
    {
        set_err(GL_INVALID_OPERATION);
        return;
    }

    gAssert( (it->programPtr != 0) && "Prog ptr is 0!\n" );

    Program *prog = it->programPtr;

    

    switch(pname)
    {
    case GL_DELETE_STATUS:
        *params = (prog->deleteStatus)? GL_TRUE : GL_FALSE;
        break;

    case GL_LINK_STATUS:
        *params = (prog->linkStatus)? GL_TRUE: GL_FALSE;
        break;

    case GL_VALIDATE_STATUS:
        *params = (prog->validationStatus)? GL_TRUE: GL_FALSE;
        break;

    case GL_INFO_LOG_LENGTH:
        *params = prog->getInfoLogLength();
        break;

    case GL_ATTACHED_SHADERS:
        *params = prog->getNumAttachedShaders();
        break;

    case GL_ACTIVE_ATTRIBUTES:
        *params = prog->vars.getNumActiveAttributes();
        break;

    case GL_ACTIVE_ATTRIBUTE_MAX_LENGTH:
        *params = prog->vars.getActiveAttributeMaxLength();
        break;

    case GL_ACTIVE_UNIFORMS:
        *params = prog->vars.getNumActiveUniforms();
        break;

    case GL_ACTIVE_UNIFORM_MAX_LENGTH:
        *params = prog->vars.getActiveUniformMaxLength();
        break;

    default:
        set_err(GL_INVALID_ENUM);
    }
}


GL_API void GL_APIENTRY glGetProgramInfoLog (GLuint program, GLsizei bufsize, GLsizei *length, char *infolog)
{
        GET_GL_STATE(ctx);

    if(!infolog)  //TODO: SPEC AMBIGUITY: behaviour when infolog is null?
    {
        return;
    }    
    
    //-------- Check if the program name a valid program name --------
    ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);

    if( (progIt == ctx->shaderProgNames.end()) )
    {
        set_err(GL_INVALID_VALUE);
        return;
    }

    if( progIt->type != PROGRAM )
    {
        set_err(GL_INVALID_OPERATION);
        return; 
    }
    
    gAssert( (progIt->programPtr != 0) && "Prog ptr is 0!\n" );

    if(bufsize < 0)
    {
        set_err(GL_INVALID_VALUE);
        return;
    }
    #if 0                                           //chanchal
    if(bufsize <1)
    {
        return;
    }
    #endif

    int len = Plat::min(size_t(bufsize-1), strlen(progIt->programPtr->infoLog.c_str()));

    Plat::memcpy(infolog, progIt->programPtr->infoLog.c_str(), len);
    infolog[len] = '\0';

    if(length)
    {
        *length = len;
    }

}

GL_API void GL_APIENTRY glGetShaderiv (GLuint shader, GLenum pname, GLint *params)
{
    GET_GL_STATE(ctx);
    //fprintf(stderr, "\n\n\n\n#######shader = %d\n############pname = %d, \n###########params = %p\n\n\n\n", shader, pname, params);

    if(shader == 0)
    {
        set_err(GL_INVALID_VALUE);
        return;
    }

    if(!params)  //TODO: SPEC AMBIGUITY: behaviour when params is null?
    {
        return;
    }

    ShaderProgNameList::const_iterator shaderIt = ctx->shaderProgNames.find(shader);

    if(shaderIt == ctx->shaderProgNames.end())
    {
        set_err(GL_INVALID_VALUE);
        return;
    }
    
    if(shaderIt->type != SHADER)
    {
        set_err(GL_INVALID_OPERATION);
        return;
    }
    
    gAssert((shaderIt->shaderPtr != 0) && "shader ptr in name list is 0!!!");
    
    Shader* sh = shaderIt->shaderPtr;

    switch(pname)
    {
    case GL_SHADER_TYPE:
        if(sh->type == VERTEX_SHADER)
        {
            *params = GL_VERTEX_SHADER;
        }
        else if(sh->type == FRAGMENT_SHADER)
        {
            *params = GL_FRAGMENT_SHADER;
        }
        else
        {
            gAssert(false && "Shader type is not vertex or fragment!");
        }
        break;

    case GL_DELETE_STATUS:
        *params = (sh->deleteStatus) ? GL_TRUE : GL_FALSE;
        break;

    case GL_COMPILE_STATUS:
        *params = (sh->compileStatus) ? GL_TRUE : GL_FALSE;
        break;

    case GL_INFO_LOG_LENGTH:
        *params = sh->getInfoLogLength();
        break;

    case GL_SHADER_SOURCE_LENGTH:
        *params = 0; //No online compiling for now
        break;

    default:
        set_err(GL_INVALID_ENUM);
    }
}

int getUniform(GLuint program, GLint location, GLfloat *params)
{
    GET_GL_STATE(ctx);

    if(!params)
    {
        return 0;
    }

//    if(program == 0)                                         //error checking added by chanchal
//    {
//        set_err(GL_INVALID_VALUE);
//        return 0;
//    }
    
    if(location == -1) 
    {
        set_err(GL_INVALID_OPERATION);                      //added by chanchal
        return 0;
    }

    ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);

    if( (progIt == ctx->shaderProgNames.end()) )
    {
        set_err(GL_INVALID_VALUE);
        return 0;
    }

    if( progIt->type != PROGRAM )
    {
        set_err(GL_INVALID_OPERATION);
        return 0; 
    }
    
    gAssert( (progIt->programPtr != 0) && "Prog ptr is 0!\n" );

    Program* prog = progIt->programPtr;
    
    if(!prog->linkStatus)
    {
        set_err(GL_INVALID_OPERATION);
        return 0;
    }

    if(location >= static_cast<int>(prog->vars.uniformTbl.uniforms.size()))
    {
        set_err(GL_INVALID_OPERATION);
        return 0;
    }

    //All uniforms are currently stored as floats
    GLenum type = prog->vars.uniformTbl.uniforms[location].type;
    int numComponents = getNumComponents(type);
    int fsLoc = prog->vars.uniformTbl.uniforms[location].fsLocation;
    int vsLoc = prog->vars.uniformTbl.uniforms[location].vsLocation;
    
    unsigned int regSet = 0;
    float* fMemBlock = 0;
    char* iMemBlock = 0;
    char* bMemBlock = 0;

    int loc = -1;
    SamplerMapEntry *samplerMappings =0;

    //Need to get either the one in vs constants or fs constants, whichever exists.
    //If both exist they will have the same value.
    if(vsLoc != -1)
    {
       loc = vsLoc;
       fMemBlock = prog->vars.vsFloatMemBlock;
       iMemBlock = prog->vars.vsIntMemBlock;
       bMemBlock = prog->vars.vsBoolMemBlock;
       regSet = prog->vars.uniformTbl.uniforms[location].vsRegSet;
       samplerMappings = prog->progExecutable.vsSamplerMappings;
    }
    else if(fsLoc != -1)
    {
        loc = fsLoc;
        fMemBlock = prog->vars.fsFloatMemBlock;
        iMemBlock = prog->vars.fsIntMemBlock;
        bMemBlock = prog->vars.fsBoolMemBlock;
        regSet = prog->vars.uniformTbl.uniforms[location].fsRegSet;
        samplerMappings = prog->progExecutable.fsSamplerMappings;
    }
    else
    {
        gAssert(loc != -1);
    }
    
    const float* memLoc = fMemBlock+loc;

    if(isVector(type))
    {
        if(regSet == DCL_REGSET_CONST_FLOAT)
            Plat::memcpy(params, memLoc, numComponents*4);
        else if(regSet == DCL_REGSET_CONST_INT)
            for(int i=0; i< numComponents; i++)
            {
                params[i] = float(*(iMemBlock+4*loc+i));
            }
        else if(regSet == DCL_REGSET_CONST_BOOL)
           //LOGMSG("//TBD: handle getting of bool\n");

        return numComponents;
    }
    else if(isMatrix(type))
    {
        if(type == GL_FLOAT_MAT2)
        {
            Plat::memcpy(params, memLoc, 4*4);
            return 4;
        }
        else if(type == GL_FLOAT_MAT3)
        {
            Plat::memcpy(params, memLoc, 3*4);
            Plat::memcpy(params+3, memLoc+4, 3*4);
            Plat::memcpy(params+6, memLoc+8, 3*4);
            return 9;
        }
        else if(type == GL_FLOAT_MAT4)
        {
            Plat::memcpy(params, memLoc, 16*4);
            return 16;
        }
        else 
        {
            gAssert(false && "isMatrix has a bug!");
        }

    }
    else if(isSampler(type))
    {
        params[0] = float(samplerMappings[loc].glTexUnit);
        return 1;
    }
    else
    {
        gAssert(false && " Uniform type is not Vector or Matrix or Sampler!");
    }

    return 0; 
}

GL_API void GL_APIENTRY glGetUniformfv (GLuint program, GLint location, GLfloat *params)
{
        //GET_GL_STATE(ctx);

    getUniform(program, location, params);
    
}

GL_API void GL_APIENTRY glGetUniformiv (GLuint program, GLint location, GLint *params)
{
        //GET_GL_STATE(ctx);

    if(!params)
    {
        return;
    }

    GLfloat vals[16];

    int count = getUniform(program, location, vals);
    
    for(int i=0; i< count; i++)
    {
        params[i] = GLint(vals[i]);
    }

}

GL_API int  GL_APIENTRY glGetUniformLocation (GLuint program, const char *name)
{
    GET_GL_STATE(ctx);

//    if(program == 0)                                         //error checking added by chanchal
//    {
//        set_err(GL_INVALID_VALUE);
//        return -1;
//    }

    ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);

    if( (progIt == ctx->shaderProgNames.end()) )
    {
        set_err(GL_INVALID_VALUE);
        return -1;
    }

    if( progIt->type != PROGRAM )
    {
        set_err(GL_INVALID_OPERATION);
        return -1; 
    }
    
    gAssert( (progIt->programPtr != 0) && "Prog ptr is 0!\n" );

    if( progIt->programPtr->linkStatus != true )
    {
        set_err(GL_INVALID_OPERATION);
        return -1;
    }

    return progIt->programPtr->vars.uniformTbl.getLocation(name);

}

GL_API GLboolean GL_APIENTRY glIsProgram (GLuint program)
{
        GET_GL_STATE(ctx);
    
    ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);

    if((progIt != ctx->shaderProgNames.end())&&(progIt->type == PROGRAM))
    {
        return GL_TRUE;
    }
    
    return GL_FALSE;
}

GL_API GLboolean GL_APIENTRY glIsShader (GLuint shader)
{
        GET_GL_STATE(ctx);

    ShaderProgNameList::const_iterator shaderIt = ctx->shaderProgNames.find(shader);

    if((shaderIt != ctx->shaderProgNames.end())&&(shaderIt->type == SHADER))
    {
        return GL_TRUE;
    }
    
    return GL_FALSE;
    
}


// glLinkProgram    Tries to link the program
// Notes:           link can fail for the following reasons:
//                  'CL' means this error should be caught by the compiler and 
//                  low level linker and not by linkProgram as it will not 
//                  the required information
//                  '+' indicated the following step is implemented
//
//    * The number of active attribute variables supported by the implementation has been exceeded.
//    * The storage limit for uniform variables has been exceeded.
//    * The number of active uniform variables supported by the implementation has been exceeded.
//    CL->* The main function is missing for the vertex shader or the fragment shader. 
//    * A varying variable actually used in the fragment shader is not declared in the same way (or is not declared at all) in the vertex shader.
//    CL->* A reference to a function or variable name is unresolved.
//    +* One or more of the attached shader objects has not been successfully compiled.
//    * Binding a generic attribute matrix caused some rows of the matrix to fall outside the allowed maximum of GL_MAX_VERTEX_ATTRIBS.
//    * Not enough contiguous vertex attribute slots could be found to bind attribute matrices.

GL_API void GL_APIENTRY glLinkProgram (GLuint program)
{
        GET_GL_STATE(ctx);

     /*the error invalid_value is generated if program is not a valid shader object created with glCreateProgram()*/
//     if(program == 0)                                        //error checking added by chanchal
//     {
//         set_err(GL_INVALID_VALUE);
//         return;
//     }
 
     //-------- Check if the program name is a valid program name --------
     ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);
 
     if( (progIt == ctx->shaderProgNames.end()) )
     {
         set_err(GL_INVALID_VALUE);
         return;
     }
 
     if( progIt->type != PROGRAM )
     {
         set_err(GL_INVALID_OPERATION);
         return; 
     }
     
     gAssert( (progIt->programPtr != 0) && "Prog ptr is 0!\n" );
 
     Program*    prog = progIt->programPtr;
 
         // if the current program is been linked then we make a deep copy of it.
         if(prog->id == ctx->current_program)
     {
                 //ctx->current_executable = (Executable*)Plat::malloc(sizeof(struct  Executable));
                 ctx->current_executable = new Executable();
                 ctx->current_executable->copyFrom(prog->progExecutable);
                 ctx->cur_exe_own_memory = GL_TRUE;
     }
     
     //ALERT! link status is being set to false here!
     prog->linkStatus        = false;
     prog->validationStatus  = false;
 
     //-------- Clear the generated parts of program --------
 
     prog->vars.clear();
     prog->infoLog.clear();
     prog->progExecutable.clear();
 
     //-------- Check that both vs and fs exist and they are compiled --------
     if( (prog->vertexShader == 0) || (prog->fragmentShader == 0) )
     {
 //        prog->infoLog.append("LINK FAILED: shader missing\n");
         return;
     }
 
     if( prog->vertexShader->compileStatus != true )
     {
 //        prog->infoLog.append("LINK FAILED: vertex shader is not compiled\n");
         return;
     }
     if( prog->fragmentShader->compileStatus != true)
     {
 //        prog->infoLog.append("LINK FAILED: fragment shader is not compiled\n");
         return;
     }
 
     //-------- Back patching linking ---------
     //TODO: call the low level linker if needed
 
     
     //-------- INSERT SYMBOL TABLE ENTRIES --------
     
     bool retVal = false;
     ustl::string errString;
     //-------- Insert uniform entries while checking link errors (match + limits)---------
     //-------- Insert sampler entries while checking link errors (match + limits)-------
     //-------- Insert attribute entries while checking link errors (match + limits)---------
     //-------- Insert varying entries while checking link errors (match + limits)-------
     
     retVal = prog->vars.insertVars(prog->progExecutable, prog->vertexShader->shaderBinary, 
         prog->vertexShader->sizeShaderBinary, VERTEX_SHADER, errString);
 
 //    if( !retVal) {  prog->infoLog.append(errString); prog->vars.clear(); return; }
 
     retVal = prog->vars.insertVars(prog->progExecutable, prog->fragmentShader->shaderBinary,
         prog->fragmentShader->sizeShaderBinary, FRAGMENT_SHADER, errString);
     
 //    if( !retVal) {  prog->infoLog.append(errString); prog->vars.clear(); return; }
 
     //Update varying register map in exe
     retVal = prog->vars.updateVaryingMap(prog->progExecutable, errString);
 //    if( !retVal) {  prog->infoLog.append(errString); prog->vars.clear(); prog->progExecutable.clear(); return; }
   
         //Update the attribute map
     //retVal = prog->vars.updateAttribMap(prog->progExecutable, errString);
     retVal = prog->updateAttribMap(errString);
     
 //      if( !retVal) {  prog->infoLog.append(errString); prog->vars.clear(); prog->progExecutable.clear(); return; }
   
     //DEBUG:
     //prog->vars.printVaryings();
 
     //-------- Generate attrib bindings while checking link errors (match + limits)--------
 
     /*
     retVal = prog->vars.insertAttribs(prog->vertexShader->shaderBinary,
         prog->vertexShader->sizeShaderBinary, VERTEX_SHADER, errString);
 
     if( !retVal) {  prog->infoLog.append(errString); return; }
 */
     //-------- Make executable --------
     //-------- Copy VS instructions --------
     prog->progExecutable.vs.binaryCodeSize = ((const ShaderHeader*)(prog->vertexShader->shaderBinary))->instruction_sz*16;
     prog->progExecutable.vs.pcStart = prog->vertexShader->pcStart;
     prog->progExecutable.vs.binaryCode = (unsigned char*)Plat::malloc(prog->progExecutable.vs.binaryCodeSize);
     
     int shaderInstrOffset = ((const ShaderHeader*)(prog->vertexShader->shaderBinary))->header_sz;
     
     Plat::memcpy(prog->progExecutable.vs.binaryCode, ((unsigned char*)(prog->vertexShader->shaderBinary))+shaderInstrOffset,
         prog->progExecutable.vs.binaryCodeSize);
 
     //-------- Copy FS instructions --------
     prog->progExecutable.fs.binaryCodeSize = ((const ShaderHeader*)(prog->fragmentShader->shaderBinary))->instruction_sz*16;
     prog->progExecutable.fs.pcStart = prog->fragmentShader->pcStart;
     prog->progExecutable.fs.binaryCode = (unsigned char*)Plat::malloc(prog->progExecutable.fs.binaryCodeSize);
     
     shaderInstrOffset = ((const ShaderHeader*)(prog->fragmentShader->shaderBinary))->header_sz;
     
     Plat::memcpy(prog->progExecutable.fs.binaryCode, ((unsigned char*)(prog->fragmentShader->shaderBinary))+shaderInstrOffset,
         prog->progExecutable.fs.binaryCodeSize);
 
 #ifdef FSO_JITO
         retVal = prog->progExecutable.vs.extractJitoData(VERTEX_SHADER);
         //LOGMSG("JIT optimization is %s for this VS\n",retVal ? "enabled" : "disabled");
 
         retVal = prog->progExecutable.fs.extractJitoData(FRAGMENT_SHADER);
         //LOGMSG("JIT optimization is %s for this FS\n",retVal ? "enabled" : "disabled");
 
 #endif
 
     // Linking successful!
 //    prog->infoLog.append("LINK SUCCESSFUL\n");
     prog->linkStatus = true;
 
     //-------- Update current executable if this is the current program --------
    #if 0
     if(prog->id == ctx->current_program)
     {
         ctx->current_executable->copyFrom(prog->progExecutable);
     }
         #else
         if(prog->id == ctx->current_program)
     {
                 
                 ctx->current_executable->clear();
                 delete ctx->current_executable;
                 //Plat::safe_free(ctx->current_executable);
                 ctx->current_executable = &(prog->progExecutable);
                 ctx->cur_exe_own_memory = GL_FALSE;
     }
 
         #endif
 
 }
 
 //TODO: update the design a bit?
 bool updateCurrentExecutable()
 {
 
     GET_GL_STATE(ctx);
     
     if(ctx->current_program == 0)
     {
         return false;
     }
     
 
         Program*    prog = ctx->current_program_ptr;  
     gAssert((prog != 0) && "Prog ptr is 0!\n");
     ctx->current_executable = &(prog->progExecutable);
 
          //TODO: move to linker?
         //FIMG specific stuff
     ////LOGMSG("Varying (VS: %d  FS: %d)\n", ctx->current_executable.numVSVarying,ctx->current_executable.numFSVarying);
 #if 0
     if((ctx->current_executable->numVSVarying == 1)&&(ctx->current_executable->numFSVarying==0))
     {
             //Add a dummy varying 
             ctx->current_executable->numVSVarying++;
             ctx->current_executable->numFSVarying++;
             ctx->current_executable->varyingMap[1]=0;
             
     }
 #else
     if((ctx->current_executable->numVSVarying >= 1)&&(ctx->current_executable->numFSVarying==0)){
         //Add a dummy varying 
         //ctx->current_executable->numVSVarying++;
         ctx->current_executable->numFSVarying++;
         ctx->current_executable->varyingMap[1]=0;  
     }
 
 #endif
 
 
 #ifdef FSO_JITO
         ctx->current_executable->vs.jitoptimize(ctx->cgd);
         ctx->current_executable->fs.jitoptimize(ctx->cgd);
 #endif
 
     return true;
 }
 
 template <typename T>
 void  setValues(T* vsMemBlock, T* fsMemBlock, UniformVar* first, const void* data,int count, int numComponents)
 {
     if(getNumComponents(first->type) != numComponents)
     {
         set_err(GL_INVALID_OPERATION);
         return;
     }
 
     T* vsDst = vsMemBlock;
     T* fsDst = fsMemBlock;
     const unsigned char* src = static_cast<const unsigned char*>(data);
     UniformVar* cur = first;
     int size = numComponents*sizeof(T);
     
     for( int i=0; i< count; i++)
     {
         if(!cur)
         {
             return;
         }
 
         if(cur->vsLocation >=0)
             Plat::memcpy(vsDst + cur->vsLocation, src, size);
 
         if(cur->fsLocation >=0)
             Plat::memcpy(fsDst + cur->fsLocation, src, size);
 
         src += size;
         cur = cur->nextElement;
     }
 }
 
 template <typename D, typename S, ShaderType sType>
 void setValues(D* dst, UniformVar* first, const void* data, int count, int numComponents)
 {
     const S* src = static_cast<const S*>(data);
     UniformVar* cur = first;
     for (int i=0; i <count; i++)
     {
         if(!cur)
         {
             return;
         }
 
         if(sType == VERTEX_SHADER)
         {
             if(cur->vsLocation >=0)
             {
                 for(int k=0; k<numComponents; k++)
                 {
                     *(dst+cur->vsLocation+k) = static_cast<D>( *(src + k));
                 }
             }
         }
         else if(sType == FRAGMENT_SHADER)
         {
             if(cur->fsLocation >=0)
             {
                 for(int k=0; k<numComponents; k++)
                 {
                     *(dst+cur->fsLocation+k) = static_cast<D>( *(src + k));
                 }
             }
         }
 
         src += numComponents;
         cur = cur->nextElement;
 
     }
 }
 
 template <typename D, typename S, ShaderType sType>
 void setValuesBool(D* dst, UniformVar* first, const void* data, int count, int numComponents)
 {
     const S* src = static_cast<const S*>(data);
     UniformVar* cur = first;
     for (int i=0; i <count; i++)
     {
         if(!cur)
         {
             return;
         }
 
         if(sType == VERTEX_SHADER)
         {
             if(cur->vsLocation >=0)
             {
                 for(int k=0; k<numComponents; k++)
                 {
                     *(dst+cur->vsLocation+k) = static_cast<D>( (*(src + k)) ? 1 : 0  );
                 }
             }
         }
         else if(sType == FRAGMENT_SHADER)
         {
             if(cur->fsLocation >=0)
             {
                 for(int k=0; k<numComponents; k++)
                 {
                     *(dst+cur->fsLocation+k) = static_cast<D>( (*(src + k)) ? 1 : 0 );
                 }
             }
         }
 
         src += numComponents;
         cur = cur->nextElement;
 
     }
 }
 
 template <typename S>
 void setBoolValues(unsigned int* vsMemBlock, unsigned int* fsMemBlock, UniformVar* first, const S* data, int count, int numComponents)
 {
     if(getNumComponents(first->type) != numComponents)
     {
         set_err(GL_INVALID_OPERATION);
         return;
     }
 
     const S* src = static_cast<const S*>(data);
     int size = numComponents*sizeof(S);
 
     UniformVar* cur = first;
 
     for( int i=0; i< count; i++)
     {
         if(!cur)
         {
             return;
         }
 
         if(cur->vsLocation >=0)
         {
             for(int k=0; k<numComponents; k++)
             {
                 unsigned int temp = *vsMemBlock;
                 unsigned int mask = 1<<(cur->vsLocation+k);
                 *vsMemBlock = temp & (~mask) | ( (src[0] == 0)? 0 : mask);
             }
         }
         if(cur->fsLocation >=0)
         {
             for(int k=0; k<numComponents; k++)
             {
                 unsigned int temp = *fsMemBlock;
                 unsigned int mask = 1<<(cur->fsLocation+k);
                 *fsMemBlock = temp & (~mask) | ( (src[0] == 0)? 0 : mask);
             }
         }
         src += size;
         cur = cur->nextElement;
     }
 }
 
 //uniform mats can only be floats
 void setUniformMat(GLint location, GLsizei count, const void* data, GLenum type)
 {
     GET_GL_STATE(ctx);
 
     if(ctx->current_program == 0)
         {
                 set_err(GL_INVALID_OPERATION);
                 return;    
         }
 
         if(location == -1)
         {
                 return;                 //Silently ignore
         }
     
     if(count < 0)
     {
         set_err(GL_INVALID_VALUE);
         return;         
     }
 
     Program* prog = ctx->current_program_ptr;
     gAssert( (prog != 0) && "ProgramPtr is 0!!");
 
     if( location >= static_cast<int>(prog->vars.uniformTbl.uniforms.size()))
     {
                 set_err(GL_INVALID_OPERATION);
                 return; 
     }
 
     if((count > 1) && (!(prog->vars.uniformTbl.uniforms[location].isArray)))
     {
                 set_err(GL_INVALID_OPERATION);
                 return;  
     }
 
     const GLenum varType = prog->vars.uniformTbl.uniforms[location].type;
     
     if( (varType != GL_FLOAT_MAT2) && (varType != GL_FLOAT_MAT3) && (varType != GL_FLOAT_MAT4))
     {
         set_err(GL_INVALID_OPERATION);
         return;
     }
 
     if( varType != type)
     {
         set_err(GL_INVALID_OPERATION);
         return;
     }
 
    
 
     const float* src = static_cast<const float*>(data);
     UniformVar* cur = &(prog->vars.uniformTbl.uniforms[location]);
    
     const int numFloats = getNumComponents(type);
     const int compSize = sizeof(GLfloat);
     float * vsMemBlock = prog->vars.vsFloatMemBlock;
     float * fsMemBlock = prog->vars.fsFloatMemBlock;
 
     for(int i=0; i< count ; i++)
     {
         if(!cur)
             return;
 
         if(cur->vsLocation >= 0)
         {
             switch(varType)
             {
              case GL_FLOAT_MAT2:    
                  Plat::memcpy(vsMemBlock + cur->vsLocation, src, compSize*numFloats); 
                  break;
              case GL_FLOAT_MAT3:
                  Plat::memcpy(vsMemBlock + cur->vsLocation, src, compSize*3);
                  Plat::memcpy(vsMemBlock + cur->vsLocation+4, src+3, compSize*3);
                  Plat::memcpy(vsMemBlock + cur->vsLocation+8, src+6, compSize*3);
                  break;
              case GL_FLOAT_MAT4:
                  //TODO: can be done in a single memcpy
                  Plat::memcpy(vsMemBlock + cur->vsLocation, src, compSize*4);
                  Plat::memcpy(vsMemBlock + cur->vsLocation+4, src+4, compSize*4);
                  Plat::memcpy(vsMemBlock + cur->vsLocation+8, src+8, compSize*4);
                  Plat::memcpy(vsMemBlock + cur->vsLocation+12, src+12, compSize*4);
                  break;
             }
         }
         if(cur->fsLocation >= 0)
         {
             switch(varType)
             {
              case GL_FLOAT_MAT2:    
                  Plat::memcpy(fsMemBlock + cur->fsLocation, src, compSize*numFloats); 
                  break;
              case GL_FLOAT_MAT3:
                  Plat::memcpy(fsMemBlock + cur->fsLocation, src, compSize*3);
                  Plat::memcpy(fsMemBlock + cur->fsLocation+4, src+3, compSize*3);
                  Plat::memcpy(fsMemBlock + cur->fsLocation+8, src+6, compSize*3);
                  break;
              case GL_FLOAT_MAT4:
                  //TODO: can be done in a single memcpy
                  Plat::memcpy(fsMemBlock + cur->fsLocation, src, compSize*4);
                  Plat::memcpy(fsMemBlock + cur->fsLocation+4, src+4, compSize*4);
                  Plat::memcpy(fsMemBlock + cur->fsLocation+8, src+8, compSize*4);
                  Plat::memcpy(fsMemBlock + cur->fsLocation+12, src+12, compSize*4);
                  break;
             }
         }
 
         cur = cur->nextElement;
         src += numFloats;
     }
 }
 
 //NOTE: does not support setting of bools yet.
 void  setUniformVec(GLint location, GLsizei count, const void* data, GLenum type)
 {
     GET_GL_STATE(ctx);
 
     if(ctx->current_program == 0)
         {
                 set_err(GL_INVALID_OPERATION);
                 return;    
         }
 
         if(location == -1)
         {
                 return;                 //Silently ignore
         }
     
     if(count < 0)
     {
         set_err(GL_INVALID_VALUE);
         return;         
     }
 
     Program* prog = ctx->current_program_ptr;
     gAssert( (prog != 0) && "ProgramPtr is 0!!");
 
 
     if( location >= static_cast<int>(prog->vars.uniformTbl.uniforms.size()))
     {
                 set_err(GL_INVALID_OPERATION);
                 return; 
     }
     
     if((count > 1) && (!(prog->vars.uniformTbl.uniforms[location].isArray)))
     {
                 set_err(GL_INVALID_OPERATION);
                 return;  
     }
 
     if(data == 0)
     {
         return;
     }
 
     UniformVar& uniform = prog->vars.uniformTbl.uniforms[location];
     const GLenum varType = uniform.type;
     const GLenum baseType = getBaseType(type);
     const GLenum baseVarType = getBaseType(varType);
     
     //NEW:
     if(!isSampler(varType))
     {
         if(getNumComponents(varType) != getNumComponents(type))
         {
             set_err(GL_INVALID_OPERATION);
             return;
         }
     }
 
 
     //type checking:
     if( baseType == GL_FLOAT)
     {
         if( (varType == GL_FLOAT) || (varType == GL_FLOAT_VEC2) || (varType == GL_FLOAT_VEC3) || (varType == GL_FLOAT_VEC4))
         {
             //Check the storage regset and set the appropriate one
             //setValues<float>(prog->vars.vsFloatMemBlock, prog->vars.fsFloatMemBlock, 
             //    &(uniform), data,count, getNumComponents(type));
             if(uniform.vsLocation>=0) { gAssert(uniform.vsRegSet == DCL_REGSET_CONST_FLOAT); }
             if(uniform.fsLocation>=0) { gAssert(uniform.fsRegSet == DCL_REGSET_CONST_FLOAT); }
             setValues<float,float,VERTEX_SHADER>(prog->vars.vsFloatMemBlock, 
                 &(uniform),data,count, getNumComponents(type));
             setValues<float,float,FRAGMENT_SHADER>(prog->vars.fsFloatMemBlock, 
                 &(uniform),data,count, getNumComponents(type));
 
             return;
         }
         else if( baseVarType == GL_BOOL)
         {
             //Check the storage regset and set the appropriate one
             if(uniform.vsLocation>=0) { gAssert(uniform.vsRegSet == DCL_REGSET_CONST_FLOAT); }
             if(uniform.fsLocation>=0) { gAssert(uniform.fsRegSet == DCL_REGSET_CONST_FLOAT); }
             setValuesBool<float,float,VERTEX_SHADER>(prog->vars.vsFloatMemBlock, 
                 &(uniform),data,count, getNumComponents(type));
             setValuesBool<float,float,FRAGMENT_SHADER>(prog->vars.fsFloatMemBlock, 
                 &(uniform),data,count, getNumComponents(type));
         }
         else
         {
             set_err(GL_INVALID_OPERATION);
             return;
         }
     }
     else if( baseType == GL_INT) 
     {
         if( baseVarType == GL_INT)
         {
             //Check the storage regset and set the appropriate one
                         if(uniform.vsLocation>=0) { gAssert(uniform.vsRegSet == DCL_REGSET_CONST_FLOAT); }
             if(uniform.fsLocation>=0) { gAssert(uniform.fsRegSet == DCL_REGSET_CONST_FLOAT); }
 
             setValues<float,int,VERTEX_SHADER>(prog->vars.vsFloatMemBlock, 
                 &(uniform),data,count, getNumComponents(type));
             setValues<float,int,FRAGMENT_SHADER>(prog->vars.fsFloatMemBlock, 
                 &(uniform),data,count, getNumComponents(type));
         
         }
         else if( baseVarType == GL_BOOL)
         {
             //Check the storage regset and set the appropriate one
             if(uniform.vsLocation>=0) { gAssert(uniform.vsRegSet == DCL_REGSET_CONST_FLOAT); }
             if(uniform.fsLocation>=0) { gAssert(uniform.fsRegSet == DCL_REGSET_CONST_FLOAT); }
             setValuesBool<float,int,VERTEX_SHADER>(prog->vars.vsFloatMemBlock, 
                 &(uniform),data,count, getNumComponents(type));
             setValuesBool<float,int,FRAGMENT_SHADER>(prog->vars.fsFloatMemBlock, 
                 &(uniform),data,count, getNumComponents(type));
         
         }
         else if( isSampler(varType))
         {
             if(getNumComponents(type) != 1)
             {
                 set_err(GL_INVALID_OPERATION);
                 return;
             }
 
             const int* src = static_cast<const int*>(data);
             UniformVar* cur = &(uniform);
 
             for(int i=0; i< count ; i++)
             {
                 if(!cur)
                     return;
 
                 if(cur->vsLocation >= 0)
                                 {
                                         if(prog->progExecutable.vsSamplerMappings[cur->vsLocation].isUsed != true)
                                         {
                                                 gAssert(false&&"sampler entry does not exist in exe but is used from setUniformVec");
                                                 return;
                                         }
 
                                         prog->progExecutable.vsSamplerMappings[cur->vsLocation].glTexUnit = *src;
                                 }
 
                 if(cur->fsLocation >= 0)
                                 {
                                         if(prog->progExecutable.fsSamplerMappings[cur->fsLocation].isUsed != true)
                                         {
                                                 gAssert(false&&"sampler entry does not exist in exe but is used from setUniformVec");
                                                 return;
                                         }
 
                                         prog->progExecutable.fsSamplerMappings[cur->fsLocation].glTexUnit = *src;
                                 }
 
                 cur = cur->nextElement;
                 src += 1;
             }
         }
         else
         {
             set_err(GL_INVALID_OPERATION);
             return;
         }
     }
 
 
 
 }
 GL_API void GL_APIENTRY glUniform1i (GLint location, GLint x)
 {
     if(location == -1) { return; }
     
         GET_GL_STATE(ctx);
 
     ctx->tempInt4[0] = x;
 
     setUniformVec(location, 1, ctx->tempInt4, GL_INT);
 }
 
 GL_API void GL_APIENTRY glUniform2i (GLint location, GLint x, GLint y)
 {
     if(location == -1) { return; }
     
         GET_GL_STATE(ctx);
 
     ctx->tempInt4[0] = x;
     ctx->tempInt4[1] = y;
 
     setUniformVec(location, 1, ctx->tempInt4, GL_INT_VEC2);
 }
 
 GL_API void GL_APIENTRY glUniform3i (GLint location, GLint x, GLint y, GLint z)
 {
     if(location == -1) { return; }
     
         GET_GL_STATE(ctx);
 
     ctx->tempInt4[0] = x;
     ctx->tempInt4[1] = y;
     ctx->tempInt4[2] = z;
 
     setUniformVec(location, 1, ctx->tempInt4, GL_INT_VEC3);
 }
 
 GL_API void GL_APIENTRY glUniform4i (GLint location, GLint x, GLint y, GLint z, GLint w)
 {
     if(location == -1) { return; }
     
         GET_GL_STATE(ctx);
 
 
     ctx->tempInt4[0] = x;
     ctx->tempInt4[1] = y;
     ctx->tempInt4[2] = z;
     ctx->tempInt4[3] = w;
 
     setUniformVec(location, 1, ctx->tempInt4, GL_INT_VEC4);
 }
 
 GL_API void GL_APIENTRY glUniform1f (GLint location, GLfloat x)
 {
     if(location == -1) { return; }
     
         GET_GL_STATE(ctx);
 
     ctx->tempFloat4[0] = x;
 
     setUniformVec(location, 1, ctx->tempFloat4, GL_FLOAT);
 }
 
 GL_API void GL_APIENTRY glUniform2f (GLint location, GLfloat x, GLfloat y)
 {
     if(location == -1) { return; }
     
         GET_GL_STATE(ctx);
 
 
     ctx->tempFloat4[0] = x;
     ctx->tempFloat4[1] = y;
 
     setUniformVec(location, 1, ctx->tempFloat4, GL_FLOAT_VEC2);
 }
 
 GL_API void GL_APIENTRY glUniform3f (GLint location, GLfloat x, GLfloat y, GLfloat z)
 {
     if(location == -1) { return; }
     
         GET_GL_STATE(ctx);
 
     ctx->tempFloat4[0] = x;
     ctx->tempFloat4[1] = y;
     ctx->tempFloat4[2] = z;
 
     setUniformVec(location, 1, ctx->tempFloat4, GL_FLOAT_VEC3);
 }
 
 GL_API void GL_APIENTRY glUniform4f (GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
 {
     if(location == -1) { return; }
     
         GET_GL_STATE(ctx);
 
     ctx->tempFloat4[0] = x;
     ctx->tempFloat4[1] = y;
     ctx->tempFloat4[2] = z;
     ctx->tempFloat4[3] = w;
 
     setUniformVec(location, 1, ctx->tempFloat4, GL_FLOAT_VEC4);
     
 }
 
 GL_API void GL_APIENTRY glUniform1iv (GLint location, GLsizei count, const GLint *v)
 {
     if(location == -1) { return; }
 
         setUniformVec(location, count, v, GL_INT);
 }
 
 GL_API void GL_APIENTRY glUniform2iv (GLint location, GLsizei count, const GLint *v)
 {
     if(location == -1) { return; }
 
         setUniformVec(location, count, v, GL_INT_VEC2);
 }
 
 GL_API void GL_APIENTRY glUniform3iv (GLint location, GLsizei count, const GLint *v)
 {
     if(location == -1) { return; }
 
         setUniformVec(location, count, v, GL_INT_VEC3);
 }
 
 GL_API void GL_APIENTRY glUniform4iv (GLint location, GLsizei count, const GLint *v)
 {
     if(location == -1) { return; }
 
         setUniformVec(location, count, v, GL_INT_VEC4);
 }
 
 GL_API void GL_APIENTRY glUniform1fv (GLint location, GLsizei count, const GLfloat *v)
 {
     if(location == -1) { return; }
 
         setUniformVec(location, count, v, GL_FLOAT);
 }
 
 GL_API void GL_APIENTRY glUniform2fv (GLint location, GLsizei count, const GLfloat *v)
 {
     if(location == -1) { return; }
 
         setUniformVec(location, count, v, GL_FLOAT_VEC2);
 }
 
 GL_API void GL_APIENTRY glUniform3fv (GLint location, GLsizei count, const GLfloat *v)
 {
     if(location == -1) { return; }
 
         setUniformVec(location, count, v, GL_FLOAT_VEC3);
 }
 
 GL_API void GL_APIENTRY glUniform4fv (GLint location, GLsizei count, const GLfloat *v)
 {
     if(location == -1) { return; }
 
     setUniformVec(location, count, v, GL_FLOAT_VEC4);
 }
 
 GL_API void GL_APIENTRY glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *v)
 {
     if(location == -1) { return; }
 
         if(transpose)   //can only be false in GL ES 2.0 
         {
                 set_err(GL_INVALID_VALUE);
         }
         else
         {
                 setUniformMat(location, count, v, GL_FLOAT_MAT2);
         }
 }
 
 GL_API void GL_APIENTRY glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *v)
 {
     if(location == -1) { return; }
     
 
         if(transpose)  //can only be false in GL ES 2.0
         {
                 set_err(GL_INVALID_VALUE);
         }
         else
         {
                 setUniformMat(location, count, v, GL_FLOAT_MAT3);
         }
 }
 
 GL_API void GL_APIENTRY glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *v)
 {
     if(location == -1) { return; }
     
 
         if(transpose)  //can only be false in GL ES 2.0
         {
                 set_err(GL_INVALID_VALUE);
         }
         else
         {
                 setUniformMat(location, count, v, GL_FLOAT_MAT4);
         }
 }
 
 GL_API void GL_APIENTRY glUseProgram (GLuint program)
 {
         GET_GL_STATE(ctx);
 
 
     if(program == 0)
     {
          //Not an error just should refer to invalid program object
         ctx->current_program = 0;
         ctx->current_program_ptr = NULL ;                
         return;
     }
         
         //can be any program we need to find out it's structure
     ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);
 
     if(progIt == ctx->shaderProgNames.end())
     {
         set_err(GL_INVALID_VALUE);
         return;
     }
     
     if(progIt->type != PROGRAM)
     {
         set_err(GL_INVALID_OPERATION);
         return;
     }
     
     gAssert( (progIt->programPtr != 0) && "ProgramPtr is 0!!");
 
         // if link satus fail, the current rendering state is not modified & the error INVALID OPERATION is generated.
     if(progIt->programPtr->linkStatus != GL_TRUE)
     {
                 set_err(GL_INVALID_OPERATION);
         return;         //TODO: Spec ambiguous if this should set an error or should be slient
     }
 
         // this condition is satisfied when link of the current program fail &( new program is different & no fail link status)
         if(ctx->cur_exe_own_memory == GL_TRUE){
                  ctx->current_executable->clear();
                  //Plat::safe_free(ctx->current_executable);
                  delete ctx->current_executable;
                  ctx->current_executable = 0;
                  ctx->cur_exe_own_memory = GL_FALSE;
         }
         
     if(progIt->programPtr->validate())
     {
         // Set this as the current program
         
         GLuint oldProg = ctx->current_program;
         ctx->current_program = program;
                 ctx->current_program_ptr = progIt->programPtr;
 
         ctx->current_executable = &(progIt->programPtr->progExecutable);
 
         if((oldProg != 0) && (oldProg != program))
         {
             ShaderProgNameList::const_iterator oldProgIt = ctx->shaderProgNames.find(oldProg);
 
             gAssert((oldProgIt != ctx->shaderProgNames.end()) && "Invalid value program was bound as the current program!");
          
             gAssert((oldProgIt->type == PROGRAM) && "Non program-type object was bound as the current program!");
                         
             gAssert((oldProgIt->programPtr != 0) && "ProgramPtr is 0!");
             
             // Delete if marked for deletion
             if(oldProgIt->programPtr->deleteStatus == GL_TRUE)
             {
                 ctx->shaderProgNames.remove(oldProg);
             }
         }
 
     }
         else
         {
                 //TODO: error?
         }
 
 }
 
 //TO DO CHECK THE IMPLEMENTATION  
 GL_API void GL_APIENTRY glValidateProgram (GLuint program)
 {
 
     GET_GL_STATE(ctx);
 
     //-------- Check if the program name is a valid program name --------
     ShaderProgNameList::const_iterator progIt = ctx->shaderProgNames.find(program);
     if( program == 0 )
     {
         set_err(GL_INVALID_VALUE);
         return; 
     }
     if( (progIt == ctx->shaderProgNames.end()) )
     {
         set_err(GL_INVALID_VALUE);
         return;
     }
 
     if( progIt->type != PROGRAM )
     {
         set_err(GL_INVALID_OPERATION);
         return; 
     }
     
    // gAssert( (progIt->programPtr != 0) && "Prog ptr is 0!\n" );
 
     Program*    prog = progIt->programPtr;
 
         //-------- Check that both vs and fs exist and they are compiled --------
     if( (prog->vertexShader == 0) || (prog->fragmentShader == 0) )
     {
 //        prog->infoLog.append("LINK FAILED: shader is  missing\n");
         return;
     }
 
         if( prog->vertexShader->compileStatus != true )
     {
 //        prog->infoLog.append("LINK FAILED: vertex shader is not compiled\n");
         return;
     }
     if( prog->fragmentShader->compileStatus != true)
     {
 //        prog->infoLog.append("LINK FAILED: fragment shader is not compiled\n");
         return;
     }
 
         
         //if not linked then there is no executable 
         if(prog->linkStatus == GL_FALSE){
 //              prog->infoLog.append("LINK FAILED : the program is not linked or the linking has falied\n");
         return;
     }
     
     Executable* currentExec = &(prog->progExecutable);
 
 
 

     
     //It is not allowed to have variables of different sampler types pointing to the
     //same texture image unit within a program object.
     int texUnitUsage = 0;
 
     for(int i = 0; i < MAX_TEXTURE_UNITS; i++)
     {
         if(currentExec->fsSamplerMappings[i].isUsed == false)
             continue;
 
          int texUnit = currentExec->fsSamplerMappings[i].glTexUnit;
 
          //tex unit used can not greater than the max texture unit available
          if(texUnit >= MAX_TEXTURE_UNITS){
                 //TO DO
                 //LOGMSG(" the value of the texUNit is greater than max. texture unit : %s", __FUNCTION__);
                 SET_ERR(ctx, GL_INVALID_OPERATION, "glTexImage2D");
                 return ;
         }
          
         GLenum texType = currentExec->fsSamplerMappings[i].type; 
         int bitToSet = 1<<( 3 * texUnit + (texType - GL_SAMPLER_2D));
         int previousSamplerValue = (texUnitUsage >> (3 * texUnit)) & 7 ;
 
         if(previousSamplerValue == 0){
             texUnitUsage |=  bitToSet;
         }
         else if( previousSamplerValue != (1 << (texType - GL_SAMPLER_2D))){
             fprintf(stderr, "error /n");
             SET_ERR(ctx, GL_INVALID_OPERATION, "glTexImage2D");
             return ;
         } 
 
     }
 
     prog->validationStatus = GL_TRUE;;
         
 }
 
 
 void printShaderHeaderInfo(const void* bin)
 {
 
     const ShaderHeader* sh = static_cast<const ShaderHeader*>(bin);
 
     static const char* regClassStrings[NR_DCL_REG_CLASSES] = 
     {
         "INPUT", "OUTPUT", "UNIFORM", "SAMPLER",
     };
 
     ShaderType sType = shaderHeaderType(sh);
 
     if(VERTEX_SHADER == sType)
         {
                 //LOGMSG("**************************************\n");
                 //LOGMSG("\tVertex Shader\n");
                 //LOGMSG("**************************************\n");
         }
         else if(FRAGMENT_SHADER == sType)
         {
                 //LOGMSG("**************************************\n");
                 //LOGMSG("\tFragment Shader\n");
                 //LOGMSG("**************************************\n");
         }
     else 
     {
         //LOGMSG(" unsupported header type!");
         return;
     }
 
     //LOGMSG("\tversion: %X\n", sh->version);
 
     //LOGMSG("\theaderSize: %d\n",sh->header_sz);
 
     //LOGMSG("\tInstruction Size: %d\n",sh->instruction_sz);
     //LOGMSG("\tConst float size: %d\n",sh->const_float_sz);
     //LOGMSG("\tConst int size: %d\n",sh->const_int_sz);
     //LOGMSG("\tConst bool size: %d\n",sh->const_bool_sz);
 
     for(int i=0; i < NR_DCL_REG_CLASSES; i++)
     {
         //LOGMSG("REG CLASS: %s ----\n", regClassStrings[i]);
         //LOGMSG("\tDCL Base Type Entries: %d\n",sh->dcl_base_type_dcls_table_sz[i]);
         //LOGMSG("\tDCL Array Type Entries: %d\n",sh->dcl_array_type_dcls_table_sz[i]);
         //LOGMSG("\n");
     }
     //LOGMSG("\tString Table size: %d\n",sh->dcl_string_table_sz);
 
     //LOGMSG("__________________________________________\n");
 }
 
 GL_API void GL_APIENTRY glShaderBinary(GLint n, GLuint *shaders, GLenum binaryformat, const void *binary, GLint length)
 {
         GET_GL_STATE(ctx);
         
     const int NUM_SHADER_TYPES = 2;
     
     //-------- Validate params --------
     if((n < 1)||(n>NUM_SHADER_TYPES))
     {
         set_err(GL_INVALID_VALUE);
         return;
     }
     
     //TODO: check "binaryformat"!
 #if 0
     if(binary == NULL)
     {
         //set_err(GL_INVALID_VALUE);                        //modified by chanchal
         return;
     }
 #endif
     if(shaders == NULL)
     {
         //set_err(GL_INVALID_VALUE);                        //modified by chanchal
         return;
     }
     
     //-------- Check if the shader ids are valid --------
     ShaderProgNameList::const_iterator shaderIt[NUM_SHADER_TYPES];
 
     Shader* vs=0;
     Shader* fs=0;
 
     for(int i=0; i< n; i++)
     {
 
         if(shaders[i] == 0)                                 //chanchal added error checking for safe side.. Need to check
         {
             set_err(GL_INVALID_VALUE);
             return;
         }
    
         shaderIt[i] = ctx->shaderProgNames.find(shaders[i]);
 
         if(shaderIt[i] == ctx->shaderProgNames.end())
         {
             set_err(GL_INVALID_VALUE);
             return;
         }
         else if( shaderIt[i]->type != SHADER)    
         {
             set_err(GL_INVALID_OPERATION);
             return;
         }
 
         if(binary == NULL)
         {
            //shaderIt[i]->shaderPtr->compileStatus = true;
             return;
         }
 
         gAssert((shaderIt[i]->shaderPtr!=0) && "ShaderPtr is 0!!");
 
         if(shaderIt[i]->shaderPtr->type == VERTEX_SHADER)
         {
             vs = shaderIt[i]->shaderPtr;
             vs->compileStatus = false;          //TODO: Spec ambiguity verify if 
                                                 // compilation status should be
                                                 // affected by a failed load
         }
         else if(shaderIt[i]->shaderPtr->type == FRAGMENT_SHADER)
         {
             fs = shaderIt[i]->shaderPtr;
             fs->compileStatus = false;          //TODO: Spec ambiguity verify if 
                                                 // compilation status should be
                                                 // affected by a failed load
         }
         else
         {
             gAssert(false && "Shader type is not VS or FS!");
         }
     }
 
     //TODO: if we support linked binaries, then an "ExeHeader" should be used below insetad of ShaderHeader
     ShaderHeader tempSH;
 
     Plat::memcpy(&tempSH, binary,sizeof(ShaderHeader)); 
         
     const ShaderHeader* sh = &tempSH;
     //const ShaderHeader* sh = static_cast<const ShaderHeader*>( binary);
     
     if(length < static_cast<int>(sizeof(ShaderHeader)))
     {
         return;
     }
 
 
         //check version number
         if(sh->version != BINARY_SHADER_VERSION)
         {
                 //LOGMSG("Unsupported Shader binary version!\n");
                 return;
         }
 
 
     const void* vsBin=0;
     int         vsBinSize =0;
     const void* fsBin=0;
     int         fsBinSize =0;
 
     ShaderType sType = shaderHeaderType(binary);
         
     if(VERTEX_SHADER == sType)
     {
         vsBin = binary;
         vsBinSize = length;
     }
     else if(FRAGMENT_SHADER == sType)
     {
         fsBin = binary;
         fsBinSize = length;
     }
     else
     {   
         //TODO: handle the linked executable format!!
         gAssert(false && "unrecognized header type!");
     }
 
     if(vsBin && vs)
     {
//         printShaderHeaderInfo((const void*)sh);
           vs->loadShaderBin(vsBin, vsBinSize);
     }
         
     if(fsBin && fs)
     {
//        printShaderHeaderInfo((const void*)sh);
          fs->loadShaderBin(fsBin, fsBinSize);
     }
         
 }
 
 #ifdef EN_EXT_VARYING_INTERPOLATION_EXP
 GL_API void GL_APIENTRY glVaryingInterpolationEXP(const char* name, GLenum mode)
 {
 
         GET_GL_STATE(ctx);
 
         if(ctx->current_program == 0)
         {
                 set_err(GL_INVALID_OPERATION);
                 return;    
         }
 
         if((mode!=GL_SMOOTH_EXP)&&(mode!=GL_FLAT_EXP))
         {
                 set_err(GL_INVALID_ENUM);
                 return;
         }
         
     Program* prog = ctx->current_program_ptr;
     gAssert( (prog != 0) && "ProgramPtr is 0!!");   
  
         ustl::string varyingName = ustl::string(name);
 
         if(varyingName == "gl_Position")
         {
                 return; //can't be changed.
         }
 
         int i=0;
         for(; i< prog->vars.numVaryingVars; i++)
         {
                 if(prog->vars.varyingArray[i].name == varyingName)
                 {
                         break;
                 }
         }
 
         if( i== prog->vars.numVaryingVars) 
         {
                 //Varying specified by "name" couldn't be found so ignore.
                 return;
         }
 
         unsigned int varyingNum = prog->progExecutable.varyingMap[prog->vars.varyingArray[i].vsRegIndex>>2];
 
         gAssert((varyingNum!=0)&&"varying Num = 0 , expected > 0");
 
         
         
         if(mode == GL_SMOOTH_EXP)
         {
                 prog->progExecutable.varyingInterpolation &= ~(1<<varyingNum);
         }
         else
         {
                 prog->progExecutable.varyingInterpolation |= (1<<varyingNum);
         }
 }
 
 GL_API void GL_APIENTRY glGetVaryingInterpolationEXP(const char *name, GLint* params)
 {
     GET_GL_STATE(ctx);
 
         if(ctx->current_program == 0)
         {
                 set_err(GL_INVALID_OPERATION);
                 return;    
         }
     
     if(params == NULL)
     {
         return;
     }
         
          Program* prog = ctx->current_program_ptr;
         gAssert( (prog != 0) && "ProgramPtr is 0!!");
     
         ustl::string varyingName = ustl::string(name);
 
         if(varyingName == "gl_Position")
         {
                 return; //can't be changed/not allowed.
         }
 
         int i=0;
         for(; i< prog->vars.numVaryingVars; i++)
         {
                 if(prog->vars.varyingArray[i].name == varyingName)
                 {
                         break;
                 }
         }
 
         if( i== prog->vars.numVaryingVars) 
         {
                 //Varying specified by "name" couldn't be found so ignore.
                 return;
         }
 
         unsigned int varyingNum = prog->progExecutable.varyingMap[prog->vars.varyingArray[i].vsRegIndex>>2];
 
         gAssert((varyingNum!=0)&&"varying Num = 0 , expected > 0");
 
         *params = (prog->progExecutable.varyingInterpolation & (1<<varyingNum) ) ? GL_FLAT_EXP : GL_SMOOTH_EXP;
 }
 #endif
 
 
 
 GL_API void GL_APIENTRY glGetShaderInfoLog (GLuint shader, GLsizei bufsize, GLsizei *length, char *infolog)
 {
             
     GET_GL_STATE(ctx);
 
     if(!infolog)  //TODO: SPEC AMBIGUITY: behaviour when infolog is null?
     {
         return;
     }    
     
     //-------- Check if the shader  name a valid shader name --------
     ShaderProgNameList::const_iterator shaderIt = ctx->shaderProgNames.find(shader);
 
     if( (shaderIt == ctx->shaderProgNames.end()) )
     {
         set_err(GL_INVALID_VALUE);
         return;
     }
 
     if( shaderIt->type != SHADER )
     {
         set_err(GL_INVALID_OPERATION);
         return; 
     }
     
     gAssert( (shaderIt->shaderPtr != 0) && "shader ptr is 0!\n" );
 
     if(bufsize < 0)
     {
         set_err(GL_INVALID_VALUE);
         return;
     }
 
 
     int len = Plat::min(size_t(bufsize-1), size_t(shaderIt->shaderPtr->lengthInfoLog));
    // fprintf(stderr, "\n######length = %d", len);
     Plat::memcpy(infolog, shaderIt->shaderPtr->infoLog, len);
     infolog[len] = '\0';
 
     if(length)
     {
         *length = len;
     }
     
 
 }
 
 
 
 
 //NOT SUPPORTED: these all are for run time compilation(GL_SHADER_COMPILER is GL_TRUE)
 //If the value of GL_SHADER_COMPILER is not TRUE,then the error INVALID OPERATION is generated ...spec
 
 GL_API void GL_APIENTRY glShaderSource(GLuint shader, GLsizei count, const char **string, const GLint *length)
 {
     set_err(GL_INVALID_OPERATION);
    
 }
 
 GL_API void GL_APIENTRY glCompileShader (GLuint shader)
 {
     set_err(GL_INVALID_OPERATION);
 
 }
 
 
 GL_API void GL_APIENTRY glReleaseShaderCompiler(void)
 {
     set_err(GL_INVALID_OPERATION);
 
 }
 
 
 GL_API void GL_APIENTRY glGetShaderSource (GLuint shader, GLsizei bufsize, GLsizei *length, char *source)
 {
     set_err(GL_INVALID_OPERATION);
 
 }
 
 
 GL_API void GL_APIENTRY glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision)
 {
 
 #if 0
         if((shadertype != GL_VERTEX_SHADER) && (shadertype != GL_FRAGMENT_SHADER)){
                 set_err(GL_INVALID_ENUM);
                 return;
                 
         }
 
         if((precisiontype != GL_LOW_FLOAT) && (precisiontype != GL_MEDIUM_FLOAT) &&(precisiontype != GL_HIGH_FLOAT)
                 && (precisiontype != GL_LOW_INT) && (precisiontype != GL_MEDIUM_INT)&&(precisiontype != GL_HIGH_INT)){
                 set_err(GL_INVALID_ENUM);
                 return; 
         }
 
         //the vertex and fragment shader support only high float and high int 
         if((precisiontype == GL_HIGH_FLOAT)|| (precisiontype == GL_MEDIUM_FLOAT)
                 ||(precisiontype == GL_LOW_FLOAT)){
                 range[0]         = GLF_SHADERRANGE_FLOATMIN; //TO DO
                 range[1]         = GLF_SHADERRANGE_FLOATMAX;
                 precision[0] = GLF_SHADERPRECISION_FLOAT;
         
         }
 
         if((precisiontype == GL_HIGH_INT)|| (precisiontype == GL_MEDIUM_INT)
                 ||((precisiontype == GL_LOW_INT))){
                 range[0]         = GLF_SHADERRANGE_INTMIN ; //TO DO
                 range[1]     =  GLF_SHADERRANGE_INTMAX;
                 precision[0] =  GLF_SHADERPRECISION_INT; //TO DO
                 
         
 }
 
 #else
      set_err(GL_INVALID_OPERATION);
 
 #endif
     
         
 }
