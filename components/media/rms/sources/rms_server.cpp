#include "shared.h"

using namespace media::rms;

namespace
{

/*
    ���������
*/

const size_t RESOURCE_ARRAY_RESERVE_SIZE = 128;   //������������� ����� �������� � ������ ��������
const size_t SERVER_ARRAY_RESERVE_SIZE   = 8;     //������������� ����� �������� � ������ ��������
const bool   DEFAULT_CACHE_STATE         = false; //��������� ���� �������� �� ���������

/*
    ��������� ���������� �� �������� �������
*/

class IResourceDestroyListener
{
  public:
    virtual void OnDestroyResource (const char* name) {}

  protected:
    virtual ~IResourceDestroyListener () {}
};

/*
    ������ �� ������� �������� ��������
*/

template <class Fn> void do_query (ICustomServer** servers, size_t count, const char* resource_name, const Fn& fn, bool ignore_exceptions, bool reverse_order = false)
{
  if (ignore_exceptions)
  {
    if (reverse_order)
    {
      for (size_t i=0; i<count; i++)
      {
        try
        {
          (servers [count-i-1]->*fn)(resource_name);
        }
        catch (...)
        {
          //������������� ����������
        }
      }      
    }
    else
    {
      for (size_t i=0; i<count; i++)
      {
        try
        {
          (servers [i]->*fn)(resource_name);
        }
        catch (...)
        {
          //������������� ����������
        }
      }
    }
  }
  else
  {
    try
    {
      if (reverse_order)
      {
        for (size_t i=0; i<count; i++)
        {
          (servers [i]->*fn)(resource_name);
        }
      }
      else
      {
        for (size_t i=0; i<count; i++)
        {
          (servers [count-i-1]->*fn)(resource_name);
        }        
      }
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("media::rms::do_query");
      throw;
    }
  }
}

/*
    �������� �������
*/

enum ResourceState
{
  ResourceState_Unloaded,   //������ ��������
  ResourceState_Prefetched, //������ ����������
  ResourceState_Loaded      //������ ��������
};

class Resource: public xtl::noncopyable, public xtl::reference_counter
{
  public:
///�����������
    Resource (const char* in_name, IResourceDestroyListener* in_destroy_listener) :
      name (in_name),
      state (ResourceState_Unloaded),
      destroy_listener (in_destroy_listener)
    {}

///����������
    ~Resource ()
    {
      if (destroy_listener)
        destroy_listener->OnDestroyResource (name.c_str ());
    }

///��� �������
    const char* Name () const { return name.c_str (); }

///��������� �������
    ResourceState State () const { return state; }

    void SetState (ResourceState new_state) { state = new_state; }

  private:
    stl::string               name;             //��� �������
    ResourceState             state;            //��������� �������
    IResourceDestroyListener* destroy_listener; //��������� �������� �������
};

typedef xtl::intrusive_ptr<Resource>                         ResourcePtr;
typedef stl::vector<ResourcePtr>                             ResourceArray;
typedef stl::hash_map<stl::hash_key<const char*>, Resource*> ResourceMap; //weak-ref

/*
    ���������� � ������� ��������
*/

class GroupBinding;

typedef stl::list<GroupBinding*>    GroupBindingList;
typedef stl::vector<ICustomServer*> ServerList;

class GroupBinding: public ICustomBinding, public xtl::trackable
{
  public:
///�����������
    GroupBinding (ServerList& in_servers, ResourceArray& in_resources, GroupBindingList& in_group_bindings)
      : servers (in_servers),
        group_bindings (in_group_bindings)
    {
        //������������� ������� �������� ������

      resources.swap (in_resources);

        //���������� � ������ ����� ����������

      group_bindings_pos = group_bindings.insert (group_bindings.end (), this);
    }

///����������
    ~GroupBinding ()
    {
      try
      {
          //���������� � ��������� �������� �������� �� ������� ������ �� ������ ������

        SetResourcesState (ResourceState_Unloaded, &ICustomServer::UnloadResource, DestructorPredicate (), true, true);
      }
      catch (...)
      {
        //���������� ���� ����������
      }

        //�������� �� ������ ����� ����������

      group_bindings.erase (group_bindings_pos);
    }

///����������� ������ ��������
    void Prefetch ()
    {
      SetResourcesState (ResourceState_Prefetched, &ICustomServer::PrefetchResource, StatePredicate<stl::less<ResourceState> > (), true);
    }

///�������� ������ ��������
    void Load ()
    {
      SetResourcesState (ResourceState_Loaded, &ICustomServer::LoadResource, StatePredicate<stl::less<ResourceState> > (), false);
    }

///�������� ������ ��������
    void Unload ()
    {
      SetResourcesState (ResourceState_Unloaded, &ICustomServer::UnloadResource, StatePredicate<stl::greater<ResourceState> > (), true, true);
    }

///��������� �������, ������������ � ��������� �������� GroupBinding
    xtl::trackable* GetTrackable () { return this; }

///��������� ������� ��������
    ResourceArray& Resources () { return resources; }
        
