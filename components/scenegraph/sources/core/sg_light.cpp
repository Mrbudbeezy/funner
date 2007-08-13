#include <sg/light.h>
#include <xtl/visitor.h>

using namespace scene_graph;
using namespace math;

/*
    �������� ���������� Light
*/

struct Light::Impl
{
  vec3f color;         //���� �����
  vec3f attenuation;   //������������ ��������� (x - constant, y - linear, z - quadratic)
  float range;         //���������� �������� �����
};

/*
    ����������� / ����������
*/

Light::Light ()
  : impl (new Impl)
{
  impl->color       = 0;
  impl->attenuation = 0;
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
  ComputeBV ();
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
