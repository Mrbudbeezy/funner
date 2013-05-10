#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    �������� ���������� ��������� ��������
*/

struct ShaderManager::Impl: public DeviceObject
{
  ShaderLibraryPtr shader_library; //���������� ��������

/// �����������
  Impl (const DeviceManager& device_manager)
    : DeviceObject (device_manager)
    , shader_library (new ShaderLibrary (device_manager), false)
  {
  }
};

/*
    ����������� / ����������
*/

ShaderManager::ShaderManager (const DeviceManager& device_manager)
  : impl (new Impl (device_manager))
{
}

ShaderManager::~ShaderManager ()
{
}

/*
    �������� ��������
*/

IProgramParametersLayout* ShaderManager::CreateProgramParametersLayout (const ProgramParametersLayoutDesc& desc)
{
  try
  {
    return new ProgramParametersLayout (desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManager::CreateProgramParametersLayout");
    throw;
  }
}

IProgram* ShaderManager::CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  try
  {
    return new Program (*impl->shader_library, shaders_count, shader_descs, error_log);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManager::CreateProgram");
    throw;
  }
}

/*
    �������� ������������ ������
*/

IBuffer* ShaderManager::CreateConstantBuffer (const BufferDesc& desc, const void* data)
{
  try
  {
    return new SourceConstantBuffer (desc, data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManager::CreateConstantBuffer");
    throw;
  }
}
    
/*
    ��������� ������ �������������� ���������� �������� ��������
*/

const char* ShaderManager::GetShaderProfilesString () const
{
  return "hlsl.vs hlsl.ps hlsl";
}

/*
    ��������� ���������� ��������
*/

ShaderLibrary& ShaderManager::GetShaderLibrary () const
{
  return *impl->shader_library;
}
