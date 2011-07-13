#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    �����������
*/

GlslProgram::GlslProgram (const ContextManager& manager, size_t shaders_count, IShader** in_shaders, const LogFunction& error_log)
  : ContextObject (manager),
    handle (0),
    last_bindable_program_id (0)
{
  static const char* METHOD_NAME = "render::low_level::opengl::GlslProgram::GlslProgram";

  try
  {
      //��������� �������� ���������

    MakeContextCurrent ();

      //�������� ���������

    if (glCreateProgram) handle = glCreateProgram ();
    else                 handle = glCreateProgramObjectARB ();

    if (!handle)
      RaiseError (METHOD_NAME);

      //������������� �������� � ���������

    shaders.reserve (shaders_count);

    for (size_t i=0; i<shaders_count; i++)
    {
      ShaderPtr shader = cast_object<GlslShader> (*this, in_shaders [i], METHOD_NAME, "shaders");

      if (!shader)
        throw xtl::format_exception<xtl::null_argument_exception> (METHOD_NAME, "shaders[%u]", i);

      if (glAttachShader) glAttachShader    (handle, shader->GetHandle ());
      else                glAttachObjectARB (handle, shader->GetHandle ());

      shaders.push_back (shader);
    }

      //�������� ���������

    GLint link_status = 0;

    if (glLinkProgram) glLinkProgram    (handle);
    else               glLinkProgramARB (handle);

      //���������������� ������

    if (glGetProgramiv) glGetProgramiv            (handle, GL_LINK_STATUS, &link_status);
    else                glGetObjectParameterivARB (handle, GL_LINK_STATUS, &link_status);

    stl::string log_buffer;

    GetProgramLog (log_buffer);
    error_log     (common::format ("linker: %s", log_buffer.c_str ()).c_str ());
    
      //��������� �������� ���������� ���������
    
    GLint parameters_count = 0, max_parameter_name_length = 0;
    
    glGetObjectParameterivARB (handle, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &parameters_count);
    glGetObjectParameterivARB (handle, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &max_parameter_name_length);
    
    parameters.reserve (parameters_count);        
    
    stl::string parameter_name;    
    
    for (GLint location=0; location<parameters_count; location++)
    {            
      parameter_name.fast_resize (max_parameter_name_length);      
      
      GLint  name_length = 0, elements_count = 0;
      GLenum type = 0;
      
      glGetActiveUniformARB (handle, location, parameter_name.size (), &name_length, &elements_count, &type, &parameter_name [0]);
      
      if ((size_t)name_length > parameter_name.size ())
        name_length = parameter_name.size ();
        
      if (name_length < 0)
        name_length = 0;
        
      parameter_name.fast_resize (name_length);

      if (strstr (parameter_name.c_str (), "[0]") == parameter_name.end () - 3)  //����� ����������� ���������� �������� � ��������� nVidia
        parameter_name.fast_resize (parameter_name.size () - 3);
      
      Parameter parameter;
      
      parameter.name           = parameter_name;
      parameter.name_hash      = common::strhash (parameter.name.c_str ());
      parameter.elements_count = (size_t)elements_count;
      
      switch (type)
      {
        case GL_FLOAT:
          parameter.type = ProgramParameterType_Float;
          break;
        case GL_FLOAT_VEC2_ARB:
          parameter.type = ProgramParameterType_Float2;
          break;
        case GL_FLOAT_VEC3_ARB:
          parameter.type = ProgramParameterType_Float3;
          break;
        case GL_FLOAT_VEC4_ARB:
          parameter.type = ProgramParameterType_Float4;
          break;
        case GL_INT:
          parameter.type = ProgramParameterType_Int;
          break;
        case GL_INT_VEC2_ARB:
          parameter.type = ProgramParameterType_Int2;
          break;        
        case GL_INT_VEC3_ARB:
          parameter.type = ProgramParameterType_Int3;
          break;        
        case GL_INT_VEC4_ARB:
          parameter.type = ProgramParameterType_Int4;
          break;
        case GL_BOOL_ARB:
        case GL_BOOL_VEC2_ARB:
        case GL_BOOL_VEC3_ARB:
        case GL_BOOL_VEC4_ARB:
          throw xtl::format_not_supported_exception (METHOD_NAME, "GLboolean uniform '%s' not supported (type=%04x)", parameter.name.c_str (), type);
        case GL_FLOAT_MAT2_ARB:
          parameter.type = ProgramParameterType_Float2x2;
          break;
        case GL_FLOAT_MAT3_ARB:
          parameter.type = ProgramParameterType_Float3x3;
          break;
        case GL_FLOAT_MAT4_ARB:
          parameter.type = ProgramParameterType_Float4x4;
          break;
        case GL_SAMPLER_1D_ARB:
        case GL_SAMPLER_2D_ARB:
        case GL_SAMPLER_3D_ARB:
        case GL_SAMPLER_CUBE_ARB:
        case GL_SAMPLER_1D_SHADOW_ARB:
        case GL_SAMPLER_2D_SHADOW_ARB:
        case GL_SAMPLER_2D_RECT_ARB:
        case GL_SAMPLER_2D_RECT_SHADOW_ARB:
          parameter.type = ProgramParameterType_Int;
          break;                
        default:
          throw xtl::format_operation_exception (METHOD_NAME, "Unknown uniform '%s' type %04x with %u element(s)", parameter.name.c_str (), type, elements_count);
      }
      
      parameters.push_back (parameter);
    }    

      //�������� ������

    if (!link_status)
      RaiseError (METHOD_NAME);

    CheckErrors (METHOD_NAME);
  }
  catch (...)
  {
    DeleteProgram ();
    throw;
  }
}

