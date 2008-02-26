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

FppShaderManager::~FppShaderManager ()
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
    RaiseOutOfRange ("render::low_level::opengl::FppShaderManager::GetProfile", "index", index, 1);
    
  return "fpp";
}

/*
    �������� �������
*/

Shader* FppShaderManager::CreateShader  (const ShaderDesc& shader_desc, const LogFunction& error_log)
{
  return new FppShader (shader_desc, error_log);
}

/*
    �������� ���������
*/

Program* FppShaderManager::CreateProgram (size_t shaders_count, ShaderPtr* shaders, const LogFunction&)
{
  return new FppProgram (GetContextManager (), shaders_count, shaders);
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

ShaderManager* create_fpp_shader_manager (const ContextManager& context_manager)
{
  return new FppShaderManager (context_manager);
}

}

}

}
