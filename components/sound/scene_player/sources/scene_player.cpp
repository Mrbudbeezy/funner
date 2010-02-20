#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/ref.h>

#include <common/action_queue.h>
#include <common/time.h>

#include <sg/scene.h>
#include <sg/node.h>
#include <sg/entity.h>
#include <sg/listener.h>
#include <sg/sound_emitter.h>

#include <sound/scene_player.h>

#include <math/vector.h>

using namespace sound;
using namespace common;
using namespace scene_graph;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

/*
    ���������
*/

const double STOP_EMITTER_DELAY = 0.2; //������������ ��� ��������� �����

}

/*
    ������� �����
*/

class ScenePlayerEmitter: public xtl::trackable
{
  public:
///�����������
    ScenePlayerEmitter (SoundEmitter& in_scene_emitter)
      : sound_manager (0)
      , scene_emitter (in_scene_emitter)
      , play_start_time (0)
      , play_start_offset (0.0f)
      , is_playing (false)
      , need_release_scene_emitter (false)
    {
      emitter.SetSource (scene_emitter.SoundDeclarationName ());

      connect_tracker (scene_emitter.RegisterEventHandler (SoundEmitterEvent_OnPlay,   xtl::bind (&ScenePlayerEmitter::OnPlay, this)));
      connect_tracker (scene_emitter.RegisterEventHandler (SoundEmitterEvent_OnStop,   xtl::bind (&ScenePlayerEmitter::OnStop, this)));
      connect_tracker (scene_emitter.RegisterEventHandler (NodeEvent_AfterUpdate,      xtl::bind (&ScenePlayerEmitter::OnUpdate, this)));
      
      on_change_scene = scene_emitter.RegisterEventHandler (NodeEvent_AfterSceneChange, xtl::bind (&ScenePlayerEmitter::OnSceneChange, this));
    }
    
///����������
    ~ScenePlayerEmitter ()
    {
      try
      {
        on_change_scene.disconnect ();
        
        OnStop ();
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }  
    
///��������� ��������� ������
    void SetSoundManager (SoundManager* in_sound_manager)
    {
      if (!in_sound_manager)
      {
        if (sound_manager)
          sound_manager->StopSound (emitter);

        sound_manager = 0;

        return;
      }

      sound_manager = in_sound_manager;
      
      if (!sound_manager)
        return;
      
      if (!is_playing)
        return;
        
      play_start_offset += (common::milliseconds () - play_start_time) / 1000.0f;
      play_start_time    = common::milliseconds ();

      sound_manager->PlaySound (emitter, play_start_offset);
    }
    
    SoundManager* GetSoundManager () { return sound_manager; }
    
  private:
///���������� ������� ���������� �������� �����
    void OnUpdate ()
    {
      emitter.SetPosition  (scene_emitter.WorldPosition ());
      emitter.SetDirection (scene_emitter.WorldOrtZ ());
      emitter.SetVolume    (scene_emitter.Gain ());
    }
    
///���������� ������ ������������ ����� � ��������
    void OnPlay ()
    {
      if (is_playing)
        OnStop ();
            
      emitter.SetSampleIndex (rand ());

      is_playing        = true;
      play_start_time   = common::milliseconds ();
      play_start_offset = 0.f;

      if (!sound_manager)
        return;

      sound_manager->PlaySound (emitter);

      scene_emitter.AddRef ();
      
      need_release_scene_emitter = true;

      if (!sound_manager->IsLooping (emitter))
      {
        auto_stop_action = common::ActionQueue::PushAction (xtl::bind (&ScenePlayerEmitter::OnStop, this),
          common::ActionThread_Current, sound_manager->Duration (emitter) + STOP_EMITTER_DELAY);
      }      
    }

///���������� ��������� ������������ ����� � ��������
    void OnStop ()
    {      
      if (!is_playing)
        return;                
      
      auto_stop_action.Cancel ();
      
      auto_stop_action = Action ();
      is_playing       = false;      

      if (sound_manager)
        sound_manager->StopSound (emitter);

      if (need_release_scene_emitter)
      {
        need_release_scene_emitter = false;

        scene_emitter.Release (); //������ ���� ��������� ���������, ��������� ����� ������������ �������� �������� �������
      }      
    }

///���������� ������� ��������� �����
    void OnSceneChange ()
    {     
      delete this;
    }

  private:
    SoundManager*        sound_manager;              //�������� ������
    SoundEmitter&        scene_emitter;              //������� � �����
    sound::Emitter       emitter;                    //������� � ��������� ������
    size_t               play_start_time;            //����� ������ ������������
    float                play_start_offset;          //�������� ������ ������������
    bool                 is_playing;                 //������������� �� ����
    bool                 need_release_scene_emitter; //����� ����������� ������� � �����
    Action               auto_stop_action;           //�������� �������������� ��������� �����
    xtl::auto_connection on_change_scene;            //���������� ������� ��������� �����
};

/*
    ��������� �����
*/

class ScenePlayerListener: public xtl::trackable
{
  public:
///�����������  
    ScenePlayerListener (scene_graph::Listener& in_scene_listener)
      : sound_manager (0)
      , scene_listener (in_scene_listener)
    {
      connect_tracker (scene_listener.RegisterEventHandler (NodeEvent_AfterUpdate, xtl::bind (&ScenePlayerListener::OnUpdate, this)));
      connect_tracker (scene_listener.RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&ScenePlayerListener::OnDestroy, this)));
    }
    
