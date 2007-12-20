#include "shared.h"

/*
    ���������
      1) ����� ������ ������ ������ GL ����� ������ MakeContextCurrent ()
         � ����������� ���� �����, �� ���������� ���� ����� � try { MakeContextCurrent (); gl-commands; } catch (...) { nothing }
      2) ����� ���������� ������� � ������� ������ const � proteced
      3) ��������� enum-��: BufferAccessFlag -> AccessFlag, BufferUsageMode -> UsageMode - ��� ����� � ������� � ���������
      4) �� ����� ����� Map/Unmap. ��� ���� ������ Set/Get. ���� ��� ����� ���������� ���������
*/

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///����������� / ����������
VboBuffer::VboBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& desc)
  : Buffer (desc), ContextObject (context_manager), target (in_target)
{ 
  glGenBuffers(1, &buffer_id);            // ������������� ����� ������
  
  glBindBuffer(target, buffer_id);     // ������� ��� ��������
  
  BufferDesc bd;                          // ����� ����������! ������!
  GetDesc(bd);
  
  size_t buffermode;                      // ���������� ��� ����� ������
  
  switch (bd.usage_mode)                  // ��� ���������, ��� ������ ���� ������-����� - ��������� ����
  {
    case UsageMode_Default:         // �� ������� ������ �������, ��� �������������, � ������ 
      switch (bd.access_flags)            // ���� ������ ����� ������! ������ ��� � ���������� ��� ���
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
    case UsageMode_Static:          // ����������� �����
      switch (bd.access_flags)
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
    case UsageMode_Dynamic:         // ������������ �����
      switch (bd.access_flags)
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
    case UsageMode_Stream:          // �������� �����
      switch (bd.access_flags)
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
  
  glBufferData(target, bd.size, NULL, buffermode); // �������������� �����
}

VboBuffer::~VboBuffer ()
{
  glDeleteBuffers(1, &buffer_id);                     // ���������� �����
}

///������ � ������� ������
void VboBuffer::SetData (size_t offset, size_t size, const void* data)
{
  Bind();
  
  glBufferSubData(target, offset, size, data);
}

void VboBuffer::GetData (size_t offset, size_t size, void* data)
{

  BufferDesc bd;                                // � ����� ����������
  GetDesc(bd);

  if (offset < 0 || offset >= bd.size)          // ��������
    return;

  Bind();

  void* buffer;
  buffer = glMapBuffer(target, GL_READ_ONLY);   // ����� �����
  
  // ���������
  char* begin = (char*)buffer + offset;
  char* end   = (char*)buffer + offset + size;
  char* ptr   = (char*)data;
  // �����������
  for (; begin < end; begin++, ptr++)
    *ptr = *begin;
  
  glUnmapBuffer(target);                        // ������� �����
}

///��������� ������ � �������� OpenGL
void VboBuffer::Bind ()
{
  glBindBuffer(target, buffer_id);
}

///C������� �� ������ ������ (��� ���������� �������)
void* VboBuffer::GetDataPointer ()
{
  return 0; //������������ �������� �� ������ ������
}
