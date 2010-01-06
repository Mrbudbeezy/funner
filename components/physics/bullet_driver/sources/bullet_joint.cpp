#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

/*
    �������� ���������� ���������� ���
*/

struct Joint::Impl
{
};

/*
    �����������
*/

Joint::Joint ()
  : impl (new Impl)
  {}

/*
   ������� �����
*/

/*
   ��������� ��������
*/

size_t Joint::ObjectsCount ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Joint::ObjectsCount");
}

RigidBody* Joint::GetObject (size_t index)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Joint::GetObject");
}
