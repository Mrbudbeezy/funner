#include "shared.h"

using namespace render::mid_level;

/*
    ������������ / ���������� / ������������
*/

PrimitiveBuffers::PrimitiveBuffers (PrimitiveBuffersImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

PrimitiveBuffers::PrimitiveBuffers (const PrimitiveBuffers& p)
  : impl (p.impl)
{
  addref (impl);
}

PrimitiveBuffers::~PrimitiveBuffers ()
{
  release (impl);
}

PrimitiveBuffers& PrimitiveBuffers::operator = (const PrimitiveBuffers& p)
{
  PrimitiveBuffers (p).Swap (*this);
  
  return *this;
}

/*
    ���������� �������
*/

void PrimitiveBuffers::Add (const media::geometry::VertexStream& buffer, MeshBufferUsage usage)
{
  impl->Add (buffer, usage);
}

void PrimitiveBuffers::Add (const media::geometry::VertexBuffer& buffer, MeshBufferUsage usage)
{
  impl->Add (buffer, usage);
}

void PrimitiveBuffers::Add (const media::geometry::IndexBuffer& buffer, MeshBufferUsage usage)
{
  impl->Add (buffer, usage);
}

/*
    ���������� �������
*/

void PrimitiveBuffers::Update (const media::geometry::VertexStream& buffer)
{
  impl->Update (buffer);
}

void PrimitiveBuffers::Update (const media::geometry::VertexBuffer& buffer)
{
  impl->Update (buffer);
}

void PrimitiveBuffers::Update (const media::geometry::IndexBuffer& buffer)
{
  impl->Update (buffer);
}

/*
    �������� �������
*/

void PrimitiveBuffers::Remove (const media::geometry::VertexStream& buffer)
{
  impl->Remove (buffer);
}

void PrimitiveBuffers::Remove (const media::geometry::VertexBuffer& buffer)
{
  impl->Remove (buffer);
}

void PrimitiveBuffers::Remove (const media::geometry::IndexBuffer& buffer)
{
  impl->Remove (buffer);
}

void PrimitiveBuffers::Clear ()
{
  impl->Clear ();
}

/*
    �������������� ��������������� ����������
*/

void PrimitiveBuffers::ReserveLines (size_t count)
{
  impl->ReserveLines (count);
}

void PrimitiveBuffers::ReserveSprites (size_t count)
{
  impl->ReserveSprites (count);
}

size_t PrimitiveBuffers::LinesCapacity () const
{
  return impl->LinesCapacity ();
}

size_t PrimitiveBuffers::SpritesCapacity () const
{
  return impl->SpritesCapacity ();
}

/*
    ����� ������������� ������� ��������������� ����������
*/

MeshBufferUsage PrimitiveBuffers::LinesBufferUsage () const
{
  return impl->LinesBufferUsage ();  
}

MeshBufferUsage PrimitiveBuffers::SpritesBufferUsage () const
{
  return impl->SpritesBufferUsage ();
}
