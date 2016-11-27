#include "shared.h"

using namespace render::manager;

/*
===================================================================================================
    BatchStateBlock
===================================================================================================
*/

/*
    Описание реализации блока состояний пакета
*/

struct BatchStateBlock::Impl
{
  BatchingManager&      batching_manager;             //ссылка на менеджер упаковки
  MaterialImpl&         material;                     //ссылка на материал
  LowLevelStateBlockPtr cached_state_block;           //состояние устройства
  size_t                cached_state_block_mask_hash; //хэш маски состояния устройства

/// Конструктор
  Impl (BatchingManager& in_batching_manager, MaterialImpl& in_material)
    : batching_manager (in_batching_manager)
    , material (in_material)
    , cached_state_block_mask_hash ()
  {
  }
};

/*
    Конструктор / деструктор
*/

BatchStateBlock::BatchStateBlock (BatchingManager& batching_manager, MaterialImpl& material)
  : impl (new Impl (batching_manager, material))
{
  AttachCacheSource (batching_manager);
  AttachCacheSource (material);
}

BatchStateBlock::~BatchStateBlock ()
{
}

/*
    Блок состояний
*/

LowLevelStateBlockPtr BatchStateBlock::StateBlock ()
{
  UpdateCache ();

  return impl->cached_state_block;
}

/*
    Управление кэшированием
*/

void BatchStateBlock::UpdateCacheCore ()
{
  try
  {
    BatchingManager&                   batching_manager = impl->batching_manager;
    render::manager::DeviceManager&    device_manager   = batching_manager.DeviceManager ();
    render::low_level::IDevice&        device           = device_manager.Device ();
    render::low_level::IDeviceContext& context          = device_manager.ImmediateContext ();
    LowLevelStateBlockPtr              prev_state_block = impl->cached_state_block;

    LowLevelStateBlockPtr material_state_block = impl->material.StateBlock ();

    render::low_level::StateBlockMask mask;

    if (material_state_block)
    {
      material_state_block->Apply (&context);
      material_state_block->GetMask (mask);
    }

      //установка вершинных/индексного буфера

    mask.is_index_buffer       = true;
    mask.is_layout             = true;
    mask.is_vertex_buffers [0] = true;

    context.ISSetVertexBuffer (0, batching_manager.DynamicVertexBuffer ().LowLevelBuffer ().get ());
    context.ISSetInputLayout  (device_manager.InputLayoutManager ().DynamicPrimitivesInputLayout ().get ());
    context.ISSetIndexBuffer  (batching_manager.DynamicIndexBuffer ().LowLevelBuffer ().get ());

      //обновление блока состояний примитива
    
    size_t mask_hash = mask.Hash ();

    if (impl->cached_state_block_mask_hash != mask_hash)
    {
      impl->cached_state_block           = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
      impl->cached_state_block_mask_hash = mask_hash;
    }

    impl->cached_state_block->Capture (&context);    

    if (prev_state_block != impl->cached_state_block)
      InvalidateCacheDependencies ();
  }
  catch (xtl::exception& e)
  {
    impl->cached_state_block           = LowLevelStateBlockPtr ();
    impl->cached_state_block_mask_hash = 0;

    e.touch ("render::manager::BatchStateBlock::UpdateCacheCore");

    throw;
  }
  catch (...)
  {
    impl->cached_state_block           = LowLevelStateBlockPtr ();
    impl->cached_state_block_mask_hash = 0;

    throw;
  }
}

void BatchStateBlock::ResetCacheCore ()
{
  impl->cached_state_block           = LowLevelStateBlockPtr ();
  impl->cached_state_block_mask_hash = 0;
}

/*
===================================================================================================
    BatchingManager
===================================================================================================
*/

/*
    Описание реализации менеджера пакетирования
*/

