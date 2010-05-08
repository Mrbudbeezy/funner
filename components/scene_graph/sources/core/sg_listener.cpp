#include "shared.h"

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

Listener::Pointer Listener::Create ()
{
  return Pointer (new Listener, false);
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
    throw xtl::make_range_exception ("Listener::SetGain", "gain", gain, 0.f, 1e9f);

  impl->gain = gain;

  UpdateNotify ();
}

/*
    �����, ���������� ��� ��������� �������
*/

void Listener::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
