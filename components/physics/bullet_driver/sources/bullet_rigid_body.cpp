#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

typedef xtl::com_ptr<Shape> ShapePtr;

/*
    �������� ���������� �������� ����
*/

struct RigidBody::Impl
{
  btRigidBody   *body;           //����
  btMotionState *motion_state;   //�������� ���������� ��������� ��������
  ShapePtr      shape;           //�������������� ������������� ����
  Transform     world_transform; //��������� ���� � ������� �����������

  Impl (IShape* in_shape, float mass)
  {
    if (!in_shape)
      throw xtl::make_null_argument_exception ("physics::low_level::bullet::RigidBody::RigidBody", "shape");

    shape = (bullet::Shape*)in_shape;

    motion_state = new btDefaultMotionState ();

    body = new btRigidBody (mass, motion_state, shape->BulletCollisionShape ());
  }

  ~Impl ()
  {
    delete body;
    delete motion_state;
  }
};

/*
    �����������
*/

RigidBody::RigidBody (IShape* shape, float mass)
  : impl (new Impl (shape, mass))
  {}

/*
   �������������� �������������
*/

Shape* RigidBody::Shape ()
{
  return impl->shape.get ();
}

/*
   ��������
*/

Material* RigidBody::Material ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::Material");
}

void RigidBody::SetMaterial (IMaterial* material)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SetMaterial");
}

/*
   ����� ���������
*/

size_t RigidBody::Flags ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::Flags");
}

void RigidBody::SetFlags (size_t flags)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SetFlags");
}

/*
   ���������� ����/��������/��������
*/

void RigidBody::AddForce (const math::vec3f& force, const math::vec3f& relative_position)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::AddForce");
}

void RigidBody::AddImpulse (const math::vec3f& impulse, const math::vec3f& relative_position)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::AddImpulse");
}

void RigidBody::AddTorque (const math::vec3f& torgue)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::AddTorgue");
}

/*
   ���������� ��������/������� ����������
*/

const math::vec3f& RigidBody::LinearVelocity ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::LinearVelocity");
}

const math::vec3f& RigidBody::AngularVelocity ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::AngularVelocity");
}

void RigidBody::SetLinearVelocity (const math::vec3f& velocity)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SetLinearVelocity");
}

void RigidBody::SetAngularVelocity (const math::vec3f& velocity)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SetAngularVelocity");
}

/*
   ���������� ���������� ������ ����
*/

const math::vec3f& RigidBody::CenterOfMassLocalPosition ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::CenterOfMassLocalPosition");
}

void RigidBody::SetCenterOfMassLocalPosition (const math::vec3f& value)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SetCenterOfMassLocalPosition");
}

/*
   ���������� ������
*/

float RigidBody::Mass ()
{
  float inverse_mass = impl->body->getInvMass ();

  return inverse_mass ? 1.f / inverse_mass : 0;
}

void RigidBody::SetMass (float mass)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SetMass");
}

/*
   ���������� ��������
*/

const math::vec3f& RigidBody::MassSpaceInertiaTensor ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::MassSpaceInertiaTensor");
}

void RigidBody::SetMassSpaceInertiaTensor (const math::vec3f& value)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SetMassSpaceInertiaTensor");
}

/*
   ���������� ����
*/

float RigidBody::SleepLinearVelocity ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SleepLinearVelocity");
}

float RigidBody::SleepAngularVelocity ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SleepAngularVelocity");
}

void RigidBody::SetSleepLinearVelocity (float value)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SetSleepLinearVelocity");
}

void RigidBody::SetSleepAngularVelocity (float value)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SetSleepAngularVelocity");
}

/*
   ���������� ��������� ��������� �������� �������� � ����� ������������ ��������
*/

float RigidBody::CcdMotionThreshold ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::CcdMotionThreshold");
}

void RigidBody::SetCcdMotionThreshold (float)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SetCcdMotionThreshold");
}

/*
   ������������ ������� � �������
*/

size_t RigidBody::CollisionGroup ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::CollisionGroup");
}

void RigidBody::SetCollisionGroup (size_t group_number)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::RigidBody::SetCollisionGroup");
}

/*
   ������� ���������
*/

const Transform& RigidBody::WorldTransform ()
{
  const btTransform& world_transform = impl->body->getWorldTransform ();

  const btVector3& origin = world_transform.getOrigin ();

  impl->world_transform.position.x = origin.x ();
  impl->world_transform.position.y = origin.y ();
  impl->world_transform.position.z = origin.z ();

  const btQuaternion& rotation = world_transform.getRotation ();

  impl->world_transform.orientation.x = rotation.getX ();
  impl->world_transform.orientation.y = rotation.getY ();
  impl->world_transform.orientation.z = rotation.getZ ();
  impl->world_transform.orientation.w = rotation.getW ();

  return impl->world_transform;
}

void RigidBody::SetWorldTransform (const Transform& transform)
{
  btTransform new_world_transform;
  btVector3&  transform_position = new_world_transform.getOrigin ();

  transform_position.setX (transform.position.x);
  transform_position.setY (transform.position.y);
  transform_position.setZ (transform.position.z);

  btQuaternion transform_rotation;

  transform_rotation.setX (transform.orientation.x);
  transform_rotation.setY (transform.orientation.y);
  transform_rotation.setZ (transform.orientation.z);
  transform_rotation.setW (transform.orientation.w);

  new_world_transform.setRotation (transform_rotation);

  impl->body->setWorldTransform (new_world_transform);
}

/*
   ��������� bullet ����
*/

btRigidBody* RigidBody::BulletRigidBody ()
{
  return impl->body;
}
