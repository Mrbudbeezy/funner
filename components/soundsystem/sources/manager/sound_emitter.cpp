#include <sound/manager.h>
#include <stl/string>
#include <xtl/signal.h>

using namespace sound;
using namespace math;

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
};

/*
    ����������� / ����������
*/

Emitter::Emitter (const char* source_name)
{
}

Emitter::~Emitter ()
{
}

/*
    �������� �����
*/

const char* Emitter::Source () const
{
  return impl->source.c_str ();
}

/*
    ��������� ���������� ��������� �����
*/

void Emitter::SetVolume (float volume)
{
}

float Emitter::Volume () const
{
  return impl->volume;
}

/*
    ��������� ������������ ���������� ����������
*/

void Emitter::SetPosition  (const math::vec3f&)
{
}

void Emitter::SetDirection (const math::vec3f&)
{
}

void Emitter::SetVelocity (const math::vec3f&)
{
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
}

void Emitter::Deactivate ()
{
}

bool Emitter::IsActive () const
{
  return impl->activation_count != 0;
}

/*
    �������� �� �������
*/

//xtl::connection Emitter::RegisterEventHandler (EmitterEvent, const EventHandler&)
//{
//}
