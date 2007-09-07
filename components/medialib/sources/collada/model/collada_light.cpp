#include <media/collada/scene.h>

using namespace media::collada;
using namespace common;
using namespace math;

/*
    �������� ���������� ��������� �����
*/

struct Light::Impl
{
  LightType type;                    //��� ��������� �����
  float     params [LightParam_Num]; //��������� ��������� �����
  vec3f     color;                   //���� ���������
  
  Impl () : type (LightType_Point)
  {
    for (size_t i=0; i<LightParam_Num; i++)
      params [i] = 0.0f;
  }
};

/*
    ����������� / ����������
*/

Light::Light (ModelImpl* owner, const char* id)
  : Entity (owner, id), impl (new Impl)
  {}

Light::~Light ()
{
  delete impl;
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
      RaiseInvalidArgument ("media::collada::Light::SetType", "type", type);
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
    RaiseInvalidArgument ("media::collada::Light::SetParam", "param", param);
    
  impl->params [param] = value;
}

float Light::Param (LightParam param) const
{
  if (param < 0 || param >= LightParam_Num)
    RaiseInvalidArgument ("media::collada::Light::Param", "param", param);
    
  return impl->params [param];
}