namespace
{

/// Пул для выделения динамических вершин / индексов
template <class T> class Pool: public xtl::noncopyable
{
  public:
/// Конструктор
    Pool () : start (), pos (), end () {}

/// Начало буфера
    T* const* DataStart () { return &start; }

/// Выделение
    T* Allocate (unsigned int count)
    {
      T* prev_pos = pos;

      pos += count;

      if (pos <= end)
        return prev_pos;

      pos = prev_pos;

      return 0;
    }    

/// Предвыделение
    bool Preallocate (unsigned int count)
    {
      end -= count;

      if (end >= pos)
        return true;

      end += count;

      return false;
    }

/// Сброс
    void Reset (T* new_start, unsigned int size, bool save_relative_position)
    {
      unsigned int offset = (unsigned int)(pos - start);

      start = new_start;
      end   = new_start + size;
      pos   = save_relative_position ? start + offset : start;
    }

/// Размер
    unsigned int Size () { return (unsigned int)(pos - start); }

/// Установка размера
    bool SetSize (unsigned int size)
    {
      if (size > end - start)
        return false;

      pos = start + size;

      return true;
    }

  private:
    T* start;
    T* pos;
    T* end;  
};

typedef Pool<DynamicPrimitiveVertex> DynamicVertexPool;
typedef Pool<DynamicPrimitiveIndex>  DynamicIndexPool;

/// Вхождение в кэш
struct BatchStateBlockEntry
{
  BatchStateBlockPtr state_block;     //блок состояний
  FrameTime          last_use_time;   //последнее время использования
  FrameId            last_use_frame;  //последний кадр использования

  BatchStateBlockEntry (const BatchStateBlockPtr& in_state_block, FrameTime use_time, FrameId use_frame)
    : state_block (in_state_block)
    , last_use_time (use_time)
    , last_use_frame (use_frame)
  {
  }
};

typedef stl::hash_map<MaterialImpl*, BatchStateBlockEntry> StateBlockMap;

}

typedef xtl::uninitialized_storage<DynamicPrimitiveIndex> IndexArray;

struct BatchingManager::Impl: public Cache
{
  DeviceManagerPtr                      device_manager;       //менеджер устройства отрисовки
  render::manager::DynamicVertexBuffer  dynamic_vb;           //динамический вершинный буфер
  render::manager::DynamicIndexBuffer   dynamic_ib;           //динамический индексный буфер
  IndexArray                            temp_ib;              //временный индексный буфер
  DynamicVertexPool                     dynamic_vertex_pool;  //пул динамических вершин
  DynamicIndexPool                      dynamic_index_pool;   //пул динамических индексов
  DynamicIndexPool                      temp_index_pool;      //пул динамических индексов для построения примитивов
  StateBlockMap                         state_blocks;         //блоки состояний
  const void*                           pass_tag;             //тэг прохода
  unsigned int                          pass_first_index;     //первый индекс в проходе
  FrameId                               active_frame;         //номер активного кадра

/// Конструктор
  Impl (const DeviceManagerPtr& in_device_manager)
    : Cache (&in_device_manager->CacheManager ())
    , device_manager (in_device_manager)
    , dynamic_vb (render::low_level::UsageMode_Stream, render::low_level::BindFlag_VertexBuffer)
    , dynamic_ib (render::low_level::UsageMode_Stream, render::low_level::BindFlag_IndexBuffer)
    , pass_tag ()
    , pass_first_index ()
    , active_frame ()
  {
  }

/// Резервирование динамических буферов
  void ReserveDynamicBuffers (unsigned int vertices_count, unsigned int indices_count)
  {
    try
    {
      dynamic_vb.Reserve (vertices_count);
      dynamic_ib.Reserve (indices_count);
      temp_ib.reserve (indices_count);

      dynamic_vb.SyncBuffers (device_manager->Device ());
      dynamic_ib.SyncBuffers (device_manager->Device ());

      dynamic_vertex_pool.Reset (dynamic_vb.Data (), vertices_count, false);
      dynamic_index_pool.Reset (dynamic_ib.Data (), indices_count, false);
      temp_index_pool.Reset (temp_ib.data (), indices_count, false);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::BatchingManager::ReserveDynamicBuffers");
      throw;
    }
  }

/// Сброс кэша
  void FlushCache ()
  {
    if (state_blocks.empty ())
      return;

    FrameTime current_time  = CurrentTime (), time_delay = TimeDelay ();
    FrameId   current_frame = CurrentFrame (), frame_delay = FrameDelay ();

    for (StateBlockMap::iterator iter=state_blocks.begin (); iter!=state_blocks.end ();)
      if (iter->second.state_block->use_count () == 1 && current_time - iter->second.last_use_time <= time_delay && current_frame - iter->second.last_use_frame <= frame_delay)
      {
        StateBlockMap::iterator next = iter;

        ++next;

        state_blocks.erase (iter);  

        iter = next;
      }
      else ++iter;
  }
};

