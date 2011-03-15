#include "shared.h"

using namespace physics;

/*
   ���������� �����
*/

/*
   ����������� / ���������� / �����������
*/

SceneInternal::SceneInternal (SceneImpl* impl)
  : Scene (impl)
  {}

Scene::Scene (SceneImpl* source_impl)
  : impl (source_impl)
  {}

Scene::Scene (const Scene& source)
  : impl (source.impl)
{
  addref (impl);
}

Scene::~Scene ()
{
  release (impl);
}

Scene& Scene::operator = (const Scene& source)
{
  Scene (source).Swap (*this);

  return *this;
}

/*
   �������� ���
*/

RigidBody Scene::CreateBody (const char* name)
{
  return impl->CreateBody (name);
}

RigidBody Scene::CreateBody (const Shape& shape, float mass)
{
  return impl->CreateBody (shape, mass);
}

/*
   �������� ������ ����� ������
*/

Joint Scene::CreateSphericalJoint (const JointBind& bind1, const JointBind& bind2)
{
  return impl->CreateSphericalJoint (bind1, bind2);
}

Joint Scene::CreateConeTwistJoint (const JointBind& bind1, const JointBind& bind2, float swing_limit_in_degrees, float twist_limit)
{
  return impl->CreateConeTwistJoint (bind1, bind2, swing_limit_in_degrees, twist_limit);
}

Joint Scene::CreateHingeJoint (const JointBind& bind1, const JointBind& bind2)
{
  return impl->CreateHingeJoint (bind1, bind2);
}

Joint Scene::CreatePrismaticJoint (const JointBind& bind1, const JointBind& bind2)
{
  return impl->CreatePrismaticJoint (bind1, bind2);
}

/*
   ���������� �����������
*/

const math::vec3f& Scene::Gravity () const
{
  return impl->Gravity ();
}

void Scene::SetGravity (const math::vec3f& value)
{
  impl->SetGravity (value);
}

/*
   ��������� ���������
*/

float Scene::SimulationStep () const
{
  return impl->SimulationStep ();
}

void Scene::SetSimulationStep (float step)
{
  impl->SetSimulationStep (step);
}

/*
   ���������
*/
void Scene::PerformSimulation (float dt)
{
  impl->PerformSimulation (dt);
}

/*
  ���������� ������������ ��������. ������� ������������, ����:
   - �� ����� ������;
   - ����� ��������� ������ � collides = true;
   - �������� ������ ���������� true � collides = true;
   - �������� ������ ���������� false � collides = false.
  ��������� ����������: ��� ����� �������� ������, ��� ���� ��� ���������
*/

size_t Scene::AddCollisionFilter (const char* group1_mask, const char* group2_mask, bool collides, const BroadphaseCollisionFilter& filter)
{
  return impl->AddCollisionFilter (group1_mask, group2_mask, collides, filter);
}

void Scene::RemoveCollisionFilter (size_t id)
{
  impl->RemoveCollisionFilter (id);
}

void Scene::RemoveAllCollisionFilters ()
{
  impl->RemoveAllCollisionFilters ();
}

/*
   ��������� ������������ ��������
*/

xtl::connection Scene::RegisterCollisionCallback (const char* group1_mask, const char* group2_mask, CollisionEventType event_type, const CollisionCallback& callback_handler)
{
  return impl->RegisterCollisionCallback (group1_mask, group2_mask, event_type, callback_handler);
}

/*
   ���������� ���������
*/

void Scene::Draw (render::debug::PrimitiveRender& render)
{
  impl->Draw (render);
}

/*
   �����
*/

void Scene::Swap (Scene& scene)
{
  stl::swap (impl, scene.impl);
}

namespace physics
{

/*
   �����
*/

void swap (Scene& scene1, Scene& scene2)
{
  scene1.Swap (scene2);
}

}
