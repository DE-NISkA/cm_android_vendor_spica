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
* \file   pixel.cpp
* \author Sandeep Kakarlapudi (s.kakarla@samsung.com)
* \brief  Pixel Conversion functions & encoding/decoding function definitions
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
* 03.04.2007  Sandeep Kakarlapudi   Initial Version
*
*   20.06.2007  Sandeep kakarlapudi     Bug-Fix for color expansion
*
*******************************************************************************
*/

#include "pixel.h"
#include "platform.h"

//#include <vector>
#include "ustl.h"

enum { R, G, B, A};

#define TYPE2B enum{numBytes=2}; typedef unsigned short PixType
#define TYPE4B enum{numBytes=4}; typedef unsigned int PixType
//The following types are typically not used!
#define TYPE1B enum{numBytes=1}; typedef unsigned char PixType
#define TYPE3B enum{numBytes=3}; typedef unsigned int PixType


//Debug functions
void printBits(unsigned int a)
{
    //LOGMSG("%x ",a);
    ustl::string s;

    do
    {
        s.insert(0,(a&1)?"1":"");
    a>>=1;
    }while(a);

    LOGMSG("%s ",s.c_str());
}

void printPix(int a,int b,int c,int d, unsigned int val)
{
    unsigned int x=val&((1<<a)-1);
    val>>=a;
    unsigned int y = val&(( 1<<b)-1);
    val>>=b;
    unsigned int z = val&(( 1<<c)-1);
    val>>=c;
    unsigned int w = val&(( 1<<d)-1);
    val>>=d;

    LOGMSG("[ ");
    printBits(w);
    printBits(z);
    printBits(y);
    printBits(x);
    LOGMSG("]\n");
}

#define DEFPRINT static void print(PixType s) { printPix(Bx,By,Bz,Bw,s); }

/* REFERENCE:
[2:0] Mode used in Frame Buffer Color
000b = 555, RGB, 16-bit (top bit written as register alpha[7])
001b = 565, RGB, 16-bit
010b = 4444, RGB, 16-bit
011b = 1555, ARGB, 16-bit
100b = 0888, RGB, 32-bit(top byte written as register alpha)
101b = 8888, ARGB, 32-bit
110 ~ 111b = reserved
*/

#define FMT(f) enum { Fmt = (f) }
#define ORDER(a,b,c,d) enum { X=(d), Y=(c), Z=(b), W=(a) }
#define BITS(a,b,c,d) enum {Bx=(d), By=(c), Bz=(b), Bw=(a) }

//ORDER, First component is writen at LSBs
// X,Y,Z,W is the order in which bits are "popped off" the LSBS are the top of the "stack"
// => X corresponds to the LSBs, W corresponds to MSBs

struct PxARGB8 { FMT(E_ARGB8); ORDER(A,R,G,B); BITS(8,8,8,8); TYPE4B; DEFPRINT };
struct PxARGB0888 { FMT(E_ARGB0888); ORDER(A,R,G,B); BITS(0,8,8,8); TYPE4B; DEFPRINT };
struct PxRGB8  { FMT(E_RGB8); ORDER(A,R,G,B); BITS(0,8,8,8); TYPE3B; DEFPRINT };
struct PxARGB4 { FMT(E_ARGB4); ORDER(A,R,G,B); BITS(4,4,4,4); TYPE2B; DEFPRINT };
struct PxRGB565{ FMT(E_RGB565); ORDER(A,R,G,B); BITS(0,5,6,5); TYPE2B; DEFPRINT };
struct PxRGB5  { FMT(E_RGB5); ORDER(A,R,G,B); BITS(0,5,5,5); TYPE2B; DEFPRINT };
struct PxARGB1555 { FMT(E_ARGB1555); ORDER(A,R,G,B); BITS(1,5,5,5); TYPE2B; DEFPRINT };

struct PxLUMINANCE8 { FMT(E_LUMINANCE8); ORDER(A,R,G,B); BITS(0,8,0,0); TYPE1B; DEFPRINT };
struct PxLUMINANCE_ALPHA88 { FMT(E_LUMINANCE_ALPHA88); ORDER(A,R,G,B); BITS(8,8,0,0); TYPE2B; DEFPRINT };
struct PxLUMINANCE_ALPHA80 { FMT(E_LUMINANCE_ALPHA80); ORDER(A,R,G,B); BITS(0,8,0,0); TYPE2B; DEFPRINT };
struct PxLUMINANCE_ALPHA08 { FMT(E_LUMINANCE_ALPHA08); ORDER(A,R,G,B); BITS(8,0,0,0); TYPE2B; DEFPRINT };

struct PxABGR8 {FMT(E_ABGR8); ORDER(A,B,G,R); BITS(8,8,8,8); TYPE4B; DEFPRINT };
struct PxBGR8{FMT(E_BGR8); ORDER(A,B,G,R); BITS(0,8,8,8); TYPE3B; DEFPRINT};
struct PxRGBA4{FMT(E_RGBA4); ORDER(R,G,B,A); BITS(4,4,4,4); TYPE2B; DEFPRINT};
struct PxRGBA5551{FMT(E_RGBA5551); ORDER(R,G,B,A); BITS(5,5,5,1); TYPE2B; DEFPRINT};
struct PxALPHA_LIMUNANCE88 { FMT(E_ALPHA_LUMINANCE88); ORDER(A,R,G,B); BITS(8,8,0,0); TYPE2B; DEFPRINT };
struct PxALPHA8 { FMT(E_ALPHA8); ORDER(A,R,G,B); BITS(8,0,0,0); TYPE1B; DEFPRINT };
// RGB565 & ARGB8  ALREADY DFINED
/*
*******************************************************************************
* Component level encoder / decoder
*******************************************************************************
*/

//Scaffolding to help with function specialization

