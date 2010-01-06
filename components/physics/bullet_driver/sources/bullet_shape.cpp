#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

/*
    �������� ���������� ��������������� ����
*/

struct Shape::Impl
{
  btCollisionShape* collision_shape;

  Impl (btCollisionShape* shape)
    : collision_shape (shape)
  {
    if (!shape)
      throw xtl::make_null_argument_exception ("physics::low_level::bullet::Shape::Shape", "shape");
  }

  ~Impl ()
  {
    delete collision_shape;
  }
};

/*
    �����������
*/

Shape::Shape (btCollisionShape* shape)
  : impl (new Impl (shape))
  {}

/*
   ������� �����
*/

float Shape::Margin ()
{
  return impl->collision_shape->getMargin ();
}

void Shape::GetMargin (float value)
{
  impl->collision_shape->setMargin (value);
}

/*
   ��������� bullet ����
*/

btCollisionShape* Shape::BulletCollisionShape ()
{
  return impl->collision_shape;
}
