#include "shared.h"

using namespace scene_graph;
using namespace xtl;
using namespace common;

/*
    �������� ���������� SoundEmitter
*/

typedef xtl::signal<void (SoundEmitter& sender, SoundEmitterEvent event)> SoundEmitterSignal;

struct SoundEmitter::Impl
{
  stl::string        sound_declaration_name;                   //��� �������� �����
  SoundEmitterSignal signals [SoundEmitterEvent_Num];          //�������
  bool               signal_process [SoundEmitterEvent_Num];   //����� ��������� ��������
  float              gain;                                     //���������
  float              play_start_offset;                        //�������� ��� ������ ������������

  Impl (const char* in_sound_declaration_name)
    : sound_declaration_name (in_sound_declaration_name)
    , gain (1.f)
    , play_start_offset (0.f)
  {
    memset (signal_process, 0, sizeof (signal_process));
  }

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

void SoundEmitter::Play (float play_start_offset)
{
  impl->play_start_offset = play_start_offset;

  impl->Notify (*this, SoundEmitterEvent_OnPlay);
}

float SoundEmitter::PlayStartOffset () const
{
  return impl->play_start_offset;
}

void SoundEmitter::Stop ()
{
  impl->Notify (*this, SoundEmitterEvent_OnStop);
}

/*
   ���������� ����������
*/

void SoundEmitter::SetGain (float gain)
{
  if (gain > 1.f) gain = 1.f;
  if (gain < 0.f) gain = 0.f;

  impl->gain = gain;

  UpdateNotify ();
}

float SoundEmitter::Gain ()
{
  return impl->gain;
}

/*
   �������� �� ������� SoundEmitter
*/

xtl::connection SoundEmitter::RegisterEventHandler (SoundEmitterEvent event, const EventHandler& handler) const
{
  if (event < 0 || event >= SoundEmitterEvent_Num)
    throw xtl::make_argument_exception ("scene_graph::SoundEmitter::Event", "event", event);

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

/*
    ���������� �������
*/

void SoundEmitter::BindProperties (common::PropertyBindingMap& bindings)
{
  Entity::BindProperties (bindings);

  bindings.AddProperty ("Gain", xtl::bind (&SoundEmitter::Gain, this), xtl::bind (&SoundEmitter::SetGain, this, _1));
  bindings.AddProperty ("SoundDeclarationName", xtl::bind (&SoundEmitter::SoundDeclarationName, this));
}
