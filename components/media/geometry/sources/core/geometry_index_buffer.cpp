#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

/*
    �������� ���������� ���������� ������
*/

struct IndexBuffer::Impl: public xtl::reference_counter
{
  Buffer indices; //������ ��������
};

/*
    ������������ / ����������
*/

IndexBuffer::IndexBuffer ()
  : impl (new Impl, false)
  {}
  
IndexBuffer::IndexBuffer (Impl* in_impl)
  : impl (in_impl, false)
  {}

IndexBuffer::IndexBuffer (size_t indices_count)
  : impl (new Impl, false)
{
  Resize (indices_count);
}
  
IndexBuffer::IndexBuffer (const IndexBuffer& ib)
  : impl (ib.impl)
  {}

IndexBuffer::~IndexBuffer ()
{
}

/*
    ������������
*/

IndexBuffer& IndexBuffer::operator = (const IndexBuffer& ib)
{
  impl = ib.impl;

  return *this;
}

/*
    �������� �����
*/

IndexBuffer IndexBuffer::Clone () const
{
  return IndexBuffer (new Impl (*impl));
}

/*
    ������������� ������
*/

size_t IndexBuffer::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
}

/*
    ������
*/

const unsigned int* IndexBuffer::Data () const
{
  return (const unsigned int*)impl->indices.Data ();
}

unsigned int* IndexBuffer::Data ()
{
  return (unsigned int*)impl->indices.Data ();
}

/*
    ���������� ��������
*/

size_t IndexBuffer::Size () const
{
  return impl->indices.Size () / sizeof (unsigned int);
}

void IndexBuffer::Resize (size_t indices_count)
{
  impl->indices.Resize (indices_count * sizeof (unsigned int));
}

/*
    ������� ������
*/

void IndexBuffer::Clear ()
{
  impl->indices.Resize (0);
}

/*
    �������������� ������
*/

size_t IndexBuffer::Capacity () const
{
  return impl->indices.Capacity () / sizeof (unsigned int);
}

void IndexBuffer::Reserve (size_t indices_count)
{
  impl->indices.Reserve (indices_count * sizeof (unsigned int));
}

/*
    �����
*/

void IndexBuffer::Swap (IndexBuffer& ib)
{
  impl.swap (ib.impl);
}

namespace media
{

namespace geometry
{
    
void swap (IndexBuffer& ib1, IndexBuffer& ib2)
{
  ib1.Swap (ib2);
}

}

}
