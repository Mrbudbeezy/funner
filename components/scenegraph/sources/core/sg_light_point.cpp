#include <sg/light.h>
#include <xtl/visitor.h>
//#include <bv/sphere.h>

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;

const float INFINITY = 1e9;  //���� ������/���������� ��������� ����� ��������� ��� ��������, ��������������� ����������� bv

/*
    ����������� / ����������
*/

PointLight::PointLight ()
{
}

PointLight::~PointLight ()
{
}

/*
    �������� ��������� �����
*/

PointLight* PointLight::Create ()
{
  return new PointLight;
}

/*
   ������� ��������������� ������
*/

void PointLight::UpdateBoundsCore ()
{
  if (Range () >= INFINITY)
    SetInfiniteBounds ();
  else;
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
