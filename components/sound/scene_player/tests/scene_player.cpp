#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/bind.h>
#include <syslib/window.h>
#include <syslib/timer.h>
#include <syslib/application.h>
#include <sg/scene.h>
#include <sg/listener.h>
#include <sg/sound_emitter.h>
#include <sound/manager.h>
#include <sound/scene_player.h>
#include <sound/device.h>

using namespace sound;
using namespace sound::low_level;
using namespace xtl;
using namespace syslib;
using namespace scene_graph;
using namespace math;

const size_t TEST_WORK_TIME = 8000;  //����� ������ ����� (� ������������)
const size_t ACTION_TIME    = 5000;
const size_t ACTION2_TIME   = 100;
const char* library_file    = "data/test.snddecl";

typedef scene_graph::Listener::Pointer     ListenerPtr;
typedef scene_graph::SoundEmitter::Pointer SoundEmitterPtr;

SoundEmitterPtr sound_emitter  = SoundEmitter::Create ("declaration1"),
                sound_emitter2 = SoundEmitter::Create ("declaration2");

void TimerHandler (Timer&)
{
  sound_emitter->Stop ();
  sound_emitter2->Play ();
}

void TimerHandler2 (Timer&)
{
  sound_emitter->SetPosition (sound_emitter->Position () * -1.f);
}

int main ()
{
  printf ("ScenePlayer test.\n");

  try
  {
    Scene           scene;
    Window          window;
    SoundManager    manager (window, SoundSystem::FindConfiguration ("OpenAL", "*"));
    ListenerPtr     listener (scene_graph::Listener::Create ());

    srand (clock ());

    listener->BindToParent (scene.Root ());
    sound_emitter->BindToParent (scene.Root ());
    sound_emitter->SetPosition (0.1f, 0.1f, 0.1f);

    manager.LoadSoundLibrary (library_file);

    ScenePlayer scene_player;

    scene_player.SetListener (listener.get ());

    scene_player.SetManager (&manager);

    sound_emitter2->BindToParent (scene.Root ());

    sound_emitter->Play ();

    Timer timer (bind (&Application::Exit, 0), TEST_WORK_TIME);
    Timer timer2 (&TimerHandler, ACTION_TIME);
    Timer timer3 (&TimerHandler2, ACTION2_TIME);
  
    Application::Run ();
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
  }
  
  printf ("exit\n");

  return Application::GetExitCode ();
}
