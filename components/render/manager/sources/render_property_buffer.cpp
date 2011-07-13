#include "shared.h"

using namespace render;
using namespace common;

/*
    ���������
*/

const size_t MIN_BUFFER_SIZE = 16; //����������� ������ ������ ��������

/*
    �������� ���������� ������ �������
*/

struct PropertyBuffer::Impl
{
  DeviceManagerPtr     device_manager;     //�������� ���������� ���������
  LowLevelBufferPtr    buffer;             //����������� �����
  common::PropertyMap  properties;         //�������� ��������
  xtl::auto_connection update_connection;  //���������� ���������� �������
  bool                 need_update;        //���� ������������� ���������� ������
  size_t               cached_buffer_size; //�������������� ������ ������

///�����������
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , need_update (true)
    , cached_buffer_size (0)
  {
  }  
};

/*
    ����������� / ����������
*/

PropertyBuffer::PropertyBuffer (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
  impl->update_connection = impl->properties.RegisterEventHandler (PropertyMapEvent_OnUpdate, xtl::bind (&PropertyBuffer::OnPropertiesUpdated, this));
}

PropertyBuffer::~PropertyBuffer ()
{
}

/*
    ��������� �������
*/

void PropertyBuffer::SetProperties (const common::PropertyMap& in_properties)
{    
  if (impl->properties.Id () == in_properties.Id ())
    return;

  impl->update_connection = in_properties.RegisterEventHandler (PropertyMapEvent_OnUpdate, xtl::bind (&PropertyBuffer::OnPropertiesUpdated, this));
  impl->properties        = in_properties;  
  impl->need_update       = true;
}                    

const common::PropertyMap& PropertyBuffer::Properties ()
{
  return impl->properties;
}

/*
    �������� ���������
*/

void PropertyBuffer::OnPropertiesUpdated ()
{
  if (impl->need_update)
    return;
    
  impl->need_update = true;
    
  InvalidateCache (false);
}

/*
    ��������� ������
*/

const LowLevelBufferPtr& PropertyBuffer::Buffer ()
{
  try
  {
    if (!impl->need_update)
      return impl->buffer;

    UpdateCache ();
        
    return impl->buffer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PropertyBuffer::Buffer");
    throw;
  }
}

/*
    ���������� ����
*/

void PropertyBuffer::UpdateCacheCore ()
{
  try
  {
    if (!impl->need_update)
      return;
    
    size_t buffer_size = impl->properties.BufferSize ();
    
    bool need_recreate_buffer = !impl->buffer || buffer_size > impl->cached_buffer_size;
    
    if (need_recreate_buffer)
    {
        //�������� ������ ������
        
      render::low_level::BufferDesc desc;

      memset (&desc, 0, sizeof (desc));
      
      desc.size         = stl::max (buffer_size, MIN_BUFFER_SIZE);
      desc.usage_mode   = render::low_level::UsageMode_Default;
      desc.bind_flags   = render::low_level::BindFlag_ConstantBuffer;
      desc.access_flags = render::low_level::AccessFlag_ReadWrite;
      
      impl->buffer             = LowLevelBufferPtr (impl->device_manager->Device ().CreateBuffer (desc), false);
      impl->cached_buffer_size = desc.size;
      
      InvalidateCacheDependencies ();
    }

    impl->buffer->SetData (0, buffer_size, impl->properties.BufferData ());
    
    impl->need_update = false;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PropertyBuffer::UpdateCacheCore");
    throw;
  }
}