template <typename S, unsigned char component, unsigned char bits>
struct _pixPopHelper   //Handles bits in [4,8]
{
    static inline void popBits(typename S::PixType& s, PxRGBAData& d)
    {
        //This implemetation gives accurate results for "bits" in range [4,8]


        const unsigned char srcVal = (s&((1<<bits)-1));
        d.c[component] = ( srcVal<<(8-bits) ) | (srcVal>>(2*bits-8));

        //Old implementation:
        //                       MASK           SHIFT UP       set LSBs to 1
        //d.c[component] =  ( (s&((1<<bits)-1))<< (8-bits) ) |  ( ((1<<(8-bits))-1) );

        s >>= bits;

        //LOGMSG("d.[%d] = %x \n",component,d.val );
        //LOGMSG("s = %x \n",s );
    }
};

template <typename S, unsigned char component>
struct _pixPopHelper<S,component,0> //Handles bits = 0
{
    static inline void popBits(typename S::PixType& s, PxRGBAData& d)
    {
        d.c[component] = 0xFF;
        //s>>= bits;
    }
};

template <typename S, unsigned char component>
struct _pixPopHelper<S,component,1> //Handles bits = 1
{
    static inline void popBits(typename S::PixType& s, PxRGBAData& d)
    {
        d.c[component] = (s&1)? 0xFF : 0;
        s>>= 1;//bits;
    }
};

template <typename S, unsigned char component>
struct _pixPopHelper<S,component,2> //Handles bits = 2
{
    static inline void popBits(typename S::PixType& s, PxRGBAData& d)
    {
        unsigned char tmp = s&0x3;
        tmp |= (tmp<<2);
        d.c[component] = (tmp<<4)|tmp;
        s>>= 2;
    }
};

template <typename S, unsigned char component>
struct _pixPopHelper<S,component,3> //Handles bits = 3
{
    static inline void popBits(typename S::PixType& s, PxRGBAData& d)
    {
        unsigned char tmp = s&0x7;
        d.c[component] = (tmp<<5)|(tmp<<2)|(tmp>>1);
        s>>= 3;
    }
};

template <unsigned char component, typename D, unsigned char bits>
struct _pixPushHelper
{
    static inline void pushBits(typename D::PixType& d, const PxRGBAData& s)
      {
             d<<=bits;
        d|= s.c[component]>>(8-bits);
      }
};


template <unsigned char component>
struct _pixPushHelper<component,PxARGB0888,0>
{
    static inline void pushBits(PxARGB0888::PixType& d, const PxRGBAData& s)
      {
    //d<<=8; Not required since A is the first component!
        d |= 0xFF;

      }
};
template <>
//When copying to PxLUMINANCE_ALPHA80, we make alpha as 0xff to aid hardware and to conform to GL.
struct _pixPushHelper<A,PxLUMINANCE_ALPHA80,0>
{
    static inline void pushBits(PxLUMINANCE_ALPHA80::PixType& d, const PxRGBAData& s)
      {
    //d<<=8; Not required since A is the first component!
        d |= 0xFF;

      }
};

template <>
//When copying to PxLUMINANCE_ALPHA80, we make alpha as 0xff to aid hardware and to conform to GL.
struct _pixPushHelper<R,PxLUMINANCE_ALPHA08,0>
{
    static inline void pushBits(PxLUMINANCE_ALPHA08::PixType& d, const PxRGBAData& s)
      {
    d<<=8; //Not required since A is the first component!
                d |= 0xFF;  //to test

      }
};
/*
*******************************************************************************
*  Main encode/decode functions
*******************************************************************************
*/
template <typename D, unsigned char component, unsigned char bits>
inline void pushBits(typename D::PixType& d, const PxRGBAData& s)
{
 //   d<<=bits;
 //   d|= s.c[component]>>(8-bits);

  _pixPushHelper<component,D,bits>::pushBits(d,s);

}

template <typename S, unsigned char component, unsigned char bits>
inline void popBits(typename S::PixType& s, PxRGBAData& d)
{
    _pixPopHelper<S,component,bits>::popBits(s,d);
}

/*
*******************************************************************************
* Pixel converter templates
*******************************************************************************
*/

template <typename D>
inline typename D::PixType
convertFromRGBAData(PxRGBAData s)
{
    typename D::PixType d=0;
    pushBits<D, D::W, D::Bw>(d, s);
    pushBits<D, D::Z, D::Bz>(d, s);
    pushBits<D, D::Y, D::By>(d, s);
    pushBits<D, D::X, D::Bx>(d, s);
    return d;
}

template <typename S>
inline PxRGBAData
convertToRGBAData(typename S::PixType s)
{
    PxRGBAData d;
    popBits<S, S::X, S::Bx>(s, d);
    popBits<S, S::Y, S::By>(s, d);
    popBits<S, S::Z, S::Bz>(s, d);
    popBits<S, S::W, S::Bw>(s, d);

    return d;
}
template<typename S>
inline void genMipMapCore( void* pIn, void* pOut,GLsizei widthOut, GLsizei heightOut,GLsizei widthIn, GLsizei heightIn)
{

  typename S::PixType* srcTmp = (typename S::PixType*)pIn;
  typename S::PixType* dstTmp = (typename S::PixType*)pOut;

  typename S::PixType srcT ;

  PxRGBAData d1,d2,d3,d4 ;

  if(heightIn == 1)
    widthIn =0;
  int flag = 0;
  if(widthIn == 1)
    flag = 2;
  //handle win ==1 AND hin ==1 case
  for (int y = 0; y < heightOut ; ++y)
      for(int x =0; x< widthOut; ++x)
      {
        srcT = srcTmp[2*x  + 2*y * widthIn];
        popBits<S, S::X, S::Bx>(srcT, d1);
        popBits<S, S::Y, S::By>(srcT, d1);
        popBits<S, S::Z, S::Bz>(srcT, d1);
        popBits<S, S::W, S::Bw>(srcT, d1);

        srcT = srcTmp[ 2*x+1  + 2*y * widthIn];
        popBits<S, S::X, S::Bx>(srcT, d2);
        popBits<S, S::Y, S::By>(srcT, d2);
        popBits<S, S::Z, S::Bz>(srcT, d2);
        popBits<S, S::W, S::Bw>(srcT, d2);

        srcT = srcTmp[ 2*x  + (2*y + 1) * widthIn];
        popBits<S, S::X, S::Bx>(srcT, d3);
        popBits<S, S::Y, S::By>(srcT, d3);
        popBits<S, S::Z, S::Bz>(srcT, d3);
        popBits<S, S::W, S::Bw>(srcT, d3);

        srcT = srcTmp[ (2*x+1)  + (2*y +1) * widthIn - flag];
        popBits<S, S::X, S::Bx>(srcT, d4);
        popBits<S, S::Y, S::By>(srcT, d4);
        popBits<S, S::Z, S::Bz>(srcT, d4);
        popBits<S, S::W, S::Bw>(srcT, d4);

        d1.c[0] =  (d1.c[0] + d2.c[0] + d3.c[0] + d4.c[0])>>2;
        d1.c[1] =  (d1.c[1] + d2.c[1] + d3.c[1] + d4.c[1])>>2;
        d1.c[2] =  (d1.c[2] + d2.c[2] + d3.c[2] + d4.c[2])>>2;
        d1.c[3] =  (d1.c[3] + d2.c[3] + d3.c[3] + d4.c[3])>>2;

        pushBits<S, S::W, S::Bw>(dstTmp[x + y* widthOut], d1);
        pushBits<S, S::Z, S::Bz>(dstTmp[x + y* widthOut], d1);
        pushBits<S, S::Y, S::By>(dstTmp[x + y* widthOut], d1);
        pushBits<S, S::X, S::Bx>(dstTmp[x + y* widthOut], d1);

      }
}

