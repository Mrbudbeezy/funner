#include "shared.h"

using namespace common;
using namespace nv;

namespace
{

typedef unsigned char  uchar;

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

void unpack (const Color32& src_color, rgb8_t& dst_color)
{
  dst_color.red   = src_color.b;
  dst_color.green = src_color.g;
  dst_color.blue  = src_color.r;
}

void unpack (const Color32& src_color, rgba8_t& dst_color)
{
  dst_color.red   = src_color.b;
  dst_color.green = src_color.g;
  dst_color.blue  = src_color.r;
  dst_color.alpha = src_color.a;  
}

template <class Block, class Pixel>
void unpack (unsigned int width, unsigned int height, const Block* dxt_data, Pixel* unpacked_data)
{
  ColorBlock color_block;
  Block*     dxt_data_pointer = (Block*)dxt_data;
  
  unsigned int columns = stl::min (width, (unsigned int)4), rows = stl::min (height, (unsigned int)4);

  for (unsigned int i = 0; i < height; i += 4)
    for (unsigned int j = 0; j < width; j += 4, dxt_data_pointer++)
    {
      dxt_data_pointer->decodeBlock (&color_block);

      const Color32* colors = color_block.colors ();

      for (unsigned int k=0; k < rows; k++)
        for (unsigned int l = 0; l < columns; l++)
          unpack (colors [k * 4 + l], unpacked_data [(i + k) * width + j + l]);
    }
}

}

/*
   ������ � DXT �������� ��� ���������� ���������� ���������
*/

namespace render
{

namespace low_level
{

namespace opengl
{

void unpack_dxt (PixelFormat format, unsigned int width, unsigned int height, const void* dxt_data, void* unpacked_data)
{
  switch (format)
  {
    case PixelFormat_DXT1: 
      unpack (width, height, reinterpret_cast<const BlockDXT1*> (dxt_data), reinterpret_cast<rgb8_t*> (unpacked_data));
      break;
    case PixelFormat_DXT3: 
      unpack (width, height, reinterpret_cast<const BlockDXT3*> (dxt_data), reinterpret_cast<rgba8_t*> (unpacked_data));
      break;
    case PixelFormat_DXT5:
      unpack (width, height, reinterpret_cast<const BlockDXT5*> (dxt_data), reinterpret_cast<rgba8_t*> (unpacked_data));
      break;    
    default:
      throw xtl::make_argument_exception ("render::low_level::opengl::unpack_dxt", "format");
  }
}

}

}

}
