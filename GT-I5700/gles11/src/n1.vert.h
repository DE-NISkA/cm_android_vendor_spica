#ifndef _n1Vert_h_
#define _n1Vert_h_


// Header generated from binary by WriteAsBinHeader()..
static const int n1VertLength = 109;
static const unsigned int n1Vert[n1VertLength]={
	0x20205356,	0xFFFF0008,	0x00000048,	0x01020000,	0x00000006,	0x00000006,	0x00000000,	0x00000000,	0x00000002,	0x00000002,
	0x00000001,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000046,	0x00000000,	0x00000000,	0x02020000,
	0x237820E4,	0x00000000,	0x00E40100,	0x02035500,	0x2EF820E4,	0x00000000,	0x00E40100,	0x0204AA00,	0x2EF820E4,	0x00000000,
	0x00E40100,	0x0205FF00,	0x0EF800E4,	0x00000000,	0x00000000,	0x00010000,	0x00F801E4,	0x00000000,	0x00000000,	0x00000000,
	0x1E000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x3F800000,	0x3F800000,	0x3F800000,	0x3F800000,
	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x0000000C,	0x00000006,	0x00000009,	0x00000000,
	0x00000000,	0x00000013,	0x0000000E,	0x00000009,	0x00000000,	0x00000004,	0x00000000,	0x0000000B,	0x00000009,	0x00010004,
	0x00000000,	0x0000003C,	0x00000009,	0x00000009,	0x00010004,	0x00000004,	0x00000022,	0x00000019,	0x0000000E,	0x00020001,
	0x00000008,	0x505F6C67,	0x7469736F,	0x006E6F69,	0x74726556,	0x4D007865,	0x69746C75,	0x43786554,	0x64726F6F,	0x6F4D0030,
	0x566C6564,	0x50776569,	0x656A6F72,	0x6F697463,	0x74614D6E,	0x00786972,	0x43786554,	0x64726F6F,	0x00000030,};

//checksum generated by simpleCheckSum()
static const unsigned int n1VertCheckSum = 20;

static const char* n1VertText = 
	"#ifdef GL_ES\n"
	"precision highp float;\n"
	"#endif\n"
	"\n"
	"attribute vec4 Vertex;\n"
	"attribute vec4 MultiTexCoord0;\n"
	"\n"
	"uniform mat4 ModelViewProjectionMatrix;\n"
	"\n"
	"varying vec4 TexCoord0;\n"
	"invariant gl_Position;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    gl_Position = ModelViewProjectionMatrix * Vertex;\n"
	"    TexCoord0 = MultiTexCoord0;\n"
	"}\n"
	"\n"
	"";

#ifdef GL_HELPERS_INCLUDED
//glHelpers.h must be included BEFORE any of the shader header files. Also make sure you have the latest version of glHelpers.h
static ghShader n1VertShader(n1VertText, n1Vert, n1VertLength, n1VertCheckSum);


#endif


#endif //_n1Vert_h_