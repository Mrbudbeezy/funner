#include "shared.h"

using namespace physics;

/*
   ���������� �����
*/

typedef xtl::com_ptr<physics::low_level::IDriver> DriverPtr;

struct Scene::Impl : public xtl::reference_counter
{
  DriverPtr                                           driver;
  ScenePtr                                            scene;
  media::physics::PhysicsLibrary::RigidBodyCollection body_collection;

  Impl (physics::low_level::IDriver* in_driver, ScenePtr in_scene, const media::physics::PhysicsLibrary::RigidBodyCollection& in_body_collection)
    : scene (in_scene), body_collection (in_body_collection)
  {
    if (!driver)
      throw xtl::make_null_argument_exception ("physics::Scene::Impl::Impl", "driver");

    driver = in_driver;
  }

  ///�������� ���
  RigidBody CreateBody (const char* name)
  {
    try
    {
      media::physics::RigidBody *media_body = body_collection.Find (name);

      if (!media_body)
        throw xtl::format_operation_exception ("", "Body '%s' was not loaded", name);

      Shape        shape        (ShapeImplProvider::CreateShape (driver.get (), media_body->Shape ()));
      RigidBodyPtr body         (scene->CreateRigidBody (ShapeImplProvider::LowLevelShape (shape), media_body->Mass ()));
      Material     material     (MaterialImplProvider::CreateMaterial (driver.get (), media_body->Material ()));
      RigidBody    return_value (RigidBodyImplProvider::CreateRigidBody (body, shape, material));

      return_value.SetMassSpaceInertiaTensor (media_body->MassSpaceInertiaTensor ());

      size_t flags = 0, media_flags = media_body->Flags ();

      if (media_flags && media::physics::RigidBodyFlag_FrozenPositionX)
        flags |= physics::RigidBodyFlag_FrozenPositionX;
      if (media_flags && media::physics::RigidBodyFlag_FrozenPositionY)
        flags |= physics::RigidBodyFlag_FrozenPositionY;
      if (media_flags && media::physics::RigidBodyFlag_FrozenPositionZ)
        flags |= physics::RigidBodyFlag_FrozenPositionZ;
      if (media_flags && media::physics::RigidBodyFlag_FrozenRotationX)
        flags |= physics::RigidBodyFlag_FrozenRotationX;
      if (media_flags && media::physics::RigidBodyFlag_FrozenRotationY)
        flags |= physics::RigidBodyFlag_FrozenRotationY;
      if (media_flags && media::physics::RigidBodyFlag_FrozenRotationZ)
        flags |= physics::RigidBodyFlag_FrozenRotationZ;
      if (media_flags && media::physics::RigidBodyFlag_Kinematic)
        flags |= physics::RigidBodyFlag_Kinematic;

      return_value.SetFlags (flags);

      return return_value;
    }
    catch (xtl::exception& e)
    {
      e.touch ("physics::PhysicsManager::CreateMaterial (const char*)");
      throw;
    }
  }

  RigidBody CreateBody (const Shape& shape, float mass)
  {
    RigidBodyPtr body (scene->CreateRigidBody (ShapeImplProvider::LowLevelShape (shape), mass));
    Material     material (MaterialImplProvider::CreateMaterial (driver.get ()));

    return RigidBodyImplProvider::CreateRigidBody (body, shape, material);
  }

  ///�������� ������ ����� ������
  Joint CreateSphericalJoint (const JointBind& bind1, const JointBind& bind2)
  {
    RigidBodyArray bodies_array;

    bodies_array.push_back (bind1.body);
    bodies_array.push_back (bind2.body);

    physics::low_level::SphericalJointDesc joint_desc;

    joint_desc.anchor [0] = bind1.anchor;
    joint_desc.anchor [1] = bind2.anchor;

    JointPtr low_level_joint (scene->CreateSphericalJoint (RigidBodyImplProvider::LowLevelBody (bind1.body), RigidBodyImplProvider::LowLevelBody (bind2.body), joint_desc), false);

    return JointImplProvider::CreateJoint (low_level_joint, bodies_array);
  }

