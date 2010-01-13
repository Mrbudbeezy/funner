#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

namespace
{

const size_t JOINT_BODIES_COUNT = 2;     //���������� ��� � ����� ����������

typedef xtl::signal<void (Joint*)> BeforeDestroySignal;

}

/*
    �������� ���������� ���������� ���
*/

struct Joint::Impl
{
  size_t               bodies_count;                                  //���������� ���
  RigidBody*           bodies [JOINT_BODIES_COUNT];                   //������ ����
  btTypedConstraint*   joint;                                         //����������
  BeforeDestroySignal  before_destroy_signal;                         //������ �������� ����������
  xtl::auto_connection body_destroy_connections [JOINT_BODIES_COUNT]; //���������� �������� ���

  Impl (RigidBody* body1, RigidBody* body2, btTypedConstraint* in_joint)
    : bodies_count (JOINT_BODIES_COUNT), joint (in_joint)
  {
    bodies [0] = body1;
    bodies [1] = body2;

    body_destroy_connections [0] = body1->RegisterDestroyHandler (xtl::bind (&Joint::Impl::OnBodyDestroy, this));
    body_destroy_connections [1] = body2->RegisterDestroyHandler (xtl::bind (&Joint::Impl::OnBodyDestroy, this));
  }

  ~Impl ()
  {
    delete joint;
  }

  void OnBodyDestroy ()
  {
    bodies_count = 0;
  }
};

/*
    ����������� / ����������
*/

Joint::Joint (RigidBody* body1, RigidBody* body2, btTypedConstraint* joint)
  : impl (new Impl (body1, body2, joint))
  {}

Joint::~Joint ()
{
  impl->before_destroy_signal (this);
}


/*
   ��������� ��������
*/

size_t Joint::ObjectsCount ()
{
  return impl->bodies_count;
}

IRigidBody* Joint::GetObject (size_t index)
{
  if (index >= ObjectsCount ())
    throw xtl::make_range_exception ("physics::low_level::bullet::Joint::GetObject", "index", index, 0u, ObjectsCount ());

  return impl->bodies [index];
}

/*
   ��������� bullet ����������
*/

btTypedConstraint* Joint::BulletJoint ()
{
  return impl->joint;
}

/*
   �������� �� �������� �������
*/

xtl::connection Joint::RegisterDestroyHandler (const BeforeDestroyHandler& handler)
{
  return impl->before_destroy_signal.connect (handler);
}
