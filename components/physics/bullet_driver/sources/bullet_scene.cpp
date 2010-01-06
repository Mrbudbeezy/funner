#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

namespace
{

const math::vec3f DEFAULT_GRAVITY (0, -9.8, 0);
const float       DEFAULT_SIMULATION_STEP = 1.f / 60.f;
const size_t      MAX_SIMULATION_SUBSTEPS = 120;

}

/*
    �������� ���������� ���������� �����
*/

struct Scene::Impl
{
  btDefaultCollisionConfiguration     *collision_configuration; //������������ ����������� ��������
  btCollisionDispatcher               *collision_dispatcher;    //���������� ��������
  btBroadphaseInterface               *broadphase_interface;    //���������� ���� broadphase
  btSequentialImpulseConstraintSolver *solver;                  //���������� ����������
  btDiscreteDynamicsWorld             *dynamics_world;          //���������� ���
  math::vec3f                         gravity;                  //����������
  float                               simulation_step;          //��� ���������

  Impl ()
  {
    collision_configuration = new btDefaultCollisionConfiguration ();
    collision_dispatcher    = new btCollisionDispatcher (collision_configuration);

    broadphase_interface = new btDbvtBroadphase ();

    solver = new btSequentialImpulseConstraintSolver ();

    dynamics_world = new btDiscreteDynamicsWorld (collision_dispatcher, broadphase_interface, solver, collision_configuration);
  }

  ~Impl ()
  {
    delete dynamics_world;
    delete solver;
    delete broadphase_interface;
    delete collision_dispatcher;
    delete collision_configuration;
  }
};

/*
    �����������
*/

Scene::Scene ()
  : impl (new Impl)
{
  SetGravity (DEFAULT_GRAVITY);
  SetSimulationStep (DEFAULT_SIMULATION_STEP);
}

/*
   ���������� �����������
*/

const math::vec3f& Scene::Gravity ()
{
  return impl->gravity;
}

void Scene::SetGravity (const math::vec3f& value)
{
  impl->gravity = value;

  btVector3 new_gravity (value.x, value.y, value.z);

  impl->dynamics_world->setGravity (new_gravity);
}

/*
   ���������
*/

float Scene::SimulationStep ()
{
  return impl->simulation_step;
}

void Scene::SetSimulationStep (float step)
{
  impl->simulation_step = step;
}

void Scene::PerformSimulation (float dt)
{
  impl->dynamics_world->stepSimulation (dt, MAX_SIMULATION_SUBSTEPS, impl->simulation_step);
}

/*
   �������� ��� � ����� (����, ���������� ������� ������, �������� ������������)
*/

RigidBody* Scene::CreateRigidBody (IShape* shape, float mass)
{
  if (!shape)
    throw xtl::make_null_argument_exception ("physics::low_level::bullet::Scene::CreateRigidBody", "shape");

  RigidBody* return_value = new RigidBody (shape, mass);

  impl->dynamics_world->addRigidBody (return_value->BulletRigidBody ());

  return return_value;
}

/*
   �������� ���������� ����� ������
*/

Joint* Scene::CreateSphericalJoint (IRigidBody* body1, IRigidBody* body2, const SphericalJointDesc& desc)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Scene::CreateSphericalJoint");
}

Joint* Scene::CreateConeTwistJoint (IRigidBody* body1, IRigidBody* body2, const ConeTwistJointDesc& desc)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Scene::CreateConeTwistJoint");
}

Joint* Scene::CreateHingeJoint (IRigidBody* body1, IRigidBody* body2, const HingeJointDesc& desc)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Scene::CreateHingeJoint");
}

Joint* Scene::CreatePrismaticJoint (IRigidBody* body1, IRigidBody* body2, const PrismaticJointDesc& desc)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Scene::CreatePrismaticJoint");
}

/*
   ���������� ������������ ��������
*/

void Scene::SetCollisionFilter (size_t group1, size_t group2, bool collides, const BroadphaseCollisionFilter& filter = BroadphaseCollisionFilter ())
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Scene::SetCollisionFilter");
}

/*
   ��������� ������������ ��������
*/

xtl::connection Scene::RegisterCollisionCallback (CollisionEventType event_type, const CollisionCallback& callback_handler)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Scene::RegisterCollisionCallback");
}
