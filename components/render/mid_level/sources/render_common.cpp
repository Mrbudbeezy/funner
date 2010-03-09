#include "shared.h"

using namespace render::mid_level;

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
