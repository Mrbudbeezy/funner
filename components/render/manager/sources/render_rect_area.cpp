#include "shared.h"

using namespace render;

/*
    ������������
*/

RectAreaImpl::RectAreaImpl ()
{
}

RectAreaImpl::RectAreaImpl (const render::Rect& in_rect)
  : rect (in_rect)
{
}
    
/*
    ������� �������
*/

void RectAreaImpl::SetRect (const render::Rect& in_rect)
{
  rect = in_rect;
}
