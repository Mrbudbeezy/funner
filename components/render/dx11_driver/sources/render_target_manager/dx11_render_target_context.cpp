#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    �������� ���������� ��������� ����� ����������
*/

typedef xtl::trackable_ptr<View> ViewPtr;

namespace
{

struct RenderTargetDesc
{
  ViewPtr  view;                      //������� ����������� ������� �����
  Viewport viewport;                  //������� ������
  Rect     scissor;                   //������� ���������
  size_t   viewport_hash;             //��� ������� ������
  size_t   viewport_rect_hash;        //��� ������� ������ ��� ������� ���������
  size_t   scissor_hash;              //��� ������� ���������
  bool     need_recalc_viewport_hash; //���� ������������� ��������� ���� ������� ������
  bool     need_recalc_scissor_hash;  //���� ������������� ��������� ���� ������� ���������

  RenderTargetDesc () 
    : viewport_hash ()
    , viewport_rect_hash ()
    , scissor_hash ()
    , need_recalc_viewport_hash (true)
    , need_recalc_scissor_hash (true)
  {
    memset (&viewport, 0, sizeof viewport);
    memset (&scissor, 0, sizeof scissor);
  }
};

}

struct RenderTargetContext::Impl
{
  RenderTargetDesc render_targets [DEVICE_RENDER_TARGET_SLOTS_COUNT]; //����������� ����� ����������
  ViewPtr          depth_stencil_view;                                //������� ����������� ������ ������������� ���������
  bool             is_dirty;                                          //���� "���������"

/// �����������
  Impl ()
    : is_dirty (true)
  {
  }

/// ��������� ����
  RenderTargetDesc& GetRenderTarget (size_t view_index, const char* source)
  {
    if (view_index >= DEVICE_RENDER_TARGET_SLOTS_COUNT)
      throw xtl::make_range_exception (source, "render_target_slot", view_index, DEVICE_RENDER_TARGET_SLOTS_COUNT);

    return render_targets [view_index];
  }

/// ���������� �� ��������� ���������
  void UpdateNotify ()
  {
    is_dirty = true;
  }
};

/*
    ����������� / ����������
*/

RenderTargetContext::RenderTargetContext ()
  : impl (new Impl)
{
}

RenderTargetContext::~RenderTargetContext ()
{
}

/*
    ���� ������������ ���������
*/

bool RenderTargetContext::IsDirty () const
{
  return impl->is_dirty;
}

/*
    ����� ������� �����������
*/

void RenderTargetContext::SetRenderTargetView (size_t render_target_slot, IView* render_target_view)
{
//  View* render_target_view = cast_object<View> (GetContextManager (), in_render_target_view, METHOD_NAME, "render_target_view");

/*  RenderTargetDesc& render_target = impl->GetRenderTarget (view_index, "render::low_level::dx11::RenderTargetContext::SetRenderTargetView");

  if (render_target.view == view)
    return;

  render_target.view = view;
  
  impl->UpdateNotify ();*/
}

void RenderTargetContext::SetDepthStencilView (IView* depth_stencil_view)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IView* RenderTargetContext::GetRenderTargetView (size_t render_target_slot) const
{
/*  RenderTargetDesc& render_target = impl->GetRenderTarget (view_index, "render::low_level::dx11::RenderTargetContext::GetRenderTargetView");

  if (render_target.view == view)
    return;

  return &*render_target.view;*/
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IView* RenderTargetContext::GetDepthStencilView () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void RenderTargetContext::HasRenderTargetViews (bool states [DEVICE_RENDER_TARGET_SLOTS_COUNT]) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� ��������� ������������
*/

void RenderTargetContext::SetViewport (size_t render_target_slot, const Viewport& viewport)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void RenderTargetContext::SetScissor (size_t render_target_slot, const Rect& scissor_rect)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

const Viewport& RenderTargetContext::GetViewport (size_t render_target_slot) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

const Rect& RenderTargetContext::GetScissor (size_t render_target_slot) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ������ / �������������� ���������
*/

void RenderTargetContext::CopyTo (const StateBlockMask&, RenderTargetContext& dst_state) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ������� ������� ���������
*/

void RenderTargetContext::ClearRenderTargetView (const DxContextPtr& context, size_t render_target_slot, const Color4f& color)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void RenderTargetContext::ClearDepthStencilView (const DxContextPtr& context, size_t clear_flags, float depth, unsigned char stencil)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void RenderTargetContext::ClearViews (const DxContextPtr& context, size_t clear_flags, size_t views_count, const size_t* view_indices, const Color4f* colors, float depth, unsigned char stencil)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    �������
*/

void RenderTargetContext::Bind (const DxContextPtr& context)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