void genMipMaps(PxFmt fmt, void* pIn, void* pOut, GLsizei widthOut, GLsizei heightOut, GLsizei widthIn, GLsizei heightIn)
{

    switch(fmt)
    {
    case E_ARGB8:   return genMipMapCore<PxARGB8>( pIn,  pOut, widthOut,  heightOut, widthIn, heightIn);
    case E_ARGB0888:   return genMipMapCore<PxARGB0888>( pIn,  pOut, widthOut,  heightOut, widthIn, heightIn);
    case E_RGB8:    return genMipMapCore<PxRGB8>(pIn,  pOut,  widthOut,  heightOut, widthIn, heightIn);
    case E_ARGB4:   return genMipMapCore<PxARGB4>(pIn,  pOut, widthOut,  heightOut, widthIn, heightIn);
    case E_RGB565:  return genMipMapCore<PxRGB565>(pIn,  pOut,widthOut,  heightOut, widthIn, heightIn);
    case E_RGB5:    return genMipMapCore<PxRGB5>(pIn,  pOut,widthOut,  heightOut, widthIn, heightIn);
    case E_ARGB1555:    return genMipMapCore<PxARGB1555>(pIn,  pOut,widthOut,  heightOut, widthIn, heightIn);
    case E_LUMINANCE8:    return genMipMapCore<PxLUMINANCE8>(pIn,  pOut,widthOut,  heightOut, widthIn, heightIn);
    case E_LUMINANCE_ALPHA88:    return genMipMapCore<PxLUMINANCE_ALPHA88>(pIn,  pOut,widthOut,  heightOut, widthIn, heightIn);
    case E_LUMINANCE_ALPHA80:    return genMipMapCore<PxLUMINANCE_ALPHA80>(pIn,  pOut,widthOut,  heightOut, widthIn, heightIn);
    case E_LUMINANCE_ALPHA08:    return genMipMapCore<PxLUMINANCE_ALPHA08>(pIn,  pOut,widthOut,  heightOut, widthIn, heightIn);

  //added for new format
  case E_ABGR8:   return genMipMapCore<PxABGR8>( pIn,  pOut, widthOut,  heightOut, widthIn, heightIn);
  case E_BGR8:    return genMipMapCore<PxBGR8>(pIn,  pOut,  widthOut,  heightOut, widthIn, heightIn);
  case E_RGBA4:   return genMipMapCore<PxRGBA4>(pIn,  pOut, widthOut,  heightOut, widthIn, heightIn);
  case E_RGBA5551:    return genMipMapCore<PxRGBA5551>(pIn,  pOut,widthOut,  heightOut, widthIn, heightIn);
  case E_ALPHA_LUMINANCE88:    return genMipMapCore<PxALPHA_LIMUNANCE88>(pIn,  pOut,widthOut,  heightOut, widthIn, heightIn);
  case E_ALPHA8:   return genMipMapCore<PxALPHA8>( pIn,  pOut, widthOut,  heightOut, widthIn, heightIn);

//  default:
        LOGMSG("ERROR: unknown/unsupported destination pixel format: %d\n",fmt);
    }

}

/*
*******************************************************************************
* Main Converters
*******************************************************************************
*/

unsigned int convertFromRGBA(PxFmt dFmt, float rval, float gval, float bval, float aval)
{
    PxRGBAData s;
    //gAssert((0.0<=rval)&&(rval<=1.0));
    s.c[R] = (unsigned char)(rval*255.0f);
    s.c[G] = (unsigned char)(gval*255.0f);
    s.c[B] = (unsigned char)(bval*255.0f);
    s.c[A] = (unsigned char)(aval*255.0f);
    return convertFromRGBA(dFmt, s);

}

unsigned int convertFromRGBA(PxFmt dFmt, unsigned char rval, unsigned char gval, unsigned char bval, unsigned char aval)
{
    PxRGBAData s;
    s.c[R] = rval;
    s.c[G] = gval;
    s.c[B] = bval;
    s.c[A] = aval;
    return convertFromRGBA(dFmt, s);
}

unsigned int convertFromRGBA(PxFmt dFmt, PxRGBAData s)
{
    switch(dFmt)
    {
    case E_ARGB8:   return convertFromRGBAData<PxARGB8>(s);
    case E_ARGB0888:   return convertFromRGBAData<PxARGB0888>(s);
    case E_RGB8:    return convertFromRGBAData<PxRGB8>(s);
    case E_ARGB4:   return convertFromRGBAData<PxARGB4>(s);
    case E_RGB565:  return convertFromRGBAData<PxRGB565>(s);
    case E_RGB5:    return convertFromRGBAData<PxRGB5>(s);
    case E_ARGB1555:    return convertFromRGBAData<PxARGB1555>(s);

  //added for new format
  case E_ABGR8:   return convertFromRGBAData<PxABGR8>(s);
    case E_BGR8:    return convertFromRGBAData<PxBGR8>(s);
    case E_RGBA4:   return convertFromRGBAData<PxRGBA4>(s);
    case E_RGBA5551:  return convertFromRGBAData<PxRGBA5551>(s);

//  default:
        LOGMSG("ERROR: unknown/unsupported destination pixel format: %d\n",dFmt);
    }

    return 0;
}

