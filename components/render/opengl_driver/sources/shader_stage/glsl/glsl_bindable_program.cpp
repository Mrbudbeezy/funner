#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

#ifdef min
#undef min
#endif

/*
    �������� ��������� � ������ ���������� ���������
*/

struct GlslBindableProgram::Parameter
{
  int                  location;  //��� ���������
  ProgramParameterType type;      //��� ���������
  unsigned int         count;     //���������� ��������� �������
  unsigned int         offset;    //�������� ������������ ������ ������������ ������
};

struct GlslBindableProgram::Group
{
  unsigned int slot;       //����� ����� � ���������� �������
  size_t       data_hash;  //��� ������ ������������ ������
  unsigned int count;      //���������� ��������� ������
  Parameter*   parameters; //��������� �� ������ ������� � ����������
};

/*
    �����������
*/

GlslBindableProgram::GlslBindableProgram
 (const ContextManager&    manager,
  GlslProgram&             in_program,
  ProgramParametersLayout* parameters_layout)
   : ContextObject (manager),
     program (in_program)
{
  try
  {
    const GlslProgram::Parameter* uniforms       = program.GetParameters ();
    unsigned int                  uniforms_count = program.GetParametersCount ();

    stl::vector<int> uniform_linked (uniforms_count);

    if (parameters_layout && parameters_layout->GetParametersCount ())
    {
        //�������������� ����� ��� �������� ���������� � ����� ����������

      parameters.reserve (parameters_layout->GetParametersCount ());
      groups.reserve     (parameters_layout->GetGroupsCount ());

        //��������� ������� ����������� �������� ���������

      const ContextCaps& caps = GetCaps ();  

        //�������������� ����������
        
      GLhandleARB handle = program.GetHandle ();

      for (size_t i=0, count=parameters_layout->GetGroupsCount (); i<count; i++)
      {
        size_t start_parameters_count = parameters.size ();

        const ProgramParameterGroup& src_group = parameters_layout->GetGroup (i);

        for (size_t j=0; j<src_group.count; j++)
        {          
          const ProgramParameter& src_parameter = src_group.parameters [j];
          Parameter               dst_parameter;

          dst_parameter.location = caps.glGetUniformLocation_fn (handle, src_parameter.name);      

          if (dst_parameter.location == -1)
            continue; //���������� ���������, ������������� � ���������

          size_t                        name_hash  = common::strhash (src_parameter.name);
          const GlslProgram::Parameter* uniform    = 0;
            
          for (unsigned int k=0; k<uniforms_count; k++)
          {
            if (uniforms [k].name_hash == name_hash && uniforms [k].name == src_parameter.name)
            {
              uniform = &uniforms [k];
              
              if (uniform->type != src_parameter.type)
                throw xtl::format_operation_exception ("", "Layout parameter '%s' type %s mismatch with program uniform parameter type %s", src_parameter.name,
                  get_name (src_parameter.type), get_name (uniform->type));
                  
              if (uniform->elements_count > src_parameter.count)
                throw xtl::format_operation_exception ("", "Layout parameter '%s' with type %s has %u elements but program uniform parameter type required at least %u elements", src_parameter.name,
                  get_name (src_parameter.type), src_parameter.count, uniform->elements_count);

              break;
            }
          }

          if (uniform)
            uniform_linked [uniform - uniforms] = true;

          dst_parameter.type   = src_parameter.type;
          dst_parameter.count  = uniform ? stl::min (src_parameter.count, uniform->elements_count) : src_parameter.count; //������������ ���������� ����� ��������� ������ ����� ����������, ��� ��������� ���������
          dst_parameter.offset = src_parameter.offset;

            //���������� ������ ���������

          parameters.push_back (dst_parameter);
        }

          //���������� ����� ������

        Group dst_group;

        dst_group.slot       = src_group.slot;
        dst_group.data_hash  = 0;
        dst_group.count      = (unsigned int)(parameters.size () - start_parameters_count);
        dst_group.parameters = &parameters [start_parameters_count];

        if (!dst_group.count)
          continue; //���������� ������ ������

        groups.push_back (dst_group);
      }    
    }
    
      //�������� ����������� ���� ���������� ���������

    for (unsigned int i=0; i<uniforms_count; i++)
      if (!uniform_linked [i])
        throw xtl::format_operation_exception ("", "Program uniform parameter '%s' with type %s and elements count %u has not found in program parameters layout", uniforms [i].name.c_str (),
          get_name (uniforms [i].type), uniforms [i].elements_count);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::GlslBindableProgram::GlslBindableProgram");
    throw;
  }
}

/*
    ��������� � �������� OpenGL
*/

