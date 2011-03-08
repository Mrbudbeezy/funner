#include "shared.h"

using namespace render;

namespace render
{

/*
===================================================================================================
    ResourceProxyManagerImpl
===================================================================================================
*/

template <class Ptr> struct ResourceProxyManagerImpl
{
  typedef Ptr                                              Pointer;
  typedef ResourceProxy<Pointer>                           Proxy;
  typedef stl::hash_map<stl::hash_key<const char*>, Proxy> ProxyMap;
  
  ProxyMap proxies;        //������ �������
  Pointer  default_object; //������ �� ���������
};

}

/*
===================================================================================================
    ResourceProxy
===================================================================================================
*/

template <class Ptr> struct ResourceProxy<Ptr>::Impl: public xtl::reference_counter
{
  typedef typename ManagerImpl::ProxyMap ProxyMap;
  typedef typename ProxyMap::iterator    ProxyMapIterator;

  Pointer          object;         //�������� ������  
  ManagerImpl&     manager;        //������ �� �������� 
  stl::string      name;           //��� �������
  ProxyMapIterator proxy_position; //������� � ����� ������ ��������
  
  Impl (ManagerImpl& in_manager, const char* in_name)
    : manager (in_manager)
    , name (in_name ? in_name : "")
  {
  }
};

/*
    ������������ / ���������� / ������������
*/

template <class Ptr>
ResourceProxy<Ptr>::ResourceProxy (ManagerImpl& manager, const char* name)
  : impl (new Impl (manager, name))
{
  try
  {
    stl::pair<typename Impl::ProxyMapIterator, bool> result = impl->manager.proxies.insert_pair (impl->name.c_str (), *this);
    
    if (!result.second)
      throw xtl::format_operation_exception ("render::ResourceProxy<Ptr>::ResourceProxy", "Internal error: resource with name '%s' already exists", impl->name.c_str ());
      
    impl->proxy_position = result.first;
  }
  catch (...)
  {
    release (impl);
    throw;
  }
}

template <class Ptr>
ResourceProxy<Ptr>::ResourceProxy (const ResourceProxy& proxy)
  : impl (proxy.impl)
{
  addref (impl);
}

template <class Ptr>
ResourceProxy<Ptr>::~ResourceProxy ()
{
  release (impl);
}

template <class Ptr>
ResourceProxy<Ptr>& ResourceProxy<Ptr>::operator = (const ResourceProxy& proxy)
{
  ResourceProxy tmp (proxy);
  
  stl::swap (impl, tmp.impl);
  
  return *this;
}

/*
    ��������� ������
*/

template <class Ptr>
typename ResourceProxy<Ptr>::Pointer ResourceProxy<Ptr>::Data ()
{
  if (impl->object)
    return impl->object;
    
  if (impl->manager.default_object)
    return impl->manager.default_object;
    
  throw xtl::format_operation_exception ("render::ResourceProxy<Ptr>::Data", "Resource '%s' not found. Default object hasn't set", impl->name.c_str ());
}

template <class Ptr>
const char* ResourceProxy<Ptr>::Name ()
{
  return impl->name.c_str ();
}

/*
===================================================================================================
    ResourceProxyManager
===================================================================================================
*/

/*
    ����������� / ����������
*/

template <class Ptr>
ResourceProxyManager<Ptr>::ResourceProxyManager ()
  : impl (new Impl)
{
}

template <class Ptr>
ResourceProxyManager<Ptr>::~ResourceProxyManager ()
{
}

/*
    �������� ������
*/

template <class Ptr>
typename ResourceProxyManager<Ptr>::Proxy ResourceProxyManager<Ptr>::CreateProxy (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::ResourceProxyManager::CreateProxy", "name");
    
    //����� ������ � ����
    
  typedef typename Impl::ProxyMapIterator ProxyMapIterator;
  
  ProxyMapIterator iter = impl->proxies.find (name);
  
  if (iter != impl->proxies.end ())
    return iter->second;
    
    //�������� ������ ������
    
  return Proxy (*impl, name);
}

/*
    ��������� ������� �� ���������
*/

template <class Ptr>
void ResourceProxyManager<Ptr>::SetDefault (const Pointer& ptr)
{
  impl->default_object = ptr;
}

template <class Ptr>
typename ResourceProxyManager<Ptr>::Pointer ResourceProxyManager<Ptr>::Default ()
{
  return impl->default_object;
}

/*
    ���������������
*/

template class ResourceProxy<TexturePtr>;
template class ResourceProxy<MaterialPtr>;
template class ResourceProxy<PrimitivePtr>;
