#include "shared.h"

using namespace medialib::geometry;
using namespace common;

/*
    �������� ���������� ���������� ������
*/

struct IndexBuffer::Impl: public InstanceResource
{
  Buffer indices; //������ ��������
};

/*
    ������������ / ����������
*/

IndexBuffer::IndexBuffer ()
  : impl (new Impl)
  {}

IndexBuffer::IndexBuffer (size_t indices_count)
  : impl (new Impl)
{
  Resize (indices_count);
}
  
IndexBuffer::IndexBuffer (const IndexBuffer& ib, BufferCloneMode mode)
  : impl (clone_resource (ib.impl, mode, "medialib::geometry::IndexBuffer::IndexBuffer"))
  {}

IndexBuffer::~IndexBuffer ()
{
  release_resource (impl);
}

/*
    ������������
*/

void IndexBuffer::Assign (const IndexBuffer& ib, BufferCloneMode mode)
{
  IndexBuffer (ib, mode).Swap (*this);
}

IndexBuffer& IndexBuffer::operator = (const IndexBuffer& ib)
{
  Assign (ib);

  return *this;
}

/*
    ������
*/

const size_t* IndexBuffer::Data () const
{
  return (const size_t*)impl->indices.Data ();
}

size_t* IndexBuffer::Data ()
{
  return (size_t*)impl->indices.Data ();
}

/*
    ���������� ��������
*/

size_t IndexBuffer::Size () const
{
  return impl->indices.Size () / sizeof (size_t);
}

void IndexBuffer::Resize (size_t indices_count)
{
  impl->indices.Resize (indices_count * sizeof (size_t));
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
  return impl->indices.Capacity () / sizeof (size_t);
}

void IndexBuffer::Reserve (size_t indices_count)
{
  impl->indices.Reserve (indices_count * sizeof (size_t));
}

/*
    �����
*/

void IndexBuffer::Swap (IndexBuffer& ib)
{
  Impl* tmp = ib.impl;
  ib.impl   = impl;
  impl      = tmp;
}

namespace medialib
{

namespace geometry
{
    
void swap (IndexBuffer& ib1, IndexBuffer& ib2)
{
  ib1.Swap (ib2);
}

}

}
