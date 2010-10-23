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
* \file         glShader.h
* \author       Sandeep Kakarlapudi (s.kakarla@samsung.com)
* \brief        Struct & function Definitions for GL Shaders,programs and loader
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*       02.08.2006      Sandeep Kakarlapudi             Initial functionality for firmware demo
*
*******************************************************************************
*/

#ifndef __GL_SHADER_H__
#define __GL_SHADER_H__

/*
*******************************************************************************
* Includes
*******************************************************************************
*/

#include "gl.h"
#include "glConfig.h"
#include "binaryShaderFormat.h"
#include "platform.h"
//#include "shaderOpt.h"

#include "ustl.h"

/*
*******************************************************************************
* Macro definitions and enumerations
*******************************************************************************
*/

//! Shader Type
enum ShaderType{
    INVALID_SHADER,
        VERTEX_SHADER,
        FRAGMENT_SHADER
};

enum ShaderProgramObjectType{
    INVALID_TYPE,
    SHADER,
    PROGRAM,
};

/*
*******************************************************************************
* Type, Structure & Class Definitions
*******************************************************************************
*/


/*
class StrLT : public ustl::binary_function<const char*, const char*, bool>
{
public:
    bool    operator()(const char* s1, const char* s2) const
    {
        return (strcmp(s1, s2) < 0);
    }
};
*/

//TODO: This should end up as a hash map finally

typedef ustl::string NameString;

struct NameInfo
{
    int index;
    int size;

    NameInfo(int index_=-1, int size_=0) : index(index_), size(size_) {};
};

typedef ustl::map<NameString, NameInfo> NameMap;

//! Uniform Variable
//  Note:   1) This does not care about memory management because all pointers
//          point into an allocated memory block. 
//
//                      2) The type of the UniformVar should ideally tell you in which const
//                      const resgister set (float, int or bool) the value will be stored.
//                      However, currently the compiler stores int and bools in floats too.
//                      Since bools are only very few, it may be that in future versions of
//                      the compiler the bools are stored in the floats. Hence we have  
//                      separate fields vsRegSetIndex and fsRegSetIndex to indicate the
//                      regset

struct UniformVar
{
    NameString      name;           //!< Uniform name
        int             size;           //!< size in units of type - will be 1
        GLenum          type;           //!< GL Uniform type

        int           vsLocation;     //!< offset into VS constants mem block
        int           fsLocation;     //!< offset into FS constants mem block

    UniformVar*     nextElement;    //!< Points to next element if an array

    unsigned char       vsRegSet;       //!< VS Constant register set
        unsigned char   fsRegSet;       //!< FS Constant register set
    bool            isArray;        //!< true if a uniformvar object is an
                                    //!< element of an array
    bool            isInitialized;  //!< true if vertex or fragment info is
                                    //!< entered
    UniformVar();

    void    setDefaults();
};

#include <stdio.h>

//! Sampler Map Entry
struct SamplerMapEntry
{
    NameString      name;           //!< Sampler Name
        GLenum          type;           //!< Denotes GL sampler type

    int             glTexUnit;     //!< gltexUnit initialized to 0 (since all uniforms are
                                                                                //!< initialized to 0

        bool                    isUsed;                 //!< true if this is a used entry

    SamplerMapEntry();

    SamplerMapEntry& operator= (const SamplerMapEntry& rvalue) {
//              Plat::printf("->\n");
//      Plat::printf("  name 0x%x, %d\n", rvalue.name, rvalue.name.length());
//              Plat::printf("  %s\n", rvalue.name.c_str());
//      if(rvalue.name.length() != 0) name.assign(rvalue.name);
        type = rvalue.type;
        glTexUnit = rvalue.glTexUnit;
        isUsed = rvalue.isUsed;
//              Plat::printf("<-\n");
        return *this;
    }

    void    setDefaults();

};

//! Varying Variable
struct VaryingVar
{

    NameString      name;           //!< Varying name
        GLenum             type;           //!< Denotes GL sampler type

    int             vsRegIndex;     //!< Index of the vs varying reg
    int             fsRegIndex;     //!< Index of the fs varying reg

    VaryingVar();

    void    setDefaults();
};

//! Attribute Variable
struct AttributeVar
{
    NameString      name;           //!< Attribute name
    GLenum          type;           //!< Denotes type

