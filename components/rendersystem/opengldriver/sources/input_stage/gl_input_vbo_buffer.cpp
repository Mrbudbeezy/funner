#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///����������� / ����������
VboBuffer::VboBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& desc)
  : Buffer(context_manager, desc), target (in_target)
{
  MakeContextCurrent();

  glGenBuffers(1, &buffer_id);          // ������������� ����� ������
  
  if (!buffer_id)
    return;
  
  glBindBuffer(target, buffer_id);      // ������� ��� ��������
  
  size_t buffermode;                    // ���������� ��� ����� ������
  
  switch (buffer_desc.usage_mode)       // ��� ���������, ��� ������ ���� ������-����� - ��������� ����
  {
    case UsageMode_Default:             // �� ������� ������ �������, ��� �������������, � ������ 
      switch (buffer_desc.access_flags) // ���� ������ ����� ������! ������ ��� � ���������� ��� ���
      {
        case AccessFlag_Read:
          buffermode = GL_DYNAMIC_READ;
          break;
        case AccessFlag_Write:
          buffermode = GL_DYNAMIC_DRAW;
          break;
        case AccessFlag_Read | AccessFlag_Write:
          buffermode = GL_DYNAMIC_COPY;
          break;
      }
      break;
    case UsageMode_Static:              // ����������� �����
      switch (buffer_desc.access_flags)
      {
        case AccessFlag_Read:
          buffermode = GL_STATIC_READ;
          break;
        case AccessFlag_Write:
          buffermode = GL_STATIC_DRAW;
          break;
        case AccessFlag_Read | AccessFlag_Write:
          buffermode = GL_STATIC_COPY;
          break;
      }
      break;
    case UsageMode_Dynamic:             // ������������ �����
      switch (buffer_desc.access_flags)
      {
        case AccessFlag_Read:
          buffermode = GL_DYNAMIC_READ;
          break;
        case AccessFlag_Write:
          buffermode = GL_DYNAMIC_DRAW;
          break;
        case AccessFlag_Read | AccessFlag_Write:
          buffermode = GL_DYNAMIC_COPY;
          break;
      }
      break;
    case UsageMode_Stream:              // �������� �����
      switch (buffer_desc.access_flags)
      {
        case AccessFlag_Read:
          buffermode = GL_STREAM_READ;
          break;
        case AccessFlag_Write:
          buffermode = GL_STREAM_DRAW;
          break;
        case AccessFlag_Read | AccessFlag_Write:
          buffermode = GL_STREAM_COPY;
          break;
      }
      break;  
  }
  // �������������� �����
  glBufferData(target, buffer_desc.size, NULL, buffermode);
  
  CheckErrors("render::low_level::opengl::VboBuffer::VboBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& desc)");
}

VboBuffer::~VboBuffer ()
{
  MakeContextCurrent();
  glDeleteBuffers(1, &buffer_id);     // ���������� �����

  CheckErrors("render::low_level::opengl::VboBuffer::~VboBuffer ()");
}

///������ � ������� ������
void VboBuffer::SetData (size_t offset, size_t size, const void* data)
{
  Bind();
  
  if (offset >= buffer_desc.size)
    return;
  
  size = offset + size > buffer_desc.size ?   // ������ �������
           buffer_desc.size - offset : size;

  glBufferSubData(target, offset, size, data);

  CheckErrors("render::low_level::opengl::VboBuffer::SetData (size_t offset, size_t size, const void* data)");
}

void VboBuffer::GetData (size_t offset, size_t size, void* data)
{
  Bind();

  if (offset >= buffer_desc.size)
    return;
  
  size = offset + size > buffer_desc.size ?   // ������ �������
           buffer_desc.size - offset : size;

  glGetBufferSubData(target, offset, size, data);

  CheckErrors("render::low_level::opengl::VboBuffer::GetData (size_t offset, size_t size, void* data)");
}

///��������� ������ � �������� OpenGL
void VboBuffer::Bind ()
{
  MakeContextCurrent();
  glBindBuffer(target, buffer_id);

  CheckErrors("render::low_level::opengl::VboBuffer::Bind ()");
}

///C������� �� ������ ������ (��� ���������� �������)
void* VboBuffer::GetDataPointer ()
{
  return 0; //������������ �������� �� ������ ������
}
