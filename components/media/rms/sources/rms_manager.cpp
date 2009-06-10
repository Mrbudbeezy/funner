#include "shared.h"

using namespace media::rms;

namespace
{

/*
    ���������� ��������� � �������� ��������
*/

class ManagerBinding;

typedef stl::list<ManagerBinding*> ManagerBindingList;

class ManagerBinding: public ICustomBinding, public xtl::trackable
{
  public:
///�����������
    ManagerBinding (ManagerBindingList& in_bindings) : bindings (in_bindings)
    {
        //����������� � ������ ����������

      bindings_pos = bindings.insert (bindings.end (), this);
    }

///����������
    ~ManagerBinding ()
    {
      bindings.erase (bindings_pos);
    }

///���������� ���������� ������ �������� � ��������
    void AddBinding (const Group& group, IServerGroupInstance* server_group)
    {
      server_bindings.push_back (ServerBinding (group, server_group));
    }

///�������� ���������� � ��������
    void RemoveBinding (IServerGroupInstance* server_group)
    {
      for (ServerBindingList::iterator iter=server_bindings.begin (), end=server_bindings.end (); iter!=end;)
      {
        if (iter->server_group == server_group)
        {
          ServerBindingList::iterator next = iter;

          ++next;

          server_bindings.erase (iter);

          iter = next;
        }
        else ++iter;
      }
    }

///����������� ��������
    void Prefetch ()
    {
      for (ServerBindingList::iterator iter=server_bindings.begin (), end=server_bindings.end (); iter!=end; ++iter)
      {
        try
        {
          iter->binding.Prefetch ();
        }
        catch (...)
        {
          //���������� ���� ����������
        }
      }
    }

///�������� ��������
    void Load ()
    {
      for (ServerBindingList::iterator iter=server_bindings.begin (), end=server_bindings.end (); iter!=end; ++iter)
      {
          //���������� �������� �� �����������

        iter->binding.Load ();
      }
    }

///�������� ��������
    void Unload ()
    {
      for (ServerBindingList::iterator iter=server_bindings.begin (), end=server_bindings.end (); iter!=end; ++iter)
      {
        try
        {
          iter->binding.Unload ();
        }
        catch (...)
        {
          //���������� ���� ����������
        }
      }
    }

///��������� �������, ������������ �� �������� �������� ������� (����� ���� 0)
    xtl::trackable* GetTrackable () { return this; }

  private:
    struct ServerBinding
    {
      Binding               binding;
      IServerGroupInstance* server_group;

      ServerBinding (const Group& resource_group, IServerGroupInstance* in_server_group)
        : binding (in_server_group->Instance ().CreateBinding (resource_group))
        , server_group (in_server_group)
      {
      }
    };

    typedef stl::list<ServerBinding> ServerBindingList;

  private:
    ServerBindingList            server_bindings; //������ ���������� ����� �������� � ��������
    ManagerBindingList&          bindings;       //������ ����������
    ManagerBindingList::iterator bindings_pos;   //��������� � ������ ����������
};

}

/*
    �������� ���������� ��������� ��������
*/

typedef stl::vector<IServerGroupInstance*> ServerGroupList;

struct ResourceManagerImpl::Impl
{
  ManagerBindingList bindings; //������ ����������
  ServerGroupList    groups;   //������ �����

///����������
  ~Impl ()
  {
    while (!bindings.empty ())
      delete bindings.front ();
  }
};

/*
    ������������ / ���������� / ������������
*/

ResourceManagerImpl::ResourceManagerImpl ()
  : impl (new Impl)
{
}

ResourceManagerImpl::~ResourceManagerImpl ()
{
}

/*
    ���������� � ��������� ��������
*/

Binding ResourceManagerImpl::CreateBinding (const Group& group)
{
  try
  {
    ManagerBinding* manager_binding = new ManagerBinding (impl->bindings);
    Binding         binding (manager_binding);

    for (ServerGroupList::iterator iter=impl->groups.begin (), end=impl->groups.end (); iter!=end; ++iter)
      manager_binding->AddBinding (group, *iter);

    return binding;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ResourceManager::CreateBinding");
    throw;
  }
}

/*
    ����������� ������ ��������
*/

void ResourceManagerImpl::RegisterServerGroup (const char* name, IServerGroupInstance* group)
{
  impl->groups.push_back (group);
}

void ResourceManagerImpl::UnregisterServerGroup (IServerGroupInstance* group)
{
  for (ServerGroupList::iterator iter=impl->groups.begin (), end=impl->groups.end (); iter!=end; ++iter)
    if (*iter == group)
    {
        //�������� ���������� � ������ ������� ��������

      for (ManagerBindingList::iterator binding=impl->bindings.begin (), binding_end=impl->bindings.end (); binding!=binding_end; ++binding)
        (*binding)->RemoveBinding (group);

        //�������� ������ �������� �� ������

      impl->groups.erase (iter);

      return;
    }
}

/*
    ����� ������ ��������
*/

IServerGroupInstance* ResourceManagerImpl::FindServerGroup (const char* name) const
{
  if (!name)
    return 0;

  for (ServerGroupList::iterator iter=impl->groups.begin (), end=impl->groups.end (); iter!=end; ++iter)
    if (!strcmp ((*iter)->Name (), name))
      return *iter;

  return 0;
}

/*
    ������� �����
*/

size_t ResourceManagerImpl::ServerGroupsCount () const
{
  return impl->groups.size ();
}

IServerGroupInstance* ResourceManagerImpl::ServerGroup (size_t index) const
{
  if (index >= impl->groups.size ())
    throw xtl::make_range_exception ("media::rms::ResourceManagerImpl::ServerGroup", "index", index, impl->groups.size ());
    
  return impl->groups [index];
}

/*
    ����� �������������� ��������
*/

void ResourceManagerImpl::FlushUnusedResources ()
{
  for (ServerGroupList::iterator iter=impl->groups.begin (), end=impl->groups.end (); iter!=end; ++iter)
    (*iter)->Instance ().FlushUnusedResources ();
}

/*
    ������������� ������� ResourceManager
*/

Binding ResourceManager::CreateBinding (const Group& group)
{
  return ResourceManagerSingleton::Instance ().CreateBinding (group);
}

size_t ResourceManager::ServerGroupsCount ()
{
  return ResourceManagerSingleton::Instance ().ServerGroupsCount ();
}

media::rms::ServerGroup ResourceManager::ServerGroup (size_t index)
{
  return ResourceManagerSingleton::Instance ().ServerGroup (index)->Instance ();
}

void ResourceManager::FlushUnusedResources ()
{
  ResourceManagerSingleton::Instance ().FlushUnusedResources ();
}
