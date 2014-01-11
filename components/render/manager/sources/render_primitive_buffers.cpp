#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

/*
    �������� ���������� PrimitiveBuffers
*/

namespace
{

template <class T> struct BufferHolder
{
  T                 source;
  LowLevelBufferPtr buffer;
  MeshBufferUsage   usage;
  
  BufferHolder (const T& in_source, const LowLevelBufferPtr& in_buffer, MeshBufferUsage in_usage)
    : source (in_source)
    , buffer (in_buffer)
    , usage (in_usage)
  {
  }
};

struct VertexStreamHolder: public BufferHolder<media::geometry::VertexStream>
{
  size_t vertex_size;
  size_t vertices_count;

  VertexStreamHolder (const media::geometry::VertexStream& vs, const LowLevelBufferPtr& buffer, MeshBufferUsage usage)
    : BufferHolder<media::geometry::VertexStream> (vs, buffer, usage)
    , vertex_size (vs.VertexSize ())
    , vertices_count (vs.Size ())
  {
  }
};

struct IndexBufferHolder: public BufferHolder<media::geometry::IndexBuffer>
{
  size_t indices_count;
  
  IndexBufferHolder (const media::geometry::IndexBuffer& ib, const LowLevelBufferPtr& buffer, MeshBufferUsage usage)
    : BufferHolder<media::geometry::IndexBuffer> (ib, buffer, usage)
    , indices_count (ib.Size ())
  {
  }
};

struct VertexBufferHolder
{
  media::geometry::VertexBuffer source;
  VertexBufferPtr               buffer;
  
  VertexBufferHolder (const media::geometry::VertexBuffer& in_source, const VertexBufferPtr& in_buffer)
    : source (in_source)
    , buffer (in_buffer)
  {
  }
};

typedef stl::hash_map<size_t, VertexStreamHolder> VertexStreamMap;
typedef stl::hash_map<size_t, VertexBufferHolder> VertexBufferMap;
typedef stl::hash_map<size_t, IndexBufferHolder>  IndexBufferMap;
typedef stl::hash_map<size_t, LowLevelBufferPtr>  CacheBufferMap;
typedef stl::hash_map<size_t, VertexBufferPtr>    CacheVertexBufferMap;

}

struct PrimitiveBuffersImpl::Impl
{
  DeviceManagerPtr                      device_manager;       //�������� ���������� ���������
  VertexStreamMap                       vertex_streams;       //��������� ������
  VertexBufferMap                       vertex_buffers;       //��������� ������
  IndexBufferMap                        index_buffers;        //��������� ������
  CacheBufferMap                        vertex_streams_cache; //��� ��������� �������
  CacheBufferMap                        index_buffers_cache;  //��� ��������� �������
  CacheVertexBufferMap                  vertex_buffers_cache; //��� ��������� �������
  bool                                  cache_state;          //��������� ����
  size_t                                lines_capacity;       //������������� ���������� �����
  size_t                                sprites_capacity;     //������������� ���������� ��������
  render::manager::DynamicVertexBuffer  dynamic_vb;           //������������ ��������� �����
  render::manager::DynamicIndexBuffer   dynamic_ib;           //������������ ��������� �����
  
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , cache_state (false)
    , lines_capacity ()
    , sprites_capacity ()
    , dynamic_vb (render::low_level::UsageMode_Stream, render::low_level::BindFlag_VertexBuffer)
    , dynamic_ib (render::low_level::UsageMode_Stream, render::low_level::BindFlag_IndexBuffer)
  {
  }

  void ReserveDynamicPrimitives (size_t sprites_count, size_t lines_count)
  {
    try
    {
      size_t vertices_count = 4 * sprites_count + 2 * lines_count,
             indices_count  = 6 * sprites_count + 2 * lines_count;

      dynamic_vb.Reserve (vertices_count);
      dynamic_ib.Reserve (indices_count);

      dynamic_vb.SyncBuffers (device_manager->Device ());
      dynamic_ib.SyncBuffers (device_manager->Device ());

      sprites_capacity = sprites_count;
      lines_capacity   = lines_capacity;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::PrimitiveBuffersImpl::ReserveDynamicPrimitives");
      throw;
    }
  }
};

/*
    ������������ / ���������� / ������������
*/

PrimitiveBuffersImpl::PrimitiveBuffersImpl (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
}

PrimitiveBuffersImpl::~PrimitiveBuffersImpl ()
{
}

/*
    �������� ����������� �������
*/

