#include "shared.h"
#include <memory.h>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

SystemMemoryBuffer::SystemMemoryBuffer (const ContextManager& context_manager, const BufferDesc& in_desc)
  : Buffer (context_manager, in_desc), buffer (in_desc.size)
  {}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
}

/*
    ��������� / ������ ������ ����� ���������
*/

void SystemMemoryBuffer::SetDataCore (size_t offset, size_t size, const void* data)
{
  memcpy (buffer.data () + offset, data, size);
}

void SystemMemoryBuffer::GetDataCore (size_t offset, size_t size, void* data)
{
  memcpy (data, buffer.data () + offset, size);
}

/*
    ��������� ������ � �������� OpenGL
*/

void SystemMemoryBuffer::Bind ()
{
  //��������� �� ���������
}

/*
    ��������� ��������� �� ������ ������
*/

void* SystemMemoryBuffer::GetDataPointer ()
{
  return buffer.data ();
}
