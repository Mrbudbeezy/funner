#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
===================================================================================================
    OutputManagerContextState
===================================================================================================
*/

struct OutputManagerContextState::Impl: public DeviceObject
{
  BlendStatePtr        blend_state;         //��������� ��������� ���������� ������
  DepthStencilStatePtr depth_stencil_state; //��������� ��������� ���������
  size_t               stencil_ref;         //����������� �������� ��������
  RasterizerStatePtr   rasterizer_state;    //��������� ��������� ������������
  bool                 is_dirty;            //���� "���������"

/// �����������
  Impl (const DeviceManager& manager)
    : DeviceObject (manager)
    , stencil_ref ()
    , is_dirty (true)
  {
  }

/// ����������
  virtual ~Impl () {}

/// ���������� �� ���������
  void UpdateNotify ()
  {
    is_dirty = true;
  }
};

/*
    ����������� / ����������
*/

OutputManagerContextState::OutputManagerContextState (const DeviceManager& manager)
  : impl (new Impl (manager))
{
}

OutputManagerContextState::OutputManagerContextState (Impl* in_impl)
  : impl (in_impl)
{
}

OutputManagerContextState::~OutputManagerContextState ()
{
}

/*
    ��������� ����������
*/

OutputManagerContextState::Impl& OutputManagerContextState::GetImpl () const
{
  return *impl;
}

/*
    ��������� ��������� ������������
*/

void OutputManagerContextState::SetRasterizerState (IRasterizerState* in_state)
{
  try
  {
    RasterizerState* state = cast_object<RasterizerState> (*impl, in_state, "", "state");

    if (state == impl->rasterizer_state)
      return;

    impl->rasterizer_state = state;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContextState::SetRasterizerState");
    throw;
  }
}

IRasterizerState* OutputManagerContextState::GetRasterizerState () const
{
  return impl->rasterizer_state.get ();
}

/*
    ��������� ��������� ���������� ������
*/

void OutputManagerContextState::SetBlendState (IBlendState* in_state)
{
  try
  {
    BlendState* state = cast_object<BlendState> (*impl, in_state, "", "state");

    if (state == impl->blend_state)
      return;

    impl->blend_state = state;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContextState::SetBlendState");
    throw;
  }
}

IBlendState* OutputManagerContextState::GetBlendState () const
{
  return impl->blend_state.get ();
}

/*
    ��������� ��������� ������������� ���������
*/

void OutputManagerContextState::SetDepthStencilState (IDepthStencilState* in_state)
{
  try
  {
    DepthStencilState* state = cast_object<DepthStencilState> (*impl, in_state, "", "state");

    if (state == impl->depth_stencil_state)
      return;

    impl->depth_stencil_state = state;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContextState::SetDepthStencilState");
    throw;
  }
}

void OutputManagerContextState::SetStencilReference (size_t reference)
{
  if (impl->stencil_ref == reference)
    return;

  impl->stencil_ref = reference;

  impl->UpdateNotify ();
}

IDepthStencilState* OutputManagerContextState::GetDepthStencilState () const
{
  return impl->depth_stencil_state.get ();
}

size_t OutputManagerContextState::GetStencilReference () const
{
  return impl->stencil_ref;
}

/*
===================================================================================================
    OutputManagerContext
===================================================================================================
*/

/*
    �������� ���������� ���������
*/

struct OutputManagerContext::Impl: public OutputManagerContextState::Impl
{
  DxContextPtr         context;                     //�������� ����������
  BlendStatePtr        null_blend_state;            //��������� ��������� ���������� � ����������� �������
  DepthStencilStatePtr null_depth_stencil_state;    //��������� ��������� ��������� � ����������� ����������
  RasterizerStatePtr   null_rasterizer_state;       //�������� ��������� ������������ �� ���������

/// �����������
  Impl (const DeviceManager& manager, const DxContextPtr& in_context, const DefaultResources& default_resources)
    : OutputManagerContextState::Impl (manager)
    , context (in_context)
  {
    static const char* METHOD_NAME = "render::low_level::dx11::OutputManagerContext::Impl::Impl";

    if (!context)
      throw xtl::make_null_argument_exception (METHOD_NAME, "context");

    if (!default_resources.blend_state)
      throw xtl::make_null_argument_exception (METHOD_NAME, "null_blend_state");

    null_blend_state = cast_object<BlendState> (manager, default_resources.blend_state.get (), METHOD_NAME, "null_blend_state");    

    if (!default_resources.depth_stencil_state)
      throw xtl::make_null_argument_exception (METHOD_NAME, "null_depth_stencil_state");

    null_depth_stencil_state = cast_object<DepthStencilState> (manager, default_resources.depth_stencil_state.get (), METHOD_NAME, "null_depth_stencil_state");     

    if (!default_resources.rasterizer_state)
      throw xtl::make_null_argument_exception (METHOD_NAME, "null_rasterizer_state");

    null_rasterizer_state = cast_object<RasterizerState> (manager, default_resources.rasterizer_state.get (), METHOD_NAME, "null_rasterizer_state");
  }
};

/*
    ����������� / ����������
*/

OutputManagerContext::OutputManagerContext (const DeviceManager& manager, const DxContextPtr& context, const DefaultResources& default_resources)
  : OutputManagerContextState (new Impl (manager, context, default_resources))
{
}

OutputManagerContext::~OutputManagerContext ()
{
}

/*
    ��������� ����������
*/

OutputManagerContext::Impl& OutputManagerContext::GetImpl () const
{
  return static_cast<Impl&> (OutputManagerContextState::GetImpl ());
}

/*
    ��������� ��������� ������ � ��������
*/

void OutputManagerContext::Bind ()
{
  try
  {
      //�������� ����� "���������"

    Impl& impl = GetImpl ();

    if (!impl.is_dirty)
      return;

      //��������� ��������� ��������� ���������� ������

    BlendState* blend_state = impl.blend_state.get ();

    if (!blend_state)
      blend_state = impl.null_blend_state.get ();    

    static const float blend_factors [] = {1.0f, 1.0f, 1.0f, 1.0f};

    impl.context->OMSetBlendState (&blend_state->GetHandle (), blend_factors, 0xffffffff);

      //��������� ��������� ��������� ���������

    DepthStencilState* depth_stencil_state = impl.depth_stencil_state.get ();

    if (!depth_stencil_state)
      depth_stencil_state = impl.null_depth_stencil_state.get ();

    impl.context->OMSetDepthStencilState (&depth_stencil_state->GetHandle (), impl.stencil_ref);

      //��������� ��������� ��������� ������������

    RasterizerState* rasterizer_state = impl.rasterizer_state.get ();

    if (!rasterizer_state)
      rasterizer_state = impl.null_rasterizer_state.get ();

    impl.context->RSSetState (&rasterizer_state->GetHandle ());

      //������� ����� "���������"

    impl.is_dirty = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContext::Bind");
    throw;
  }
}
