#include <sg/light.h>
#include <xtl/visitor.h>

using namespace sg;
using namespace math;

/*
    �������� ���������� Light
*/

struct Light::Impl
{
  LightType type;          //��� ��������� �����
  vec3f     color;         //���� �����
  float     spot_angle;    //���� (��� Spot)
  float     spot_exponent; //���������� ��������� �� ���� (��� Spot)
  vec3f     attenuation;   //������������ ��������� (x - constant, y - linear, z - quadratic)
  float     range;         //���������� �������� �����
};

/*
    ����������� / ����������
*/

Light::Light ()
  : impl (new Impl)
{
  impl->type          = LightType_Point;
  impl->color         = 0;
  impl->spot_angle    = 0;
  impl->spot_exponent = 0;
  impl->attenuation   = 0;
  impl->range         = DEFAULT_LIGHT_RANGE;
}

Light::~Light ()
{
  delete impl;
}

/*
    �������� ��������� �����
*/

Light* Light::Create ()
{
  return new Light;
}

/*
    ��� ��������� �����
*/

void Light::SetType (LightType type)
{
  impl->type = type;
}

LightType Light::Type () const
{
  return impl->type;
}

/*
    ���� ��������� �����
*/

void Light::SetLightColor (const math::vec3f& color)
{
  impl->color = color;
}

const math::vec3f& Light::LightColor () const
{
  return impl->color;
}

/*
    ��������� ��������� �����
*/

void Light::SetSpotAngle (float angle)
{
  impl->spot_angle = angle;
}

float Light::SpotAngle () const
{
  return impl->spot_angle;
}

void Light::SetSpotExponent (float exponent)
{
  impl->spot_exponent = exponent;
}

float Light::SpotExponent () const
{
  return impl->spot_exponent;
}

/*
    ��������� (constant, linear, quadratic)
*/

void Light::SetAttenuation (const math::vec3f& multiplier)
{
  impl->attenuation = multiplier;
}

const math::vec3f& Light::Attenuation () const
{
  return impl->attenuation;
}

/*
    ���������� �������� ��������� �����
*/

void Light::SetRange (float range)
{
  impl->range = range;
}

float Light::Range () const
{
  return impl->range;
}

/*
    ������������ ���������������    
*/

void Light::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Node::AcceptCore (visitor);
}
