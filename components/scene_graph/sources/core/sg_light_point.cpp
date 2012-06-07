#include "shared.h"

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;

const float LIGHT_INFINITY = 1e9;  //���� ������/���������� ��������� ����� ��������� ��� ��������, ��������������� ����������� bv

/*
    ����������� / ����������
*/

PointLight::PointLight ()
{
  xtl::get_instance_counter<PointLight> () += 1;
}

PointLight::~PointLight ()
{
  xtl::get_instance_counter<PointLight> () -= 1;
}

/*
    �������� ��������� �����
*/

PointLight::Pointer PointLight::Create ()
{
  return Pointer (new PointLight, false);
}

/*
   ������� ��������������� ������
*/

void PointLight::UpdateBoundsCore ()
{
  if (Range () >= LIGHT_INFINITY)
    SetInfiniteBounds ();
  else;
    SetBoundBox (axis_aligned_box <float> (-Range (), -Range (), -Range (), Range (), Range (), Range ()));
//    SetBoundBox (sphere <float> (vec3f (0), Range ()));
}

/*
    �����, ���������� ��� ��������� �������
*/

void PointLight::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Light::AcceptCore (visitor);
}
