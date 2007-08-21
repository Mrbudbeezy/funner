#include "shared.h"

using namespace medialib;
using namespace common;

/*
    �������� ���������� ���������� �������
*/

struct VertexStream::Impl: public InstanceResource
{
  VertexFormat format;         //������ ������
  size_t       vertex_size;    //������ �������
  MeshBuffer   data_buffer;    //����� � �������
  size_t       vertices_count; //���������� ������
  
  Impl (const VertexFormat&);
  Impl (const VertexFormat&, size_t vertex_size);
};

/*
    VertexStream::Impl
*/

VertexStream::Impl::Impl (const VertexFormat& in_format)
  : format (in_format), vertices_count (0)
{
  vertex_size = format.GetMinimalVertexSize ();
}

VertexStream::Impl::Impl (const VertexFormat& in_format, size_t in_vertex_size)
  : format (in_format), vertex_size (in_vertex_size), vertices_count (0)
{
  size_t min_vertex_size = format.GetMinimalVertexSize ();

  if (vertex_size < min_vertex_size)
    vertex_size = min_vertex_size;
}

/*
    VertexStream
*/

/*
    ������������ / ����������
*/
  
VertexStream::VertexStream (const VertexFormat& format)
  : impl (new Impl (format))
  {}

VertexStream::VertexStream (const VertexFormat& format, size_t vertex_size)
  : impl (new Impl (format, vertex_size))
  {}

VertexStream::VertexStream (size_t vertices_count, const VertexFormat& format)
  : impl (new Impl (format))
{
  Resize (vertices_count);
}

VertexStream::VertexStream (size_t vertices_count, const VertexFormat& format, size_t vertex_size)
  : impl (new Impl (format, vertex_size))
{
  Resize (vertices_count);
}

VertexStream::VertexStream (const VertexStream& vs, BufferCloneMode mode)
  : impl (clone_resource (vs.impl, mode, "medialib::VertexStream::VertexStream"))
  {}

VertexStream::~VertexStream ()
{
  release_resource (impl);
}

/*
    ������������
*/

void VertexStream::Assign (const VertexStream& vs, BufferCloneMode mode)
{
  VertexStream (vs, mode).Swap (*this);
}

VertexStream& VertexStream::operator = (const VertexStream& vs)
{
  Assign (vs);
  return *this;
}

/*
    ������
*/

const VertexFormat& VertexStream::Format () const
{
  return impl->format;
}

/*
    ������ �������
*/

size_t VertexStream::VertexSize () const
{
  return impl->vertex_size;
}

/*
    ��������� ������
*/

const void* VertexStream::Data () const
{
  return impl->data_buffer.Data ();
}

void* VertexStream::Data ()
{
  return impl->data_buffer.Data ();
}

/*
    ���������� ������
*/

size_t VertexStream::Size () const
{
  return impl->vertices_count;
}

void VertexStream::Resize (size_t vertices_count)
{
  impl->data_buffer.Resize (vertices_count * impl->vertex_size);
  
  impl->vertices_count = vertices_count;
}

/*
    ������� �������
*/

void VertexStream::Clear ()
{
  Resize (0);
}

/*
    �������������� ������
*/

size_t VertexStream::Capacity () const
{
  return impl->vertex_size ? impl->data_buffer.Capacity () / impl->vertex_size : 0;
}

void VertexStream::Reserve (size_t vertices_count)
{
  impl->data_buffer.Reserve (vertices_count * impl->vertex_size);
}

/*
    �����
*/

void VertexStream::Swap (VertexStream& vs)
{
  Impl* tmp  = vs.impl;
  vs.impl    = impl;
  impl       = tmp;
}

namespace medialib
{

void swap (VertexStream& vs1, VertexStream& vs2)
{
  vs1.Swap (vs2);
}
    
}
