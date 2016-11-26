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
    ///Подсчёт ссылок
    void AddRef () { addref (this); }
    void Release () { release (this); }
};

class TestScene : public IScene, public Object
{
  public:
    TestScene () : gravity (0.0f, -9.8f, 0.0f), simulation_step (1.f / 60.f) {}

    ///Управление гравитацией
    const math::vec3f& Gravity () { return gravity; }

    void SetGravity (const math::vec3f& value) { gravity = value; }

    ///Симуляция
    float SimulationStep () { return simulation_step; }

    void  SetSimulationStep (float step) { simulation_step = step; }

    void  PerformSimulation (float dt)
    {
      throw xtl::make_not_implemented_exception ("TestScene::PerformSimulation");
    }

    ///Создание тел в сцене
    IRigidBody* CreateRigidBody (IShape* shape, float mass)
    {
      throw xtl::make_not_implemented_exception ("TestScene::CreateRigidBody");
    }

    ///Создание соединений между телами
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

    ///Фильтрация столкновений объектов
    void SetCollisionFilter (size_t group1, size_t group2, bool collides, const BroadphaseCollisionFilter& filter)
    {
      throw xtl::make_not_implemented_exception ("TestScene::SetCollisionFilter");
    }

    void SetDefaultCollisionFilter (const BroadphaseCollisionFilter& filter)
    {
      throw xtl::make_not_implemented_exception ("TestScene::SetDefaultCollisionFilter");
    }

    ///Обработка столкновений объектов
    xtl::connection RegisterCollisionCallback (CollisionEventType event_type, const CollisionCallback& callback_handler)
    {
      throw xtl::make_not_implemented_exception ("TestScene::RegisterCallback");
    }

    ///Трассировка луча, порядок вызова не соответствует удаленности объекта
    void RayTest (const math::vec3f& ray_origin, const math::vec3f& ray_end, RayTestMode mode, const RayTestCallback& callback_handler)
    {
      throw xtl::make_not_implemented_exception ("TestScene::RayTest");
    }

    void RayTest (const math::vec3f& ray_origin, const math::vec3f& ray_end, size_t collision_groups_count,
                  const size_t* collision_groups, RayTestMode mode, const RayTestCallback& callback_handler)
    {
      throw xtl::make_not_implemented_exception ("TestScene::RayTest");
    }

    ///Отладочная отрисовка
    void Draw (render::debug::PrimitiveRender&)
    {
      throw xtl::make_not_implemented_exception ("TestScene::Draw");
    }

  private:
    math::vec3f gravity;
    float       simulation_step;
};

//тестовый драйвер
class TestDriver: public IDriver, public Object
{
  public:
    TestDriver () {}

    ///Описание драйвера
    const char* GetDescription () { return "TestDriver"; }

    ///Создаение сцены
    IScene* CreateScene ()
    {
      return new TestScene ();
    }

    ///Создание материала
    IMaterial* CreateMaterial ()
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateMaterial");
    }

    ///Создание геометрических тел
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

    IShape* CreateConvexShape (unsigned int vertices_count, const math::vec3f* vertices)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateConvexShape");
    }

    IShape* CreateTriangleMeshShape (unsigned int vertices_count, const math::vec3f* vertices, unsigned int triangles_count, unsigned int* triangles)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateTriangleShape");
    }

    IShape* CreateCompoundShape (unsigned int shapes_count, IShape** shapes, Transform* local_transforms)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateCompoundShape");
    }
};

#endif
