#include <stl/string>

#include <xtl/string.h>

#include <sound/scene_player.h>

#include "shared.h"

using namespace engine;
using namespace common;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

/*
    ���������
*/

const char* SUBSYSTEM_NAME = "ScenePlayer";                   //��� ����������
const char* COMPONENT_NAME = "engine.subsystems.ScenePlayer"; //��� ����������
const char* LOG_NAME       = COMPONENT_NAME;                  //��� ����

/*
   ���������� ������� �����
*/

class ScenePlayerSubsystem : public ISubsystem, public IAttachmentRegistryListener<scene_graph::Listener>,
  public media::rms::ICustomServer, public xtl::reference_counter
{
  public:
/// �����������/����������
    ScenePlayerSubsystem (common::ParseNode& node)
      : attachment (get<const char*> (node, "Listener")),
        log (LOG_NAME),
        sound_manager (get<const char*> (node, "DriverMask", "*"), get<const char*> (node, "DeviceMask", "*"), get<const char*> (node, "InitString", ""))
    {
      try
      {
        scene_player.SetManager (&sound_manager);

        AttachmentRegistry::Attach<scene_graph::Listener> (this, AttachmentRegistryAttachMode_ForceNotify);
        
        try
        {
          resource_server = new media::rms::ServerGroupAttachment (get<const char*> (node, "ResourceServer", SUBSYSTEM_NAME), *this);
        }
        catch (...)
        {
          AttachmentRegistry::Detach<scene_graph::Listener> (this, AttachmentRegistryAttachMode_ForceNotify);
          throw;
        }
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("engine::ScenePlayerSubsystem::ScenePlayerSubsystem");
        throw;
      }
    }

    ~ScenePlayerSubsystem ()
    {
      resource_server.reset ();
      
      AttachmentRegistry::Detach<scene_graph::Listener> (this, AttachmentRegistryAttachMode_ForceNotify);
    }

///������� ��������� / �������� ���������
    void OnRegisterAttachment (const char* attachment_name, scene_graph::Listener& listener)
    {
      if (attachment == attachment_name)
        scene_player.SetListener (&listener);
    }

    void OnUnregisterAttachment (const char* attachment_name, scene_graph::Listener&)
    {
      if (attachment == attachment_name)
        scene_player.SetListener (0);
    }

///���������� ���������
    void PrefetchResource (const char* resource_name)
    {
      //??????: do this !!!!!!!!!
    }

    void LoadResource (const char* resource_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::ScenePlayerSubsystem::LoadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        sound_manager.LoadSoundLibrary (resource_name);
      }
      catch (xtl::exception& exception)
      {
        log.Printf ("Can't load resource '%s', exception: %s", resource_name, exception.what ());

        exception.touch (METHOD_NAME);

        throw;
      }
    }

    void UnloadResource (const char* resource_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::ScenePlayerSubsystem::UnloadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        sound_manager.UnloadSoundLibrary (resource_name);
      }
      catch (xtl::exception& exception)
      {
        log.Printf ("Can't unload resource '%s', exception: %s", resource_name, exception.what ());

        exception.touch (METHOD_NAME);

        throw;
      }
    }

/// ������� ������
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    ScenePlayerSubsystem (const ScenePlayerSubsystem&);             //no impl
    ScenePlayerSubsystem& operator = (const ScenePlayerSubsystem&); //no impl

  private:
    stl::string                                      attachment;
    common::Log                                      log;
    sound::SoundManager                              sound_manager;
    sound::ScenePlayer                               scene_player;
    stl::auto_ptr<media::rms::ServerGroupAttachment> resource_server;
};

/*
   ��������� ����������� ������������� �����
*/

class ScenePlayerComponent
{
  public:
    //�������� ����������
    ScenePlayerComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<ScenePlayerSubsystem> subsystem (new ScenePlayerSubsystem (node), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::ScenePlayerComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<ScenePlayerComponent> ScenePlayerSubsystem (COMPONENT_NAME);

}

}
