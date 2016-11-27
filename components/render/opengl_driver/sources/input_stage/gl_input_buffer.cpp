#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор
*/

Buffer::Buffer (const ContextManager& context_manager, const BufferDesc& in_desc)
  : ContextObject (context_manager), desc (in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Buffer::Buffer";

    //проверка корректности режима использования буфера

  switch (desc.usage_mode)
  {
    case UsageMode_Default:
    case UsageMode_Static:
    case UsageMode_Dynamic:
    case UsageMode_Stream:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.usage_mode", desc.usage_mode);
  }

    //проверка корректности флагов доступа

  switch (desc.access_flags)
  {
    case 0:
    case AccessFlag_Read:
    case AccessFlag_Write:
    case AccessFlag_Read | AccessFlag_Write:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.access_flags", (size_t)desc.access_flags);
  }
}

/*
    Получение дескриптора
*/

void Buffer::GetDesc (BufferDesc& out_desc)
{
  out_desc = desc;
}

/*
    Работа с данными буфера
*/

void Buffer::SetData (unsigned int offset, unsigned int size, const void* data, IDeviceContext* context)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Buffer::SetData";

  if (context)
    throw xtl::format_operation_exception (METHOD_NAME, "Context must be null");

    //проверка возможности установки данных
    
  if (!(desc.access_flags & AccessFlag_Write))
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set buffer data (no AccessFlag_Write in desc.access_flags)");

    //отсечение

  if (offset >= desc.size)
    return;
    
  if (desc.size - offset < size)
    size = desc.size - offset;
    
  if (!size)
    return;

    //проверка корректности переданных данных

  if (!data)
    throw xtl::make_null_argument_exception (METHOD_NAME, "data");

    //установка данных

  SetDataCore (offset, size, data);
}

void Buffer::GetData (unsigned int offset, unsigned int size, void* data, IDeviceContext* context)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Buffer::GetData";

  if (context)
    throw xtl::format_operation_exception (METHOD_NAME, "Context must be null");

    //проверка возможности установки данных
    
  if (!(desc.access_flags & AccessFlag_Read))
    throw xtl::format_operation_exception (METHOD_NAME, "Can't get buffer data (no AccessFlag_Read in desc.access_flags)");

    //отсечение

  if (offset >= desc.size)
    return;

  if (desc.size - offset < size)
    size = desc.size - offset;

  if (!size)
    return;

    //проверка корректности результирующего буфера

  if (!data)
    throw xtl::make_null_argument_exception (METHOD_NAME, "data");

    //чтение данных

  GetDataCore (offset, size, data);
}
