#ifndef PHYSICS_LOW_LEVEL_TESTS_SHARED_HEADER
#define PHYSICS_LOW_LEVEL_TESTS_SHARED_HEADER

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <physics/low_level/common.h>
#include <physics/low_level/driver.h>
#include <physics/low_level/joints.h>
#include <physics/low_level/material.h>
#include <physics/low_level/rigid_body.h>
#include <physics/low_level/scene.h>
#include <physics/low_level/shape.h>

#ifdef _MSC_VER
  #pragma warning (disable : 4250) //'class1' : inherits 'class2::member' via dominance
#endif

using namespace physics::low_level;

class Object : virtual public IObject, public xtl::reference_counter
{
  public:
    ///������� ������
    void AddRef () { addref (this); }
    void Release () { release (this); }
};

class TestScene : public IScene, public Object
{
  public:
    TestScene () : gravity (0.0f, -9.8f, 0.0f), simulation_step (1.f / 60.f) {}

    ///���������� �����������
    const math::vec3f& Gravity () { return gravity; }

    void SetGravity (const math::vec3f& value) { gravity = value; }

    ///���������
    float SimulationStep () { return simulation_step; }

    void  SetSimulationStep (float step) { simulation_step = step; }

    void  PerformSimulation (float dt)
    {
      throw xtl::make_not_implemented_exception ("TestScene::PerformSimulation");
    }

    ///�������� ��� � �����
    IRigidBody* CreateRigidBody (IShape* shape, float mass)
    {
      throw xtl::make_not_implemented_exception ("TestScene::CreateRigidBody");
    }

    ///�������� ���������� ����� ������
    IJoint* CreateSphericalJoint (IRigidBody* body1, IRigidBody* body2, const SphericalJointDesc& desc)
    {
      throw xtl::make_not_implemented_exception ("TestScene::CreateSphericalJoint");
    }

    IJoint* CreateConeTwistJoint (IRigidBody* body1, IRigidBody* body2, const ConeTwistJointDesc& desc)
    {
      throw xtl::make_not_implemented_exception ("TestScene::CreateConeTwistJoint");
    }

    IJoint* CreateHingeJoint     (IRigidBody* body1, IRigidBody* body2, const HingeJointDesc& desc)
    {
      throw xtl::make_not_implemented_exception ("TestScene::CreateHingeJoint");
    }

    IJoint* CreatePrismaticJoint (IRigidBody* body1, IRigidBody* body2, const PrismaticJointDesc& desc)
    {
      throw xtl::make_not_implemented_exception ("TestScene::CreatePrismaticJoint");
    }

    ///���������� ������������ ��������
    void SetCollisionFilter (size_t group1, size_t group2, bool collides, const BroadphaseCollisionFilter& filter)
    {
      throw xtl::make_not_implemented_exception ("TestScene::SetCollisionFilter");
    }

    ///��������� ������������ ��������
    xtl::connection RegisterCollisionCallback (CollisionEventType event_type, const CollisionCallback& callback_handler)
    {
      throw xtl::make_not_implemented_exception ("TestScene::RegisterCallback");
    }

    ///���������� ���������
    void Draw (render::debug::PrimitiveRender&)
    {
      throw xtl::make_not_implemented_exception ("TestScene::Draw");
    }

  private:
    math::vec3f gravity;
    float       simulation_step;
};

//�������� �������
class TestDriver: public IDriver, public Object
{
  public:
    TestDriver () {}

    ///�������� ��������
    const char* GetDescription () { return "TestDriver"; }

    ///��������� �����
    IScene* CreateScene ()
    {
      return new TestScene ();
    }

    ///�������� �������������� ���
    IShape* CreateBoxShape (const math::vec3f& half_dimensions)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateBoxShape");
    }

    IShape* CreateSphereShape (float radius)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateSphereShape");
    }

    IShape* CreateCapsuleShape (float radius, float height)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateCapsuleShape");
    }

    IShape* CreatePlaneShape (const math::vec3f& normal, float d)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreatePlaneShape");
    }

    IShape* CreateConvexShape (size_t vertices_count, math::vec3f* vertices)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateConvexShape");
    }

    IShape* CreateTriangleMeshShape (size_t vertices_count, math::vec3f* vertices, size_t triangles_count, size_t* triangles)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateTriangleShape");
    }

    IShape* CreateCompoundShape (size_t shapes_count, IShape** shapes, Transform* local_transforms)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateCompoundShape");
    }
};

#endif
