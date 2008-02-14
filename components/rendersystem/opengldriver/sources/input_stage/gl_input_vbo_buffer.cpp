#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

//check for ARB or version 1.5!!!!!!

/*
    ����������� / ����������
*/

VboBuffer::VboBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& in_desc)
  : Buffer (context_manager, in_desc), target (in_target), buffer_id (0)
{
  try
  {
      //����������� ������ ������������� ������

    GLenum gl_usage_mode;    

    switch (in_desc.usage_mode)
    {
      case UsageMode_Static:
        gl_usage_mode = GL_STATIC_DRAW;
        break;
      case UsageMode_Default:
      case UsageMode_Dynamic:
        gl_usage_mode = GL_DYNAMIC_DRAW;
        break;
      case UsageMode_Stream:
        gl_usage_mode = GL_STREAM_DRAW;
        break;  
      default:
        RaiseInvalidArgument ("", "desc.usage_mode", in_desc.usage_mode);
        break;
    }

      //����� �������� ���������

    MakeContextCurrent ();

      //�������� ������
      
        //ARB !ARB!!!!

    glGenBuffers (1, &buffer_id);

    if (!buffer_id)
      RaiseError ("");
      
    glBindBuffer (target, buffer_id);    
    glBufferData (target, in_desc.size, 0, gl_usage_mode);
    
    CheckErrors ("");
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::VboBuffer::VboBuffer");
    throw;
  }
}

VboBuffer::~VboBuffer ()
{
    //�������� �� ����������!!!!

  MakeContextCurrent();
  glDeleteBuffers(1, &buffer_id);     // ���������� �����

  CheckErrors("render::low_level::opengl::VboBuffer::~VboBuffer ()");
}

/*
    ��������� / ������ ������ ����� ���������
*/

void VboBuffer::SetDataCore (size_t offset, size_t size, const void* data)
{
  Bind ();
  
  glBufferSubData (target, offset, size, data);

  CheckErrors ("render::low_level::opengl::VboBuffer::SetData");
}

void VboBuffer::GetDataCore (size_t offset, size_t size, void* data)
{
  Bind ();

  glGetBufferSubData (target, offset, size, data);

  CheckErrors ("render::low_level::opengl::VboBuffer::GetData");
}

/*
    ��������� ������ � �������� OpenGL
*/

void VboBuffer::Bind ()
{
  MakeContextCurrent ();
  
  glBindBuffer (target, buffer_id);

  CheckErrors ("render::low_level::opengl::VboBuffer::Bind");
}

/*
    ��������� ��������� �� ������ ������
*/

void* VboBuffer::GetDataPointer ()
{
  return 0; //������������ �������� �� ������ ������
}
