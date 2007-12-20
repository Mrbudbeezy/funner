#include "shared.h"
#include <memory.h>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� - SystemMemoryBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

SystemMemoryBuffer::SystemMemoryBuffer (const BufferDesc& desc)
  : bufferdesc(desc)
{
  buffer = (void*)new char[bufferdesc.size]; // ����������� ������
}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
  delete[] buffer;                          // ������ �� �� �����
}
  
///������ � ������� ������
void SystemMemoryBuffer::SetData (size_t offset, size_t size, const void* data)
{
  if (offset < 0 || offset >= bufferdesc.size)      // ��������� ��������
    return;
  
  char* begin = (char*)buffer + offset;             // ������ �������
  size        = offset + size > bufferdesc.size ?   // ������ �������
                  bufferdesc.size - offset : size;
  // ���� �����������
  memcpy((void*) begin, data, size);
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
  if (offset < 0 || offset >= bufferdesc.size)      // ��������� ��������
    return;

  char* begin = (char*)buffer + offset;
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
