#include "shared.h"

using namespace media::collada;
using namespace common;
using namespace math;

/*
    �������� ���������� ��������� �����
*/

struct Light::Impl: public xtl::reference_counter
{
  LightType   type;                    //��� ��������� �����
  float       params [LightParam_Num]; //��������� ��������� �����
  vec3f       color;                   //���� ���������
  stl::string id;                      //������������� ���������
  
  Impl () : type (LightType_Point)
  {
    for (size_t i=0; i<LightParam_Num; i++)
      params [i] = 0.0f;
  }
};

/*
    ������������ / ���������� / ������������
*/

Light::Light ()
  : impl (new Impl, false)
  {}
  
Light::Light (Impl* in_impl)
  : impl (in_impl, false)
  {}
  
Light::Light (const Light& light)
  : impl (light.impl)
  {}

Light::~Light ()
{
}

Light& Light::operator = (const Light& light)
{
  impl = light.impl;

  return *this;
}

/*
    �������� �����
*/

Light Light::Clone () const
{
  return Light (new Impl (*impl));
}

/*
    ������������� ���������
*/

const char* Light::Id () const
{
  return impl->id.c_str ();
}

void Light::SetId (const char* id)
{
  if (!id)
    raise_null_argument ("media::collada::Light::SetId", "id");
    
  impl->id = id;
}
  
/*
    ��� ���������
*/

LightType Light::Type () const
{
  return impl->type;
}

void Light::SetType (LightType type)
{
  switch (type)
  {
    case LightType_Ambient:
    case LightType_Point:
    case LightType_Spot:
    case LightType_Direct:
      break;
    default:
      raise_invalid_argument ("media::collada::Light::SetType", "type", type);
      break;
  }
  
  impl->type = type;
}

/*
    ���� ���������
*/

const vec3f& Light::Color () const
{
  return impl->color;
}

void Light::SetColor (const vec3f& color)
{
  impl->color = color;
}

/*
    ��������� ���������
*/

void Light::SetParam (LightParam param, float value)
{
  if (param < 0 || param >= LightParam_Num)
    raise_invalid_argument ("media::collada::Light::SetParam", "param", param);
    
  impl->params [param] = value;
}

float Light::Param (LightParam param) const
{
  if (param < 0 || param >= LightParam_Num)
    raise_invalid_argument ("media::collada::Light::Param", "param", param);
    
  return impl->params [param];
}
