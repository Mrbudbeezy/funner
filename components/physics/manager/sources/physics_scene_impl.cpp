#include "shared.h"

using namespace physics;

/*
   ���������� �����
*/

struct SceneImpl::Impl : public xtl::reference_counter
{
  ScenePtr scene;

  Impl (ScenePtr in_scene)
    : scene (in_scene)
    {}
};

/*
   ����������� / ���������� / �����������
*/

SceneImpl::SceneImpl (ScenePtr scene)
  : impl (new Impl (scene))
  {}

SceneImpl::SceneImpl (const SceneImpl& source)
  : impl (source.impl)
{
  addref (impl);
}

SceneImpl::~SceneImpl ()
{
  release (impl);
}

SceneImpl& SceneImpl::operator = (const SceneImpl& source)
{
  SceneImpl (source).Swap (*this);

  return *this;
}

/*
   �������� ���
*/

RigidBody SceneImpl::CreateBody (const char* name)
{
  throw xtl::make_not_implemented_exception ("physics::SceneImpl::CreateBody");
}

RigidBody SceneImpl::CreateBody (const Shape& shape, float mass)
{
  throw xtl::make_not_implemented_exception ("physics::SceneImpl::CreateBody");
}

/*
   �������� ������ ����� ������
*/

Joint SceneImpl::CreateSphericalJoint (const JointBind& bind1, const JointBind& bind2)
{
  throw xtl::make_not_implemented_exception ("SceneImpl::CreateSphericalJoint");
}

Joint SceneImpl::CreateConeTwistJoint (const JointBind& bind1, const JointBind& bind2, float swing_limit_in_degrees, float twist_limit)
{
  throw xtl::make_not_implemented_exception ("SceneImpl::CreateConeTwistJoint");
}

Joint SceneImpl::CreateHingeJoint (const JointBind& bind1, const JointBind& bind2)
{
  throw xtl::make_not_implemented_exception ("SceneImpl::CreateHingeJoint");
}

Joint SceneImpl::CreatePrismaticJoint (const JointBind& bind1, const JointBind& bind2)
{
  throw xtl::make_not_implemented_exception ("SceneImpl::CreatePrismaticJoint");
}

/*
   ���������� �����������
*/

const math::vec3f& SceneImpl::Gravity () const
{
  return impl->scene->Gravity ();
}

void SceneImpl::SetGravity (const math::vec3f& value)
{
  impl->scene->SetGravity (value);
}

/*
   ��������� ���������
*/

float SceneImpl::SimulationStep () const
{
  return impl->scene->SimulationStep ();
}

void SceneImpl::SetSimulationStep (float step)
{
  impl->scene->SetSimulationStep (step);
}

/*
   ���������
*/
void SceneImpl::PerformSimulation (float dt)
{
  impl->scene->PerformSimulation (dt);
}

/*
  ���������� ������������ ��������. ������� ������������, ����:
   - �� ����� ������;
   - ����� ��������� ������ � collides = true;
   - �������� ������ ���������� true � collides = true;
   - �������� ������ ���������� false � collides = false.
  ��������� ����������: ��� ����� �������� ������, ��� ���� ��� ���������
*/

size_t SceneImpl::AddCollisionFilter (const char* group1_mask, const char* group2_mask, bool collides, const Scene::BroadphaseCollisionFilter& filter)
{
  throw xtl::make_not_implemented_exception ("physics::SceneImpl::AddCollisionFilter");
}

void SceneImpl::RemoveCollisionFilter (size_t id)
{
  throw xtl::make_not_implemented_exception ("physics::SceneImpl::RemoveCollisionFilter");
}

void SceneImpl::RemoveAllCollisionFilters ()
{
  throw xtl::make_not_implemented_exception ("physics::SceneImpl::RemoveAllCollisionFilters");
}

/*
   ��������� ������������ ��������
*/

xtl::connection SceneImpl::RegisterCollisionCallback (const char* group1_mask, const char* group2_mask, CollisionEventType event_type, const CollisionCallback& callback_handler)
{
  throw xtl::make_not_implemented_exception ("physics::SceneImpl::RegisterCollisionCallback");
}

/*
   ���������� ���������
*/

void SceneImpl::Draw (render::debug::PrimitiveRender& render)
{
  impl->scene->Draw (render);
}

/*
   �����
*/

void SceneImpl::Swap (SceneImpl& scene)
{
  stl::swap (impl, scene.impl);
}