    int             vsRegIndex;     //!< Register index of the attribute variable
    int             attributeIndex; //!< Attribute index

    AttributeVar();
    void    setDefaults();

        AttributeVar(NameString n, GLenum t, int vsReg, int attribIndx)
                : name(n), type(t), vsRegIndex(vsReg), attributeIndex(attribIndx)
        {
        };
};

//! Uniform Table
struct UniformTable
{
    NameMap                 uniformNameMap;     //!< Uniform Name Map
    ustl::vector<UniformVar> uniforms;           //!< Uniform Var array
    

    UniformTable();

    bool            insertOrUpdateBaseEntry(const DclBaseType* bt, ShaderType sType, const char* name, ustl::string& errString, const uint32 constFloatSize);
    bool            insertOrUpdateArrayEntry(const DclArrayType* at, ShaderType sType, const char* name, ustl::string& errString, const uint32 constFloatSize);


    int             getLocation(const char* uniformName);

    void            clear();
};

//! DepthRange
struct GLESBuiltinUniforms
{
    int depthRange_near;
    int depthRange_far;
    int depthRange_diff;
};

//! Shader Executable
//Note: This has its own copy of the constants mem blocks and hence
//      should manage the memory on its own
struct ShaderExecutable
{
    float*    constFloatMemBlock;     //!< Ptr to Float mem block
    char*     constIntMemBlock;       //!< Ptr to Int mem block
    char*     constBoolMemBlock;      //!< Ptr to Bool mem block

    int             fmbSize;                //!< Float mem block size
    int             imbSize;                //!< Int mem block size
    int             bmbSize;                //!< Bool mem block size

    unsigned char*  binaryCode;             //!< Ptr to shader instructions
    unsigned int    binaryCodeSize;         //!< Size of instruction list
    unsigned int    pcStart;                //!< Starting program counter

    GLESBuiltinUniforms      builtinUniformIndices;             //!< struct of pointer to global f
#ifdef FSO_JITO
        unsigned char*  origBinaryCode;                 //!< Ptr to original shader instructions
                                                                                        //      This will point to null if the shader 
                                                                                        //  cannot be optimized.
        unsigned int    origBinaryCodeSize;             //!< Size of original instruction list
        unsigned int    origPcStart;                    //!< original Starting program counter
                
        bool                    jitoEnabled;                    //!< just in time optimization is performed
                                                                                        // only if jitoEnabled is true; 
        FsoOptData              jitoData;                               //!< data for jit optimization

        inline bool     extractJitoData(ShaderType shadertype)          //!< builds the relavant JITO data
        {
                jitoData.clear();

                if(fsoBuildOptData(jitoData, binaryCode, binaryCodeSize/FIMG_INSTR_SZ, pcStart) < 0)
                {
                        jitoEnabled = false;
                        return false;
                }
                
                //.. fsoData available!
// disabling this will force jito to run even if its not necessary. useful for perf testing
#if 1
                //Check if this shader really needs JIT optimization
                if(fsoShaderNeedsJito(jitoData) < 0)
                {
                        jitoEnabled = false;
                        return false;
                }
#endif

                //orig* should all be  cleared during glLink

                jitoEnabled                     = true;
                origBinaryCodeSize      = binaryCodeSize;
                origPcStart                     = pcStart;

                origBinaryCode          = (unsigned char*) Plat::malloc(origBinaryCodeSize);
                Plat::memcpy(origBinaryCode, binaryCode, origBinaryCodeSize);

                return true;
        }


        inline void             jitoptimize(CodegenData& cgd)                   //!< function that produces optimized shader code
        {
                //TODO: check if float data is dirty, reopt only if float data is dirty
                if(!jitoEnabled ) {
                        return;
                }

                setCodegenData(&cgd, jitoData.numbblocks, binaryCode, origBinaryCode, origBinaryCodeSize/FIMG_INSTR_SZ,
                                                constFloatMemBlock, fmbSize/16);

                fsoJito(&cgd, &binaryCodeSize, &pcStart, jitoData, origPcStart); 
//Enable this to force use of unoptimized binary - useful for jit perf test
#if 0
        pcStart = origPcStart;
        Plat::memcpy(binaryCode, origBinaryCode, origBinaryCodeSize);
        binaryCodeSize = origBinaryCodeSize;
#endif

        }
#endif

