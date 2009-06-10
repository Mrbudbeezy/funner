#include "shared.h"

using namespace media::rms;

namespace
{

/*
    ���������
*/

const size_t MAX_QUERY_NAMES             = 64;    //������������ ���������� ��������, ������������ � ������� � ICustomServer
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
    ��������� ������ ��������
*/

class IServerList
{
  public:
    typedef void (ICustomServer::*CallbackHandler)(size_t count, const char** resource_names);  

    virtual void DoQuery (const CallbackHandler& handler, size_t names_count, const char** resource_names, bool ignore_exceptions) = 0;

  protected:
    virtual ~IServerList () {}
};

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
    ������ � ������� ��������
*/

class ServerQuery
{
  public:
    typedef void (ICustomServer::*CallbackHandler)(size_t count, const char** resource_names);

///�����������
    ServerQuery (IServerList& in_servers, CallbackHandler in_callback, bool in_ignore_exceptions)
      : servers (in_servers)
      , callback (in_callback)
      , names_count (0) 
      , ignore_exceptions (in_ignore_exceptions)
    {
    }

///���������� ����� ������� � �������
    void Add (const char* name)
    {
      if (names_count == MAX_QUERY_NAMES)
        DoQuery ();

      names [names_count] = name;

      names_count++;
    }

///���������� �������
    void DoQuery ()
    {
      if (!names_count)
        return;

      servers.DoQuery (callback, names_count, names, ignore_exceptions);

      names_count = 0;
    }

  private:
    IServerList&    servers;                 //������ �������� ��������
    CallbackHandler callback;                //���������� ��������� ������
    size_t          names_count;             //���������� ��� �������� � �������
    const char*     names [MAX_QUERY_NAMES]; //����� �������� � �������
    bool            ignore_exceptions;       //������������� ����������
};

/*
    ���������� � ������� ��������
*/

class GroupBinding;

typedef stl::list<GroupBinding*> GroupBindingList;

class GroupBinding: public ICustomBinding, public xtl::trackable
{
  public:
///�����������
    GroupBinding (IServerList& in_servers, ResourceArray& in_resources, GroupBindingList& in_group_bindings)
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

        ServerQuery query (servers, &ICustomServer::UnloadResources, true);

        SetResourcesState (ResourceState_Unloaded, query, DestructorPredicate ());
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
      ServerQuery query (servers, &ICustomServer::PrefetchResources, true);

      SetResourcesState (ResourceState_Prefetched, query, StatePredicate<stl::less<ResourceState> > ());
    }

///�������� ������ ��������
    void Load ()
    {
      ServerQuery query (servers, &ICustomServer::LoadResources, false);

      SetResourcesState (ResourceState_Loaded, query, StatePredicate<stl::less<ResourceState> > ());
    }

///�������� ������ ��������
    void Unload ()
    {
      ServerQuery query (servers, &ICustomServer::UnloadResources, true);

      SetResourcesState (ResourceState_Unloaded, query, StatePredicate<stl::greater<ResourceState> > ());
    }

///��������� �������, ������������ � ��������� �������� GroupBinding
    xtl::trackable* GetTrackable () { return this; }

///��������� ������� ��������
    ResourceArray& Resources () { return resources; }

