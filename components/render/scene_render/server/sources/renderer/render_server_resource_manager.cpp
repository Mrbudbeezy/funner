#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    ��������������� ������
*/

namespace
{

/// ������
struct Resource: public xtl::reference_counter
{
  stl::string name; //��� �������

/// �����������
  Resource (const char* in_name) : name (in_name) {}
};

typedef xtl::intrusive_ptr<Resource>                           ResourcePtr;
typedef stl::hash_map<stl::hash_key<const char*>, ResourcePtr> ResourceMap;

}

namespace render
{

namespace scene
{

namespace server
{

/// ���������� ��������
struct ResourceLibrary: public xtl::reference_counter
{
  RenderManager render_manager; //�������� ���������� 
  ResourceMap   resources;      //�������

/// �����������
  ResourceLibrary (const RenderManager& in_render_manager)
    : render_manager (in_render_manager)
  {
  }

/// ����������
  virtual ~ResourceLibrary () {}
};

typedef xtl::intrusive_ptr<ResourceLibrary> ResourceLibraryPtr;

}

}

}

/*
===================================================================================================
    ResourceClient
===================================================================================================
*/

/*
    �������� ���������� �������
*/

struct ResourceClient::Impl: public xtl::reference_counter
{
  ResourceMap        resources; //�������
  ResourceLibraryPtr library;   //���������� ��������

/// �����������
  Impl (ResourceLibrary* in_library)
    : library (in_library)
  {
    if (!library)
      throw xtl::make_null_argument_exception ("render::scene::server::ResourceClient::Impl::Impl", "library");
  }

/// ����������
  ~Impl ()
  {
    for (ResourceMap::iterator iter=resources.begin (), end=resources.end (); iter!=end; ++iter)
    {
      ResourcePtr resource = iter->second;

      if (resource->use_count () != 3)
        continue;

      library->resources.erase (resource->name.c_str ());

      try
      {
        library->render_manager.Log ().Printf ("Unloading resource '%s'...", resource->name.c_str ());

        library->render_manager.Manager ().UnloadResource (resource->name.c_str ());
      }
      catch (...)
      {
      }
    }
  }
};

/*
    ������������ / ���������� / ������������
*/

ResourceClient::ResourceClient (ResourceLibrary* library)
{
  try
  {
    impl = new Impl (library);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ResourceClient::ResourceClient(ResourceLibrary*)");
    throw;
  }
}

ResourceClient::ResourceClient (const ResourceClient& client)
  : impl (client.impl)
{
  addref (impl);
}

ResourceClient::~ResourceClient ()
{
  release (impl);
}

ResourceClient& ResourceClient::operator = (const ResourceClient& client)
{
  ResourceClient tmp (client);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    �������� / �������� ��������  
*/

void ResourceClient::LoadResource (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    ResourceMap::iterator iter = impl->resources.find (name);

    if (iter != impl->resources.end ())
      return; //resource has been already loaded to this client

    iter = impl->library->resources.find (name);

    if (iter != impl->library->resources.end ())
    {
        //resource has been already loaded to the library -> add it to this client

      impl->resources.insert_pair (name, iter->second);

      return;
    }

      //resource has not been loaded to the library

    ResourcePtr resource (new Resource (name), false);

    impl->library->render_manager.Log ().Printf ("Loading resource '%s'...", name);

    try
    {
      impl->library->render_manager.Manager ().LoadResource (name);
    }
    catch (std::exception& e)
    {
      impl->library->render_manager.Log ().Printf ("%s\n    at render::scene::server::ResourceClient::LoadResource('%s')", e.what (), name);
    }
    catch (...)
    {
      impl->library->render_manager.Log ().Printf ("unknown exception\n    at render::scene::server::ResourceClient::LoadResource('%s')", name);
    }

    try
    {
      impl->library->resources.insert_pair (name, resource);

      impl->resources.insert_pair (name, resource);
    }
    catch (...)
    {
      impl->library->resources.erase (name);

      impl->library->render_manager.Manager ().UnloadResource (name);

      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ResourceClient::LoadResource");
    throw;
  }
}

void ResourceClient::UnloadResource (const char* name)
{
  if (!name)
    return;

    //remove from the client's map

  ResourceMap::iterator iter = impl->resources.find (name);

  if (iter == impl->resources.end ())
    return;  

  impl->resources.erase (iter);

    //remove from the library's map  

  iter = impl->library->resources.find (name);

  if (iter == impl->library->resources.end ())
    return;

  ResourcePtr resource = iter->second;

  if (resource->use_count () == 2)
  {
      //there are no clients which referes to this resource

    impl->library->resources.erase (iter);

    impl->library->render_manager.Log ().Printf ("Unloading resource '%s'...", name);

    try
    {
      impl->library->render_manager.Manager ().UnloadResource (name);
    }
    catch (...)
    {
    }
  }
}

/*
===================================================================================================
    ResourceManager
===================================================================================================
*/

/*
    �������� ���������� ���������
*/

struct ResourceManager::Impl: public ResourceLibrary
{  
/// �����������
  Impl (const RenderManager& render_manager)
    : ResourceLibrary (render_manager)
  {
  }
};

/*
    ������������ / ���������� / ������������
*/

ResourceManager::ResourceManager (const RenderManager& render_manager)
  : impl (new Impl (render_manager))
{
}

ResourceManager::ResourceManager (const ResourceManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

ResourceManager::~ResourceManager ()
{
  release (impl);
}

ResourceManager& ResourceManager::operator = (const ResourceManager& manager)
{
  ResourceManager tmp (manager);

  stl::swap (tmp.impl, impl);

  return *this;
}

/*
    �������� �������
*/

ResourceClient ResourceManager::CreateClient ()
{
  try
  {
    return ResourceClient (&*impl);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ResourceManager::CreateClient");
    throw;
  }
}