    ShaderExecutable();

    void    clear();
    void    copyFrom(const ShaderExecutable& se);

};

//! TextureDims
struct TextureDims
{
    int tex[MAX_TEXTURE_UNITS];     //!< fs.constFloatMemBlock[tex[i]] is the width
                                    //!< fs.constFloatMemBlock[tex[i]+1] is the height
};

//! Program Executable
struct  Executable
{
    ShaderExecutable    vs;         //!< Vertex shader executable
    ShaderExecutable    fs;         //!< Fragment shader executable

        int                              attribMap[MAX_VERTEX_ATTRIB_VARS];

    int              varyingMap[MAX_VARYING_MAPPINGS];//!< Varying map
                                    //!< varyingMap[vsRegId] = fsRegId
#ifdef EN_EXT_VARYING_INTERPOLATION_EXP
        unsigned int            varyingInterpolation;//!< Varying interpolation control
                                                                        //!< "i"th varying has flat interpolation if 
                                                                                //!< "i"th lsb is set, else smooth interpolation
#endif

        SamplerMapEntry         vsSamplerMappings[MAX_VERTEX_TEXTURE_UNITS];
        SamplerMapEntry         fsSamplerMappings[MAX_TEXTURE_UNITS];
    TextureDims         dimensions;

    int                 numAttribs; //!< No. of attributes
    int                 numVSVarying; //!< No. of varyings.
    int                 numFSVarying;
    int                 pointCoordIndex;
        int                                     pointSizeIndex; //!< VS out

    void copyFrom(const Executable& e);

    void    clear();
    void    dumpConstants(); //Debug function
};



//! Program Variables Struct
//NOTE:     Program Vars does not own any of the memory block pointers, they are owned by
//          ShaderExecutable
struct ProgramVars
{
    UniformTable    uniformTbl;             //!< Uniform Table

    VaryingVar      varyingArray[MAX_VARYING_VARS];  //!< Varying Table
    int             numVaryingVars;         //!< Num used in varyingArray

    AttributeVar    attributeArray[MAX_VERTEX_ATTRIB_VARS]; //!< Attribute Table
    unsigned int    numAttributeVars;       //!< Num used in attributeArray

    float*          vsFloatMemBlock;        //!< Mem block for VS const floats
    float*          fsFloatMemBlock;        //!< Mem block for FS const floats

    char*           vsIntMemBlock;          //!< Mem block for VS const ints
    char*           fsIntMemBlock;          //!< Mem block for FS const ints

    char*            vsBoolMemBlock;         //!< Mem block for VS const bools
    char*            fsBoolMemBlock;         //!< Mem block for FS const bools

    bool            insertVars(Executable& exe, const unsigned char* bin, int size, ShaderType sType, ustl::string& errString);
    bool            insertVaryingVar(ShaderType sType, ustl::string name, GLenum type, int regIndex, ustl::string& errString);
    //bool            insertAttribs(const unsigned char* bin, int size, ShaderType sType, ustl::string& errString);
    bool            updateVaryingMap(Executable& exe, ustl::string& errString);
        bool                    updateAttribMap(Executable& exe, ustl::string& errString);
    void            printVaryings() const;

    unsigned int    getNumActiveAttributes() const;
    int             getActiveAttributeMaxLength() const;
    unsigned int    getNumActiveUniforms() const;
    int             getActiveUniformMaxLength() const;

    ProgramVars();
    ~ProgramVars();

    void    clear();
};

//! Attribute Bindings
class AttribBindings
{
public:



    AttribBindings();
    ~AttribBindings();

    void    clear();

    int     getLocation(const char* name);
    bool    setBinding(const char* name, int attribLoc);

private:
    ustl::map<ustl::string,int> attribMap;
};


//! Shader object structure
struct Shader
{
    // GL Shader Object State
        unsigned int    id;                         //!< GL shader name
        ShaderType      type;                       //!< Shader type

    bool            deleteStatus;               //!< Deletion Status
    bool            compileStatus;              //!< Compilation Status

    char*           infoLog;                    //!< Information log
    int             lengthInfoLog;              //!< Information log length

