#include "shared.h"

using namespace engine;
using namespace common;
using namespace media::rms;

namespace
{

/*
    ���������
*/

const char* SUBSYSTEM_NAME = "ResourceManager";                   //��� ����������
const char* COMPONENT_NAME = "engine.subsystems.ResourceManager"; //��� ����������
const char* LOG_NAME       = COMPONENT_NAME;

/*
   ���������� ���������� ���������
*/

class ResourceManager : public ISubsystem, public xtl::reference_counter
{
  public:
/// �����������/����������
    ResourceManager (common::ParseNode& node)
    {
      for (Parser::NamesakeIterator iter=node.First ("Server"); iter; ++iter)
      {
        const char* server_name = get<const char*> (*iter, "Name");
        const char* filters     = get<const char*> (*iter, "Filters", "*");
        bool        cache_state = get<bool> (*iter, "Cache", false);

        ServerGroup server_group (server_name);

        server_group.SetFilters (filters);
        server_group.SetCacheState (cache_state);

        server_groups.push_back (server_group);
      }
    }

    ~ResourceManager ()
    {
    }

/// ������� ������
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    typedef stl::vector<ServerGroup> ServerGroupList;

  private:
    ServerGroupList server_groups;
};

/*
   ���������
*/

class ResourceManagerComponent
{
  public:
    //�������� ����������
    ResourceManagerComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<ResourceManager> subsystem (new ResourceManager (node), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::ResourceManagerComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<ResourceManagerComponent> ResourceManagerSubsystem (COMPONENT_NAME);

}

}
