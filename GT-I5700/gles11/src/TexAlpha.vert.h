#ifndef _TexAlphaVert_h_
#define _TexAlphaVert_h_


// Header generated from binary by WriteAsBinHeader()..
static const int TexAlphaVertLength = 129;
static const unsigned int TexAlphaVert[TexAlphaVertLength]={
	0x20205356,	0xFFFF0008,	0x00000048,	0x01020000,	0x00000007,	0x00000006,	0x00000000,	0x00000000,	0x00000003,	0x00000003,
	0x00000001,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x0000005F,	0x00000000,	0x00000000,	0x02020000,
	0x237820E4,	0x00000000,	0x00E40100,	0x02035500,	0x2EF820E4,	0x00000000,	0x00E40100,	0x0204AA00,	0x2EF820E4,	0x00000000,
	0x00E40100,	0x0205FF00,	0x0EF800E4,	0x00000000,	0x00000000,	0x00010000,	0x00F801E4,	0x00000000,	0x00000000,	0x00020000,
	0x00F802E4,	0x00000000,	0x00000000,	0x00000000,	0x1E000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x3F800000,	0x3F800000,	0x3F800000,	0x3F800000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x0000000C,	0x00000006,	0x00000009,	0x00000000,	0x00000000,	0x00000013,	0x0000000E,	0x00000009,	0x00000000,	0x00000004,
	0x00000022,	0x0000000E,	0x00000009,	0x00000000,	0x00000008,	0x00000000,	0x0000000B,	0x00000009,	0x00010004,	0x00000000,
	0x0000004B,	0x00000009,	0x00000009,	0x00010004,	0x00000004,	0x00000055,	0x00000009,	0x00000009,	0x00010004,	0x00000008,
	0x00000031,	0x00000019,	0x0000000E,	0x00020001,	0x00000008,	0x505F6C67,	0x7469736F,	0x006E6F69,	0x74726556,	0x4D007865,
	0x69746C75,	0x43786554,	0x64726F6F,	0x754D0030,	0x5469746C,	0x6F437865,	0x3164726F,	0x646F4D00,	0x69566C65,	0x72507765,
	0x63656A6F,	0x6E6F6974,	0x7274614D,	0x54007869,	0x6F437865,	0x3064726F,	0x78655400,	0x726F6F43,	0x00003164,};

//checksum generated by simpleCheckSum()
static const unsigned int TexAlphaVertCheckSum = 72;

static const char* TexAlphaVertText = 
	"#ifdef GL_ES\n"
	"precision highp float;\n"
	"#endif\n"
	"\n"
	"attribute vec4 Vertex;\n"
	"attribute vec4 MultiTexCoord0;\n"
	"attribute vec4 MultiTexCoord1;\n"
	"\n"
	"uniform mat4 ModelViewProjectionMatrix;\n"
	"\n"
	"varying vec4 TexCoord0;\n"
	"varying vec4 TexCoord1;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    gl_Position = ModelViewProjectionMatrix * Vertex;\n"
	"    TexCoord0 = MultiTexCoord0;\n"
	"    TexCoord1 = MultiTexCoord1;\n"
	"}\n"
	"\n"
	"";

#ifdef GL_HELPERS_INCLUDED
//glHelpers.h must be included BEFORE any of the shader header files. Also make sure you have the latest version of glHelpers.h
static ghShader TexAlphaVertShader(TexAlphaVertText, TexAlphaVert, TexAlphaVertLength, TexAlphaVertCheckSum);


#endif


#endif //_TexAlphaVert_h_