///����������
    ~ScenePlayerListener ()
    {
      try
      {
        SetSoundManager (0);
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }
    
///��������� ��������� ������
    void SetSoundManager (SoundManager* in_sound_manager)
    {
      if (sound_manager)
      {
        sound_manager->SetMute (true);
      }
      
      sound_manager = in_sound_manager;
      
      if (!sound_manager)
        return;
        
      sound_manager->SetMute (false);
      
      OnUpdate ();
    }
    
    SoundManager* GetSoundManager () { return sound_manager; }
    
///��������� �����
    scene_graph::Listener& GetSceneListener () { return scene_listener; }

  private:
///��������� ������� ���������� ���������
    void OnUpdate ()
    {
      if (!sound_manager)
        return;        

      sound::Listener sound_listener;

      math::mat4f listener_world_tm = scene_listener.WorldTM ();

      sound_listener.position  = listener_world_tm * math::vec3f (0.f);
      sound_listener.direction = listener_world_tm * math::vec4f (0.f, 0.f, -1.f, 0.f);
      sound_listener.up        = listener_world_tm * math::vec4f (0.f, 1.f, 0.f, 0.f);

      sound_manager->SetListener (sound_listener);
      sound_manager->SetVolume   (scene_listener.Gain ());
    }
    
///��������� ������� �������� ���������
    void OnDestroy ()
    {
      delete this;
    }

  private:
    SoundManager*          sound_manager;  //�������� ������
    scene_graph::Listener& scene_listener; //��������� � �����
};

/*
    �������� ���������� ������������� ������ �����
*/

struct ScenePlayer::Impl
{
  public:
///�����������
    Impl ()
      : sound_manager (0)
      , scene (0)
    {
    }

///����������
    ~Impl ()
    {
      try      
      {
        SetSoundManager (0);
        SetListener (0);
        SetScene (0);
      }
      catch (...)
      {
        //���������� ���� ����������
      }
    }

///��������� ���������
    void SetListener (scene_graph::Listener* in_listener, bool need_destroy_listener = true)
    {
      if (!listener && !in_listener || listener && &listener->GetSceneListener () == in_listener)
        return;        
        
      on_destroy_listener.disconnect ();
      on_change_listener_scene.disconnect ();        
      
      if (need_destroy_listener) listener.reset ();
      else                       listener.release ();      

      if (!in_listener)
      {
        SetScene (0);
        return;
      }

      listener = stl::auto_ptr<ScenePlayerListener> (new ScenePlayerListener (*in_listener));

      try
      {
        SetScene (in_listener->Scene ());

        listener->SetSoundManager (sound_manager);
        
        on_destroy_listener      = listener->connect_tracker (xtl::bind (&ScenePlayer::Impl::OnDestroyListener, this));
        on_change_listener_scene = in_listener->RegisterEventHandler (NodeEvent_AfterSceneChange, xtl::bind (&ScenePlayer::Impl::OnChangeListenerScene, this));
      }
      catch (...)
      {
        SetListener (0);
        throw;
      }      
    }