  private:
    template <class Fn, class Pred, class Iter> void SetResourcesState (ResourceState state, Fn fn, Pred pred, bool ignore_exceptions, bool reverse_order, Iter first, Iter last)
    {
      for (; first!=last; ++first)        
      {
        Resource& resource = **first;

        if (pred (resource, state))
        {
          do_query (&servers [0], servers.size (), resource.Name (), fn, ignore_exceptions, reverse_order);
          resource.SetState (state);
        }
      }
    }
  
    template <class Fn, class Pred> void SetResourcesState (ResourceState state, Fn fn, Pred pred, bool ignore_exceptions, bool reverse_order = false)
    {
      if (reverse_order)
      {
        SetResourcesState (state, fn, pred, ignore_exceptions, reverse_order, resources.rbegin (), resources.rend ());
      }
      else
      {
        SetResourcesState (state, fn, pred, ignore_exceptions, reverse_order, resources.begin (), resources.end ());
      }
    }

    template <class Pred> struct StatePredicate
    {
      bool operator () (const Resource& resource, ResourceState state) const { return Pred () (resource.State (), state); }
    };

    struct DestructorPredicate
    {
      bool operator () (const Resource& resource, ResourceState state) const
      {
        return resource.State () > state && resource.use_count () == 1;
      }
    };

  private:
    ServerList&                servers;            //������ �������� ��������
    ResourceArray              resources;          //�������
    GroupBindingList&          group_bindings;     //������ ����� ����������
    GroupBindingList::iterator group_bindings_pos; //��������� � ������ ����� ����������
};

}

/*
    �������� ���������� ������� ��������
*/

