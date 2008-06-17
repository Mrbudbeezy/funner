#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    �����������
*/

Buffer::Buffer (const ContextManager& context_manager, const BufferDesc& in_desc)
  : ContextObject (context_manager), desc (in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Buffer::Buffer";

    //�������� ������������ ������ ������������� ������

  switch (desc.usage_mode)
  {
    case UsageMode_Default:
    case UsageMode_Static:
    case UsageMode_Dynamic:
    case UsageMode_Stream:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.usage_mode", desc.usage_mode);
      break;
  }

    //�������� ������������ ������ �������

  switch (desc.access_flags)
  {
    case 0:
    case AccessFlag_Read:
    case AccessFlag_Write:
    case AccessFlag_Read | AccessFlag_Write:
      break;
    default:
      throw xtl::make_argument_exception ("", "desc.access_flags", desc.access_flags);
      break;
  }
}

/*
    ��������� �����������
*/

void Buffer::GetDesc (BufferDesc& out_desc)
{
  out_desc = desc;
}

/*
    ������ � ������� ������
*/

void Buffer::SetData (size_t offset, size_t size, const void* data)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Buffer::SetData";

    //�������� ����������� ��������� ������
    
  if (!(desc.access_flags & AccessFlag_Write))
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set buffer data (no AccessFlag_Write in desc.access_flags)");

    //���������

  if (offset >= desc.size)
    return;
    
  if (desc.size - offset < size)
    size = desc.size - offset;
    
  if (!size)
    return;

    //�������� ������������ ���������� ������

  if (!data)
    throw xtl::make_null_argument_exception (METHOD_NAME, "data");

    //��������� ������

  SetDataCore (offset, size, data);
}

void Buffer::GetData (size_t offset, size_t size, void* data)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Buffer::GetData";

    //�������� ����������� ��������� ������
    
  if (!(desc.access_flags & AccessFlag_Read))
    throw xtl::format_operation_exception (METHOD_NAME, "Can't get buffer data (no AccessFlag_Read in desc.access_flags)");

    //���������

  if (offset >= desc.size)
    return;

  if (desc.size - offset < size)
    size = desc.size - offset;

  if (!size)
    return;

    //�������� ������������ ��������������� ������

  if (!data)
    throw xtl::make_null_argument_exception (METHOD_NAME, "data");

    //������ ������

  GetDataCore (offset, size, data);
}
