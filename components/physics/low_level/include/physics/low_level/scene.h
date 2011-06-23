#ifndef PHYSICS_LOW_LEVEL_SCENE_HEADER
#define PHYSICS_LOW_LEVEL_SCENE_HEADER

#include <xtl/functional_fwd>

#include <math/matrix.h>
#include <math/vector.h>

#include <physics/low_level/joints.h>

namespace render
{

namespace debug
{

///forward declaration
class PrimitiveRender;

}

}

namespace physics
{

namespace low_level
{

//forward declarations
class IRigidBody;
class IShape;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CollisionEventType
{
  CollisionEventType_Begin,    //������ ������������
  CollisionEventType_Process,  //� �������� ������������
  CollisionEventType_End,      //����� ������������

  CollisionEventType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ray-test
///////////////////////////////////////////////////////////////////////////////////////////////////
enum RayTestMode
{
  RayTestMode_Closest,    //��������� �����������
  RayTestMode_Farthest,   //����� ������� �����������
  RayTestMode_All,        //��� �����������

  RayTestMode_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct CollisionEvent
{
  CollisionEventType type;         //��� �������
  IRigidBody*        body [2];     //������������� ����
  math::vec3f        point;        //������� ���������� ����� ������������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class IScene : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const math::vec3f& Gravity    () = 0;
    virtual void               SetGravity (const math::vec3f& value) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual float SimulationStep    () = 0;
    virtual void  SetSimulationStep (float step) = 0;

    virtual void  PerformSimulation (float dt) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��� � ����� (����, ���������� ������� ������, �������� ������������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IRigidBody* CreateRigidBody (IShape* shape, float mass) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IJoint* CreateSphericalJoint (IRigidBody* body1, IRigidBody* body2, const SphericalJointDesc& desc) = 0;
    virtual IJoint* CreateConeTwistJoint (IRigidBody* body1, IRigidBody* body2, const ConeTwistJointDesc& desc) = 0;
    virtual IJoint* CreateHingeJoint     (IRigidBody* body1, IRigidBody* body2, const HingeJointDesc& desc) = 0;
    virtual IJoint* CreatePrismaticJoint (IRigidBody* body1, IRigidBody* body2, const PrismaticJointDesc& desc) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ �������� (������� ������������, ���� �� ����� ������, ���� ����� ���������
///������ � collides = true, ���� �������� ������ ���������� true � collides = true, � ���� ��������
///������ ���������� false � collides = false)
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<bool (IRigidBody*, IRigidBody*)> BroadphaseCollisionFilter;

    virtual void SetCollisionFilter (size_t group1, size_t group2, bool collides, const BroadphaseCollisionFilter& filter = BroadphaseCollisionFilter ()) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ ��������, ��� ������� �� ����� ������ ������������ �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetDefaultCollisionFilter (const BroadphaseCollisionFilter& filter = BroadphaseCollisionFilter ()) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const CollisionEvent& event)> CollisionCallback;

    virtual xtl::connection RegisterCollisionCallback (CollisionEventType event_type, const CollisionCallback& callback_handler) = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����, ������� ������ �� ������������� ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (IRigidBody* body, const math::vec3f& position, const math::vec3f& normal)> RayTestCallback;

    virtual void RayTest (const math::vec3f& ray_origin, const math::vec3f& ray_end, RayTestMode mode, const RayTestCallback& callback_handler) = 0;
    virtual void RayTest (const math::vec3f& ray_origin, const math::vec3f& ray_end, size_t collision_groups_count,
                          const size_t* collision_groups, RayTestMode mode, const RayTestCallback& callback_handler) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Draw (render::debug::PrimitiveRender&) = 0;
};

}

}

#endif
