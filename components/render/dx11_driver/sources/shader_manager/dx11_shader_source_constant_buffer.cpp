#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    �����������
*/

SourceConstantBuffer::SourceConstantBuffer (const BufferDesc& in_desc)
  : desc (in_desc)
  , hash ()
  , need_update_hash (true)
{
  try
  {
      //�������� ������������ ������ ������������� ������

    switch (desc.usage_mode)
    {
      case UsageMode_Default:
      case UsageMode_Static:
      case UsageMode_Dynamic:
      case UsageMode_Stream:
        break;
      default:
        throw xtl::make_argument_exception ("", "desc.usage_mode", desc.usage_mode);
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
    }

    if (desc.bind_flags != BindFlag_ConstantBuffer)
      throw xtl::make_argument_exception ("", "desc.bind_flags", desc.bind_flags, "BindFlag_ConstantBuffer have to be used");

    if (!desc.size)
      throw xtl::make_null_argument_exception ("", "desc.size");

    buffer.resize (desc.size);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::SourceConstantBuffer::SourceConstantBuffer");
    throw;
  }
}

/*
    ��������� ����������� ������
*/

void SourceConstantBuffer::GetDesc (BufferDesc& out_desc)
{
  out_desc = desc;
}

/*
    ������ / ������ �� ������
*/

void SourceConstantBuffer::SetData (size_t offset, size_t size, const void* data)
{
  try
  {
      //�������� ����������� ��������� ������
      
    if (!(desc.access_flags & AccessFlag_Write))
      throw xtl::format_operation_exception ("", "Can't set buffer data (no AccessFlag_Write in desc.access_flags)");

      //���������

    if (offset >= desc.size)
      return;
      
    if (desc.size - offset < size)
      size = desc.size - offset;
      
    if (!size)
      return;

      //�������� ������������ ���������� ������

    if (!data)
      throw xtl::make_null_argument_exception ("", "data");

      //��������� ������

    memcpy (buffer.data () + offset, data, size);

    need_update_hash = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::SourceConstantBuffer::SetData");
    throw;
  }
}

void SourceConstantBuffer::GetData (size_t offset, size_t size, void* data)
{
  try
  {
      //�������� ����������� ��������� ������
      
    if (!(desc.access_flags & AccessFlag_Read))
      throw xtl::format_operation_exception ("", "Can't get buffer data (no AccessFlag_Read in desc.access_flags)");

      //���������

    if (offset >= desc.size)
      return;

    if (desc.size - offset < size)
      size = desc.size - offset;

    if (!size)
      return;

      //�������� ������������ ��������������� ������

    if (!data)
      throw xtl::make_null_argument_exception ("", "data");

      //������ ������

    memcpy (data, buffer.data () + offset, size);   
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::SourceConstantBuffer::GetData");
    throw;
  }
}

/*
    ��� ������
*/

size_t SourceConstantBuffer::GetHash () const
{
  if (!need_update_hash)
    return hash;

  hash             = buffer.size () ? common::crc32 (buffer.data (), buffer.size ()) : 0;
  need_update_hash = false;

  return hash;
}