    char*           shaderSourceString;         //!< Shader source string
    int             lengthShaderSourceString;   //!< Shader source length

    // Internal State
    unsigned char*  shaderBinary;               //!< Compiled shader binary
    unsigned int    sizeShaderBinary;           //!< Shader binary size
    unsigned int    pcStart;                    //!< Starting Program counter

    int             refCount;                   //!< Reference counter, tracks
                                                //!< number of programs using
                                                //!< it.

    Shader(ShaderType sType, GLuint id_);
    ~Shader();

    void    setDefaults();                      //!< Sets initial GL shader
                                                //!< object state
    bool    compile();

    void    loadShaderBin(const void* bin, int binSize);  //Loads shader bin
    int    getInfoLogLength() const;

private:
    Shader();
    Shader& operator=(const Shader& );
};


//! GL Program Object structure
struct Program
{
    // GL Program object state
    unsigned int    id;                         //!< GL program name

    bool            deleteStatus;               //!< program delete status
    bool            linkStatus;                 //!< program link status
    bool            validationStatus;           //!< program validation status

    ustl::string     infoLog;                    //!< Information log


    ProgramVars     vars;                       //!< Program variables

    Shader*         vertexShader;               //!< Attahed Vertex Shader
    Shader*         fragmentShader;             //!< Attached Fragment Shader

    AttribBindings  attribBindings;             //!< Attribute Bindings

    // Internal State
    Executable      progExecutable;             //!< Program Executable

    bool            validate();                 //!< Validates the program
    bool            link();                     //!< Links the program

    Program(GLuint id_);
    ~Program();

    void            setDefaults();              //!< Sets Program object
                                                //!< default state

        bool                    updateAttribMap(ustl::string& errString);

    bool            isAttached(Shader* s);
    void            attachShader(Shader* s);
    void            detachShader(Shader* s);

    void            detachAll();

    int             getNumAttachedShaders() const;
    int             getInfoLogLength() const;

private:



    Program();
    Program& operator=(const Program& );

};

struct ShaderProgramNameMap
{
    union
    {
        Shader*     shaderPtr;
        Program*    programPtr;
    };

    ShaderProgramObjectType type;
    GLuint                  id;

    ShaderProgramNameMap(Shader* s, GLuint name) { shaderPtr = s; type = SHADER; id = name; }
    ShaderProgramNameMap(Program* p, GLuint name) { programPtr = p; type = PROGRAM; id = name;}
    ShaderProgramNameMap()
    {
                type = INVALID_TYPE;
                shaderPtr = NULL;
                id = 0xBADBAD;
    }

    ~ShaderProgramNameMap()
    {
    }

    void release()
    {
        if(type == SHADER)
        {
            //Plat::printf("\n..Deleting Shader %d ..", id);
            delete shaderPtr;
            shaderPtr = 0;
            //Plat::printf("Done\n");
        }
        else if(type == PROGRAM)
        {
            //Plat::printf("\n..Deleting Program %d ..", id);
            delete programPtr;
            programPtr = 0;
            //Plat::printf("Done\n");
        }
        id = 0;
    }
private:
//    ShaderProgramNameMap();   //liyue 090316
};

class ShaderProgNameList : public ustl::vector<ShaderProgramNameMap>
{
public:


    ShaderProgNameList::const_iterator find(GLuint id) const;

        
    void remove(GLuint id);

    //! This function will only return an unused id and will not mark it as
    //! used. An id I is treated as used only if a ShaderProgramNameMap element
    //! is added to this object with its member id set to I.
        
    GLuint  getUnusedName() const;
                
        
        void cleanup();

        
    void freeUnused();
        
    ~ShaderProgNameList();
};

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

//-----Util Functions-----

GLenum          translateDclType(DclDataType dclType);

const char* getGLTypeString(GLenum type);

int                     getNumComponents(GLenum type);

unsigned int getNumSlots(GLenum type);

bool            isSampler(GLenum type);

bool        isVector(GLenum type);

bool        isMatrix(GLenum type);

GLenum          getBaseType(GLenum type);

ShaderType      shaderHeaderType(const void* bin);

struct OGLState;

bool updateCurrentExecutable();
int  InitShaderState(OGLState* ctx);
int  DeInitShaderState(OGLState* ctx);


#endif /*__GL_SHADER_H__*/