/*
    Конструктор / деструктор
*/

BatchingManager::BatchingManager (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
}

BatchingManager::~BatchingManager ()
{
}

/*
    Ссылка на менеджер устройства
*/

render::manager::DeviceManager& BatchingManager::DeviceManager ()
{
  return *impl->device_manager;
}

/*
    Резервирование буферов для динамических примитивов
*/

void BatchingManager::ReserveDynamicBuffers (unsigned int vertices_count, unsigned int indices_count)
{
  impl->ReserveDynamicBuffers (vertices_count, indices_count);

  InvalidateCacheDependencies ();
}

unsigned int BatchingManager::DynamicVerticesCount () const
{
  return impl->dynamic_vb.Capacity ();
}

unsigned int BatchingManager::DynamicIndicesCount () const
{
  return impl->dynamic_ib.Capacity ();
}

/*
    Динамические буферы
*/

render::manager::DynamicVertexBuffer& BatchingManager::DynamicVertexBuffer ()
{
  return impl->dynamic_vb;
}

render::manager::DynamicIndexBuffer& BatchingManager::DynamicIndexBuffer ()
{
  return impl->dynamic_ib;
}

const DynamicPrimitiveIndex* const* BatchingManager::TempIndexBuffer ()
{
  return impl->temp_index_pool.DataStart ();
}

/*
    Выделение вершин и индексов
*/

DynamicPrimitiveVertex* BatchingManager::AllocateDynamicVertices (unsigned int count, unsigned int* out_base_vertex_index)
{
  DynamicPrimitiveVertex* result = impl->dynamic_vertex_pool.Allocate (count);

  if (result)
  {
    impl->dynamic_vb.Resize (impl->dynamic_vertex_pool.Size ());

    if (out_base_vertex_index)
      *out_base_vertex_index = (unsigned int)(result - impl->dynamic_vb.Data ());

    return result;
  }

  throw xtl::format_operation_exception ("render::manager::BatchingManager::AllocateDynamicVertices", "Can't allocate %u vertices in dynamic vertex buffer (%u from %u have been already allocated)",
    count, impl->dynamic_vertex_pool.Size (), impl->dynamic_vb.Capacity ());
}

DynamicPrimitiveIndex* BatchingManager::AllocateDynamicIndices (IndexPoolType pool_type, unsigned int count)
{
  static const char* METHOD_NAME = "render::manager::BatchingManager::AllocateDynamicIndices";

  switch (pool_type)
  {
    case IndexPoolType_Linear:
    {
      DynamicPrimitiveIndex* result = impl->dynamic_index_pool.Allocate (count);

      if (result)
      {
        impl->dynamic_ib.Resize (impl->dynamic_index_pool.Size ());
        return result;
      }

      return 0; //null is indicator for effect renderer
    }
    case IndexPoolType_Temporary: 
    {
      DynamicPrimitiveIndex* result = impl->temp_index_pool.Allocate (count);

      if (result)
        return result;

      throw xtl::format_operation_exception (METHOD_NAME, "Can't allocate %u indices in dynamic temporary index buffer (%u from %u have been already allocated)",
        count, impl->temp_index_pool.Size (), impl->dynamic_ib.Capacity ());
    }
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "pool_type", pool_type);
  }  
}