void GlslBindableProgram::Bind (ConstantBufferPtr* constant_buffers)
{
  static const char* METHOD_NAME = "render::low_level::opengl::GlslBindableProgram::Bind";

    //�������� ������������ ����������

  if (!constant_buffers)
    throw xtl::make_null_argument_exception (METHOD_NAME, "constant_buffers");

    //��������� �������� ���������

  MakeContextCurrent ();
  
    //��������� ������ �� ���� ���������

  const ContextCaps& caps            = GetCaps ();
  const size_t       current_program = GetContextCacheValue (CacheEntry_UsedProgram);

  if (current_program != program.GetId ())
  {
      //��������� ��������� �������� � �������� OpenGL

    caps.glUseProgram_fn (program.GetHandle ());    

    SetContextCacheValue (CacheEntry_UsedProgram, program.GetId ());
  }
  
    //���������� ���������� � ������� ��������� (�� ��������� ���������� ����������� glUniform �� ������ GlslBindableProgram ��������� � GlslProgram ������ ���������)
    
  bool use_cache = program.GetLastBindableProgramId () == GetId ();
    
  program.SetLastBindableProgramId (GetId ());

    //��������� ���������� ��������� � �������� OpenGL

  for (GroupArray::iterator iter=groups.begin (), end=groups.end (); iter!=end; ++iter)
  {
    Group& group = *iter;

    if (!constant_buffers [group.slot])
      throw xtl::format_operation_exception (METHOD_NAME, "Null constant buffer #%u", group.slot);
      
    size_t constant_buffer_hash = constant_buffers [group.slot]->GetDataHash ();

    if (use_cache && group.data_hash == constant_buffer_hash)
      continue;

      //���������� ���� ���������� ������

    group.data_hash = constant_buffer_hash;

      //��������� ����������

    char* buffer_base = (char*)constant_buffers [group.slot]->GetDataPointer ();

    for (size_t j=0; j<group.count; j++)
    {
      const Parameter& parameter      = group.parameters [j];
      void*            parameter_data = buffer_base + parameter.offset;      

      switch (parameter.type)
      {
        case ProgramParameterType_Int:      caps.glUniform1iv_fn       (parameter.location, parameter.count, (GLint*)parameter_data);    break;
        case ProgramParameterType_Float:    caps.glUniform1fv_fn       (parameter.location, parameter.count, (float*)parameter_data);    break;
        case ProgramParameterType_Int2:     caps.glUniform2iv_fn       (parameter.location, parameter.count, (GLint*)parameter_data);    break;
        case ProgramParameterType_Float2:   caps.glUniform2fv_fn       (parameter.location, parameter.count, (float*)parameter_data);    break;
        case ProgramParameterType_Int3:     caps.glUniform3iv_fn       (parameter.location, parameter.count, (GLint*)parameter_data);    break;
        case ProgramParameterType_Float3:   caps.glUniform3fv_fn       (parameter.location, parameter.count, (float*)parameter_data);    break;
        case ProgramParameterType_Int4:     caps.glUniform4iv_fn       (parameter.location, parameter.count, (GLint*)parameter_data);    break;
        case ProgramParameterType_Float4:   caps.glUniform4fv_fn       (parameter.location, parameter.count, (float*)parameter_data);    break;
#ifndef OPENGL_ES2_SUPPORT
        case ProgramParameterType_Float2x2: caps.glUniformMatrix2fv_fn (parameter.location, parameter.count, GL_TRUE, (float*)parameter_data); break;
        case ProgramParameterType_Float3x3: caps.glUniformMatrix3fv_fn (parameter.location, parameter.count, GL_TRUE, (float*)parameter_data); break;
        case ProgramParameterType_Float4x4: caps.glUniformMatrix4fv_fn (parameter.location, parameter.count, GL_TRUE, (float*)parameter_data); break;
#else
        case ProgramParameterType_Float2x2: caps.glUniformMatrix2fv_fn (parameter.location, parameter.count, GL_FALSE, GetTransposedMatrixes<2> (parameter.count, (float*)parameter_data)); break;
        case ProgramParameterType_Float3x3: caps.glUniformMatrix3fv_fn (parameter.location, parameter.count, GL_FALSE, GetTransposedMatrixes<3> (parameter.count, (float*)parameter_data)); break;
        case ProgramParameterType_Float4x4: caps.glUniformMatrix4fv_fn (parameter.location, parameter.count, GL_FALSE, GetTransposedMatrixes<4> (parameter.count, (float*)parameter_data)); break;
#endif
        default: break;
      }
    }
  }  

    //�������� ������

  CheckErrors (METHOD_NAME);
}

/*
   ��������� ����������������� ������
*/

template <unsigned int Size>
float* GlslBindableProgram::GetTransposedMatrixes (unsigned int count, float* data)
{
  float* buffer = (float*)GetContextManager ().GetTempBuffer (count * sizeof (math::matrix<float, Size>));

  math::matrix<float, Size> *out_matrix = (math::matrix<float, Size>*)buffer,
                            *in_matrix  = (math::matrix<float, Size>*)data;

  for (unsigned int i = 0; i < count; i++, out_matrix++, in_matrix++)
    *out_matrix = math::transpose (*in_matrix);

  return buffer;
}

/*
    �������� ������������ ��������� ���������� ���������
*/

void GlslBindableProgram::Validate ()
{
  try
  {
    GLhandleARB handle = program.GetHandle ();
    
      //�������� ��������� ���������

    if (glValidateProgram)
    {
      glValidateProgram (handle);
    }
#ifndef OPENGL_ES2_SUPPORT
    else
    {
      glValidateProgramARB (handle);
    }
#endif
    
    GLint status = 0;

    if (glGetProgramiv)
    {
      glGetProgramiv (handle, GL_OBJECT_VALIDATE_STATUS_ARB, &status);
    }
#ifndef OPENGL_ES2_SUPPORT
    else
    {
      glGetObjectParameterivARB (handle, GL_OBJECT_VALIDATE_STATUS_ARB, &status);
    }
#endif
    
      //�������� ������
    
    CheckErrors ("");          
    
    if (!status)
    {
      stl::string log_buffer;
      
      program.GetProgramLog (log_buffer);      
      
      throw xtl::format_operation_exception ("", "GLSL program validation failed. %s", log_buffer.c_str ());
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::GlslBindableProgram::Validate");
    throw;
  }
}