  Joint CreateConeTwistJoint (const JointBind& bind1, const JointBind& bind2, float swing_limit_in_degrees, float twist_limit)
  {
    RigidBodyArray bodies_array;

    bodies_array.push_back (bind1.body);
    bodies_array.push_back (bind2.body);

    physics::low_level::ConeTwistJointDesc joint_desc;

    joint_desc.anchor [0]  = bind1.anchor;
    joint_desc.anchor [1]  = bind2.anchor;
    joint_desc.swing_limit = swing_limit_in_degrees;
    joint_desc.twist_limit = twist_limit;

    JointPtr low_level_joint (scene->CreateConeTwistJoint (RigidBodyImplProvider::LowLevelBody (bind1.body), RigidBodyImplProvider::LowLevelBody (bind2.body), joint_desc), false);

    return JointImplProvider::CreateJoint (low_level_joint, bodies_array);
  }

  Joint CreateHingeJoint (const JointBind& bind1, const JointBind& bind2)
  {
    RigidBodyArray bodies_array;

    bodies_array.push_back (bind1.body);
    bodies_array.push_back (bind2.body);

    physics::low_level::HingeJointDesc joint_desc;

    joint_desc.anchor [0]  = bind1.anchor;
    joint_desc.anchor [1]  = bind2.anchor;
    joint_desc.axis [0]    = bind1.axis;
    joint_desc.axis [1]    = bind2.axis;

    JointPtr low_level_joint (scene->CreateHingeJoint (RigidBodyImplProvider::LowLevelBody (bind1.body), RigidBodyImplProvider::LowLevelBody (bind2.body), joint_desc), false);

    return JointImplProvider::CreateJoint (low_level_joint, bodies_array);
  }

  Joint CreatePrismaticJoint (const JointBind& bind1, const JointBind& bind2)
  {
    RigidBodyArray bodies_array;

    bodies_array.push_back (bind1.body);
    bodies_array.push_back (bind2.body);

    physics::low_level::PrismaticJointDesc joint_desc;

    joint_desc.anchor [0]  = bind1.anchor;
    joint_desc.anchor [1]  = bind2.anchor;
    joint_desc.axis [0]    = bind1.axis;
    joint_desc.axis [1]    = bind2.axis;

    JointPtr low_level_joint (scene->CreatePrismaticJoint (RigidBodyImplProvider::LowLevelBody (bind1.body), RigidBodyImplProvider::LowLevelBody (bind2.body), joint_desc), false);

    return JointImplProvider::CreateJoint (low_level_joint, bodies_array);
  }

  /*
    ���������� ������������ ��������. ������� ������������, ����:
     - �� ����� ������;
     - ����� ��������� ������ � collides = true;
     - �������� ������ ���������� true � collides = true;
     - �������� ������ ���������� false � collides = false.
    ��������� ����������: ��� ����� �������� ������, ��� ���� ��� ���������
  */

  size_t AddCollisionFilter (const char* group1_mask, const char* group2_mask, bool collides, const Scene::BroadphaseCollisionFilter& filter)
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::AddCollisionFilter");
  }

  void RemoveCollisionFilter (size_t id)
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::RemoveCollisionFilter");
  }

  void RemoveAllCollisionFilters ()
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::RemoveAllCollisionFilters");
  }

  /*
     ��������� ������������ ��������
  */

  xtl::connection RegisterCollisionCallback (const char* group1_mask, const char* group2_mask, CollisionEventType event_type, const CollisionCallback& callback_handler)
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::RegisterCollisionCallback");
  }

};

/*
   ����������� / ���������� / �����������
*/

Scene::Scene (Impl* source_impl)
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
  return impl->scene->Gravity ();
}

void Scene::SetGravity (const math::vec3f& value)
{
  impl->scene->SetGravity (value);
}

/*
   ��������� ���������
*/

float Scene::SimulationStep () const
{
  return impl->scene->SimulationStep ();
}

void Scene::SetSimulationStep (float step)
{
  impl->scene->SetSimulationStep (step);
}

/*
   ���������
*/
void Scene::PerformSimulation (float dt)
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
  impl->scene->Draw (render);
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

/*
   ��������
*/

Scene SceneImplProvider::CreateScene (physics::low_level::IDriver* driver, ScenePtr scene, const media::physics::PhysicsLibrary::RigidBodyCollection& body_collection)
{
  return Scene (new Scene::Impl (driver, scene, body_collection));
}
