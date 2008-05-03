#include "shared.h"

using namespace common;

/*
    �������� ���������� InputStreamBuffer
*/

struct InputStreamBuffer::Impl
{
  StreamBuffer buffer;   //�����
  ReadFunction reader;   //������� ������
  size_t       position; //������� � ������ ������
  size_t       finish;   //����� ������ ������
  
  size_t Available () const { return finish - position; }
  
  Impl (size_t buffer_size, const ReadFunction& in_reader = &DefaultReader) : reader (in_reader), position (0), finish (0)
  {
    buffer.Resize (buffer_size);
  }
};

/*
    ������������ / ����������
*/

InputStreamBuffer::InputStreamBuffer (size_t buffer_size)
  : impl (new Impl (buffer_size))
{
}

InputStreamBuffer::InputStreamBuffer (const ReadFunction& reader, size_t buffer_size)
  : impl (new Impl (buffer_size, reader))
{
}

InputStreamBuffer::~InputStreamBuffer ()
{
}

/*
    ���������� �������� ������
*/

size_t InputStreamBuffer::Size () const
{
  return impl->buffer.Size ();
}

void InputStreamBuffer::Resize (size_t buffer_size)
{
  if (buffer_size < impl->position)
    buffer_size = impl->position;
   
  impl->buffer.Resize (buffer_size, impl->finish);
}

/*
    ������
*/

size_t InputStreamBuffer::Read (void* buffer, size_t size)
{
  size_t        read_size     = 0;
  char*         ptr           = static_cast<char*> (buffer);
  StreamBuffer& stream_buffer = impl->buffer;

    //���� ����������� ��������� - ������ � ����� ������

  if (!stream_buffer.Size ())
    return impl->reader (buffer, size);

  while (size)
  {
      //���� ����� ��������� - ��������� ���

    if (!impl->Available ())
    {
      impl->position = 0;
      impl->finish   = impl->reader (stream_buffer.Data (), stream_buffer.Size ());      
      
      if (!impl->Available ())
        return read_size;
    }

    size_t block_size = size;

    if (block_size > impl->Available ())
      block_size = impl->Available ();

    memcpy (ptr, (char*)stream_buffer.Data () + impl->position, block_size);


    impl->position += block_size;
    ptr            += block_size;
    read_size      += block_size;
    size           -= block_size;
  }
  
  return read_size;
}

/*
    ������������� ����������������� ������
*/

void InputStreamBuffer::SetUserBuffer (void* buffer, size_t size)
{
  impl->buffer.SetUserBuffer (buffer, size);
  
  impl->position = 0;
  impl->finish   = size;
}

/*
    ������� ������ ������
*/

void InputStreamBuffer::SetReader (const ReadFunction& reader)
{
  impl->reader = reader;
}

const InputStreamBuffer::ReadFunction& InputStreamBuffer::Reader () const
{
  return impl->reader;
}

/*
    ������� ������ ������ �� ���������
*/

size_t InputStreamBuffer::DefaultReader (void*, size_t)
{
  return 0;
}

/*
    �����
*/

void InputStreamBuffer::Swap (InputStreamBuffer& isb)
{  
  swap (impl, isb.impl);
}

namespace common
{

void swap (InputStreamBuffer& isb1, InputStreamBuffer& isb2)
{
  isb1.Swap (isb2);
}

}