template<typename D, typename S>
inline void
convertPixelsCore(void* dstPixels, GLsizei dstWidth, GLsizei dstHeight, GLsizei dstDepth,
      GLuint dstX, GLuint dstY, GLuint dstZ,
      const void* srcPixels, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth,
      GLuint srcX, GLuint srcY, GLuint srcZ,
      GLsizei cpyWidth, GLsizei cpyHeight, GLsizei cpyDepth , GLint dstPad, GLint srcPad, GLint dir)
{

  GLubyte *src = NULL, *dst = NULL;
  typename S::PixType srcTmp ;
  typename D::PixType dstTmp = 0;
  //GLuint pixelSize = 0;
  int i = 0, j = 0, k = 0;

  // Special consideration for 2D where 0 depth is passed
  if(cpyDepth == 0) {
    cpyDepth = 1;
  }

  GLuint dstPixSize ,srcPixSize;
  srcPixSize = S::numBytes;
  dstPixSize = D::numBytes;

  //src = ((GLubyte*)srcPixels) + srcZ * srcWidth * srcHeight * srcPixSize + srcY * srcWidth * srcPixSize + srcX * srcPixSize;
  src = ((GLubyte*)srcPixels) + srcZ * srcWidth * srcHeight * srcPixSize + ( ( (1-dir)>>1)*(srcHeight-1) + (dir*srcY) ) * srcWidth * srcPixSize + srcX * srcPixSize;
  dst = ((GLubyte*)dstPixels) + dstZ * dstWidth * dstHeight * dstPixSize + dstY * dstWidth * dstPixSize + dstX * dstPixSize;


  PxRGBAData dTmp;
  for(k = 0; k < cpyDepth; k++) {
    for(j = 0; j < cpyHeight; j++) {
      for(i = 0; i < cpyWidth; i++){
        //typename S::PixType& srcTmp  = *( (typename S::PixType*)src );
        //typename D::PixType& dstTmp  = *( (typename D::PixType*)dst );
        //srcTmp = 0;
        dstTmp = 0;
        Plat::memcpy( &srcTmp,src,srcPixSize);
        //dstTmp  = (typename D::PixType*)dst;

        popBits<S, S::X, S::Bx>(srcTmp, dTmp);
        popBits<S, S::Y, S::By>(srcTmp, dTmp);
        popBits<S, S::Z, S::Bz>(srcTmp, dTmp);
        popBits<S, S::W, S::Bw>(srcTmp, dTmp);

        pushBits<D, D::W, D::Bw>(dstTmp, dTmp);
        pushBits<D, D::Z, D::Bz>(dstTmp, dTmp);
        pushBits<D, D::Y, D::By>(dstTmp, dTmp);
        pushBits<D, D::X, D::Bx>(dstTmp, dTmp);

        Plat::memcpy( dst,&dstTmp,dstPixSize);

        dst += dstPixSize;
        src += srcPixSize;

      }

      src =src + dir * ( ( srcWidth - (dir*cpyWidth) ) * srcPixSize + srcPad );
      dst += (dstWidth - cpyWidth) * dstPixSize + dstPad;
    }

    src += ((srcHeight - cpyHeight) * (srcWidth + srcPad) ) * srcPixSize;
    dst += ((dstHeight - cpyHeight) * (dstWidth + dstPad) ) * dstPixSize;
  }
}

template<typename S>
inline void
convertPixelsFrom(  PxFmt dstFmt, void* dstPixels, GLsizei dstWidth, GLsizei dstHeight, GLsizei dstDepth,
            GLuint dstX, GLuint dstY, GLuint dstZ,
          const void* srcPixels, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth,
          GLuint srcX, GLuint srcY, GLuint srcZ,
          GLsizei cpyWidth, GLsizei cpyHeight, GLsizei cpyDepth , GLint dstPad, GLint srcPad, GLint dir)
{
  switch(dstFmt)
  {
    case E_ARGB8:
      convertPixelsCore<PxARGB8,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;
    case E_ARGB0888:
      convertPixelsCore<PxARGB0888,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;
    case E_RGB8:
      convertPixelsCore<PxRGB8,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;
      case E_ARGB4:
      convertPixelsCore<PxARGB4,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;
      case E_RGB565:
      convertPixelsCore<PxRGB565,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;

    case E_RGB5:
      convertPixelsCore<PxRGB5,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;

    case E_ARGB1555:
      convertPixelsCore<PxARGB1555,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;
    case E_LUMINANCE8:
      convertPixelsCore<PxLUMINANCE8,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;
    case E_LUMINANCE_ALPHA88:
      convertPixelsCore<PxLUMINANCE_ALPHA88,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;
    case E_LUMINANCE_ALPHA80:
      convertPixelsCore<PxLUMINANCE_ALPHA80,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;
    case E_LUMINANCE_ALPHA08:
      convertPixelsCore<PxLUMINANCE_ALPHA08,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;
    // added shariq
    case E_ABGR8:
      convertPixelsCore<PxABGR8,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;

      case E_BGR8:
      convertPixelsCore<PxBGR8,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;


    case E_RGBA4:
      convertPixelsCore<PxRGBA4,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;

    case E_RGBA5551:
      convertPixelsCore<PxRGBA5551,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;
    case E_ALPHA_LUMINANCE88:
      convertPixelsCore<PxALPHA_LIMUNANCE88,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;

    case E_ALPHA8:
      convertPixelsCore<PxALPHA8,S>(dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad, dir);
      break;
//    default:
      LOGMSG("ERROR: unknown/unsupported destination pixel format: %d\n",dstFmt);

  }

}

extern GLuint GetPixSize(GLenum TextureFormat);

