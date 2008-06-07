#include "shared.h"

using namespace media::collada;
using namespace common;
using namespace math;

/*
    �������� ���������� ������
*/

struct Camera::Impl: public xtl::reference_counter
{
  CameraType  type;                     //��� ������
  float       params [CameraParam_Num]; //��������� ������
  stl::string id;                       //������������� ������
  
  Impl () : type (CameraType_Perspective)
  {
    for (size_t i=0; i<CameraParam_Num; i++)
      params [i] = 0.0f;
  }
};

/*
    ������������ / ���������� / ������������
*/

Camera::Camera ()
  : impl (new Impl, false)
  {}

Camera::Camera (Impl* in_impl)
  : impl (in_impl, false)
  {}

Camera::Camera (const Camera& camera)
  : impl (camera.impl)
  {}

Camera::~Camera ()
{
}

Camera& Camera::operator = (const Camera& camera)
{
  impl = camera.impl;

  return *this;
}

/*
    �������� �����
*/

Camera Camera::Clone () const
{
  return Camera (new Impl (*impl));
}

/*
    ������������� ������
*/

const char* Camera::Id () const
{
  return impl->id.c_str ();
}

void Camera::SetId (const char* id)
{
  if (!id)
    raise_null_argument ("media::collada::Camera::SetId", "id");

  impl->id = id;
}
  
/*
    ��� ���������
*/

CameraType Camera::Type () const
{
  return impl->type;
}

void Camera::SetType (CameraType type)
{
  switch (type)
  {
    case CameraType_Perspective:
    case CameraType_Orthographic:
      break;
    default:
      raise_invalid_argument ("media::collada::Camera::SetType", "type", type);
      break;
  }
  
  impl->type = type;
}

/*
    ��������� ������
*/

void Camera::SetParam (CameraParam param, float value)
{
  if (param < 0 || param >= CameraParam_Num)
    raise_invalid_argument ("media::collada::Camera::SetParam", "param", param);
    
  impl->params [param] = value;
}

float Camera::Param (CameraParam param) const
{
  if (param < 0 || param >= CameraParam_Num)
    raise_invalid_argument ("media::collada::Camera::Param", "param", param);
    
  return impl->params [param];
}
