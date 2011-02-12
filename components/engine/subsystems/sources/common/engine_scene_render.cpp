#include <stl/hash_map>

#include <xtl/connection.h>
#include <xtl/iterator.h>

#include <syslib/application.h>

#include <render/scene_render.h>

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

const char*  COMPONENT_NAME            = "engine.subsystems.SceneRenderSubsystem"; //��� ����������
const char*  LOG_NAME                  = COMPONENT_NAME;                           //��� ���������
const char*  SUBSYSTEM_NAME            = "SceneRender";                            //����������
const size_t DEFAULT_IDLE_RENDER_COUNT = 8;                                        //���������� ������������� ����������� �������� ������-��������

/*
   ���������� ������� �����
*/

class SceneRenderSubsystem : public ISubsystem, public IAttachmentRegistryListener<render::Screen>,
  public media::rms::ICustomServer, public xtl::reference_counter
{
  public:
///�����������
    SceneRenderSubsystem (common::ParseNode& node)
      : log (LOG_NAME),
        render (get<const char*> (node, "DriverMask"), get<const char*> (node, "RendererMask"), get<const char*> (node, "RenderPathMasks", "*")),
        idle_connection (syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&SceneRenderSubsystem::OnIdle, this)))
    {
      try
      {
        render.SetMaxDrawDepth (get<size_t> (node, "MaxDrawDepth", render.MaxDrawDepth ()));

        const char* log_name = get<const char*> (node, "Log", "");

        if (*log_name)
          Log (log_name).Swap (log);

        render.SetLogHandler (xtl::bind (&Log::Print, &log, _1));

        size_t render_target_index = 0;

        idle_render_targets.reserve (DEFAULT_IDLE_RENDER_COUNT);

        for (Parser::NamesakeIterator iter=node.First ("RenderTarget"); iter; ++iter, ++render_target_index)
        {
          const char* attachment = get<const char*> (*iter, "Screen");

          if (render_target_index >= render.RenderTargetsCount ())
          {
            iter->Log ().Error (*iter, "Wrong <RenderTarget>=%u (RenderTargetsCount=%u)", render_target_index, render.RenderTargetsCount ());

            throw xtl::make_range_exception ("", "RenderTarget", render_target_index, render.RenderTargetsCount ());
          }

          screen_map.insert_pair (attachment, render_target_index);

          if (get<bool> (node, "IdleRender", true))
            idle_render_targets.push_back (render.RenderTarget (render_target_index));
        }

        AttachmentRegistry::Attach<render::Screen> (this, AttachmentRegistryAttachMode_ForceNotify);
        
        try
        {
          resource_server = new media::rms::ServerGroupAttachment (get<const char*> (node, "ResourceServer", SUBSYSTEM_NAME), *this);
        }
        catch (...)
        {
          AttachmentRegistry::Detach<render::Screen> (this, AttachmentRegistryAttachMode_ForceNotify);
          throw;
        }
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("engine::SceneRenderSubsystem::SceneRenderSubsystem");
        throw;
      }
    }

///����������
    ~SceneRenderSubsystem ()
    {
      resource_server = 0;      
      
      AttachmentRegistry::Detach<render::Screen> (this, AttachmentRegistryAttachMode_ForceNotify);      
    }

/// ������� ���������/�������� ������
    void OnRegisterAttachment (const char* attachment_name, render::Screen& screen)
    {
      ScreenMap::iterator iter = screen_map.find (attachment_name);

      if (iter == screen_map.end ())
        return;

      render.RenderTarget (iter->second).SetScreen (&screen);
    }

    void OnUnregisterAttachment (const char* attachment_name, render::Screen&)
    {
      ScreenMap::iterator iter = screen_map.find (attachment_name);

      if (iter == screen_map.end ())
        return;

      render.RenderTarget (iter->second).SetScreen (0);
    }

///���������� ���������
    void PrefetchResource (const char* resource_name)
    {
      //??????: do this !!!!!!!!!
    }

    void LoadResource (const char* resource_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::SceneRenderSubsystem::LoadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        render.LoadResource (resource_name);
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
      static const char* METHOD_NAME = "engine::subsystems::SceneRenderSubsystem::UnloadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        render.UnloadResource (resource_name);
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
    void OnIdle ()
    {
      for (RenderTargetArray::iterator iter=idle_render_targets.begin (); iter!=idle_render_targets.end ();)
      {
        if (!iter->IsBindedToRender ()) //���� ���� ���������� ������� - ������� � �� ������ ���������
        {
          idle_render_targets.erase (iter);

          continue;
        }

          //���������� ���� ����������

        iter->Update ();

        ++iter;
      }

        //���� ��� ���� ���������� ������� - �������������� ������

      if (idle_render_targets.empty ())
        idle_connection.disconnect ();
    }

  private:
    SceneRenderSubsystem (const SceneRenderSubsystem&);             //no impl
    SceneRenderSubsystem& operator = (const SceneRenderSubsystem&); //no impl

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, size_t> ScreenMap;
    typedef stl::vector<render::RenderTarget>                 RenderTargetArray;

  private:
    Log                                              log;                 //���
    render::SceneRender                              render;              //������
    stl::auto_ptr<media::rms::ServerGroupAttachment> resource_server;     //������ �������� ����������
    ScreenMap                                        screen_map;          //������������ ������� � ������-��������
    xtl::auto_connection                             idle_connection;     //���������� ���������� ������-��������
    RenderTargetArray                                idle_render_targets; //������ ������������� ����������� ����� ����������
};

/*
   ��������� ����������� ������� �����
*/

class SceneRenderComponent
{
  public:
    SceneRenderComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (common::ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<SceneRenderSubsystem> subsystem (new SceneRenderSubsystem (node), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::SceneRenderComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<SceneRenderComponent> SceneRenderSubsystem (COMPONENT_NAME);

}

}