void BatchingManager::ResetLinearIndexBuffer ()
{
  impl->dynamic_ib.Clear ();

  impl->dynamic_index_pool.Reset (impl->dynamic_ib.Data (), impl->dynamic_ib.Capacity (), false);
}

void BatchingManager::ResetDynamicBuffers ()
{
  impl->dynamic_vb.Clear ();
  impl->dynamic_ib.Clear ();
  impl->temp_ib.resize (0, false);

  impl->dynamic_vertex_pool.Reset (impl->dynamic_vb.Data (), impl->dynamic_vb.Capacity (), false);
  impl->dynamic_index_pool.Reset (impl->dynamic_ib.Data (), impl->dynamic_ib.Capacity (), false);
  impl->temp_index_pool.Reset (impl->temp_ib.data (), (unsigned int)impl->temp_ib.capacity (), false);
}

/*
    Предвыделение вершин
*/

void BatchingManager::PreallocateDynamicVertices (unsigned int count)
{
  if (impl->dynamic_vertex_pool.Preallocate (count))
    return;

  throw xtl::format_operation_exception ("render::manager::BatchingManager::PreallocateDynamicVertices", "Can't preallocate %u vertices in dynamic vertex buffer", count);
}

void BatchingManager::ResetDynamicVerticesPreallocations ()
{
  impl->dynamic_vertex_pool.Reset (impl->dynamic_vb.Data (), impl->dynamic_vb.Capacity (), true);
}

/*
    Текущее количество выделенных вершин
*/

void BatchingManager::SetAllocatedDynamicVerticesCount (unsigned int count)
{
  if (impl->dynamic_vertex_pool.SetSize (count))
    return;

  impl->dynamic_vb.Resize (impl->dynamic_vertex_pool.Size ());

  throw xtl::format_operation_exception ("render::manager::BatchingManager::SetAllocatedDynamicVerticesCount", "Can't change dynamic vertex pool size to %u", count);  
}

unsigned int BatchingManager::AllocatedDynamicVerticesCount ()
{
  return impl->dynamic_vertex_pool.Size ();
}

/*
    Получение блока состояний по материалу
*/

BatchStateBlockPtr BatchingManager::GetStateBlock (MaterialImpl* material)
{
  try
  {
    if (!material)
      return BatchStateBlockPtr ();

    StateBlockMap::iterator iter = impl->state_blocks.find (material);

    if (iter != impl->state_blocks.end ())
    {
      iter->second.last_use_time  = impl->CurrentTime ();
      iter->second.last_use_frame = impl->CurrentFrame ();

      return iter->second.state_block;
    }

    BatchStateBlockPtr state_block (new BatchStateBlock (*this, *material), false);

    impl->state_blocks.insert_pair (material, BatchStateBlockEntry (state_block, impl->CurrentTime (), impl->CurrentFrame ()));

    return state_block;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::BatchingManager::GetStateBlock");
    throw;
  }
}

/*
    Тэг текущего прохода
*/

void BatchingManager::SetPassUserData (const void* tag)
{
  impl->pass_tag = tag;
}

const void* BatchingManager::PassUserData ()
{
  return impl->pass_tag;
}

void BatchingManager::SetPassFirstIndex (unsigned int first)
{
  impl->pass_first_index = first;
}

unsigned int BatchingManager::PassFirstIndex ()
{
  return impl->pass_first_index;
}

/*
    Номер активного кадра
*/

void BatchingManager::SetActiveFrame (FrameId frame_id)
{
  impl->active_frame = frame_id;
}

FrameId BatchingManager::ActiveFrame ()
{
  return impl->active_frame;
}
