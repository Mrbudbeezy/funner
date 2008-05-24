#include <sound/manager.h>
#include <stl/string>
#include <xtl/signal.h>
#include <common/exception.h>

using namespace sound;
using namespace math;
using namespace common;

/*
    �������� ���������� ���������� �����
*/

typedef xtl::signal<void (Emitter&, EmitterEvent)> EmitterSignal;

struct Emitter::Impl
{
  stl::string   source;                     //��� ���������
  float         volume;                     //���������� ���������
  vec3f         position;                   //���������
  vec3f         direction;                  //�����������
  vec3f         velocity;                   //��������
  EmitterSignal signals [EmitterEvent_Num]; //�������
  size_t        activation_count;           //������� ���������
  size_t        sample_index;               //������ ������ ��� ������������

  Impl ()
    : volume (1.f), activation_count (0), sample_index (0)
    {}

  void Notify (EmitterEvent event, Emitter& emitter)
  {
    try
    {
      signals [event] (emitter, event);
    }
    catch (...)
    {
    }
  }

/*
   �������� �����
*/

  void SetSource (const char* source_name)
  {
    source = source_name;
  }

  const char* Source () const
  {
    return source.c_str ();
  }

/*
   �������� ������ ������ �� ��������� �����
*/

  void SetSampleIndex (size_t in_sample_index)
  {
    sample_index = in_sample_index;
  }

  size_t SampleIndex () const
  {
    return sample_index;
  }

};

/*
    ����������� / ����������
*/

Emitter::Emitter ()
  : impl (new Impl ())
  {}

Emitter::~Emitter ()
{
  impl->Notify (EmitterEvent_OnDestroy, *this);
}

/*
    �������� �����
*/

void Emitter::SetSource (const char* source_name)
{
  if (!source_name)
    RaiseNullArgument ("sound::Emitter::SetSource", "source_name");

  impl->SetSource (source_name);
}

const char* Emitter::Source () const
{
  return impl->Source ();
}

/*
   �������� ������ ������ �� ��������� �����
*/

void Emitter::SetSampleIndex (size_t sample_index)
{
  impl->SetSampleIndex (sample_index);
}

size_t Emitter::SampleIndex () const
{
  return impl->SampleIndex ();
}

/*
    ��������� ���������� ��������� �����
*/

void Emitter::SetVolume (float volume)
{
  impl->volume = volume;
  impl->Notify (EmitterEvent_OnUpdateVolume, *this);
}

float Emitter::Volume () const
{
  return impl->volume;
}

/*
    ��������� ������������ ���������� ����������
*/

void Emitter::SetPosition  (const math::vec3f& position)
{
  impl->position = position;
  impl->Notify (EmitterEvent_OnUpdateProperties, *this);
}

void Emitter::SetDirection (const math::vec3f& direction)
{
  impl->direction = direction;
  impl->Notify (EmitterEvent_OnUpdateProperties, *this);
}

void Emitter::SetVelocity (const math::vec3f& velocity)
{
  impl->velocity = velocity;
  impl->Notify (EmitterEvent_OnUpdateProperties, *this);
}

const vec3f& Emitter::Position () const
{
  return impl->position;
}

const vec3f& Emitter::Direction () const
{
  return impl->direction;
}

const vec3f& Emitter::Velocity () const
{
  return impl->velocity;
}

/*
    ���������� �����������
      - ������ ����� Activate ���������� ������� EmitterEvent_OnActivate
      - ��������� ����� Deactivate ���������� ������� EmitterEvent_OnDeactivate
      - ���������� �������� �������� �� ��� ���, ���� ������������� ���� �� � ����� ���������
*/

void Emitter::Activate ()
{
  if (!impl->activation_count)
    impl->Notify (EmitterEvent_OnActivate, *this);

  impl->activation_count++;
}

void Emitter::Deactivate ()
{
  if (impl->activation_count)
  {
    impl->activation_count--;
    if (!impl->activation_count)
      impl->Notify (EmitterEvent_OnDeactivate, *this);
  }
}

bool Emitter::IsActive () const
{
  return impl->activation_count != 0;
}

/*
   �������� �� �������
*/

xtl::connection Emitter::RegisterEventHandler (EmitterEvent event, const EventHandler& handler)
{
  if (event < 0 || event >= EmitterEvent_Num)
    RaiseInvalidArgument ("sound::Emitter::RegisterEventHandler", "event", event);
    
  return impl->signals [event].connect (handler);
}
