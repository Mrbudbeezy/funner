#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

FppShaderManager::FppShaderManager (const ContextManager& context_manager)
  : ContextObject (context_manager)
{
}

/*
    ���������� �������������� ��������
*/

size_t FppShaderManager::GetProfilesCount ()
{
  return 1;
}

/*
    ����� �������������� ��������
*/

const char* FppShaderManager::GetProfile (size_t index)
{
  if (index)
    throw xtl::make_range_exception ("render::low_level::opengl::FppShaderManager::GetProfile", "index", index, 1);
    
  return "fpp";
}

/*
    �������� �������
*/

IShader* FppShaderManager::CreateShader  (const ShaderDesc& shader_desc, const LogFunction& error_log)
{
  return new FppProgram (GetContextManager (), shader_desc, error_log);
}

/*
    �������� ���������
*/

ICompiledProgram* FppShaderManager::CreateProgram (size_t shaders_count, IShader** shaders, const LogFunction&)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FppShaderManager::CreateProgram";

    //�������� ������������ ����������

  if (!shaders)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shaders");

  if (shaders_count > 1)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Multiple shaders not supported (shaders_count=%u)", shaders_count);

  FppProgram* program = cast_object<FppProgram> (*this, shaders [0], METHOD_NAME, "shaders[0]");

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

IShaderManager* create_fpp_shader_manager (const ContextManager& context_manager)
{
  return new FppShaderManager (context_manager);
}

}

}

}
