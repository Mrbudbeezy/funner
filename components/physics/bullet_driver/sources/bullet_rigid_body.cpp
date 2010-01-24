#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

typedef xtl::com_ptr<IMaterial>         MaterialPtr;
typedef xtl::com_ptr<Shape>             ShapePtr;
typedef xtl::signal<void (RigidBody*)>  BeforeDestroySignal;
typedef xtl::signal<void (IRigidBody*)> TransformUpdateSignal;

namespace
{

const math::vec3f DEFAULT_MASS_SPACE_INERTIA_TENSOR (1.f);

}

/*
    �������� ���������� �������� ����
*/

struct RigidBody::Impl : public btDefaultMotionState
{
  RigidBody             *rigid_body;                 //������������ ������
  btRigidBody           *body;                       //����
  ShapePtr              shape;                       //�������������� ������������� ����
  MaterialPtr           material;                    //��������
  Transform             world_transform;             //��������� ���� � ������� �����������
  size_t                collision_group;             //������ �������� ����
  size_t                flags;                       //����� ��������� ����
  math::vec3f           inertia_tensor;              //������ �������
  math::vec3f           linear_velocity;             //�������� ��������
  math::vec3f           angular_velocity;            //������� ��������
  BeforeDestroySignal   before_destroy_signal;       //������ �������� ����
  TransformUpdateSignal transform_update_signal;     //������ ���������� ��������� ����
  xtl::auto_connection  material_update_connection;  //���������� ���������� ������� ���������

  Impl (bullet::Shape* in_shape, float mass, RigidBody* in_rigid_body)
    : rigid_body (in_rigid_body), collision_group (0), flags (0)
  {
    if (!in_shape)
      throw xtl::make_null_argument_exception ("physics::low_level::bullet::RigidBody::RigidBody", "shape");

    shape = in_shape;

    body = new btRigidBody (mass, this, shape->BulletCollisionShape ());

    body->setCollisionFlags (body->getCollisionFlags () | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
  }

  ~Impl ()
  {
    delete (RigidBodyInfo*)body->getUserPointer ();

    delete body;
  }

  void UpdateMaterialProperties ()
  {
    const math::vec3f &anisotropic_friction = material->AnisotropicFriction ();

    btVector3 bullet_anisotropic_friction (anisotropic_friction.x, anisotropic_friction.y, anisotropic_friction.z);

    body->setDamping             (material->LinearDamping (), material->LinearDamping ());
    body->setFriction            (material->Friction ());
    body->setAnisotropicFriction (bullet_anisotropic_friction);
    body->setRestitution         (material->Restitution ());
  }

  //���������� btMotionState
  void setWorldTransform (const btTransform& worldTrans)
  {
    transform_update_signal (rigid_body);

    btDefaultMotionState::setWorldTransform (worldTrans);
  }
};

/*
    �����������/����������
*/

RigidBody::RigidBody (bullet::Shape* shape, float mass)
  : impl (new Impl (shape, mass, this))
{
  MaterialPtr default_material (new bullet::Material, false);

  SetMaterial (default_material.get ());

  if (mass)
    SetMassSpaceInertiaTensor (DEFAULT_MASS_SPACE_INERTIA_TENSOR);
}

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

IMaterial* RigidBody::Material ()
{
  return impl->material.get ();
}

void RigidBody::SetMaterial (IMaterial* material)
{
  static const char* METHOD_NAME = "physics::low_level::bullet::RigidBody::SetMaterial";

  if (!material)
    throw xtl::make_null_argument_exception (METHOD_NAME, "material");

  bullet::Material* bullet_material = cast_object<bullet::Material, IMaterial> (material, METHOD_NAME, "material");

  impl->material = material;

  impl->material_update_connection = bullet_material->RegisterUpdateHandler (xtl::bind (&RigidBody::Impl::UpdateMaterialProperties, impl.get ()));

  impl->UpdateMaterialProperties ();
}

/*
   ����� ���������
*/

size_t RigidBody::Flags ()
{
  return impl->flags;
}

void RigidBody::SetFlags (size_t flags)
{
  impl->flags = flags;

  btVector3 linear_factor (1.f, 1.f, 1.f);

  if (flags & RigidBodyFlag_FrozenPositionX)
    linear_factor.setX (0);
  if (flags & RigidBodyFlag_FrozenPositionY)
    linear_factor.setY (0);
  if (flags & RigidBodyFlag_FrozenPositionZ)
    linear_factor.setZ (0);

  impl->body->setLinearFactor (linear_factor);

  btVector3 angular_factor (1.f, 1.f, 1.f);

  if (flags & RigidBodyFlag_FrozenRotationX)
    angular_factor.setX (0);
  if (flags & RigidBodyFlag_FrozenRotationY)
    angular_factor.setY (0);
  if (flags & RigidBodyFlag_FrozenRotationZ)
    angular_factor.setZ (0);

  impl->body->setAngularFactor (angular_factor);

  if (flags & RigidBodyFlag_Kinematic)
    impl->body->setCollisionFlags (impl->body->getCollisionFlags () | btCollisionObject::CF_KINEMATIC_OBJECT);
  else
    impl->body->setCollisionFlags (impl->body->getCollisionFlags () & ~btCollisionObject::CF_KINEMATIC_OBJECT);
}

/*
   ���������� ����/��������/��������
*/

void RigidBody::AddForce (const math::vec3f& force, const math::vec3f& relative_position)
{
  btVector3 bullet_force, bullet_relative_position;

  bullet_vector_from_vector (force, bullet_force);
  bullet_vector_from_vector (relative_position, bullet_relative_position);

  impl->body->applyForce (bullet_force, bullet_relative_position);
}

void RigidBody::AddImpulse (const math::vec3f& impulse, const math::vec3f& relative_position)
{
  btVector3 bullet_impulse, bullet_relative_position;

  bullet_vector_from_vector (impulse, bullet_impulse);
  bullet_vector_from_vector (relative_position, bullet_relative_position);

  impl->body->applyImpulse (bullet_impulse, bullet_relative_position);
}

void RigidBody::AddTorque (const math::vec3f& torque)
{
  btVector3 bullet_torque;

  bullet_vector_from_vector (torque, bullet_torque);

  impl->body->applyTorque (bullet_torque);
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
  impl->body->updateInertiaTensor ();
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
  impl->body->updateInertiaTensor ();
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

xtl::connection RigidBody::RegisterTransformUpdateCallback (const TransformUpdateCallback& handler)
{
  return impl->transform_update_signal.connect (handler);
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
