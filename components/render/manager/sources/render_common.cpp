#include "shared.h"

using namespace render;

/*
    ������������� �������
*/

Rect::Rect ()
{
  x      = 0;
  y      = 0;
  width  = 0;
  height = 0;
}
  
Rect::Rect (int in_x, int in_y, size_t in_width, size_t in_height)
{
  x      = in_x;
  y      = in_y;
  width  = in_width;
  height = in_height;
}

/*
    ��������� ����� �������
*/

namespace render
{

const char* get_name (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_RGB8:  return "rgb8";
    case PixelFormat_RGBA8: return "rgba8";
    case PixelFormat_L8:    return "l8";
    case PixelFormat_A8:    return "a8";
    case PixelFormat_LA8:   return "la8";
    default:
      throw xtl::make_argument_exception ("render::get_name(render::PixelFormat)", "format", format);
  }
}

}
