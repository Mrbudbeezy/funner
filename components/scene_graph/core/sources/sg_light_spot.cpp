#include "shared.h"

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;

const float LIGHT_INFINITY = 1e9;  //���� ������/���������� ��������� ����� ��������� ��� ��������, ��������������� ����������� bv

/*
    �������� ���������� SpotLight
*/

struct SpotLight::Impl
{
  float angle;         //���� �����
  float exponent;      //���������� ����������� �� ����
};

/*
    ����������� / ����������
*/

SpotLight::SpotLight ()
  : impl (new Impl)
{
  impl->angle    = 0;
  impl->exponent = 0;
}

SpotLight::~SpotLight ()
{
  delete impl;
}

/*
    �������� ��������� �����
*/

SpotLight::Pointer SpotLight::Create ()
{
  return Pointer (new SpotLight, false);
}

/*
    ��������� �������� ��������� �����
*/

void SpotLight::SetAngle (float angle)
{
  impl->angle = angle;
  UpdateBoundsNotify ();
}

float SpotLight::Angle () const
{
  return impl->angle;
}

void SpotLight::SetExponent (float exponent)
{
  impl->exponent = exponent;
}

float SpotLight::Exponent () const
{
  return impl->exponent;
}

/*
   ������� ��������������� ������
*/

void SpotLight::UpdateBoundsCore ()
{
  if (Range () >= LIGHT_INFINITY)
    SetInfiniteBounds ();
  else
  {
    float half_width  = tan (deg2rad (impl->angle / 2)) * Range (); 
  
    SetBoundBox (axis_aligned_box <float> (-half_width, -half_width, 0, half_width, half_width, Range ()));
  }
}

/*
    �����, ���������� ��� ��������� �������
*/

void SpotLight::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Light::AcceptCore (visitor);
}
