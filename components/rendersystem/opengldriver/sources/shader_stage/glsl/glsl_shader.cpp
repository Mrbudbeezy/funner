#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   �����������
*/

GlslShader::GlslShader (const ContextManager& manager, const ShaderDesc& shader_desc, const LogFunction& error_log)
  : ContextObject (manager), shader (0)
{
  stl::string log_buffer;

  static const char* METHOD_NAME = "render::low_level::opengl::GlslShader::GlslShader";

  if (!shader_desc.profile)
    RaiseNullArgument (METHOD_NAME, "shader_desc.profile");
  
  if (!shader_desc.source_code)
    RaiseNullArgument (METHOD_NAME, "shader_desc.source_code");
  
  size_t shader_type = get_shader_type (shader_desc.profile);

  if (shader_type == -1)
    RaiseInvalidArgument (METHOD_NAME, "shader_desc.profile", shader_desc.profile, "Shader has uncknown type");

  MakeContextCurrent ();

  try
  {
    GLenum gl_shader_type;

    switch (shader_type)
    {
      case GlslShaderType_Fragment: gl_shader_type = GL_FRAGMENT_SHADER; break;
      case GlslShaderType_Vertex:   gl_shader_type = GL_VERTEX_SHADER;   break;
      default: Raise <NotImplementedException> (METHOD_NAME, "Shaders with profile '%s' not implemented", shader_desc.profile);
    }

    if (glCreateShader) shader = glCreateShader (gl_shader_type);
    else                shader = glCreateShaderObjectARB (gl_shader_type);

    if (!shader)
      RaiseError (METHOD_NAME);

    int compile_status = 0;

    if (glShaderSource) glShaderSource    (shader, 1, &((ShaderDesc&)shader_desc).source_code, (int*)&shader_desc.source_code_size);
    else                glShaderSourceARB (shader, 1, &((ShaderDesc&)shader_desc).source_code, (int*)&shader_desc.source_code_size);

    if (glCompileShader) glCompileShader    (shader);
    else                 glCompileShaderARB (shader);

    if (glGetShaderiv) glGetShaderiv             (shader, GL_COMPILE_STATUS, &compile_status);
    else               glGetObjectParameterivARB (shader, GL_COMPILE_STATUS, &compile_status);

    GetShaderLog (log_buffer);

    error_log (log_buffer.c_str ());

    if (!compile_status)
      RaiseError (METHOD_NAME);

    CheckErrors (METHOD_NAME);
  }
  catch (...)
  {
    DeleteShader ();
    throw;
  }
}

GlslShader::~GlslShader ()
{
  DeleteShader ();
}

/*
   �������� ��������
*/

void GlslShader::DeleteShader ()
{
  MakeContextCurrent ();

  if (shader)
  {
    if (glDeleteShader) glDeleteShader    (shader);
    else                glDeleteObjectARB (shader);
  }

  CheckErrors ("render::low_level::opengl::GlslShader::DeleteShader");
}

/*
   ��������� ���� OpenGL
*/

void GlslShader::GetShaderLog (stl::string& log_buffer)
{
  int log_length = 0;

  MakeContextCurrent ();

  if (glGetShaderiv) glGetShaderiv             (shader, GL_INFO_LOG_LENGTH, &log_length);
  else               glGetObjectParameterivARB (shader, GL_INFO_LOG_LENGTH, &log_length);

  if (log_length)
  {
    int getted_log_size = 0;

    log_buffer.resize (log_length - 1);

    if (glGetShaderInfoLog) glGetShaderInfoLog (shader, log_length, &getted_log_size, &log_buffer [0]);
    else                    glGetInfoLogARB    (shader, log_length, &getted_log_size, &log_buffer [0]);
    
    log_buffer.resize (getted_log_size - 1);
  }

  CheckErrors ("render::low_level::opengl::GlslShader::GetShaderLog");
}
