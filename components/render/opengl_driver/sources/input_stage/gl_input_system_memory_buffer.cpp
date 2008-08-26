#include "shared.h"

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

  need_data_hash_recalc = true;
}

void SystemMemoryBuffer::GetDataCore (size_t offset, size_t size, void* data)
{
  memcpy (data, buffer.data () + offset, size);
}

/*
    ��������� ��������� �� ������ ������
*/

void* SystemMemoryBuffer::GetDataPointer ()
{
  return buffer.data ();
}

/*
   ��������� ���� ������ (0 - ������ �� ����������)
*/

size_t SystemMemoryBuffer::GetDataHash ()
{
  if (!need_data_hash_recalc)
    return data_hash;

  data_hash = crc32 (buffer.data (), buffer.size ());
  need_data_hash_recalc = false;

  return data_hash;
}
