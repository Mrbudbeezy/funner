#ifndef RENDER_GL_DRIVER_SCALE
#define RENDER_GL_DRIVER_SCALE

#include "shared.h"

typedef unsigned char  uchar;
typedef unsigned short uint16;
typedef unsigned int   r24g8_t;

struct two_color8_t
{
  uchar red;
  uchar green;
};

struct rgb8_t
{
  uchar red;
  uchar green;
  uchar blue;
};

struct rgba8_t
{
  uchar red;
  uchar green;
  uchar blue;
  uchar alpha;
};

/*
  ������� ��������������� ����������� �� ������� ������� ������
*/

////////////////////////////////////////////////////////////////////////////////////////////
///�������
////////////////////////////////////////////////////////////////////////////////////////////

void ScalePixel (uchar& dest,uchar s1,uchar s2,uchar s3,uchar s4)
{
  dest = (s1 + s2 + s3 + s4) >> 2;
}

void ScalePixel (uint16& dest, uint16 s1, uint16 s2, uint16 s3, uint16 s4)
{
  dest = (size_t)(s1 + s2 + s3 + s4) >> 2;
}

void ScalePixel (r24g8_t& dest, const r24g8_t& s1, const r24g8_t& s2, const r24g8_t& s3, const r24g8_t& s4)
{
  //������������!!!
  dest = ((s1 & 0xffffff) + (s2 & 0xffffff) + (s3 & 0xffffff) + (s4 & 0xffffff)) >> 2;
  dest |= ((((s1 & 0xff000000) >> 24) + ((s2 & 0xff000000) >> 24) + ((s3 & 0xff000000) >> 24) + ((s4 & 0xff000000) >> 24)) >> 2) << 22;
}

void  ScalePixel (two_color8_t& dest, two_color8_t s1, two_color8_t s2, two_color8_t s3, two_color8_t s4)
{
  dest.red   = (size_t)(s1.red   + s2.red   + s3.red   + s4.red)   >> 2;
  dest.green = (size_t)(s1.green + s2.green + s3.green + s4.green) >> 2;
}

void  ScalePixel (rgb8_t& dest,const rgb8_t& s1,const rgb8_t& s2,const rgb8_t& s3,const rgb8_t& s4)
{
  dest.red   = (size_t)(s1.red   + s2.red   + s3.red   + s4.red)   >> 2;
  dest.green = (size_t)(s1.green + s2.green + s3.green + s4.green) >> 2;
  dest.blue  = (size_t)(s1.blue  + s2.blue  + s3.blue  + s4.blue ) >> 2;
}

void  ScalePixel (rgba8_t& dest,const rgba8_t& s1,const rgba8_t& s2,const rgba8_t& s3,const rgba8_t& s4)
{
  dest.red   = (size_t)(s1.red   + s2.red   + s3.red   + s4.red)   >> 2;
  dest.green = (size_t)(s1.green + s2.green + s3.green + s4.green) >> 2;
  dest.blue  = (size_t)(s1.blue  + s2.blue  + s3.blue  + s4.blue ) >> 2;
  dest.alpha = (size_t)(s1.alpha + s2.alpha + s3.alpha + s4.alpha) >> 2;
}

////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������������
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void ScaleImage2XDownImpl (size_t width, size_t height, const T* src,T* dest)
{
  const T *s1 = src,
          *s2 = src + 1,
          *s3 = src + width,
          *s4 = s3  + 1; 

  size_t  w2  = width >> 1,
          h2  = height >> 1;

  for (size_t h = 0; h < h2; h++, s1 += width, s2 += width, s3 += width, s4 += width)
    for (size_t w = 0; w < w2; w++, s1 += 2, s2 += 2, s3 += 2, s4 += 2, dest++)
      ScalePixel (*dest,*s1,*s2,*s3,*s4);

  if (height == 1)
  {
    s3 = s1; s4 = s2;
    for (size_t w = 0; w < w2; w++, s1 += 2, s2 += 2, s3 += 2, s4 += 2, dest++)
      ScalePixel (*dest,*s1,*s2,*s3,*s4);
  }
  if (width == 1)
  {
    s3 = s1; s4 = s2;
    for (size_t h = 0; h < h2; h++, s1 += 2, s2 += 2, s3 += 2, s4 += 2, dest++)
      ScalePixel (*dest,*s1,*s2,*s3,*s4);
  }
}

void render::low_level::opengl::ScaleImage2XDown (PixelFormat format, size_t width, size_t height, const void* src, void* dest)
{
  switch (format)
  {
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_S8:    ScaleImage2XDownImpl <uchar>        (width, height, (uchar*)       src, (uchar*)       dest); break;
    case PixelFormat_LA8:   ScaleImage2XDownImpl <two_color8_t> (width, height, (two_color8_t*)src, (two_color8_t*)dest); break;
    case PixelFormat_D16:   ScaleImage2XDownImpl <uint16>       (width, height, (uint16*)      src, (uint16*)      dest); break;
    case PixelFormat_RGB8:  ScaleImage2XDownImpl <rgb8_t>       (width, height, (rgb8_t*)      src, (rgb8_t*)      dest); break;
    case PixelFormat_RGBA8: ScaleImage2XDownImpl <rgba8_t>      (width, height, (rgba8_t*)     src, (rgba8_t*)     dest); break;
    case PixelFormat_D24S8:
    case PixelFormat_D24X8: ScaleImage2XDownImpl <r24g8_t>      (width, height, (r24g8_t*)     src, (r24g8_t*)     dest); break;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    default: return;
  }
}


#endif