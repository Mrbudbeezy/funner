#include <sg/light.h>
#include <xtl/visitor.h>
#include <bv/axis_aligned_box.h>

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;

const float INFINITY = 1e9;  //���� ������/���������� ��������� ����� ��������� ��� ��������, ��������������� ����������� bv

/*
    �������� ���������� DirectLight
*/

struct DirectLight::Impl
{
  float radius;         //������ �������� �����
};

/*
    ����������� / ����������
*/

DirectLight::DirectLight ()
  : impl (new Impl)
{
  impl->radius = DEFAULT_LIGHT_RADIUS;
}

DirectLight::~DirectLight ()
{
  delete impl;
}

/*
    �������� ��������� �����
*/

DirectLight* DirectLight::Create ()
{
  return new DirectLight;
}

/*
    ������ �������� ��������� �����
*/

void DirectLight::SetRadius (float radius)
{
  impl->radius = radius;
  UpdateBoundsNotify ();
}

float DirectLight::Radius () const
{
  return impl->radius;
}

/*
   ������� ��������������� ������
*/

void DirectLight::UpdateBoundsCore ()
{
  if (Range () >= INFINITY || impl->radius >= INFINITY)
    SetInfiniteBounds ();
  else
    SetBoundBox (axis_aligned_box <float> (-impl->radius, -impl->radius, 0, impl->radius, impl->radius, Range ()));
}

/*
    �����, ���������� ��� ��������� �������
*/

void DirectLight::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Light::AcceptCore (visitor);
}
