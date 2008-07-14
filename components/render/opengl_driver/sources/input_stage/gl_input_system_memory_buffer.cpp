#include "shared.h"
#include <memory.h>

#include <common/hash.h>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

SystemMemoryBuffer::SystemMemoryBuffer (const ContextManager& context_manager, const BufferDesc& in_desc, bool in_need_hash)
  : Buffer (context_manager, in_desc), buffer (in_desc.size), need_hash (in_need_hash)
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
    ��������� ������ � �������� OpenGL
*/

void SystemMemoryBuffer::Bind ()
{
    //������! ����� ������ ���� ����� �������� ������ � 0!!!

  GetContextDataTable (Stage_Input)[InputStageCache_BindedVboBuffer] = 0; //?????
  
    //��������� �� ���������
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
  if (!need_hash)
    return 0;

  if (need_data_hash_recalc)
  {
    data_hash = crc32 (buffer.data (), buffer.size ());
    need_data_hash_recalc = false;
  }

  return data_hash;
}