struct ServerGroup::Impl: public IResourceDestroyListener, public IServerGroupInstance, public xtl::reference_counter
{
  ServerList          servers;        //������ ���������� ��������� (������ ��� ��������)
  stl::string         name;           //��� �������
  stl::string         filters_string; //������, ���������� ������� �������������� ��������
  common::StringArray filters;        //������ �������� ����� ��������
  ResourceMap         resources;      //�������
  GroupBindingList    group_bindings; //������ ����� ����������
  bool                cache_state;    //������������ �� ��� ��������

///�����������
  Impl (const char* in_name)
    : name (in_name)
    , cache_state (DEFAULT_CACHE_STATE)
  {
    servers.reserve (SERVER_ARRAY_RESERVE_SIZE);    
      
    UpdateFilters ("*");

    ResourceManagerSingleton::Instance ().RegisterServerGroup (name.c_str (), this);
  }
  
///����������
  ~Impl ()
  {
      //������ ����������� � ��������� ��������

    try
    {
      ResourceManagerSingleton::Instance ().UnregisterServerGroup (this);
    }
    catch (...)
    {
    }

      //��������� �������������� �������� ����� ����������

    while (!group_bindings.empty ())
    {
      GroupBinding* binding = group_bindings.back ();

      binding->Unload ();

      delete binding;
    }

      //����� ���� ��������

    FlushUnusedResources ();    
  }

///���������� ������� �������� ����� ��������
  void UpdateFilters (const char* str)
  {
    common::StringArray new_filters = common::split (str);

    filters_string = str;

    filters.Swap (new_filters);
  }

///���������� �� �������� �������
  void OnDestroyResource (const char* name)
  {
    ResourceMap::iterator iter = resources.find (name);

    if (iter == resources.end ())
      return;

    if (iter->second->State () > ResourceState_Unloaded)
    {
      try
      {
        const char* resource_name = iter->second->Name ();

        do_query (&servers [0], servers.size (), resource_name, &ICustomServer::UnloadResource, true, true);
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }

    resources.erase (iter);
  }

///����� �������������� ��������
  void FlushUnusedResources ()
  {
      //������ ������ ����� ����� ������ � ������ ����������� ����������� �������������� ��������

    if (!cache_state)
      return;

    for (ResourceMap::reverse_iterator iter = resources.rbegin (); iter != resources.rend ();)
    {
      Resource& resource = *iter->second;      

      if (resource.use_count () == 1) //���� ������ ��������� ������ � ����
      {
        ResourceMap::reverse_iterator next = iter;        

        ++++next;
        
        if (resource.State () > ResourceState_Unloaded)
        {
          do_query (&servers [0], servers.size (), resource.Name (), &ICustomServer::UnloadResource, true, true);

          resource.SetState (ResourceState_Unloaded);
        }

        release (&resource);
        
        iter = --next;
      }
      else ++iter;
    }
  }
  
///��� ������
  const char* Name ()
  {
    return name.c_str ();
  }
  
///��������� ����������
  ServerGroup Instance ()
  {
    return ServerGroup (this);
  }
  
///������� ������
  void AddRef ()
  {
    addref (this);
  }
  
  void Release ()
  {
    release (this);
  }
};

/*
    ������������ / ���������� / ������������
*/

ServerGroup::ServerGroup (Impl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

ServerGroup::ServerGroup (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    IServerGroupInstance* instance = ResourceManagerSingleton::Instance ().FindServerGroup (name);    
    
    if (instance)
    {
      impl = instance->Instance ().impl;

      addref (impl);
    }
    else
    {
      impl = new Impl (name);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ServerGroup::ServerGroup");
    throw;
  }
}

ServerGroup::ServerGroup (const ServerGroup& group)
  : impl (group.impl)
{
  addref (impl);
}

ServerGroup::~ServerGroup ()
{
  release (impl);
}

ServerGroup& ServerGroup::operator = (const ServerGroup& group)
{
  ServerGroup (group).Swap (*this);

  return *this;
}

/*
    ��� ������
*/

const char* ServerGroup::Name () const
{
  return impl->name.c_str ();
}

void ServerGroup::SetName (const char* name)
{  
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    ServerGroup (name).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ServerGroup::SetName");
    throw;
  }
}

/*
    ���������� / �������� �������� ��������
*/

void ServerGroup::Attach (ICustomServer& server)
{
  try
  {
      //�������� ���������� ����������

    for (ServerList::iterator iter=impl->servers.begin (), end=impl->servers.end (); iter!=end; ++iter)
      if (*iter == &server)
        return;

      //�������������� ��������� ��������
      
    ICustomServer* server_ptr = &server;      

    for (ResourceMap::iterator iter=impl->resources.begin (), end=impl->resources.end (); iter!=end; ++iter)
    {
      Resource& resource = *iter->second;

      if (resource.State () == ResourceState_Loaded)
        do_query (&server_ptr, 1, resource.Name (), &ICustomServer::LoadResource, false);
    }

      //���������� �������

    impl->servers.push_back (&server);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ServerGroup::Attach");
    throw;
  }
}

void ServerGroup::Detach (ICustomServer& server)
{
  for (ServerList::iterator iter=impl->servers.begin (), end=impl->servers.end (); iter!=end; ++iter)
    if (*iter == &server)
    {
        //�������� �������� � �������
        
      ICustomServer* server_ptr = &server;        
             
      for (ResourceMap::reverse_iterator res_iter=impl->resources.rbegin (), res_end=impl->resources.rend (); res_iter!=res_end; ++res_iter)
      {
        Resource& resource = *res_iter->second;

        if (resource.State () > ResourceState_Unloaded)
          do_query (&server_ptr, 1, resource.Name (), &ICustomServer::UnloadResource, true);
      }

        //�������� ������� �� ������ ��������
      
      impl->servers.erase (iter);

      return;
    }
}

/*
    ������� ��� �������������� ��������
*/

const char* ServerGroup::Filters () const
{
  return impl->filters_string.c_str ();
}

void ServerGroup::SetFilters (const char* filters_string)
{
  if (!filters_string)
    throw xtl::make_null_argument_exception ("media::rms::ServerGroup::SetFilters", "filters_string");

  impl->UpdateFilters (filters_string);
}

void ServerGroup::AddFilters (const char* filters_string)
{
  if (!filters_string)
    throw xtl::make_null_argument_exception ("media::rms::ServerGroup::AddFilters", "filters_string");

  stl::string new_filters_string = impl->filters_string + ' ' + filters_string;

  impl->UpdateFilters (new_filters_string.c_str ());
}

void ServerGroup::ResetFilters ()
{
  impl->filters_string.clear ();
  impl->filters.Clear ();
}

/*
    �������� ���������� � ������� ��������
*/

Binding ServerGroup::CreateBinding (const Group& group)
{
  try
  {
      //�������� ������� �������� ������

    ResourceArray group_resources;

    group_resources.reserve (RESOURCE_ARRAY_RESERVE_SIZE);

        //�������� ������ ��������

    for (size_t i=0, group_count=group.Size (); i<group_count; i++)
    {
      const char* resource_name = group.Item (i);

      for (size_t j=0, filter_count=impl->filters.Size (); j<filter_count; j++)
      {
        const char* filter = impl->filters [j];

        if (!common::wcimatch (resource_name, filter))
          continue;

          //��� ������� ������������� �������

          //����� ������� ����� ��� ������������������

        ResourceMap::iterator iter = impl->resources.find (resource_name);

        if (iter != impl->resources.end ())
        {
            //������ ��� ������������ � ����� - ���������� ������� � ������

          group_resources.push_back (iter->second);
        }
        else
        {
            //�������� �������

          ResourcePtr resource (new Resource (resource_name, impl), false);

            //���������� ������� � ������

          group_resources.push_back (resource);

            //����������� �������

          impl->resources.insert_pair (resource_name, resource.get ());
        }
      }
    }

      //�������� ������ ����������

    GroupBinding* group_binding = new GroupBinding (impl->servers, group_resources, impl->group_bindings);

    Binding binding (group_binding);

      //�������������� ���������� ����� ������ ����� �������� ��� ����������� �����������

    if (impl->cache_state)
    {
      for (ResourceArray::iterator iter=group_binding->Resources ().begin (), end=group_binding->Resources ().end (); iter!=end; ++iter)
      {
        Resource& resource = **iter;

        if (resource.use_count () == 1) //���� ������ �������� ������ ����� �������
        {
            //����������� ����� ������ �� ������ ��� ����������� ���������� ������� � ���� ����� ��������

          addref (resource);
        }
      }
    }

    return binding;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ServerGroup::CreateBinding");
    throw;
  }
}

/*
    ���������� ������������ ��������
*/

void ServerGroup::SetCacheState (bool state)
{
  if (state == impl->cache_state)
    return;

  if (impl->cache_state)
  {
      //���������� ���� - ���������� ��� ��������, ��������� ����� ������ ���� ���������� ��������

    FlushUnusedResources ();    

    for (ResourceMap::reverse_iterator iter=impl->resources.rbegin (), end=impl->resources.rend (); iter!=end;)
    {
      Resource* resource = iter->second;

      ++iter;

      release (resource);
    }
  }
  else
  {
      //��������� ���� - ����������� ����� ������ ���� ��������

    for (ResourceMap::iterator iter=impl->resources.begin (), end=impl->resources.end (); iter!=end; ++iter)
      addref (iter->second);
  }

  impl->cache_state = state;
}

bool ServerGroup::CacheState () const
{
  return impl->cache_state;
}

/*
    ����� �������������� ��������
*/

void ServerGroup::FlushUnusedResources ()
{
  impl->FlushUnusedResources ();
}

/*
    �����
*/

void ServerGroup::Swap (ServerGroup& group)
{
  stl::swap (impl, group.impl);
}

namespace media
{

namespace rms
{

void swap (ServerGroup& group1, ServerGroup& group2)
{
  group1.Swap (group2);
}

}

}