    scene_graph::Listener* GetListener () const
    {
      return &listener->GetSceneListener ();
    }

///��������� ���������
    void SetSoundManager (sound::SoundManager* in_sound_manager)
    {
      if (sound_manager == in_sound_manager)
        return;
        
      on_manager_destroy_connection.disconnect ();
        
      if (listener)
        listener->SetSoundManager (in_sound_manager);

      for (EmitterSet::iterator i = emitters.begin (); i != emitters.end (); ++i)
        i->second->SetSoundManager (in_sound_manager);        

      sound_manager = in_sound_manager;
      
      if (!sound_manager)
        return;
      
      try
      {
        on_manager_destroy_connection = sound_manager->RegisterDestroyHandler (xtl::bind (&ScenePlayer::Impl::SetSoundManager, this, (SoundManager*)0));
      }
      catch (...)
      {
        SetSoundManager (0);
        throw;
      }
    }

    SoundManager* GetSoundManager () const
    {
      return sound_manager;
    }

  private:
///����� �����
    void SetScene (Scene* in_scene)
    {
      if (in_scene == scene)
        return;        
        
      on_bind_node_connection.disconnect ();
        
        //�������� ���� ���������
        
      while (!emitters.empty ())      
        delete emitters.begin ()->second;
        
      emitters.clear ();
                        
        //��������� �����

      scene = in_scene;      
      
      if (!scene)
        return;

      try
      {
          //���������� ���� ��������� ��������� � �����

        scene->Traverse (xtl::bind (&ScenePlayer::Impl::OnNewNode, this, _1));

          //�������� �� ��������� ����� ��������

        on_bind_node_connection = scene->Root ().RegisterEventHandler (NodeSubTreeEvent_AfterBind, xtl::bind (&ScenePlayer::Impl::OnNewNode, this, _2)); 
      }
      catch (...)
      {
        SetScene (0);
        throw;
      }      
    }
    
///��������� ������� ��������� ����� � ���������
    void OnChangeListenerScene ()
    {
      if (!listener)
        return;
      
      SetScene (listener->GetSceneListener ().Scene ());
    }
    
///��������� ������� �������� ���������
    void OnDestroyListener ()
    {
      SetListener (0, false);
    }
  
///��������� ������� ��������� ������ ����
    void OnNewNode (Node& node)
    {
      scene_graph::SoundEmitter* emitter = dynamic_cast<scene_graph::SoundEmitter*> (&node);

      if (!emitter)
        return;

      stl::auto_ptr<ScenePlayerEmitter> scene_player_emitter (new ScenePlayerEmitter (*emitter));

      scene_player_emitter->SetSoundManager (sound_manager);

      scene_player_emitter->connect_tracker (xtl::bind (&ScenePlayer::Impl::OnDestroyEmitter, this, emitter));

      emitters.insert_pair (emitter, scene_player_emitter.get ());

      scene_player_emitter.release ();
    }

///��������� ������� �������� ��������
    void OnDestroyEmitter (SoundEmitter* node)
    {
      emitters.erase (node);
    }

  private:
    typedef stl::hash_map<SoundEmitter*, ScenePlayerEmitter*> EmitterSet;      

  private:
    stl::auto_ptr<ScenePlayerListener> listener;                      //���������
    sound::SoundManager*               sound_manager;                 //�������� ������
    EmitterSet                         emitters;                      //��������
    Scene*                             scene;                         //������� �����
    xtl::auto_connection               on_manager_destroy_connection; //���������� ������� �������� ���������    
    xtl::auto_connection               on_bind_node_connection;       //���������� ������� ��������� ������ ���� � �����
    xtl::auto_connection               on_destroy_listener;           //���������� ������� �������� ���������
    xtl::auto_connection               on_change_listener_scene;      //���������� ������� ��������� ����� ���������
};

/*
   ����������� / ����������
*/

ScenePlayer::ScenePlayer ()
  : impl (new Impl)
  {}

ScenePlayer::~ScenePlayer ()
{
}

/*
   ���������/��������� ���������
*/

void ScenePlayer::SetManager (sound::SoundManager* sound_manager)
{
  impl->SetSoundManager (sound_manager);
}

sound::SoundManager* ScenePlayer::Manager () const
{
  return impl->GetSoundManager ();
}

/*
   ���������/��������� ���������
*/

void ScenePlayer::SetListener (scene_graph::Listener* listener)
{
  impl->SetListener (listener);
}

scene_graph::Listener* ScenePlayer::Listener () const
{
  return impl->GetListener ();
}