static void convertPixels_BGR2ARGB(void* dst, const void* src, GLsizei width, GLsizei height)
{
  GLsizei i;
  unsigned char* pS = (unsigned char*) src;
  unsigned int*  pD = (unsigned int*)  dst;

  for( i=0 ; i<height*width ; i++ )
  {
    *pD = 0xff000000 | (*(pS))<<16 | (*(pS+1))<<8 | (*(pS+2)) ;
    pD++;
    pS+=3;
  }
}

static void convertPixels_ABGR2ARGB(void* dst, const void* src, GLsizei width, GLsizei height)
{
  GLsizei i;
  unsigned char* pS = (unsigned char*) src;
  unsigned int*  pD = (unsigned int*)  dst;

  for( i=0 ; i<height*width ; i++ )
  {
    *pD = (*(pS+3))<<24 | (*(pS))<<16 | (*(pS+1))<<8 | (*(pS+2)) ;
    pD++;
    pS+=4;
  }
}

static void convertPixelsSubTexture_BGR2ARGB(void* dstPixels, const void* srcPixels, GLuint uiXOffset, GLuint uiYOffset, GLsizei SrcWidth, GLsizei SrcHeight, GLsizei DstWidth, GLsizei DstHeight)

{
  GLuint i, j;

  GLint * pDst = (GLint *)dstPixels;
  GLubyte * pSrc = (GLubyte *)srcPixels;
  pDst += DstWidth*uiYOffset + uiXOffset;

  for(i = 0; i < SrcHeight; i++)
  {
    for(j = 0; j < SrcWidth; j++)
    {
      *pDst = 0xff000000 | (*(pSrc))<<16 | (*(pSrc+1))<<8 | (*(pSrc+2)) ;

      pDst++;
      pSrc += 3;
    }

    pDst += DstWidth - SrcWidth;
  }
}

static void convertPixelsSubTexture_ABGR2ARGB(void* dstPixels, const void* srcPixels, GLuint uiXOffset, GLuint uiYOffset, GLsizei SrcWidth, GLsizei SrcHeight, GLsizei DstWidth, GLsizei DstHeight)
{
  GLuint i, j;

  GLint * pDst = (GLint *)dstPixels;
  GLubyte * pSrc = (GLubyte *)srcPixels;
  pDst += DstWidth*uiYOffset + uiXOffset;

  for(i = 0; i < SrcHeight; i++)
  {
    for(j = 0; j < SrcWidth; j++)
    {
      *pDst = (*(pSrc+3))<<24 | (*(pSrc))<<16 | (*(pSrc+1))<<8 | (*(pSrc+2)) ;

      pDst++;
      pSrc += 4;
    }

    pDst += DstWidth - SrcWidth;
  }
}


void convertPixelsTexure( PxFmt dstFmt, void* dstPixels, GLsizei width, GLsizei height,
          PxFmt srcFmt,const void* srcPixels)
{
  if( dstFmt==srcFmt )
  {
    Plat::memcpy(dstPixels,srcPixels,width*height * GetPixSize(dstFmt) ); // imbumoh 20090105
  }
  else if( dstFmt==E_ARGB8 && (srcFmt==E_BGR8 || srcFmt==E_ABGR8 ) )
  {
    if( srcFmt==E_BGR8 )
      convertPixels_BGR2ARGB(dstPixels,srcPixels,width,height);
    else
      //_gl_asm_convertPixels_ABGR2ARGB(dstPixels,srcPixels,width*height);
      convertPixels_ABGR2ARGB(dstPixels,srcPixels,width,height);
  }
  else
  {
    convertPixels(dstFmt, dstPixels, width, height, 0,0,0,0,
      srcFmt, srcPixels, width, height, 0,0,0,0, width, height, 0,0,0,0);
  }
}

void convertPixels( PxFmt dstFmt, void* dstPixels, GLsizei dstWidth, GLsizei dstHeight, GLsizei dstDepth,
            GLuint dstX, GLuint dstY, GLuint dstZ,
          PxFmt srcFmt,const void* srcPixels, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth,
          GLuint srcX, GLuint srcY, GLuint srcZ,
          GLsizei cpyWidth, GLsizei cpyHeight, GLsizei cpyDepth , GLint dstPad, GLint srcPad,GLint dir)
{
  switch(srcFmt)
  {
    case E_ARGB8:
      convertPixelsFrom<PxARGB8>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad,dir);
      break;
    case E_ARGB0888:
      convertPixelsFrom<PxARGB0888>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad,dir);
      break;
    case E_RGB8:
      convertPixelsFrom<PxRGB8>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad,dir);
      break;
      case E_ARGB4:
      convertPixelsFrom<PxARGB4>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad,dir);
      break;
      case E_RGB565:
      convertPixelsFrom<PxRGB565>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad,dir);
      break;

    case E_RGB5:
      convertPixelsFrom<PxRGB5>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad,  srcPad,dir);
      break;

    case E_ARGB1555:
      convertPixelsFrom<PxARGB1555>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad, srcPad,dir);
      break;

    case E_LUMINANCE8:
      convertPixelsFrom<PxLUMINANCE8>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad, srcPad,dir);
      break;
    case E_LUMINANCE_ALPHA88:
      convertPixelsFrom<PxLUMINANCE_ALPHA88>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad, srcPad,dir);
      break;
    case E_LUMINANCE_ALPHA80:
      convertPixelsFrom<PxLUMINANCE_ALPHA80>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad, srcPad,dir);
      break;
    case E_LUMINANCE_ALPHA08:
      convertPixelsFrom<PxLUMINANCE_ALPHA08>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad, srcPad,dir);
      break;

    // added shariq
    case E_ABGR8:
      convertPixelsFrom<PxABGR8>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad, srcPad,dir);
      break;

    case E_BGR8:
      convertPixelsFrom<PxBGR8>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad, srcPad,dir);
      break;

    case E_RGBA4:
      convertPixelsFrom<PxRGBA4>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad, srcPad,dir);
      break;

    case E_RGBA5551:
      convertPixelsFrom<PxRGBA5551>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad, srcPad,dir);
      break;
    case E_ALPHA_LUMINANCE88:
      convertPixelsFrom<PxALPHA_LIMUNANCE88>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad, srcPad,dir);
      break;

    case E_ALPHA8:
      convertPixelsFrom<PxALPHA8>(dstFmt,dstPixels,dstWidth,dstHeight,dstDepth,
                    dstX, dstY, dstZ,
                    srcPixels,srcWidth,srcHeight,srcDepth,
                    srcX, srcY, srcZ,
                    cpyWidth,cpyHeight,cpyDepth , dstPad, srcPad,dir);
      break;
    //default:
      LOGMSG("ERROR: unknown/unsupported source pixel format: %d\n",srcFmt);

  }
}

