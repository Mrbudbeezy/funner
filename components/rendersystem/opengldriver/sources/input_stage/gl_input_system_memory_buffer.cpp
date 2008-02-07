#include "shared.h"
#include <memory.h>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� - SystemMemoryBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

SystemMemoryBuffer::SystemMemoryBuffer (const ContextManager& context_manager, const BufferDesc& desc)
  : Buffer(context_manager, desc), buffer (buffer_desc.size)
  {}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
}

///������ � ������� ������
void SystemMemoryBuffer::SetData (size_t offset, size_t size, const void* data)
{
   // ��������� ��������
  if (offset >= buffer_desc.size)
    return;

    // ������ �������
  size = offset + size > buffer_desc.size ? buffer_desc.size - offset : size;
  // ���� �����������
  memcpy (buffer.data () + offset, data, size);
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
    // ��������� ��������
  if (offset >= buffer_desc.size)
    return;

   // ������ �������
  size = offset + size > buffer_desc.size ? buffer_desc.size - offset : size;
  // �����������
  memcpy (data, buffer.data () + offset, size);
}

///��������� ������ � �������� OpenGL
void SystemMemoryBuffer::Bind ()
{
  //��������� �� ���������
}

///��������� �� ������ ������
void* SystemMemoryBuffer::GetDataPointer ()
{
  return buffer.data ();  // ���������� ������ ������
}
