#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;
using namespace render::low_level;

namespace
{

/*
    ���������
*/

const size_t DEFAULT_VERTEX_BUFFER_SIZE = 8192; //������ ���������� ������� �� ��������� (���������� ������)

/*
    ��������� ����� �������� ��������� ������
*/

typedef xtl::uninitialized_storage<RenderableVertex> VertexArrayCache;

class VertexArrayCacheHolder
{
  public:
      //��������� ����
    static VertexArrayCache& GetCache ()
    {
      if (!instance)
        throw xtl::format_operation_exception ("render::mid_level::low_level_driver::renderer2d::VertexArrayCache::GetCache",
          "Null instance (lock cache before use)");

      return instance->cache;
    }

      //������ ����
    static void Lock ()
    {
      if (!instance)
      {
        instance = new VertexArrayCacheHolder;
      }
      else
      {
        instance->ref_count++;
      }
    }
    
      //������������ ����
    static void Unlock ()
    {
      if (!instance)
        return;

      if (!--instance->ref_count)
        delete instance;
    }

  private:
    VertexArrayCacheHolder () : ref_count (1) {}

    ~VertexArrayCacheHolder ()
    {
      instance = 0;
    }
  
  private:
    size_t                         ref_count;
    VertexArrayCache               cache;
    static VertexArrayCacheHolder* instance;
};

VertexArrayCacheHolder* VertexArrayCacheHolder::instance = 0;

}

/*
    �����������
*/

RenderableSpriteList::RenderableSpriteList ()
{
  VertexArrayCacheHolder::Lock ();

  vertex_buffer_vertices_count = 0;
}

RenderableSpriteList::~RenderableSpriteList ()
{
  VertexArrayCacheHolder::Unlock ();
}

/*
    �������������� ����� ��� ���������� ���������� ����� ��������
*/

void RenderableSpriteList::Reserve (size_t sprites_count)
{
  data_buffer.reserve (sprites_count);
}

/*
    ���������� �������� ���������
*/

void RenderableSpriteList::Add (size_t sprites_count, const RenderableSprite* sprites)
{
  const RenderableSprite* src = sprites;   

  size_t current_buffer_size = data_buffer.size ();  

  data_buffer.resize (data_buffer.size () + sprites_count);  

  const RenderableSprite** dst = data_buffer.data () + current_buffer_size;

  while (sprites_count--)
    *dst++ = &*src++;    
}

/*
    ������� ������
*/

void RenderableSpriteList::Clear ()
{
  data_buffer.resize (0);
}

/*
    �������� ���������� ������
*/

void RenderableSpriteList::SetVertexBufferSize (IDevice& device, size_t new_vertices_count)
{
  BufferDesc buffer_desc;

  memset (&buffer_desc, 0, sizeof buffer_desc);

  buffer_desc.usage_mode   = UsageMode_Stream;
  buffer_desc.bind_flags   = BindFlag_VertexBuffer;
  buffer_desc.access_flags = AccessFlag_ReadWrite;
  buffer_desc.size         = sizeof (RenderableVertex) * new_vertices_count;

  vertex_buffer                = BufferPtr (device.CreateBuffer (buffer_desc), false);
  vertex_buffer_vertices_count = new_vertices_count;
}

/*
    ���������� ���������� ������
*/

void RenderableSpriteList::UpdateVertexBuffer (IDevice& device)
{
  if (!data_buffer.size ())
    return;

    //���� ������ ���������� ������ ������ ������������ - ���������� ���
       
  size_t vertices_count = data_buffer.size () * SPRITE_VERTICES_COUNT;  

  if (data_buffer.size () > vertex_buffer_vertices_count)
    SetVertexBufferSize (device, (data_buffer.size () + DEFAULT_VERTEX_BUFFER_SIZE) * SPRITE_VERTICES_COUNT);

    //������������ ������ ������    

  VertexArrayCache& cache = VertexArrayCacheHolder::GetCache ();

  cache.resize (data_buffer.size () * SPRITE_VERTICES_COUNT, false);

  RenderableVertex*        dst_vertex = cache.data ();
  const RenderableSprite** src_sprite = data_buffer.data ();

  for (size_t count=data_buffer.size (); count--; src_sprite++, dst_vertex += SPRITE_VERTICES_COUNT)
    memcpy (dst_vertex, (*src_sprite)->vertices, sizeof (RenderableVertex) * SPRITE_VERTICES_COUNT);

    //���������� ���������� ������

  vertex_buffer->SetData (0, cache.size () * sizeof (RenderableVertex), cache.data ());
}
