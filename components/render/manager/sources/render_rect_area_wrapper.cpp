#include "shared.h"

using namespace render::manager;

RectArea::RectArea ()
  : impl (new RectAreaImpl)
{
}

RectArea::RectArea (const render::manager::Rect& in_rect)
  : impl (new RectAreaImpl (in_rect))
{
}

RectArea::RectArea (const RectArea& area)
  : impl (area.impl)
{
  addref (impl);
}

RectArea::RectArea (RectAreaImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

RectArea::~RectArea ()
{
  release (impl);
}

RectArea& RectArea::operator = (const RectArea& area)
{
  RectArea (area).Swap (*this);
  
  return *this;
}

void RectArea::SetRect (const render::manager::Rect& rect)
{
  impl->SetRect (rect);
}

void RectArea::SetRect (int x, int y, unsigned int width, unsigned int height)
{
  impl->SetRect (render::manager::Rect (x, y, width, height));
}

const render::manager::Rect& RectArea::Rect () const
{
  return impl->Rect ();
}

int RectArea::X () const
{
  return impl->Rect ().x;
}

int RectArea::Y () const
{
  return impl->Rect ().y;
}

unsigned int RectArea::Width () const
{
  return impl->Rect ().width;
}

unsigned int RectArea::Height () const
{
  return impl->Rect ().height;
}

void RectArea::Swap (RectArea& area)
{
  stl::swap (impl, area.impl);
}

namespace render
{

namespace manager
{

void swap (RectArea& area1, RectArea& area2)
{
  area1.Swap (area2);
}

}

}
