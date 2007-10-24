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

struct SoundEmitter::Impl
{
  stl::string sound_declaration_name;                   //��� �������� �����
  Signal      signals [SoundEmitterEvent_Num];          //�������
  bool        signal_process [SoundEmitterEvent_Num];   //����� ��������� ��������

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

SoundEmitter* SoundEmitter::Create (const char* sound_declaration_name)
{
  return new SoundEmitter (sound_declaration_name);
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

SoundEmitter::Signal& SoundEmitter::Event (SoundEmitterEvent event) const
{
  if (event < 0 || event >= SoundEmitterEvent_Num)
    RaiseInvalidArgument ("scene_graph::SoundEmitter::Event", "event", event);

  return impl->signals [event];
}

/*
    �����, ���������� ��� ��������� �������
*/

void SoundEmitter::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