  private:
    template <class Pred> void SetResourcesState (ResourceState state, ServerQuery& query, Pred pred)
    {
        //���������� ������� ��������

      for (ResourceArray::iterator iter=resources.begin (), end=resources.end (); iter!=end; ++iter)
      {
        Resource& resource = **iter;

        if (pred (resource, state))
          query.Add (resource.Name ());
      }

      query.DoQuery ();

        //��������� ��������� ��������

      for (ResourceArray::iterator iter=resources.begin (), end=resources.end (); iter!=end; ++iter)
      {
        Resource& resource = **iter;

        if (pred (resource, state))
          resource.SetState (state);
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
    IServerList&               servers;            //������ �������� ��������
    ResourceArray              resources;          //�������
    GroupBindingList&          group_bindings;     //������ ����� ����������
    GroupBindingList::iterator group_bindings_pos; //��������� � ������ ����� ����������
};

}

/*
    �������� ���������� ������� ��������
*/

typedef stl::vector<ICustomServer*> ServerList;

struct ServerGroup::Impl: public IResourceDestroyListener, public IServerList, public IServerGroupInstance, public xtl::reference_counter
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
  
///������ � �������� ��������
  void DoQuery (const CallbackHandler& handler, size_t names_count, const char** resource_names, bool ignore_exceptions)
  {
    if (ignore_exceptions)
    {
      for (ServerList::iterator iter=servers.begin (), end=servers.end (); iter!=end; ++iter)
      {
        try
        {
          ((**iter).*handler)(names_count, resource_names);
        }
        catch (...)
        {
          //������������� ����������
        }
      }
    }
    else
    {
      try
      {
        for (ServerList::iterator iter=servers.begin (), end=servers.end (); iter!=end; ++iter)
        {
          ((**iter).*handler)(names_count, resource_names);
        }
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("media::rms::ServerGroup::Impl::DoQuery");
        throw;
      }
    }
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
      GroupBinding* binding = group_bindings.front ();

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

        DoQuery (&ICustomServer::UnloadResources, 1, &resource_name, true);
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

    ServerQuery query (*this, &ICustomServer::UnloadResources, true);

    for (ResourceMap::iterator iter = resources.begin (), end = resources.end (); iter != end; ++iter)
    {
      Resource& resource = *iter->second;

      if (resource.use_count () == 1) //���� ������ ��������� ������ � ����
      {
        if (resource.State () > ResourceState_Unloaded)
        {
          try
          {
            query.Add (resource.Name ());
          }
          catch (...)
          {
            //���������� ���� ����������
          }

          resource.SetState (ResourceState_Unloaded);
        }
      }
    }

    try
    {
      query.DoQuery ();
    }
    catch (...)
    {
      //���������� ���� ����������
    }

    for (ResourceMap::iterator iter = resources.begin (), end = resources.end (); iter != end;)
    {
      Resource& resource = *iter->second;

      if (resource.use_count () == 1) //���� ������ ��������� ������ � ����
      {
        ResourceMap::iterator next = iter;

        ++next;

        release (&resource);

        iter = next;
      }
      else
        ++iter;
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

namespace
{

struct AttachServerList: public IServerList
{
  public:
    AttachServerList (ICustomServer& in_server) : server (in_server) {}

    void DoQuery (const CallbackHandler& handler, size_t resource_count, const char** resource_names, bool ignore_exceptions)
    {
      try
      {      
        (server.*handler)(resource_count, resource_names);
      }
      catch (...)
      {
        if (!ignore_exceptions)
          throw;
      }
    }      

  private:
    ICustomServer& server;
};

}

void ServerGroup::Attach (ICustomServer& server)
{
  try
  {
      //�������� ���������� ����������

    for (ServerList::iterator iter=impl->servers.begin (), end=impl->servers.end (); iter!=end; ++iter)
      if (*iter == &server)
        return;
        
      //�������������� ��������� ��������
      
    AttachServerList single_server_list (server);
    ServerQuery      query (single_server_list, &ICustomServer::LoadResources, false);
      
    for (ResourceMap::iterator iter=impl->resources.begin (), end=impl->resources.end (); iter!=end; ++iter)
    {
      Resource& resource = *iter->second;

      if (resource.State () > ResourceState_Unloaded)
        query.Add (resource.Name ());
    }

    query.DoQuery ();

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
        
      AttachServerList single_server_list (server);
      ServerQuery      query (single_server_list, &ICustomServer::UnloadResources, true);
      
      for (ResourceMap::iterator res_iter=impl->resources.begin (), res_end=impl->resources.end (); res_iter!=res_end; ++res_iter)
      {
        Resource& resource = *res_iter->second;

        if (resource.State () > ResourceState_Unloaded)
          query.Add (resource.Name ());
      }

      query.DoQuery ();

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

    GroupBinding* group_binding = new GroupBinding (*impl, group_resources, impl->group_bindings);

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

    for (ResourceMap::iterator iter=impl->resources.begin (), end=impl->resources.end (); iter!=end;)
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