void convertPixelsSubTexture( PxFmt dstFmt, void* dstPixels, GLsizei DstWidth, GLsizei DstHeight,GLuint uiXOffset, GLuint uiYOffset,  PxFmt srcFmt,const void* srcPixels, GLsizei SrcWidth, GLsizei SrcHeight)
{
  GLuint i;
  GLuint uiDstPixSize = pixelSize(dstFmt);
  GLuint uiSrcPixSize = pixelSize(srcFmt);

  GLuint uiDstStride = DstWidth*uiDstPixSize;
  GLuint uiSrcStride = SrcWidth*uiSrcPixSize;

  GLubyte * pDst = (GLubyte *)dstPixels;
  GLubyte * pSrc = (GLubyte *)srcPixels;
  pDst += DstWidth*uiYOffset*uiDstPixSize + uiXOffset*uiDstPixSize;

//  //LOGE("convertPixelsSubTexture");
  //LOGE("=====DstFmt = %d, SrcFmt = %d ====", dstFmt, srcFmt);
  //LOGE("=====DstHeight = %d, SrcHeight = %d ====", DstHeight, SrcHeight);
  //LOGE("=====DstWidth = %d, SrcWidth = %d ====", DstWidth, SrcWidth);

  //LOGE("=====pDst = 0x%x, pSrc = 0x%x===", pDst, pSrc);
  //LOGE("=====uiDstStride = 0x%x, uiSrcStride = 0x%x===", uiDstStride, uiSrcStride);
  if( dstFmt==srcFmt )
  {
    for(i = 0; i < SrcHeight; i++)
    {
      Plat::memcpy(pDst, pSrc, uiSrcStride); // Park Jae Sung 20090202
      pDst += uiDstStride;
      pSrc += uiSrcStride;
    }
  }
  else if( dstFmt==E_ARGB8 && (srcFmt==E_BGR8 || srcFmt==E_ABGR8 ) )
  {
    if( srcFmt==E_BGR8 )
      convertPixelsSubTexture_BGR2ARGB(dstPixels,srcPixels,uiXOffset,uiYOffset,SrcWidth,SrcHeight, DstWidth, DstHeight);
    else
      convertPixelsSubTexture_ABGR2ARGB(dstPixels,srcPixels,uiXOffset,uiYOffset,SrcWidth,SrcHeight, DstWidth, DstHeight);
  }
  else
  {
    convertPixels(dstFmt, dstPixels, DstWidth, DstHeight, 0,uiXOffset,uiYOffset,0, srcFmt, srcPixels, SrcWidth, SrcHeight, 0,0,0,0, SrcWidth, SrcHeight, 0,0,0,0);
  }

}

unsigned int convertFromDepthStencil(PxFmt dFmt, float depth, int stencil)
{

    switch(dFmt)
    {
    case E_Stencil8:
        return (stencil&((1<<8)-1))<<24;
    case E_Depth24:
        return (unsigned int)(depth*(0xFFFFFF));
    case E_Stencil8Depth24:
        return ((stencil&((1<<8)-1))<<24)|(unsigned int)(depth*(0xFFFFFF));
//    default:
        LOGMSG("ERROR: unknown destination pixel format: %d\n",dFmt);
    }

    return 0;
}

unsigned int getDepthStencilMask(PxFmt dFmt, bool depthMask, unsigned char stencilMask)
{
  //New mask computation causes unused components to be written to (faster for cpu clear)
    unsigned int mask = depthMask ? 0x00FFFFFF : 0;
    //mask |= stencilMask? 0xFF000000 : 0;
    mask |= ((unsigned int)(stencilMask) << 24);

    switch(dFmt)
    {
    case E_Stencil8:
        return (stencilMask<<24)|0xFFFFFF;
    case E_Depth24:
    return depthMask? 0xFFFFFFFF : 0x0;
    case E_Stencil8Depth24:
        return mask & 0xFFFFFFFF;
//    default:
        LOGMSG("ERROR: unknown destination pixel format: %d\n",dFmt);
    }

    return 0;
}

unsigned int getColorMask(PxFmt dFmt, bool redMask, bool greenMask, bool blueMask, bool alphaMask)
{
        return convertFromRGBA(dFmt, (unsigned char)(redMask ? 0xFF : 0), (unsigned char)(greenMask ? 0xFF : 0), (unsigned char)(blueMask ? 0xFF : 0), (unsigned char)(alphaMask ? 0xFF : 0));
}

