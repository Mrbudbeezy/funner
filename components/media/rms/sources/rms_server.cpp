#include "shared.h"

using namespace media::rms;

namespace
{

/*
    ���������
*/

const size_t MAX_QUERY_NAMES             = 64;    //������������ ���������� ��������, ������������ � ������� � ICustomServer
const size_t RESOURCE_ARRAY_RESERVE_SIZE = 128;   //������������� ����� �������� � ������ ��������
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
    ServerQuery (ICustomServer& in_server, CallbackHandler in_callback) : server (in_server), callback (in_callback), names_count (0) {}

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
      
      (server.*callback)(names_count, names);

      names_count = 0;
    }

  private:
    ICustomServer&  server;                  //������� ������ ��������
    CallbackHandler callback;                //���������� ��������� ������
    size_t          names_count;             //���������� ��� �������� � �������
    const char*     names [MAX_QUERY_NAMES]; //����� �������� � �������
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
    GroupBinding (ICustomServer& in_server, ResourceArray& in_resources, GroupBindingList& in_group_bindings)
      : server (in_server),
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

        ServerQuery query (server, &ICustomServer::UnloadResources);

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
      ServerQuery query (server, &ICustomServer::PrefetchResources);

      SetResourcesState (ResourceState_Prefetched, query, StatePredicate<stl::less<ResourceState> > ());
    }

///�������� ������ ��������
    void Load ()
    {
      ServerQuery query (server, &ICustomServer::LoadResources);
      
      SetResourcesState (ResourceState_Loaded, query, StatePredicate<stl::less<ResourceState> > ());
    }

///�������� ������ ��������
    void Unload ()
    {
      ServerQuery query (server, &ICustomServer::UnloadResources);

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
    ICustomServer&             server;             //������ ��������
    ResourceArray              resources;          //�������
    GroupBindingList&          group_bindings;     //������ ����� ����������
    GroupBindingList::iterator group_bindings_pos; //��������� � ������ ����� ����������
};

}

/*
    �������� ���������� ������� ��������
*/

struct Server::Impl: public IResourceDestroyListener, public xtl::trackable
{
  ICustomServer*      server;         //������ ���������� ��������� (������ ��� ��������)
  stl::string         name;           //��� �������
  stl::string         filters_string; //������, ���������� ������� �������������� ��������
  common::StringArray filters;        //������ �������� ����� ��������
  ResourceMap         resources;      //�������
  GroupBindingList    group_bindings; //������ ����� ����������
  bool                cache_state;    //������������ �� ��� ��������

///�����������
  Impl (ICustomServer& in_server)
    : server (&in_server),
      cache_state (DEFAULT_CACHE_STATE)
  {
    UpdateFilters ("*");
  }
  
///����������
  ~Impl ()
  {
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

        server->UnloadResources (1, &resource_name);
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

    ServerQuery query (*server, &ICustomServer::UnloadResources);

    for (ResourceMap::iterator iter=resources.begin (), end=resources.end (); iter!=end;)
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

        ++iter;

        release (&resource);
      }
      else ++iter;
    }
    
    try
    {
      query.DoQuery ();
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }  
};

/*
    ������������ / ���������� / ������������
*/

Server::Server (ICustomServer& server)
  : impl (new Impl (server))
{
}

Server::~Server ()
{
}

/*
    ��� �������
*/

const char* Server::Name () const
{
  return impl->name.c_str ();
}

void Server::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rms::Server::SetName", "name");

  impl->name = name;
}

/*
    ������� ��� �������������� ��������
*/

const char* Server::Filters () const
{
  return impl->filters_string.c_str ();
}

void Server::SetFilters (const char* filters_string)
{
  if (!filters_string)
    throw xtl::make_null_argument_exception ("media::rms::Server::SetFilters", "filters_string");
    
  impl->UpdateFilters (filters_string);
}

void Server::AddFilters (const char* filters_string)
{
  if (!filters_string)
    throw xtl::make_null_argument_exception ("media::rms::Server::AddFilters", "filters_string");

  stl::string new_filters_string = impl->filters_string + ' ' + filters_string;

  impl->UpdateFilters (new_filters_string.c_str ());
}

void Server::ResetFilters ()
{
  impl->filters_string.clear ();
  impl->filters.Clear ();
}

/*
    �������� ���������� � ������� ��������
*/

Binding Server::CreateBinding (const Group& group)
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

          ResourcePtr resource (new Resource (resource_name, impl.get ()), false);

            //���������� ������� � ������

          group_resources.push_back (resource);

            //����������� �������

          impl->resources.insert_pair (resource_name, resource.get ());
        }
      }
    }
    
      //�������� ������ ����������

    GroupBinding* group_binding = new GroupBinding (*impl->server, group_resources, impl->group_bindings);

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
    exception.touch ("media::rms::Server::CreateBinding");
    throw;
  }
}

/*
    ���������� ������������ ��������
*/

void Server::SetCacheState (bool state)
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

      release (iter->second);
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

bool Server::CacheState () const
{
  return impl->cache_state;
}

/*
    ����� �������������� ��������
*/

void Server::FlushUnusedResources ()
{
  impl->FlushUnusedResources ();
}

/*
    ��������� �������, ������������ �� �������� �������
*/

xtl::trackable& Server::GetTrackable () const
{
  return *impl;
}

namespace media
{

namespace rms
{

xtl::trackable& get_trackable (const Server& server)
{
  return server.GetTrackable ();
}

}

}
