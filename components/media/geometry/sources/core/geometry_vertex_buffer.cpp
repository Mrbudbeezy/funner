#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

const size_t DEFAULT_VERTEX_ARRAY_RESERVE = 4; //������������� ������ ���������� �������

/*
    �������� ���������� VertexBuffer
*/

typedef stl::vector<VertexStream> VertexStreamArray;

struct VertexBuffer::Impl: public xtl::reference_counter
{
  VertexStreamArray  streams;   //��������� �������
  VertexWeightStream weights;   //������ �����

  Impl ();
};

/*
    VertexBuffer::Impl
*/

VertexBuffer::Impl::Impl ()
{
  streams.reserve (DEFAULT_VERTEX_ARRAY_RESERVE);
}
  
/*
    ������������ / ����������
*/

VertexBuffer::VertexBuffer ()
  : impl (new Impl)
  {}

VertexBuffer::VertexBuffer (Impl* in_impl)
  : impl (in_impl, false)
  {}  

VertexBuffer::VertexBuffer (const VertexBuffer& vb)
  : impl (vb.impl)
  {}

VertexBuffer::~VertexBuffer ()
{
}

/*
    ������������
*/

VertexBuffer& VertexBuffer::operator = (const VertexBuffer& vb)
{
  impl = vb.impl;

  return *this;
}

/*
    �������� �����
*/

VertexBuffer VertexBuffer::Clone () const
{
  return VertexBuffer (new Impl (*impl));
}

/*
    ������������� ������
*/

size_t VertexBuffer::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
}

/*
    ���������� �������� ��������� ���������
*/

size_t VertexBuffer::StreamsCount () const
{
  return impl->streams.size ();
}

/*
    ��������� �������
*/

const VertexStream& VertexBuffer::Stream (size_t index) const
{
  if (index >= impl->streams.size ())
    raise_out_of_range ("media::geometry::VertexBuffer::Stream", "index", index, impl->streams.size ());
    
  return impl->streams [index];
}

VertexStream& VertexBuffer::Stream (size_t index)
{
  return const_cast<VertexStream&> (const_cast<const VertexBuffer&> (*this).Stream (index));
}

/*
    ��������� ������� �����
*/

const VertexWeightStream& VertexBuffer::Weights () const
{
  return impl->weights;
}

VertexWeightStream& VertexBuffer::Weights ()
{
  return impl->weights;
}

/*
    �������������/������������ ��������
*/

size_t VertexBuffer::Attach (VertexStream& vs)
{
    //�������� ��������������� �� �����
    
  size_t id = vs.Id ();
    
  for (VertexStreamArray::iterator i=impl->streams.begin (), end=impl->streams.end (); i!=end; ++i)
    if (i->Id () == id)
      return i - impl->streams.begin ();

  impl->streams.push_back (vs);

  return impl->streams.size () - 1;
}

void VertexBuffer::Detach (size_t index)
{
  if (index >= impl->streams.size ())
    return;
    
  impl->streams.erase (impl->streams.begin () + index);
}

void VertexBuffer::AttachWeights (VertexWeightStream& vws)
{
  impl->weights = vws;
}

void VertexBuffer::DetachWeights ()
{
  impl->weights = VertexWeightStream ();
}

void VertexBuffer::Clear ()
{
  impl->streams.clear ();
  DetachWeights ();
}

/*
    ���������� ������ (������� ����� ���� ��������� ��������)
*/

size_t VertexBuffer::VerticesCount () const
{
  if (impl->streams.empty ())
    return 0;

  size_t vertices_count = size_t (-1);
  
  for (VertexStreamArray::iterator i=impl->streams.begin (), end=impl->streams.end (); i!=end; ++i)
  {
    size_t size = i->Size ();
    
    if (size < vertices_count)
      vertices_count = size;
  }
  
  return vertices_count;
}

/*
    ��������� ������ �������
*/

size_t VertexBuffer::VertexSize () const
{
  size_t vertex_size = 0;

  for (VertexStreamArray::iterator i=impl->streams.begin (), end=impl->streams.end (); i!=end; ++i)
    vertex_size += i->VertexSize ();

  return vertex_size;
}
    
/*
    �����
*/

void VertexBuffer::Swap (VertexBuffer& vb)
{
  swap (vb.impl, impl);
}

namespace media
{

namespace geometry
{

void swap (VertexBuffer& vb1, VertexBuffer& vb2)
{
  vb1.Swap (vb2);
}

}

}
