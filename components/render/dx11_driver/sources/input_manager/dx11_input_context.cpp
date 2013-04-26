#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
===================================================================================================
    InputManagerContextState
===================================================================================================
*/

/*
    �������� ���������� ��������� ��������� ��������� �������� ������
*/

struct InputManagerContextState::Impl: public DeviceObject
{
  InputLayoutPtr input_layout; //������� ������  
  bool           is_dirty;     //���� "���������"

/// �����������
  Impl (const DeviceManager& device_manager)
    : DeviceObject (device_manager)
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

InputManagerContextState::InputManagerContextState (const DeviceManager& device_manager)
  : impl (new Impl (device_manager))
{
}

InputManagerContextState::InputManagerContextState (Impl* in_impl)
  : impl (in_impl)
{
}

InputManagerContextState::~InputManagerContextState ()
{
}

/*
    ����������� ����������
*/

InputManagerContextState::Impl& InputManagerContextState::GetImpl () const
{
  return *impl;
}

/*
    ���������� ������������� ������� ������
*/

void InputManagerContextState::SetInputLayout (InputLayout* state)
{  
  impl->input_layout = state;

  impl->UpdateNotify ();
}

InputLayout* InputManagerContextState::GetInputLayout () const
{
  return impl->input_layout.get ();
}

/*
    ���������� ���������� ��������
*/

void InputManagerContextState::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IBuffer* InputManagerContextState::GetVertexBuffer (size_t vertex_buffer_slot) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ���������� ��������� �������
*/

void InputManagerContextState::SetIndexBuffer (IBuffer* buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IBuffer* InputManagerContextState::GetIndexBuffer () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
===================================================================================================
    InputManagerContext
===================================================================================================
*/

struct InputManagerContext::Impl: public InputManagerContextState::Impl
{
  DxContextPtr   context;        //��������
  InputLayoutPtr default_layout; //������ �� ���������

/// �����������
  Impl (const DeviceManager& device_manager, const DxContextPtr& in_context, const InputLayoutPtr& in_default_layout)
    : InputManagerContextState::Impl (device_manager)
    , context (in_context)
    , default_layout (in_default_layout)
  {
    static const char* METHOD_NAME = "render::low_level::dx11::InputManagerContext::Impl::Impl";

    if (!context)
      throw xtl::make_null_argument_exception (METHOD_NAME, "context");

    if (!default_layout)
      throw xtl::make_null_argument_exception (METHOD_NAME, "default_layout");
  }
};

/*
    ����������� / ����������
*/

InputManagerContext::InputManagerContext (const DeviceManager& device_manager, const DxContextPtr& context, const InputLayoutPtr& input_layout)
  : InputManagerContextState (new Impl (device_manager, context, input_layout))
{
}

InputManagerContext::~InputManagerContext ()
{
}

/*
    ��������� ��������� ������ � ��������
*/

InputManagerContext::Impl& InputManagerContext::GetImpl () const
{
  return static_cast<Impl&> (InputManagerContextState::GetImpl ());
}

void InputManagerContext::Bind ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManagerContext::Bind");
    throw;
  }
}
