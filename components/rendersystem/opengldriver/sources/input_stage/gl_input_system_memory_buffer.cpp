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
  buffer = (void*)new char[buffer_desc.size]; // ����������� ������
}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
  delete[] buffer;                          // ������ ��� �� �����
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
  memcpy((void*) begin, data, size);
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
  if (offset >= buffer_desc.size)      // ��������� ��������
    return;

  char* begin = (char*)buffer + offset;
  size        = offset + size > buffer_desc.size ?   // ������ �������
                  buffer_desc.size - offset : size;
  // �����������
  memcpy(data, (void*) begin, size);
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
