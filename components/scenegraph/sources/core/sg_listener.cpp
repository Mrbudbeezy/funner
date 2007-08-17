#include <sg/listener.h>
#include <xtl/visitor.h>
#include <common/exception.h>

using namespace scene_graph;
using namespace common;

/*
    �������� ���������� Listener
*/

struct Listener::Impl
{
  float gain;     //��������
};

/*
    ����������� / ����������
*/

Listener::Listener ()
  : impl (new Impl)
{
  impl->gain = 1.f;
}

Listener::~Listener ()
{
  delete impl;
}

/*
    �������� ���������
*/

Listener* Listener::Create ()
{
  return new Listener;
}

/*
    ��������
*/

float Listener::Gain () const
{
  return impl->gain;
}

void Listener::SetGain (float gain)
{
  if (gain < 0)
    RaiseOutOfRange ("Listener::SetGain", "gain", gain, 0.f, 1e9f);

  impl->gain = gain;
}

/*
    �����, ���������� ��� ��������� �������
*/

void Listener::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
