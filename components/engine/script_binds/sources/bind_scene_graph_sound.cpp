#include "scene_graph.h"

namespace engine
{

namespace scene_graph_script_binds
{

/*
    �������� ���������
*/

Listener::Pointer create_listener ()
{
  return Listener::Create ();
}

/*
   ����������� ���������� ������ � �����������
*/

void bind_listener_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_LISTENER_LIBRARY);

    //������������

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_listener));

    //����������� ��������

  lib.Register ("set_Gain", make_invoker (&Listener::SetGain));
  lib.Register ("get_Gain", make_invoker (&Listener::Gain));

    //����������� ����� ������

  environment.RegisterType<Listener> (SCENE_LISTENER_LIBRARY);
}

/*
    �������� ��������� �����
*/

SoundEmitter::Pointer create_sound_emitter (const char* sound_declaration_name)
{
  return SoundEmitter::Create (sound_declaration_name);
}

/*
   ����������� ���������� ������ � ����������� �����
*/

void bind_sound_emitter_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_SOUND_EMITTER_LIBRARY);

    //������������

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_sound_emitter));

    //����������� ��������

  lib.Register ("set_Gain", make_invoker (&SoundEmitter::SetGain));
  lib.Register ("get_Gain", make_invoker (&SoundEmitter::Gain));

  lib.Register ("get_SoundDeclarationName", make_invoker (&SoundEmitter::SoundDeclarationName));

  lib.Register ("Play", make_invoker (&SoundEmitter::Play));
  lib.Register ("Stop", make_invoker (&SoundEmitter::Stop));

    //����������� ����� ������

  environment.RegisterType<SoundEmitter> (SCENE_SOUND_EMITTER_LIBRARY);
}

}

}
