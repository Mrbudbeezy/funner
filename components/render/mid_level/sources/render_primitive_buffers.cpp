#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    �������� ���������� PrimitiveBuffers
*/

struct PrimitiveBuffersImpl::Impl
{
  DeviceManagerPtr device_manager; //�������� ���������� ���������
  MeshBufferUsage  lines_usage;    //����� ������������� ������� ��� �����
  MeshBufferUsage  sprites_usage;  //����� ������������� ������� ��� ��������
  
  Impl (const DeviceManagerPtr& in_device_manager, MeshBufferUsage in_lines_usage, MeshBufferUsage in_sprites_usage)
    : device_manager (in_device_manager)
    , lines_usage (in_lines_usage)
    , sprites_usage (in_sprites_usage)
  {
  }
};

/*
    ������������ / ���������� / ������������
*/

PrimitiveBuffersImpl::PrimitiveBuffersImpl (const DeviceManagerPtr& device_manager, MeshBufferUsage lines_usage, MeshBufferUsage sprites_usage)
  : impl (new Impl (device_manager, lines_usage, sprites_usage))
{
}

PrimitiveBuffersImpl::~PrimitiveBuffersImpl ()
{
}

/*
    ���������� �������
*/

void PrimitiveBuffersImpl::Add (const media::geometry::VertexStream& buffer, MeshBufferUsage usage)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveBuffersImpl::Add (const media::geometry::VertexBuffer& buffer, MeshBufferUsage usage)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveBuffersImpl::Add (const media::geometry::IndexBuffer& buffer, MeshBufferUsage usage)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ���������� �������
*/

void PrimitiveBuffersImpl::Update (const media::geometry::VertexStream& buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveBuffersImpl::Update (const media::geometry::VertexBuffer& buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveBuffersImpl::Update (const media::geometry::IndexBuffer& buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    �������� �������
*/

void PrimitiveBuffersImpl::Remove (const media::geometry::VertexStream& buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveBuffersImpl::Remove (const media::geometry::VertexBuffer& buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveBuffersImpl::Remove (const media::geometry::IndexBuffer& buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveBuffersImpl::Clear ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    �������������� ��������������� ����������
*/

void PrimitiveBuffersImpl::ReserveLines (size_t count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveBuffersImpl::ReserveSprites (size_t count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t PrimitiveBuffersImpl::LinesCapacity ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t PrimitiveBuffersImpl::SpritesCapacity ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ����� ������������� ������� ��������������� ����������
*/

MeshBufferUsage PrimitiveBuffersImpl::LinesBufferUsage ()
{
  return impl->lines_usage;
}

MeshBufferUsage PrimitiveBuffersImpl::SpritesBufferUsage ()
{
  return impl->sprites_usage;
}
