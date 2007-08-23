#include "shared.h"

using namespace medialib::geometry;
using namespace common;

const size_t DEFAULT_VERTEX_ARRAY_RESERVE = 4; //������������� ������ ���������� �������

/*
    ������ ��� �������� ��������� ��������
*/

class VertexStreamWrapper: public VertexStream
{
  public:
    VertexStreamWrapper (const VertexStreamWrapper& vs) : VertexStream (vs, CloneMode_Instance), clone_mode (vs.clone_mode) {}
    VertexStreamWrapper (const VertexStream& vs, CloneMode mode) : VertexStream (vs, mode), clone_mode (mode) {}

    VertexStreamWrapper& operator = (const VertexStreamWrapper& vs)
    {
      Assign (vs, CloneMode_Instance);
      clone_mode = vs.clone_mode;
      return *this;
    }

    CloneMode clone_mode; //����� �����������
};

/*
    �������� ���������� VertexBuffer
*/

typedef stl::vector<VertexStreamWrapper> VertexStreamArray;

struct VertexBuffer::Impl: public InstanceResource
{
  VertexStreamArray  streams;   //��������� �������
  VertexWeightStream weights;   //������ �����
  
  Impl ();
  Impl (const Impl&);
};

/*
    VertexBuffer::Impl
*/

VertexBuffer::Impl::Impl ()
{
  streams.reserve (DEFAULT_VERTEX_ARRAY_RESERVE);
}
  
VertexBuffer::Impl::Impl (const Impl& impl)
  : weights (impl.weights)
{
  streams.reserve (impl.streams.size ());

  for (VertexStreamArray::const_iterator i=impl.streams.begin (), end=impl.streams.end (); i!=end; ++i)
    streams.push_back (VertexStreamWrapper (*i, i->clone_mode));
}

/*
    ������������ / ����������
*/

VertexBuffer::VertexBuffer ()
  : impl (new Impl)
  {}

VertexBuffer::VertexBuffer (const VertexBuffer& vb, CloneMode mode)
  : impl (clone_resource (vb.impl, mode, "medialib::geometry::VertexBuffer::VertexBuffer"))
  {}

VertexBuffer::~VertexBuffer ()
{
  release_resource (impl);
}

/*
    ������������
*/

void VertexBuffer::Assign (const VertexBuffer& vb, CloneMode mode)
{
  VertexBuffer (vb, mode).Swap (*this);
}

VertexBuffer& VertexBuffer::operator = (const VertexBuffer& vb)
{
  Assign (vb);

  return *this;
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
    RaiseOutOfRange ("medialib::geometry::VertexBuffer::Stream", "index", index, impl->streams.size ());
    
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

size_t VertexBuffer::Attach (VertexStream& vs, CloneMode mode)
{
  switch (mode)
  {
    case CloneMode_Copy:
    case CloneMode_Instance:
    case CloneMode_Source:
      break;
    default:
      RaiseInvalidArgument ("medialib::geometry::VertexBuffer::Attach", "mode", mode);
      break;
  }

  impl->streams.push_back (VertexStreamWrapper (vs, mode));

  return impl->streams.size () - 1;
}

void VertexBuffer::Detach (size_t index)
{
  if (index >= impl->streams.size ())
    return;
    
  impl->streams.erase (impl->streams.begin () + index);
}

void VertexBuffer::AttachWeights (VertexWeightStream& vws, CloneMode mode)
{
  impl->weights.Assign (vws, mode);
}

void VertexBuffer::DetachWeights ()
{
  VertexWeightStream ().Swap (impl->weights);
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
  stl::swap (vb.impl, impl);
}

namespace medialib
{

namespace geometry
{

void swap (VertexBuffer& vb1, VertexBuffer& vb2)
{
  vb1.Swap (vb2);
}

}

}
