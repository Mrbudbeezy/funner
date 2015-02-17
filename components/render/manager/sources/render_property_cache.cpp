#include "shared.h"

using namespace render::manager;

namespace
{

/*
    ���������
*/

const unsigned int RESERVED_BUFFER_POOL_SIZE = 256;
const unsigned int MIN_BUFER_SIZE            = 64;

/*
    ��������������� ���������
*/

///������� ���� �������
struct BufferPoolEntry
{
  LowLevelBufferPtr buffer;          //����� ������
  FrameTime         last_use_time;   //��������� ����� �������������
  FrameId           last_use_frame;  //��������� ���� �������������
};

typedef stl::vector<BufferPoolEntry> BufferArray;

///��� ������� �������
struct BufferPool: public xtl::reference_counter, private Cache
{
  BufferArray                   buffers;               //��� ��������� ������
  size_t                        first_available_index; //������ ������� ��������� ������
  render::low_level::BufferDesc buffer_desc;           //��������� ������
  SettingsPtr                   settings;              //���������
  Log                           log;                   //����� ����������� ������

///�����������
  BufferPool (unsigned int buffer_size, const DeviceManagerPtr& device_manager)
    : Cache (&device_manager->CacheManager ())
    , first_available_index (0)
    , settings (&device_manager->Settings ())
  {
    if (!settings)
      throw xtl::make_null_argument_exception ("render::manager::BufferPool::BufferPool", "settings");
    
    buffers.reserve (RESERVED_BUFFER_POOL_SIZE);
    
    memset (&buffer_desc, 0, sizeof (buffer_desc));
    
    buffer_desc.size         = buffer_size;
    buffer_desc.usage_mode   = render::low_level::UsageMode_Default;
    buffer_desc.bind_flags   = render::low_level::BindFlag_ConstantBuffer;
    buffer_desc.access_flags = render::low_level::AccessFlag_ReadWrite;
    
    if (settings->HasDebugLog ())
      log.Printf ("Property cache buffer pool created for buffer size %u", buffer_size);
  }
  
///����������
  ~BufferPool ()
  {
    if (settings->HasDebugLog ())
      log.Printf ("Property cache buffer pool destroyed for buffer size %u", buffer_desc.size);
  }
  
///�����
  void Reset ()
  {
    first_available_index = 0;
  }
  
///��������� ������
  LowLevelBufferPtr Allocate (render::low_level::IDevice& device)
  {
    if (first_available_index < buffers.size ())
    {
      BufferPoolEntry& entry = buffers [first_available_index++];

      entry.last_use_time  = CurrentTime ();
      entry.last_use_frame = CurrentFrame ();

      return entry.buffer;
    }
      
    LowLevelBufferPtr result (device.CreateBuffer (buffer_desc), false);
    
    BufferPoolEntry entry;
    
    entry.buffer         = result;
    entry.last_use_time  = CurrentTime ();
    entry.last_use_frame = CurrentFrame ();
    
    buffers.push_back (entry);
    
    first_available_index++;
    
    if (settings->HasDebugLog ())
      log.Printf ("Property cache buffer allocated with size %u", buffer_desc.size);
    
    return result;
  }
  
///����� �������������� ���������
  void FlushCache ()
  {
    if (buffers.empty ())
      return;
    
    FrameTime current_time  = CurrentTime (), time_delay = TimeDelay ();
    FrameId   current_frame = CurrentFrame (), frame_delay = FrameDelay ();

    for (BufferArray::iterator iter=buffers.end ()-1, end=buffers.begin ()-1; iter!=end; --iter)
      if (current_time - iter->last_use_time <= time_delay && current_frame - iter->last_use_frame <= frame_delay)
      {
        size_t buffers_count = buffers.end () - iter - 1;
        
        if (buffers_count)
        {
          if (settings->HasDebugLog ())
            log.Printf ("Property cache %u buffers destroyed with size %u", buffers_count, buffer_desc.size);

          buffers.erase (iter+1, buffers.end ());
        }

        return;
      }
  }
};

typedef xtl::intrusive_ptr<BufferPool> BufferPoolPtr;

//��������� ��������� ������ ������� ������
unsigned int get_next_higher_power_of_two (unsigned int k)
{
  if (!k)
    return 1;

  k--;

  for (unsigned int i=1; i < sizeof (unsigned int) * 8; i *= 2)
    k |= k >> i;

  return k + 1;
}

}

/*
    �������� ���������� ����
*/

typedef CacheMap<size_t, BufferPoolPtr> PoolMap;

struct PropertyCache::Impl
{
  DeviceManagerPtr           device_manager;   //�������� ���������� ���������
  PoolMap                    pools;            //���� ������� �������
  size_t                     last_hash;        //��������� ����������� ���
  size_t                     last_buffer_size; //��������� ����������� ������ ������
  ProgramParametersLayoutPtr last_layout;      //��������� ����������� ������
  BufferPoolPtr              last_pool;        //��������� ����������� ���
  
///�����������
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , pools (&device_manager->CacheManager ())
    , last_hash (0)
    , last_buffer_size (0)
  {
  }
};

/*
    ����������� / ����������
*/

PropertyCache::PropertyCache (const DeviceManagerPtr& device_manager)
{
  try
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
    
    impl = new Impl (device_manager); 
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PropertyCache::PropertyCache");
    throw;
  }
}

PropertyCache::~PropertyCache ()
{
}

/*
    ��������� ������� ����������
*/

void PropertyCache::Convert (const common::PropertyMap& source_map, LowLevelBufferPtr& result_buffer, ProgramParametersLayoutPtr& result_layout)
{
  try
  {
    const common::PropertyLayout& layout = source_map.Layout ();
    
    size_t       hash                  = layout.Hash ();
    unsigned int buffer_size           = (unsigned int)layout.BufferSize (),
                 corrected_buffer_size = stl::max (get_next_higher_power_of_two (buffer_size), MIN_BUFER_SIZE);
           
      //����� �������
      
    if (impl->last_hash != hash)
    {
      impl->last_layout = impl->device_manager->ProgramParametersManager ().GetParameters (ProgramParametersSlot_FrameEntity, layout, true);
      impl->last_hash   = hash;
    }    
    
      //����� ���� �������
      
    if (impl->last_buffer_size != corrected_buffer_size)
    {
      if (BufferPoolPtr* pool = impl->pools.Find (corrected_buffer_size))
      {
        impl->last_pool = *pool;
      }
      else
      {
        BufferPoolPtr new_pool (new BufferPool (corrected_buffer_size, impl->device_manager), false);
        
        impl->pools.Add (corrected_buffer_size, new_pool);

        impl->last_pool = &*new_pool;
      }

      impl->last_buffer_size = corrected_buffer_size;
    }
    
      //��������� ������      

    LowLevelBufferPtr buffer = impl->last_pool->Allocate (impl->device_manager->Device ());

      //��������� ������

    buffer->SetData (0, buffer_size, source_map.BufferData ());

      //���������� ����������
    
    result_buffer = buffer;
    result_layout = impl->last_layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PropertyCache::Convert");
    
    throw;
  }
}

/*
    ����� ����
*/

namespace
{

struct PoolReseter
{
  void operator () (size_t, const BufferPoolPtr& pool) const
  {
    pool->Reset ();
  }
};

}

void PropertyCache::Reset ()
{
  impl->pools.ForEach (PoolReseter ());
    
  impl->last_hash        = 0;
  impl->last_buffer_size = 0;
  impl->last_pool        = BufferPoolPtr ();
  impl->last_layout      = ProgramParametersLayoutPtr ();
}
