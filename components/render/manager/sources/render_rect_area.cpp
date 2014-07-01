#include "shared.h"

using namespace render::manager;

/*
    ������������
*/

RectAreaImpl::RectAreaImpl ()
{
}

RectAreaImpl::RectAreaImpl (const render::manager::Rect& in_rect)
  : rect (in_rect)
{
}
    
/*
    ������� �������
*/

void RectAreaImpl::SetRect (const render::manager::Rect& in_rect)
{
  rect = in_rect;
}
