#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

SystemMemoryBuffer::SystemMemoryBuffer (const ContextManager& context_manager, const BufferDesc& in_desc)
  : Buffer (context_manager, in_desc), buffer (in_desc.size)
{
}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
}

/*
    Установка / чтение данных после отсечения
*/

void SystemMemoryBuffer::SetDataCore (unsigned int offset, unsigned int size, const void* data)
{
  memcpy (buffer.data () + offset, data, size);

  need_data_hash_recalc = true;
}

void SystemMemoryBuffer::GetDataCore (unsigned int offset, unsigned int size, void* data)
{
  memcpy (data, buffer.data () + offset, size);
}

/*
    Получение указателя на начало буфера
*/

void* SystemMemoryBuffer::GetDataPointer ()
{
  return buffer.data ();
}

/*
   Получение хеша данных (0 - данные не хешируются)
*/

size_t SystemMemoryBuffer::GetDataHash ()
{
  if (!need_data_hash_recalc)
    return data_hash;

  data_hash = crc32 (buffer.data (), buffer.size ());
  need_data_hash_recalc = false;

  return data_hash;
}