LowLevelBufferPtr PrimitiveBuffersImpl::CreateVertexStream (const media::geometry::VertexStream& vs, MeshBufferUsage usage)
{
  try
  {
    VertexStreamMap::iterator iter = impl->vertex_streams.find (vs.Id ());
    
    if (iter != impl->vertex_streams.end ())
      return iter->second.buffer;
      
    if (impl->cache_state)
    {
      CacheBufferMap::iterator iter = impl->vertex_streams_cache.find (vs.Id ());
      
      if (iter != impl->vertex_streams_cache.end ())
        return iter->second;
    }
      
    BufferDesc desc;

    memset (&desc, 0, sizeof desc);

    desc.size         = vs.Size () * vs.VertexSize ();
    desc.bind_flags   = BindFlag_VertexBuffer;
    desc.access_flags = AccessFlag_Read | AccessFlag_Write;
    
    switch (usage)
    {
      case MeshBufferUsage_Static:
        desc.usage_mode = UsageMode_Static;
        break;
      case MeshBufferUsage_Dynamic:
        desc.usage_mode = UsageMode_Dynamic;
        break;
      case MeshBufferUsage_Stream:      
        desc.usage_mode = UsageMode_Stream;
        break;
      default:
        throw xtl::make_argument_exception ("", "usage", usage);
    }
    
    LowLevelBufferPtr buffer (impl->device_manager->Device ().CreateBuffer (desc), false);    

    buffer->SetData (0, desc.size, vs.Data ());
    
    if (impl->cache_state)
      impl->vertex_streams_cache.insert_pair (vs.Id (), buffer);
    
    return buffer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::CreateVertexStream");
    throw;
  }
}

VertexBufferPtr PrimitiveBuffersImpl::CreateVertexBuffer (const media::geometry::VertexBuffer& vb, MeshBufferUsage usage)
{
  try
  {
    VertexBufferMap::iterator iter = impl->vertex_buffers.find (vb.Id ());
    
    if (iter != impl->vertex_buffers.end ())
      return iter->second.buffer;
      
    if (impl->cache_state)
    {
      CacheVertexBufferMap::iterator iter = impl->vertex_buffers_cache.find (vb.Id ());
      
      if (iter != impl->vertex_buffers_cache.end ())
        return iter->second;
    }      
      
    VertexBufferPtr buffer (new VertexBuffer (vb, *this, impl->device_manager, usage), false);
    
    if (impl->cache_state)
      impl->vertex_buffers_cache.insert_pair (vb.Id (), buffer);
    
    return buffer;      
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::CreateVertexBuffer");
    throw;
  }
}

LowLevelBufferPtr PrimitiveBuffersImpl::CreateIndexBuffer (const media::geometry::IndexBuffer& ib, MeshBufferUsage usage)
{
  try
  {
    IndexBufferMap::iterator iter = impl->index_buffers.find (ib.Id ());

    if (iter != impl->index_buffers.end ())
      return iter->second.buffer;      
      
    if (impl->cache_state)
    {
      CacheBufferMap::iterator iter = impl->index_buffers_cache.find (ib.Id ());
      
      if (iter != impl->index_buffers_cache.end ())
        return iter->second;
    }
      
    BufferDesc desc;

    memset (&desc, 0, sizeof desc);

    desc.size         = ib.Size () * get_index_type_size (ib.DataType ());
    desc.bind_flags   = BindFlag_IndexBuffer;
    desc.access_flags = AccessFlag_ReadWrite;
    
    switch (usage)
    {
      case MeshBufferUsage_Static:
        desc.usage_mode = UsageMode_Static;
        break;
      case MeshBufferUsage_Dynamic:
        desc.usage_mode = UsageMode_Dynamic;
        break;
      case MeshBufferUsage_Stream:      
        desc.usage_mode = UsageMode_Stream;
        break;
      default:
        throw xtl::make_argument_exception ("", "usage", usage);
    }
    
    LowLevelBufferPtr buffer (impl->device_manager->Device ().CreateBuffer (desc), false);

    buffer->SetData (0, desc.size, ib.Data ());    
    
    if (impl->cache_state)
      impl->index_buffers_cache.insert_pair (ib.Id (), buffer);
        
    return buffer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::CreateIndexBuffer");
    throw;
  }
}

/*
    ���������� �������
*/

void PrimitiveBuffersImpl::Add (const media::geometry::VertexStream& vs, MeshBufferUsage usage)
{
  try
  {
    VertexStreamMap::iterator iter = impl->vertex_streams.find (vs.Id ());
    
    if (iter != impl->vertex_streams.end ())
      return;    
    
    LowLevelBufferPtr buffer = CreateVertexStream (vs, usage);
    
    impl->vertex_streams.insert_pair (vs.Id (), VertexStreamHolder (vs, buffer, usage));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::Add(const VertexStream&,MeshBufferUsage)");
    throw;
  }
}

void PrimitiveBuffersImpl::Add (const media::geometry::VertexBuffer& vb, MeshBufferUsage usage)
{
  try
  {
    VertexBufferMap::iterator iter = impl->vertex_buffers.find (vb.Id ());
    
    if (iter != impl->vertex_buffers.end ())
      return;    
    
    VertexBufferPtr buffer = CreateVertexBuffer (vb, usage);
    
    impl->vertex_buffers.insert_pair (vb.Id (), VertexBufferHolder (vb, buffer));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::Add(const VertexBuffer&,MeshBufferUsage)");
    throw;
  }
}

