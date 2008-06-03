#include <xtl/visitor.h>
#include <xtl/signal.h>
#include <stl/string>
#include <common/exception.h>
#include <sg/sound_emitter.h>

using namespace scene_graph;
using namespace xtl;
using namespace common;

/*
    �������� ���������� SoundEmitter
*/

typedef signal<void (SoundEmitter& sender, SoundEmitterEvent event)> SoundEmitterSignal;

struct SoundEmitter::Impl
{
  stl::string        sound_declaration_name;                   //��� �������� �����
  SoundEmitterSignal signals [SoundEmitterEvent_Num];          //�������
  bool               signal_process [SoundEmitterEvent_Num];   //����� ��������� ��������

  Impl (const char* in_sound_declaration_name) : sound_declaration_name (in_sound_declaration_name) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� � ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
  void Notify (SoundEmitter& emitter, SoundEmitterEvent event);
};

/*
    ���������� �������� � ����������� �������
*/

void SoundEmitter::Impl::Notify (SoundEmitter& emitter, SoundEmitterEvent event)
{
    //���� ������������ ��� - ���������� ������������
    
  if (!signals [event])
    return;

    //��������� ��� �� ������������ ������

  if (signal_process [event])
    return;
    
    //������������� ���� ��������� �������

  signal_process [event] = true;
  
    //�������� ����������� �������

  try
  {
    signals [event] (emitter, event);
  }
  catch (...)
  {
    //��� ���������� ���������� ������������ ������� ���� �����������
  }
  
    //������� ���� ��������� �������
  
  signal_process [event] = false;
}

/*
    ����������� / ����������
*/

SoundEmitter::SoundEmitter (const char* sound_declaration_name)
  : impl (new Impl (sound_declaration_name))
  {}

SoundEmitter::~SoundEmitter ()
{
  delete impl;
}

/*
    �������� ���������
*/

SoundEmitter::Pointer SoundEmitter::Create (const char* sound_declaration_name)
{
  return Pointer (new SoundEmitter (sound_declaration_name), false);
}

/*
   ��� �����
*/

const char* SoundEmitter::SoundDeclarationName () const
{
  return impl->sound_declaration_name.c_str ();
}

/*
   ������������
*/

void SoundEmitter::Play ()
{
  impl->Notify (*this, SoundEmitterEvent_Play);
}

void SoundEmitter::Stop ()
{
  impl->Notify (*this, SoundEmitterEvent_Stop);
}

/*
   �������� �� ������� SoundEmitter
*/

xtl::connection SoundEmitter::RegisterEventHandler (SoundEmitterEvent event, const EventHandler& handler) const
{
  if (event < 0 || event >= SoundEmitterEvent_Num)
    raise_invalid_argument ("scene_graph::SoundEmitter::Event", "event", event);

  return impl->signals [event].connect (handler);
}

/*
    �����, ���������� ��� ��������� �������
*/

void SoundEmitter::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