unsigned int pixelSize(PxFmt fmt)
{
    switch(fmt)
    {

    case E_ARGB8:   return PxARGB8::numBytes;
    case E_ARGB0888:   return PxARGB8::numBytes;

    case E_RGB8:    return PxRGB8::numBytes;
    case E_ARGB4:   return PxARGB4::numBytes;
    case E_RGB565:  return PxRGB565::numBytes;
    case E_RGB5:    return PxRGB5::numBytes;
    case E_ARGB1555:  return PxARGB1555::numBytes;

    case E_LUMINANCE8 : return PxLUMINANCE8::numBytes;
    case E_LUMINANCE_ALPHA88 : return PxLUMINANCE_ALPHA88::numBytes;
    case E_LUMINANCE_ALPHA80 : return PxLUMINANCE_ALPHA80::numBytes;
    case E_LUMINANCE_ALPHA08 : return PxLUMINANCE_ALPHA08::numBytes;

    case E_Stencil8:        return 4;
    case E_Depth24:         return 4;
    case E_Stencil8Depth24: return 4;

   //added for new formats
   case E_ABGR8:   return PxABGR8::numBytes;
   case E_BGR8:    return PxBGR8::numBytes;
   case E_RGBA4:   return PxRGBA4::numBytes;
   case E_RGBA5551:  return PxRGBA5551::numBytes;
   case E_ALPHA_LUMINANCE88 : return PxALPHA_LIMUNANCE88::numBytes;
   case E_ALPHA8:return PxALPHA8::numBytes;
 
 //    default:
         LOGMSG("ERROR: unknown destination pixel format: %d\n",fmt);
     }
 
     return 0;
 }
 
 bool isValidPixelFmt(PxFmt fmt)
 {
     return (fmt < E_NUM_PIXEL_FMTS);
 }
 
 bool isValidFBColorPixelFmt(PxFmt fmt)
 {
     switch(fmt)
     {
     case E_ARGB8:
     case E_RGB8:
     case E_ARGB4:
     case E_RGB565:
     case E_RGB5:
     case E_ARGB1555:
     case E_LUMINANCE8 :
     case E_LUMINANCE_ALPHA88:
     case E_LUMINANCE_ALPHA80:
     case E_LUMINANCE_ALPHA08:
         //Intentional fall through
         return true;
 
     //these are compressed formats and input formats
     case E_RGBA4:
     case E_RGBA5551:
     case E_BGR8:
     case E_ABGR8:
     case E_ALPHA8:
   case E_ALPHA_LUMINANCE88:
   case E_PALETTE4_RGB8_OES:
   case E_PALETTE4_RGBA8_OES:
   case E_PALETTE4_R5_G6_B5_OES:
   case E_PALETTE4_RGBA4_OES:
   case E_PALETTE4_RGB5_A1_OES:
   case E_PALETTE8_RGB8_OES:
   case E_PALETTE8_RGBA8_OES:
   case E_PALETTE8_R5_G6_B5_OES:
   case E_PALETTE8_RGBA4_OES:
   case E_PALETTE8_RGB5_A1_OES:
   case E_RGB_S3TC_OES:
   case E_RGBA_S3TC_OES:
       return false;
 
     default:
         return false;
     }
 
     return false;
 }
 
 //TBD
 #if 0
 bool isValidTexPixelFmt(PxFmt fmt)
 {
 }
 #endif
 
 //for framebuffer data
 PxFmt translateGLSizedInternal(GLenum format)
 {
     switch(format)
     {
     case GL_RGBA8: return E_ARGB8;
     case GL_RGB8:  return E_RGB8;
     case GL_RGBA4:  return E_ARGB4;             //added by chanchal::GL_RGBA4 should be color renderable
     case GL_RGB565: return E_RGB565;
     //case GL_RGB5:  return E_RGB5;
     case GL_RGB5_A1: return E_ARGB1555;
   case GL_DEPTH_COMPONENT16:
   case GL_DEPTH_COMPONENT24:
     return E_Depth24;
   case GL_STENCIL_INDEX:    return E_Stencil8;
 
     }
     return E_INVALID_PIXEL_FORMAT;
 }
 
 // the basic format and type combinations
 PxFmt translateGLInternal(GLenum format, GLenum type)
 {
   GLenum temp = GL_RGBA;
     switch(type)
     {
     case GL_UNSIGNED_BYTE: temp = format; break;
     case GL_UNSIGNED_SHORT_4_4_4_4:  return E_ARGB4;
     case GL_UNSIGNED_SHORT_5_6_5: return E_RGB565;
     //case GL_RGB5:  return E_RGB5;
     case GL_UNSIGNED_SHORT_5_5_5_1: return E_ARGB1555;
 
   default:
     LOGMSG("ERROR: unknown destination pixel type: %d\n",type);
     return E_INVALID_PIXEL_FORMAT;
     }
 
   switch(temp)
   {
     case GL_RGB: return E_ARGB0888;
     case GL_RGBA: return E_ARGB8;
     //case GL_LUMINANCE: return E_LUMINANCE8;
     case GL_LUMINANCE: return E_LUMINANCE_ALPHA80; //since hardware seems to return luminance for alpha if E_LUMINANCE8 is used
     case GL_LUMINANCE_ALPHA: return E_LUMINANCE_ALPHA88; //to check
     case GL_ALPHA: return E_LUMINANCE_ALPHA08;  // previously E_ALPHA8
     default:
       LOGMSG("ERROR: unknown destination pixel type: %d\n",type);
     return E_INVALID_PIXEL_FORMAT;
   }
 
   return E_INVALID_PIXEL_FORMAT;
 
 }
 
 //case is the value returned by translateGLSizedInternal function
 //i.e. framebuffer format
 FGL_PixelFormat translateToFglPixelFormat(PxFmt fmt)
 {
   switch(fmt)
     {
     case E_RGB5: return FGL_PIXEL_RGB555;
     case E_RGB565: return FGL_PIXEL_RGB565;
     case E_ARGB4: return FGL_PIXEL_ARGB4444;
     case E_ARGB1555:    return FGL_PIXEL_ARGB1555;
     case E_ARGB0888:    return FGL_PIXEL_ARGB0888;
     case E_ARGB8:   return FGL_PIXEL_ARGB8888;
 
 
     //case E_LUMINANCE8:
     //case E_LUMINANCE_ALPHA88:
     //case E_Stencil8:
     //case E_Depth24:
     //case E_Stencil8Depth24:
 
   //compressed format and formats input formats
     case E_ALPHA_LUMINANCE88:
     case E_RGBA4:
     case E_RGBA5551:
     case E_BGR8:
     case E_ABGR8:
     case E_ALPHA8:
   case E_PALETTE4_RGB8_OES:
   case E_PALETTE4_RGBA8_OES:
   case E_PALETTE4_R5_G6_B5_OES:
   case E_PALETTE4_RGBA4_OES:
   case E_PALETTE4_RGB5_A1_OES:
   case E_PALETTE8_RGB8_OES:
   case E_PALETTE8_RGBA8_OES:
   case E_PALETTE8_R5_G6_B5_OES:
   case E_PALETTE8_RGBA4_OES:
   case E_PALETTE8_RGB5_A1_OES:
   case E_RGB_S3TC_OES :
   case E_RGBA_S3TC_OES:
      return FGL_PixelFormat(255);
 
     default:
         return FGL_PixelFormat(255);
 
     }
     return FGL_PixelFormat(255); //Invalid!
 }
 
 GLenum translateToGLenum(PxFmt fmt)
 {
 
   switch(fmt)
     {
       case E_ARGB8: return  GL_RGBA8;
       case E_ARGB0888: return GL_RGB;
       case E_RGB8:  return GL_RGB8;
 
       case E_RGB565: return GL_RGB565;
 
       case E_ARGB1555 : return GL_RGB5_A1;
     case E_Depth24 : return  GL_DEPTH_COMPONENT16;
     case E_Stencil8: return  GL_STENCIL_INDEX ;
     case E_ARGB4: return GL_RGBA4;
 
     //case GL_RGB5:  return E_RGB5;
     //case GL_RGB4:  return E_ARGB4;
 
         default:
             return 0;
     }
     return 0;
 }
 
 
 //added for determining the format and the type from the GL type of framebuffer
 GLenum determineTypeFormat(int format, int Isformat)
 {
   if(Isformat == GL_TRUE){
 
     switch(format)
       {
       case GL_RGBA8:
       case GL_RGB5_A1:
          return GL_RGBA;
       case GL_RGB8:
        case GL_RGB565:
          return GL_RGB;
       //case GL_ARGB4:
       //return GL_RGBA;
       default:
       LOGMSG("ERROR: unknown FRAMEBUFFER format: %d\n",format);
       return E_INVALID_PIXEL_FORMAT;
     }
   }else if(Isformat == GL_FALSE){
 
       switch(format)
          {
           //GL_RGBA is automatically supported by the glReadPixel;
           // so we have to support some other format, supporting GL_ARGB4
         case GL_RGBA8:
         case GL_RGB8:
            return GL_UNSIGNED_BYTE;
         case GL_RGB5_A1:
            return GL_UNSIGNED_SHORT_5_5_5_1;;
         case GL_RGB565:
            return GL_UNSIGNED_SHORT_5_6_5;
         case GL_RGBA4:                                              //added by chanchal
           return GL_UNSIGNED_SHORT_4_4_4_4;
         default:
         LOGMSG("ERROR: unknown FRAMEBUFFER format: %d\n",format);
         return E_INVALID_PIXEL_FORMAT;
       }
 
     }
   return E_INVALID_PIXEL_FORMAT;
 }
 
 
 int determinePixelBitSize(PxFmt format, colorType colortype)
 {
   switch(format)
     {
       //case GL_RGB4: return 4;
     case E_ARGB8:
       if((colortype == BLUE) || (colortype == GREEN) || (colortype == RED) || (colortype == ALPHA))return 8;
       else break;
     case E_ARGB0888:
       if((colortype == BLUE) || (colortype == GREEN) || (colortype == RED) )return 8;
       else if(colortype == ALPHA) return 0;
       else break;
     //case GL_RGB5:
   //if(colortype == ALPHA )return 0;
       //else return 5;
       case E_RGB565:
         if(colortype == ALPHA) return 0;
       else if(colortype == GREEN) return 6;
       else if((colortype == BLUE) || (colortype == RED))return 5;
       else break;       case E_ARGB1555:
       if(colortype == ALPHA )return 1;
       else if ((colortype == BLUE) || (colortype == GREEN) || (colortype == RED)) return 5;
       else break;
 
       case E_RGB8:
       if(colortype == ALPHA )return 0;
       else if ((colortype == BLUE) || (colortype == GREEN) || (colortype == RED)) return 8;
       else break;
 
     case E_Stencil8:
        if(colortype == STENCIL) return 8;
        else  if(colortype == DEPTH) return 0;
        else break;
 
     case E_Depth24:
       if(colortype == DEPTH) return 24;
       if(colortype == STENCIL)return 0;
       else break;
 
     case E_Stencil8Depth24:
       if(colortype == STENCIL) return 8;
       else if(colortype == DEPTH) return 24;
       else break;
 
     default:
       LOGMSG("ERROR: unknown FRAMEBUFFER format: %d\n",format);
         return 255;
   }
   LOGMSG("\n the format passed and the colortype does not matches ");
   return 0; //to check for this value
 }
 
 /*
 *******************************************************************************
 *                           TESTS
 *******************************************************************************
 */
 
 #if 0
 //Moved here since the rest of the code does not use this.
 static const char* fmtString []={
     "E_ARGB8",
     "E_RGB8",
     "E_ARGB4",
     "E_RGB565",
     "E_RGB5",
     "E_ARGB1555",
     "E_LUMINANCE8",
     "E_LUMINANCE_ALPHA88",
     "E_LUMINANCE_ALPHA80",
     "E_LUMINANCE_ALPHA08",
 
     "E_Stencil8",
     "E_Depth24",
     "E_Stencil8Depth24",
   //added shariq
   "E_ABGR8" ,
   "E_BGR8" ,
   "E_RGBA4",
   "E_RGBA5551 ",
   "E_ALPHA_LUMINANCE88",
 };
 
 
 template <typename D>
 void testConversion(PxRGBAData s)
 {
     typename D::PixType d = convertFromRGBAData<D>(s);
     LOGMSG("%s  \t",fmtString[D::Fmt]);
     D::print(d);
 }
 
 bool testPixConvert()
 {
     PxRGBAData s;
     s.val = 0xF0EFDFCF;
 
     testConversion<PxARGB8>(s);
     LOGMSG("\n\n");
     testConversion<PxRGB8>(s);
     LOGMSG("\n\n");
     testConversion<PxARGB4>(s);
     LOGMSG("\n\n");
     testConversion<PxRGB565>(s);
     LOGMSG("\n\n");
     testConversion<PxRGB5>(s);
     LOGMSG("\n\n");
     testConversion<PxARGB1555>(s);
 
     LOGMSG("\n\n");
     return 0;
 }
 
 #endif
