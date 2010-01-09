#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

typedef xtl::com_ptr<Shape>            ShapePtr;
typedef xtl::signal<void (RigidBody*)> BeforeDestroySignal;

/*
    �������� ���������� �������� ����
*/

struct RigidBody::Impl
{
  btRigidBody         *body;                 //����
  btMotionState       *motion_state;         //�������� ���������� ��������� ��������
  ShapePtr            shape;                 //�������������� ������������� ����
  Transform           world_transform;       //��������� ���� � ������� �����������
  size_t              collision_group;       //������ �������� ����
  math::vec3f         inertia_tensor;        //������ �������
  math::vec3f         linear_velocity;       //�������� ��������
  math::vec3f         angular_velocity;      //������� ��������
  BeforeDestroySignal before_destroy_signal; //������ �������� ����

  Impl (IShape* in_shape, float mass)
    : collision_group (0)
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
    �����������/����������
*/

RigidBody::RigidBody (IShape* shape, float mass)
  : impl (new Impl (shape, mass))
  {}

RigidBody::~RigidBody ()
{
  impl->before_destroy_signal (this);
}

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
  vector_from_bullet_vector (impl->body->getLinearVelocity (), impl->linear_velocity);

  return impl->linear_velocity;
}

const math::vec3f& RigidBody::AngularVelocity ()
{
  vector_from_bullet_vector (impl->body->getAngularVelocity (), impl->angular_velocity);

  return impl->angular_velocity;
}

void RigidBody::SetLinearVelocity (const math::vec3f& velocity)
{
  btVector3 linear_velocity (velocity.x, velocity.y, velocity.z);

  impl->body->setLinearVelocity (linear_velocity);
}

void RigidBody::SetAngularVelocity (const math::vec3f& velocity)
{
  btVector3 angular_velocity (velocity.x, velocity.y, velocity.z);

  impl->body->setAngularVelocity (angular_velocity);
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
  impl->body->setMassProps (mass, impl->body->getInvInertiaDiagLocal ());
}

/*
   ���������� ��������
*/

const math::vec3f& RigidBody::MassSpaceInertiaTensor ()
{
  vector_from_bullet_vector (impl->body->getInvInertiaDiagLocal (), impl->inertia_tensor);

  return impl->inertia_tensor;  //????
}

void RigidBody::SetMassSpaceInertiaTensor (const math::vec3f& value)
{
  btVector3 inertia (value.x, value.y, value.z);

  impl->body->setInvInertiaDiagLocal (inertia); //????
}

/*
   ���������� ����
*/

float RigidBody::SleepLinearVelocity ()
{
  return impl->body->getLinearSleepingThreshold ();
}

float RigidBody::SleepAngularVelocity ()
{
  return impl->body->getAngularSleepingThreshold ();
}

void RigidBody::SetSleepLinearVelocity (float value)
{
  impl->body->setSleepingThresholds (value, SleepAngularVelocity ());
}

void RigidBody::SetSleepAngularVelocity (float value)
{
  impl->body->setSleepingThresholds (SleepLinearVelocity (), value);
}

/*
   ���������� ��������� ��������� �������� �������� � ����� ������������ ��������
*/

float RigidBody::CcdMotionThreshold ()
{
  return impl->body->getCcdMotionThreshold ();
}

void RigidBody::SetCcdMotionThreshold (float value)
{
  impl->body->setCcdMotionThreshold (value);
}

/*
   ������������ ������� � �������
*/

size_t RigidBody::CollisionGroup ()
{
  return impl->collision_group;
}

void RigidBody::SetCollisionGroup (size_t group_number)
{
  impl->collision_group = group_number;
}

/*
   ������� ���������
*/

const Transform& RigidBody::WorldTransform ()
{
  const btTransform& world_transform = impl->body->getWorldTransform ();

  vector_from_bullet_vector         (world_transform.getOrigin (),   impl->world_transform.position);
  quaternion_from_bullet_quaternion (world_transform.getRotation (), impl->world_transform.orientation);

  return impl->world_transform;
}

void RigidBody::SetWorldTransform (const Transform& transform)
{
  btTransform new_world_transform;

  bullet_vector_from_vector (transform.position, new_world_transform.getOrigin ());

  btQuaternion transform_rotation;

  bullet_quaternion_from_quaternion (transform.orientation, transform_rotation);

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

/*
   �������� �� �������� �������
*/

xtl::connection RigidBody::RegisterDestroyHandler (const BeforeDestroyHandler& handler)
{
  return impl->before_destroy_signal.connect (handler);
}
