#include "shared.h"
#include <memory.h>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� - SystemMemoryBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

SystemMemoryBuffer::SystemMemoryBuffer (const BufferDesc& desc)
  : buffer_desc(desc)
{
  try
  {
    buffer = new char [buffer_desc.size]; // ����������� ������
  }
  catch (common::Exception& exception)        // ����� ��� ����������
  {
    exception.Touch("render::low_level::opengl::SystemMemoryBuffer::SystemMemoryBuffer ()");
    throw;
  }
}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
  try
  {
    delete [] buffer;
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

void SystemMemoryBuffer::GetDesc(BufferDesc& desc)
{
  desc = buffer_desc;
}
  
///������ � ������� ������
void SystemMemoryBuffer::SetData (size_t offset, size_t size, const void* data)
{
  if (offset >= buffer_desc.size)      // ��������� ��������
    return;
  
  char* begin = (char*)buffer + offset;             // ������ �������
  size        = offset + size > buffer_desc.size ?   // ������ �������
                buffer_desc.size - offset : size;
  // ���� �����������
  memcpy (begin, data, size);
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
  if (offset >= buffer_desc.size)      // ��������� ��������
    return;

  char* begin = (char*)buffer + offset;
  size        = offset + size > buffer_desc.size ?   // ������ �������
                buffer_desc.size - offset : size;
  // �����������
  memcpy (data, begin, size);
}

///��������� ������ � �������� OpenGL
void SystemMemoryBuffer::Bind ()
{
  //��������� �� ���������
}

///��������� �� ������ ������
void* SystemMemoryBuffer::GetDataPointer ()
{
  return buffer;  // ���������� ������ ������
}
