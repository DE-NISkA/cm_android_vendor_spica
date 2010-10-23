#ifndef _TexColCFrag_h_
#define _TexColCFrag_h_


// Header generated from binary by WriteAsBinHeader()..
static const int TexColCFragLength = 116;
static const unsigned int TexColCFrag[TexColCFragLength]={
	0x20205350,	0xFFFF0008,	0x00000048,	0x01020000,	0x0000000B,	0x00000004,	0x00000000,	0x00000000,	0x00000002,	0x00000000,
	0x00000002,	0x00000001,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000031,	0x00000000,	0x18000000,	0x0718FF07,
	0x230820FF,	0x00000000,	0x08FF0708,	0x0718FF07,	0x0EF82105,	0x00000000,	0x02000000,	0x01014402,	0x037821E4,	0x00000000,
	0x00000000,	0x81010000,	0x114021E4,	0x00000000,	0x00000000,	0x01010001,	0x034021FF,	0x00000000,	0x00000000,	0x01010000,
	0x074021FF,	0x00000000,	0x00000000,	0x00000000,	0x00B821A4,	0x00000000,	0x00000000,	0x0101E407,	0x107821E4,	0x00000000,
	0x01000000,	0x0101E400,	0x037821E4,	0x00000000,	0x03000000,	0x0101E402,	0x037A10E4,	0x00000000,	0x00000000,	0x00000000,
	0x1E000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x3F800000,	0x3F800000,	0x3F800000,	0x3F800000,
	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,	0x0000001C,	0x00000009,
	0x00000009,	0x00000000,	0x00000000,	0x00000026,	0x0000000A,	0x00000009,	0x00000000,	0x00000004,	0x00000000,	0x00000007,
	0x00000003,	0x00020001,	0x00000008,	0x00000013,	0x00000008,	0x00000009,	0x00020001,	0x0000000C,	0x00000008,	0x0000000A,
	0x0000000F,	0x00030005,	0x00000000,	0x44786554,	0x00306D69,	0x49786554,	0x6567616D,	0x54003073,	0x6F437865,	0x00726F6C,
	0x43786554,	0x64726F6F,	0x72460030,	0x43746E6F,	0x726F6C6F,	0x00000000,};

//checksum generated by simpleCheckSum()
static const unsigned int TexColCFragCheckSum = 120;

static const char* TexColCFragText = 
	"#ifdef GL_ES\n"
	"precision highp float;\n"
	"#endif\n"
	"\n"
	"uniform sampler2D TexImages0;\n"
	"uniform vec4 TexColor;\n"
	"\n"
	"varying vec4 TexCoord0;\n"
	"varying vec4 FrontColor;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    gl_FragColor = texture2D(TexImages0, TexCoord0.xy) * FrontColor * TexColor;\n"
	"}";

#ifdef GL_HELPERS_INCLUDED
//glHelpers.h must be included BEFORE any of the shader header files. Also make sure you have the latest version of glHelpers.h
static ghShader TexColCFragShader(TexColCFragText, TexColCFrag, TexColCFragLength, TexColCFragCheckSum);


#endif


#endif //_TexColCFrag_h_
