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

struct ShaderManagerContextState::Impl: public DeviceObject
{
/// �����������
  Impl (const DeviceManager& device_manager)
    : DeviceObject (device_manager)
  {
  }

/// ����������
  virtual ~Impl () {}
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

void ShaderManagerContextState::SetInputLayout (IInputLayout* state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IInputLayout* ShaderManagerContextState::GetInputLayout () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ��������� ���������, �������� � ���������
*/

void ShaderManagerContextState::SetProgram (IProgram* program)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
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
  throw xtl::make_not_implemented_exception (__FUNCTION__);
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
