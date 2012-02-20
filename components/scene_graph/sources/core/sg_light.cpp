#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    �������� ���������� Light
*/

struct Light::Impl
{
  vec3f color;         //���� �����
  vec3f attenuation;   //������������ ��������� (x - constant, y - linear, z - quadratic)
  float intensity;     //������������� ��������� �����
  float range;         //���������� �������� �����
};

/*
    ����������� / ����������
*/

Light::Light ()
  : impl (new Impl)
{
  impl->color       = math::vec3f (1.0f);
  impl->attenuation = 0;
  impl->intensity   = 1.0f;
  impl->range       = DEFAULT_LIGHT_RANGE;
}

Light::~Light ()
{
  delete impl;
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
    ������������� ��������� �����
*/

void Light::SetIntensity (float value)
{
  impl->intensity = value;
}

float Light::Intensity () const
{
  return impl->intensity;
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
  UpdateBoundsNotify ();
}

float Light::Range () const
{
  return impl->range;
}

/*
    �����, ���������� ��� ��������� �������
*/

void Light::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}

/*
    ���������� �������
*/

void Light::BindProperties (common::PropertyBindingMap& bindings)
{
  Entity::BindProperties (bindings);

  bindings.AddProperty ("LightColor", xtl::bind (&Light::LightColor, this), xtl::bind (&Light::SetLightColor, this, _1));
  bindings.AddProperty ("Intensity", xtl::bind (&Light::Intensity, this), xtl::bind (&Light::SetIntensity, this, _1));
  bindings.AddProperty ("Range", xtl::bind (&Light::Range, this), xtl::bind (&Light::SetRange, this, _1));
  bindings.AddProperty ("Attenuation", xtl::bind (&Light::Attenuation, this), xtl::bind (&Light::SetAttenuation, this, _1));
}
