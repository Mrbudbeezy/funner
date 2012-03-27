#include "shared.h"

using namespace render;
using namespace render::obsolete;

/*
===================================================================================================
    RenderTarget
===================================================================================================
*/

/*
    ������������ / ����������
*/

RenderTarget::RenderTarget ()
{
  RenderTargetImpl::Pointer render_target = RenderTargetImpl::Create (0, 0, ~0u);

  impl = render_target.get ();

  addref (impl);
}

RenderTarget::RenderTarget (RenderTargetImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

RenderTarget::RenderTarget (const RenderTarget& render_target)
  : impl (render_target.impl)
{
  addref (impl);
}

RenderTarget::~RenderTarget ()
{
  release (impl);
}

RenderTarget& RenderTarget::operator = (const RenderTarget& render_target)
{
  RenderTarget (render_target).Swap (*this);
  
  return *this;
}

/*
    �������� ����� ���� ���������� � ��������
*/

bool RenderTarget::IsBindedToRender () const
{
  return impl->RenderManager () != 0;
}

/*
    ����� (�������� �������� - weak-ref)
*/

void RenderTarget::SetScreen (render::obsolete::Screen* screen)
{
  impl->SetScreen (screen);
}

Screen* RenderTarget::Screen () const
{
  return impl->Screen ();
}

/*
    ����������
*/

void RenderTarget::Update ()
{
  impl->Update ();
}

/*
    ������ ����������� (screen-shot)
*/

void RenderTarget::CaptureImage (media::Image& image)
{
  impl->CaptureImage (image);
}

void RenderTarget::CaptureImage (const char* image_name)
{
  impl->CaptureImage (image_name);
}

/*
    �����
*/

void RenderTarget::Swap (RenderTarget& render_target)
{
  stl::swap (impl, render_target.impl);
}

namespace render
{

namespace obsolete
{

void swap (RenderTarget& render_target1, RenderTarget& render_target2)
{
  render_target1.Swap (render_target2);
}

}

}
