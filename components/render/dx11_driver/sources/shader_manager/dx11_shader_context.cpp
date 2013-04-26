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
  InputLayoutPtr             input_layout;                                 //������� ������
  ProgramPtr                 program;                                      //���������
  ProgramParametersLayoutPtr parameters_layout;                            //������ ���������� ���������
  SourceConstantBufferPtr    buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]; //����������� ������
  BindableProgramContext     bindable_program_context;                     //�������� ���������
  bool                       is_dirty;                                     //���� "���������"

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

void ShaderManagerContextState::SetInputLayout (InputLayout* state)
{
  try
  {
    if (state == impl->input_layout)
      return;

    impl->input_layout = state;

    impl->bindable_program_context.input_layout = DxInputLayoutPtr ();

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContextState::SetInputLayout");
    throw;
  }
}

InputLayout* ShaderManagerContextState::GetInputLayout () const
{
  return impl->input_layout.get ();
}

/*
    ��������� ���������
*/

void ShaderManagerContextState::SetProgram (IProgram* in_program)
{
  try
  {
    Program* program = cast_object<Program> (*impl, in_program, "", "program");

    if (program == impl->program)
      return;

    impl->program = program;

    impl->bindable_program_context.Reset ();

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
  try
  {
    ProgramParametersLayout* layout = cast_object<ProgramParametersLayout> (parameters_layout, "", "parameters_layout");

    if (layout == impl->parameters_layout)
      return;

    impl->parameters_layout = layout;

    impl->bindable_program_context.Reset ();

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContextState::SetProgramParametersLayout");
    throw;
  }
}

void ShaderManagerContextState::SetConstantBuffer (size_t buffer_slot, IBuffer* in_buffer)
{
  try
  {
    if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
      throw xtl::make_range_exception ("", "buffer_slot", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

    SourceConstantBuffer* buffer = cast_object<SourceConstantBuffer> (in_buffer, "", "buffer");

    if (impl->buffers [buffer_slot] == buffer)
      return;

    impl->buffers [buffer_slot]                                = buffer;
    impl->bindable_program_context.dirty_buffers [buffer_slot] = true;
    impl->bindable_program_context.has_dirty_buffers           = true;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContextState::SetConstantBuffer");
    throw;
  }
}

/*
    ��������� ���������, �������� � ���������
*/

IProgramParametersLayout* ShaderManagerContextState::GetProgramParametersLayout () const
{
  return impl->parameters_layout.get ();
}

IProgram* ShaderManagerContextState::GetProgram () const
{
  return impl->program.get ();
}

IBuffer* ShaderManagerContextState::GetConstantBuffer (size_t buffer_slot) const
{
  try
  {
    if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
      throw xtl::make_range_exception ("", "buffer_slot", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

    return impl->buffers [buffer_slot].get ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContextState::GetConstantBuffer");
    throw;
  }
}

/*
===================================================================================================
    ShaderManagerContext
===================================================================================================
*/

/*
    �������� ���������� ���������
*/

typedef xtl::trackable_ptr<BindableProgram> BindableProgramWeakPtr;
typedef xtl::com_ptr<ShaderLibrary>         ShaderLibraryPtr;

struct ShaderManagerContext::Impl: public ShaderManagerContextState::Impl
{
  ShaderLibraryPtr       shader_library;   //���������� ��������
  DxContextPtr           context;          //��������
  BindableProgramWeakPtr bindable_program; //���������, ��������������� � ��������
  size_t                 buffer_hashes [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]; //���� �������

/// �����������
  Impl (ShaderLibrary& library, const DxContextPtr& in_context)
    : ShaderManagerContextState::Impl (library.GetDeviceManager ())
    , shader_library (&library)
    , context (in_context)
  {
    if (!context)
      throw xtl::make_null_argument_exception ("render::low_level::dx11::ShaderManagerContext::Impl::Impl", "context");

    memset (buffer_hashes, 0, sizeof (buffer_hashes));
  }
};

/*
    ����������� / ����������
*/

ShaderManagerContext::ShaderManagerContext (ShaderLibrary& library, const DxContextPtr& context)
  : ShaderManagerContextState (new Impl (library, context))
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
  try
  {
    Impl& impl = GetImpl ();

      //�������� ������������� ��������

    if (!impl.is_dirty)
    {
      SourceConstantBufferPtr* buffer = impl.buffers;
      size_t*                  hash   = impl.buffer_hashes;

      for (size_t i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++, buffer++, hash++)
      {
        size_t new_hash = *buffer ? (*buffer)->GetHash () : 0;

        if (*hash == new_hash)
          continue;

        *hash                                           = new_hash;
        impl.bindable_program_context.has_dirty_buffers = true;
        impl.bindable_program_context.dirty_buffers [i] = true;
      }

      if (!impl.bindable_program_context.has_dirty_buffers)
        return;
    }

      //��������� ���������, ��������������� � ��������

    if (!impl.bindable_program || &impl.bindable_program->GetProgram () != impl.program.get () || impl.bindable_program->GetProgramParametersLayout () != impl.parameters_layout.get ())
    { 
      if (!impl.program)
        throw xtl::format_operation_exception ("", "Null program");

      BindableProgram& bindable_program = impl.shader_library->GetBindableProgram (*impl.program, &*impl.parameters_layout);

      impl.bindable_program = &bindable_program;     
    }

      //�������� �������� �������

    if (!impl.input_layout)
      throw xtl::make_null_argument_exception ("", "Null input layout");

      //��������� � ��������

    impl.bindable_program->Bind (*impl.context, *impl.shader_library, impl.buffers, *impl.input_layout, impl.bindable_program_context);

      //���������� ������

    impl.is_dirty = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContext::Bind");
    throw;
  }
}
