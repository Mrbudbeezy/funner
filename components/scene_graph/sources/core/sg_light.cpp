#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации Light
*/

struct Light::Impl: public xtl::instance_counter<Light>
{
  vec3f color;         //цвет света
  vec3f attenuation;   //коэффициенты затухания (x - constant, y - linear, z - quadratic)
  float intensity;     //интенсивность источника света
  float range;         //расстояние действия света
};

/*
    Конструктор / деструктор
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
    Цвет источника света
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
    Интенсивность источника света
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
    Затухание (constant, linear, quadratic)
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
    Расстояние действия источника света
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
    Метод, вызываемый при посещении объекта
*/

void Light::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}

/*
    Связывание свойств
*/

void Light::BindProperties (common::PropertyBindingMap& bindings)
{
  Entity::BindProperties (bindings);

  bindings.AddProperty ("LightColor", xtl::bind (&Light::LightColor, this), xtl::bind (&Light::SetLightColor, this, _1));
  bindings.AddProperty ("Intensity", xtl::bind (&Light::Intensity, this), xtl::bind (&Light::SetIntensity, this, _1));
  bindings.AddProperty ("Range", xtl::bind (&Light::Range, this), xtl::bind (&Light::SetRange, this, _1));
  bindings.AddProperty ("Attenuation", xtl::bind (&Light::Attenuation, this), xtl::bind (&Light::SetAttenuation, this, _1));
}
