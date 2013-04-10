#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    �������� ���������� ��������� ����� ����������
*/

struct RenderTargetContextState::Impl
{
  bool is_dirty; //���� "���������"

/// �����������
  Impl ()
    : is_dirty (true)
  {
  }
};

/*
    ����������� / ����������
*/

RenderTargetContextState::RenderTargetContextState ()
  : impl (new Impl)
{
}

RenderTargetContextState::~RenderTargetContextState ()
{
}

/*
    ���� ������������ ���������
*/

bool RenderTargetContextState::IsDirty () const
{
  return impl->is_dirty;
}

void RenderTargetContextState::ClearDirtyFlag ()
{
  impl->is_dirty = false;
}

/*
    ����� ������� �����������
*/

void RenderTargetContextState::SetRenderTargetView (size_t render_target_slot, IView* render_target_view)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void RenderTargetContextState::SetDepthStencilView (IView* depth_stencil_view)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IView* RenderTargetContextState::GetRenderTargetView (size_t render_target_slot) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IView* RenderTargetContextState::GetDepthStencilView () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void RenderTargetContextState::HasRenderTargetViews (bool states [DEVICE_RENDER_TARGET_SLOTS_COUNT]) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� ��������� ������������
*/

void RenderTargetContextState::SetViewport (size_t render_target_slot, const Viewport& viewport)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void RenderTargetContextState::SetScissor (size_t render_target_slot, const Rect& scissor_rect)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

const Viewport& RenderTargetContextState::GetViewport (size_t render_target_slot) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

const Rect& RenderTargetContextState::GetScissor (size_t render_target_slot) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ������ / �������������� ���������
*/

void RenderTargetContextState::Capture (const StateBlockMask&, RenderTargetContextState& src_state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void RenderTargetContextState::Apply (const StateBlockMask&, RenderTargetContextState& dst_state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