GlslProgram::~GlslProgram ()
{
  try
  {
    if (TryMakeContextCurrent ())
      DeleteProgram ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::GlslProgram::~GlslProgram");

    LogPrintf ("%s", exception.what ());
  }
  catch (std::exception& exception)
  {
    LogPrintf ("%s", exception.what ());
  }
  catch (...)
  {
    //��������� ��� ����������
  }
}

/*
    �������� ���������
*/

void GlslProgram::DeleteProgram ()
{
  if (!handle)
    return;

    //��������� �������� ���������

  MakeContextCurrent ();

    //������������ �������� �� ���������

  for (ShaderArray::iterator iter=shaders.begin (), end=shaders.end (); iter!=end; ++iter)
  {
    if (glDetachShader) glDetachShader    (handle, (*iter)->GetHandle ());
    else                glDetachObjectARB (handle, (*iter)->GetHandle ());
  }

    //�������� ���������

  if (glDeleteProgram) glDeleteProgram   (handle);
  else                 glDeleteObjectARB (handle);

    //�������� ������

  CheckErrors ("render::low_level::opengl::GlslProgram::DeleteProgram");
}

/*
    ��������� ��������� ������ OpenGL
*/

void GlslProgram::GetProgramLog (stl::string& log_buffer)
{
    //������� ������

  log_buffer.clear ();

    //��������� �������� ���������

  MakeContextCurrent ();

    //��������� ������� ������ ��������� ���������

  GLint log_length = 0;

  if (glGetProgramiv) glGetProgramiv            (handle, GL_INFO_LOG_LENGTH, &log_length);
  else                glGetObjectParameterivARB (handle, GL_INFO_LOG_LENGTH, &log_length);

  if (!log_length)
    return;

    //������ ������ ���������

  GLsizei getted_log_size = 0;

  log_buffer.resize (log_length - 1);

  if (glGetProgramInfoLog) glGetProgramInfoLog (handle, log_length, &getted_log_size, &log_buffer [0]);
  else                     glGetInfoLogARB     (handle, log_length, &getted_log_size, &log_buffer [0]);

  if (getted_log_size)
    log_buffer.resize (getted_log_size - 1);

    //�������� ������

  CheckErrors ("render::low_level::opengl::GlslProgram::GetProgramLog");
}

/*
    �������� ���������, ��������������� � �������� OpenGL
*/

IBindableProgram* GlslProgram::CreateBindableProgram (ProgramParametersLayout* layout)
{
  try
  {
    return new GlslBindableProgram (GetContextManager (), *this, layout);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::GlslProgram::CreateBindableProgram");
    throw;
  }
}

/*
    ��������� ���������� ���������
*/

size_t GlslProgram::GetParametersCount ()
{
  return parameters.size ();
}

const GlslProgram::Parameter* GlslProgram::GetParameters ()
{
  return parameters.empty () ? (const Parameter*)0 : &parameters [0];
}