void PrimitiveBuffersImpl::Add (const media::geometry::IndexBuffer& ib, MeshBufferUsage usage)
{
  try
  {
    IndexBufferMap::iterator iter = impl->index_buffers.find (ib.Id ());
    
    if (iter != impl->index_buffers.end ())
      return;    
    
    LowLevelBufferPtr buffer = CreateIndexBuffer (ib, usage);
    
    impl->index_buffers.insert_pair (ib.Id (), IndexBufferHolder (ib, buffer, usage));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::Add(const IndexBuffer&,MeshBufferUsage)");
    throw;
  }
}

/*
    ���������� �������
*/

void PrimitiveBuffersImpl::Update (const media::geometry::VertexStream& vs)
{
  try
  {
    VertexStreamMap::iterator iter = impl->vertex_streams.find (vs.Id ());
    
    if (iter == impl->vertex_streams.end ())
      throw xtl::make_argument_exception ("", "buffer.Id()", vs.Id (), "Vertex stream not added to primitive buffers");
      
    VertexStreamHolder& holder = iter->second;
    
    if (holder.vertex_size != vs.VertexSize ())
      throw xtl::format_operation_exception ("", "Vertex size %u differes from source vertex stream size %u", vs.VertexSize (), holder.vertex_size);
      
    if (holder.vertices_count != vs.Size ())
      throw xtl::format_operation_exception ("", "Vertices count %u differes from source vertices count %u", vs.Size (), holder.vertices_count);
          
    holder.buffer->SetData (0, vs.Size () * vs.VertexSize (), vs.Data ());    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::Update(const VertexStream&)");
    throw;
  }
}

void PrimitiveBuffersImpl::Update (const media::geometry::IndexBuffer& ib)
{
  try
  {
    IndexBufferMap::iterator iter = impl->index_buffers.find (ib.Id ());
    
    if (iter == impl->index_buffers.end ())
      throw xtl::make_argument_exception ("", "buffer.Id()", ib.Id (), "Index buffer not added to primitive buffers");
      
    IndexBufferHolder& holder = iter->second;
    
    if (holder.indices_count != ib.Size ())
      throw xtl::format_operation_exception ("", "Indices count %u differes from source indices count %u", ib.Size (), holder.indices_count);
          
    holder.buffer->SetData (0, ib.Size () * get_index_type_size (ib.DataType ()), ib.Data ());    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::Update(const IndexBuffer&)");
    throw;
  }
}

/*
    �������� �������
*/

void PrimitiveBuffersImpl::Remove (const media::geometry::VertexStream& vs)
{
  impl->vertex_streams.erase (vs.Id ());
}

void PrimitiveBuffersImpl::Remove (const media::geometry::VertexBuffer& vb)
{
  impl->vertex_buffers.erase (vb.Id ());
}

void PrimitiveBuffersImpl::Remove (const media::geometry::IndexBuffer& ib)
{
  impl->index_buffers.erase (ib.Id ());
}

void PrimitiveBuffersImpl::RemoveAll ()
{
  impl->vertex_streams.clear ();
  impl->vertex_buffers.clear ();  
  impl->index_buffers.clear ();
}

/*
    �������������� ��������������� ����������
*/

void PrimitiveBuffersImpl::ReserveDynamicPrimitives (size_t sprites_count, size_t lines_count)
{
  try
  {
    impl->ReserveDynamicPrimitives (sprites_count, lines_count);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::manager::PrimitiveBuffersImpl::ReserveDynamicPrimitives");
    throw;
  }
}

size_t PrimitiveBuffersImpl::LinesCapacity ()
{
  return impl->lines_capacity;
}

size_t PrimitiveBuffersImpl::SpritesCapacity ()
{
  return impl->sprites_capacity;
}

/*
    ������������ ������
*/

render::manager::DynamicVertexBuffer& PrimitiveBuffersImpl::DynamicVertexBuffer ()
{
  return impl->dynamic_vb;
}

render::manager::DynamicIndexBuffer& PrimitiveBuffersImpl::DynamicIndexBuffer ()
{
  return impl->dynamic_ib;
}

/*
    ���������� �����
*/

void PrimitiveBuffersImpl::SetCacheState (bool state)
{
  if (state == impl->cache_state)
    return;
    
  if (impl->cache_state)
    FlushCache ();
    
  impl->cache_state = state;
}

bool PrimitiveBuffersImpl::CacheState ()
{
  return impl->cache_state;
}

void PrimitiveBuffersImpl::FlushCache ()
{
  impl->vertex_streams_cache.clear ();
  impl->vertex_buffers_cache.clear ();
  impl->index_buffers_cache.clear ();
}
