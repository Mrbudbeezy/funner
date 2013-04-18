#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
===================================================================================================
    ShaderManagerContextState
===================================================================================================
*/

/*
    �������� ���������� ��������� ��������� ��������� ��������
*/

typedef xtl::com_ptr<InputLayout> InputLayoutPtr;
typedef xtl::com_ptr<Program>     ProgramPtr;

struct ShaderManagerContextState::Impl: public DeviceObject
{
  InputLayoutPtr input_layout; //������� ������
  ProgramPtr     program;      //���������
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

ShaderManagerContextState::ShaderManagerContextState (const DeviceManager& device_manager)
  : impl (new Impl (device_manager))
{
}

ShaderManagerContextState::ShaderManagerContextState (Impl* in_impl)
  : impl (in_impl)
{
}

ShaderManagerContextState::~ShaderManagerContextState ()
{
}

/*
    ���������� ���������
*/

ShaderManagerContextState::Impl& ShaderManagerContextState::GetImpl () const
{
  return *impl;
}

/*
    ���������� ������������� ������� ������
*/

void ShaderManagerContextState::SetInputLayout (IInputLayout* in_state)
{
  try
  {
    InputLayout* state = cast_object<InputLayout> (*impl, in_state, "", "state");

    impl->input_layout = state;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContextState::SetInputLayout");
    throw;
  }
}

IInputLayout* ShaderManagerContextState::GetInputLayout () const
{
  return impl->input_layout.get ();
}

/*
    ��������� ���������, �������� � ���������
*/

void ShaderManagerContextState::SetProgram (IProgram* in_program)
{
  try
  {
    Program* program = cast_object<Program> (*impl, in_program, "", "program");

    impl->program = program;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContextState::SetProgram");
    throw;
  }
}

void ShaderManagerContextState::SetProgramParametersLayout (IProgramParametersLayout* parameters_layout)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ShaderManagerContextState::SetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� ���������, �������� � ���������
*/

IProgramParametersLayout* ShaderManagerContextState::GetProgramParametersLayout () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IProgram* ShaderManagerContextState::GetProgram () const
{
  return impl->program.get ();
}

IBuffer* ShaderManagerContextState::GetConstantBuffer (size_t buffer_slot) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
===================================================================================================
    ShaderManagerContext
===================================================================================================
*/

/*
    �������� ���������� ���������
*/

struct ShaderManagerContext::Impl: public ShaderManagerContextState::Impl
{
  DxContextPtr context; //��������

/// �����������
  Impl (const DeviceManager& device_manager, const DxContextPtr& in_context)
    : ShaderManagerContextState::Impl (device_manager)
    , context (in_context)
  {
    if (!context)
      throw xtl::make_null_argument_exception ("render::low_level::dx11::ShaderManagerContext::Impl::Impl", "context");
  }
};

/*
    ����������� / ����������
*/

ShaderManagerContext::ShaderManagerContext (const DeviceManager& device_manager, const DxContextPtr& context)
  : ShaderManagerContextState (new Impl (device_manager, context))
{
}

ShaderManagerContext::~ShaderManagerContext ()
{
}

/*
    ����������� ����������
*/

ShaderManagerContext::Impl& ShaderManagerContext::GetImpl () const
{
  return static_cast<Impl&> (ShaderManagerContextState::GetImpl ());
}

/*
    ������� ���������, �������� � ���������
*/

void ShaderManagerContext::Bind ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
