#include <stl/string>

#include <xtl/string.h>

#include <sound/scene_player.h>

#include "shared.h"

using namespace engine;
using namespace common;

namespace
{

/*
    ���������
*/

const char* SUBSYSTEM_NAME = "ScenePlayer";                   //��� ����������
const char* COMPONENT_NAME = "engine.subsystems.ScenePlayer"; //��� ����������

/*
   ���������� ������� �����
*/

class ScenePlayerSubsystem : public ISubsystem, public IAttachmentRegistryListener<scene_graph::Listener>, public xtl::reference_counter
{
  public:
/// �����������/����������
    ScenePlayerSubsystem (common::ParseNode& node)
      : attachment (get<const char*> (node, "Listener")),
        sound_manager (get<const char*> (node, "DriverMask", "*"), get<const char*> (node, "DeviceMask", "*"), get<const char*> (node, "InitString", ""))
    {
      scene_player.SetManager (&sound_manager);

      AttachmentRegistry::Attach (this, AttachmentRegistryAttachMode_ForceNotify);
    }

    ~ScenePlayerSubsystem ()
    {
      AttachmentRegistry::Detach (this);
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

/// ������� ������
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    ScenePlayerSubsystem (const ScenePlayerSubsystem&);             //no impl
    ScenePlayerSubsystem& operator = (const ScenePlayerSubsystem&); //no impl

  private:
    stl::string         attachment;
    sound::SoundManager sound_manager;
    sound::ScenePlayer  scene_player;
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

        manager.AddSubsystem (subsystem.get ());
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
