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
  BlendStatePtr blend_state; //��������� ��������� ���������� ������
  bool          is_dirty;    //���� "���������"

/// �����������
  Impl (const DeviceManager& manager)
    : DeviceObject (manager)
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

void OutputManagerContextState::SetRasterizerState (IRasterizerState* state)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContextState::SetRasterizerState");
    throw;
  }
}

IRasterizerState* OutputManagerContextState::GetRasterizerState () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
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

void OutputManagerContextState::SetDepthStencilState (IDepthStencilState* state)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContextState::SetDepthStencilState");
    throw;
  }
}

void OutputManagerContextState::SetStencilReference (size_t reference)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IDepthStencilState* OutputManagerContextState::GetDepthStencilState () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t OutputManagerContextState::GetStencilReference () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
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
  DxContextPtr context; //�������� ����������

/// �����������
  Impl (const DeviceManager& manager, const DxContextPtr& in_context)
    : OutputManagerContextState::Impl (manager)
    , context (in_context)
  {
    static const char* METHOD_NAME = "render::low_level::dx11::OutputManagerContext::Impl::Impl";

    if (!context)
      throw xtl::make_null_argument_exception (METHOD_NAME, "context");
  }
};

/*
    ����������� / ����������
*/

OutputManagerContext::OutputManagerContext (const DeviceManager& manager, const DxContextPtr& context)
  : OutputManagerContextState (new Impl (manager, context))
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

      //��������� ��������� ���������

//    static const float blend_factors [] = {1.0f, 1.0f, 1.0f, 1.0f};

//    impl.context->OMSetBlendState (impl.blend_state->GetHandle ());

    throw xtl::make_not_implemented_exception (__FUNCTION__);

      //������� ����� "���������"

    impl.is_dirty = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContext::Bind");
    throw;
  }
}
