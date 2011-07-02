#include "shared.h"

using namespace render;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

namespace
{

///������� ���� ����� �������
struct ShaderOptionsCacheEntry: public ShaderOptions, public xtl::reference_counter
{
  size_t layout_hash;   //�������������� ��� ������������ �������
  size_t options_count; //�������������� ���������� �����
  bool   is_valid;      //�������� �� ������ ������� ����������
  
///�����������
  ShaderOptionsCacheEntry () : is_valid (false), layout_hash (0xffffffff), options_count (0) {}
  
///���������� ����
  void UpdateCache (const common::PropertyMap& properties, ShaderOptionsLayout& layout)
  {
    if (is_valid && layout.Hash () == layout_hash && options_count == layout.Size ())
      return;
      
    layout.GetShaderOptions (properties, *this);
    
    layout_hash   = layout.Hash ();
    options_count = layout.Size ();
    is_valid      = true;
  }
};

typedef xtl::intrusive_ptr<ShaderOptionsCacheEntry>                     ShaderOptionsCacheEntryPtr;
typedef stl::hash_map<ShaderOptionsLayout*, ShaderOptionsCacheEntryPtr> ShaderOptionsCacheEntryMap;

}

/*
    �������� ���������� ���� ����� �������
*/

struct ShaderOptionsCache::Impl: public xtl::trackable
{
  common::PropertyMap               properties;                //������ ������ �����
  common::PropertyMap::EventHandler properties_update_handler; //���������� ������� ���������� �������
  xtl::auto_connection              on_properties_update;      //���������� � ������������ ���������� �������
  ShaderOptionsCacheEntryMap        cache;                     //��� ����� �������
  bool                              need_invalidate_cache;     //��� ��������� ��������
  
///�����������
  Impl ()
    : properties_update_handler (xtl::bind (&Impl::OnPropertiesUpdate, this))
    , need_invalidate_cache (false)
  {
  }
  
///���������� ������� ���������� �������
  void OnPropertiesUpdate ()
  {
    need_invalidate_cache = true;
  }
  
///���������� ������� �������� ������������ ����� �������
  void OnLayoutDeleted (ShaderOptionsLayout* layout)
  {
    cache.erase (layout);
  }
};

/*
    ����������� / ����������
*/

ShaderOptionsCache::ShaderOptionsCache ()
  : impl (new Impl)
{
}

ShaderOptionsCache::~ShaderOptionsCache ()
{
}

/*
    ���������� �������������� ������������ ����� �������
*/

size_t ShaderOptionsCache::CachedLayoutsCount ()
{
  return impl->cache.size ();
}

/*
    ������ ������ �����
*/

void ShaderOptionsCache::SetProperties (const common::PropertyMap& properties)
{
  impl->on_properties_update  = properties.RegisterEventHandler (common::PropertyMapEvent_OnUpdate, impl->properties_update_handler);
  impl->properties            = properties;
  impl->need_invalidate_cache = true;
}

const common::PropertyMap& ShaderOptionsCache::Properties ()
{
  return impl->properties;
}

/*
    ��������� ����� �������
*/

const ShaderOptions& ShaderOptionsCache::GetShaderOptions (ShaderOptionsLayout& layout)
{
  try
  {
      //��������� �������� ������ - ���������� �������
      
    if (impl->properties.Size () == 0)
      return layout.GetDefaultShaderOptions ();
    
      //���������� ������ �����������
    
    if (impl->need_invalidate_cache)
    {
      for (ShaderOptionsCacheEntryMap::iterator iter=impl->cache.begin (), end=impl->cache.end (); iter!=end; ++iter)
        iter->second->is_valid = false;
        
      impl->need_invalidate_cache = false;
    }
     
      //����� � ����
      
    ShaderOptionsCacheEntryMap::iterator iter = impl->cache.find (&layout);
    
    if (iter != impl->cache.end ())
    {
      iter->second->UpdateCache (impl->properties, layout);
      return *iter->second;
    }
    
      //���������� �������� ����
      
    ShaderOptionsCacheEntryPtr entry (new ShaderOptionsCacheEntry, false);
    
    xtl::connection on_deleted_connection = layout.GetTrackable ().connect_tracker (xtl::bind (&Impl::OnLayoutDeleted, &*impl, &layout), *impl);
    
    try
    {
      impl->cache.insert_pair (&layout, entry);
      
      entry->UpdateCache (impl->properties, layout);
      
      return *entry;
    }
    catch (...)
    {
      on_deleted_connection.disconnect ();
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::ShaderOptionsCache::GetShaderOptions");
    throw;
  }
}
