#include "shared.h"
#include <memory.h>

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   �����������
*/

ConstantBuffer::ConstantBuffer (const ContextManager& manager, const BufferDesc& in_desc)
  : ContextObject (manager), buffer (in_desc.size), desc (in_desc)
{
}

/*
   ��������� �����������
*/

void ConstantBuffer::GetDesc (BufferDesc& target_desc)
{
  target_desc = desc;
}

/*
   ������ / ������ �� ������
*/

void ConstantBuffer::SetData (size_t offset, size_t size, const void* data)
{
   // ��������� ��������
  if (offset >= desc.size)
    return;

    // ������ �������
  size = offset + size > desc.size ? desc.size - offset : size;
  // ���� �����������
  memcpy (buffer.data () + offset, data, size);
}

void ConstantBuffer::GetData (size_t offset, size_t size, void* data)
{
    // ��������� ��������
  if (offset >= desc.size)
    return;

   // ������ �������
  size = offset + size > desc.size ? desc.size - offset : size;
  // �����������
  memcpy (data, buffer.data () + offset, size);
}
