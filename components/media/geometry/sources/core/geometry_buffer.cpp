#include "shared.h"

using namespace media::geometry;

/*
    Buffer
*/

Buffer::Buffer ()
  : start (0), finish (0), end_of_storage (0)
  {}
  
Buffer::Buffer (uint32_t size)
  : start (0), finish (0), end_of_storage (0)
{
  Resize (size);
}

Buffer::Buffer (const Buffer& Buffer)
  : start (0), finish (0), end_of_storage (0)
{
  Resize (Buffer.Size ());
  
  memcpy (start, Buffer.start, finish - start);
}

Buffer::~Buffer ()
{
  if (start)
    ::operator delete (start);
}

void Buffer::Resize (uint32_t new_size)
{
  size_t size = finish - start;
  
  if (new_size == size)
    return;

  if (new_size > size)
    Reserve (new_size);

  finish = start + new_size;
}

void Buffer::Reserve (uint32_t new_size)
{
  size_t size = end_of_storage - start;

  if (new_size <= size)
    return;
    
  char* new_buffer = (char*)::operator new (new_size);
  
  memcpy (new_buffer, start, finish - start);
  
  if (start)
    ::operator delete (start);
    
  finish         = new_buffer + (finish - start);
  start          = new_buffer;  
  end_of_storage = new_buffer + new_size;
}

void Buffer::Swap (Buffer& buffer)
{
  stl::swap (start, buffer.start);
  stl::swap (finish, buffer.finish);
  stl::swap (end_of_storage, buffer.end_of_storage);
}
