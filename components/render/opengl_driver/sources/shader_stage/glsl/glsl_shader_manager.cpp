#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

/*
    �������� �������
*/

struct Profile
{
  const char* name;        //��� �������
  GLenum      shader_type; //��� ��������
};

//��������� �������
Profile profiles [] = {
  {"glsl.ps", GL_FRAGMENT_SHADER},
  {"glsl.vs", GL_VERTEX_SHADER}
};

//���������� ��������
const size_t PROFILES_COUNT = sizeof (profiles) / sizeof (*profiles);

}

/*
    ����������� / ����������
*/

GlslShaderManager::GlslShaderManager (const ContextManager& manager)
  : ContextObject (manager)
{
}

GlslShaderManager::~GlslShaderManager ()
{
}

/*
    ���������� �������������� ��������
*/

size_t GlslShaderManager::GetProfilesCount ()
{
  return PROFILES_COUNT;
}

/*
    ����� �������������� ��������
*/

const char* GlslShaderManager::GetProfile (size_t index)
{
  if (index >= PROFILES_COUNT)
    throw xtl::make_range_exception ("render::low_level::opengl::GlslShaderManager::GetProfile", "index", index, PROFILES_COUNT);

  return profiles [index].name;
}

/*
    �������� �������
*/

IShader* GlslShaderManager::CreateShader (const ShaderDesc& desc, const LogFunction& error_log)
{    
  static const char* METHOD_NAME = "render::low_level::opengl::GlslShaderManager::CreateShader";

    //�������� ������������ ����������
    
  if (!desc.profile)
    throw xtl::make_null_argument_exception (METHOD_NAME, "desc.profile");
    
    //����������� ���� �������

  GLenum type = 0;
  size_t i;

  for (i=0; i<PROFILES_COUNT; i++)
    if (!strcmp (desc.profile, profiles [i].name))
    {
      type = profiles [i].shader_type;
      break;
    }

  if (i == PROFILES_COUNT)
    throw xtl::make_argument_exception (METHOD_NAME, "desc.profile", desc.profile);

    //�������� �������

  return new GlslShader (GetContextManager (), type, desc, error_log);
}

ICompiledProgram* GlslShaderManager::CreateProgram (size_t shaders_count, IShader** shaders, const LogFunction& error_log)
{
  if (!shaders)
    throw xtl::make_null_argument_exception ("render::low_level::opengl::GlslShaderManager::CreateProgram", "shaders");

  return new GlslProgram (GetContextManager (), shaders_count, shaders, error_log);
}

namespace render
{

namespace low_level
{

namespace opengl
{

/*
    �������� ��������� GLSL �������
*/

IShaderManager* create_glsl_shader_manager (const ContextManager& manager)
{
  return new GlslShaderManager (manager);
}

}

}

}

