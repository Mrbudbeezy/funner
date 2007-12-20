#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///����������� / ����������
VboBuffer::VboBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& desc)
  : bufferdesc (desc), ContextObject (context_manager), target (in_target)
{
  MakeContextCurrent();

  glGenBuffers(1, &buffer_id);          // ������������� ����� ������
  
  glBindBuffer(target, buffer_id);      // ������� ��� ��������
  
  size_t buffermode;                    // ���������� ��� ����� ������
  
  switch (bufferdesc.usage_mode)        // ��� ���������, ��� ������ ���� ������-����� - ��������� ����
  {
    case UsageMode_Default:             // �� ������� ������ �������, ��� �������������, � ������ 
      switch (bufferdesc.access_flags)  // ���� ������ ����� ������! ������ ��� � ���������� ��� ���
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
      switch (bufferdesc.access_flags)
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
      switch (bufferdesc.access_flags)
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
      switch (bufferdesc.access_flags)
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
  glBufferData(target, bufferdesc.size, NULL, buffermode);
}

VboBuffer::~VboBuffer ()
{
  try
  {
    MakeContextCurrent();
    glDeleteBuffers(1, &buffer_id);     // ���������� �����
  }
  catch (...)
  { }
}

///������ � ������� ������
void VboBuffer::SetData (size_t offset, size_t size, const void* data)
{
  Bind();
  
  glBufferSubData(target, offset, size, data);
}

void VboBuffer::GetData (size_t offset, size_t size, void* data)
{
  Bind();

  glGetBufferSubData(target, offset, size, data);
}

///��������� ������ � �������� OpenGL
void VboBuffer::Bind ()
{
  MakeContextCurrent();
  glBindBuffer(target, buffer_id);
}

///C������� �� ������ ������ (��� ���������� �������)
void* VboBuffer::GetDataPointer ()
{
  return 0; //������������ �������� �� ������ ������
}
