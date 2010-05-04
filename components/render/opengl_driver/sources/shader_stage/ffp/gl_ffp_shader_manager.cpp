#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

FfpShaderManager::FfpShaderManager (const ContextManager& context_manager)
  : ContextObject (context_manager)
{
}

/*
    ���������� �������������� ��������
*/

size_t FfpShaderManager::GetProfilesCount ()
{
  return 1;
}

/*
    ����� �������������� ��������
*/

const char* FfpShaderManager::GetProfile (size_t index)
{
  if (index)
    throw xtl::make_range_exception ("render::low_level::opengl::FfpShaderManager::GetProfile", "index", index, 1);
    
  return "ffp";
}

/*
    �������� �������
*/

IShader* FfpShaderManager::CreateShader  (const ShaderDesc& shader_desc, const LogFunction& error_log)
{
  return new FfpProgram (GetContextManager (), shader_desc, error_log);
}

/*
    �������� ���������
*/

ICompiledProgram* FfpShaderManager::CreateProgram (size_t shaders_count, IShader** shaders, const LogFunction&)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FfpShaderManager::CreateProgram";

    //�������� ������������ ����������

  if (!shaders)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shaders");

  if (shaders_count > 1)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Multiple shaders not supported (shaders_count=%u)", shaders_count);

  FfpProgram* program = cast_object<FfpProgram> (*this, shaders [0], METHOD_NAME, "shaders[0]");

  if (!program)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shaders[0]");
    
    //�������� �� ��������� ��������� ��������� ��������� � ��������

  program->AddRef ();

  return program;
}

/*
    �������� ��������� ��������
*/

namespace render
{

namespace low_level
{

namespace opengl
{

IShaderManager* create_ffp_shader_manager (const ContextManager& context_manager)
{
  return new FfpShaderManager (context_manager);
}

}

}

}
