#ifndef PHYSICS_BULLET_DRIVER_SHARED_HEADER
#define PHYSICS_BULLET_DRIVER_SHARED_HEADER

#include <stl/auto_ptr.h>
#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>

#include <common/component.h>

#include <physics/low_level/common.h>
#include <physics/low_level/driver.h>
#include <physics/low_level/joints.h>
#include <physics/low_level/material.h>
#include <physics/low_level/rigid_body.h>
#include <physics/low_level/scene.h>
#include <physics/low_level/shape.h>

#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletDynamics/ConstraintSolver/btConeTwistConstraint.h>
#include <BulletDynamics/ConstraintSolver/btHingeConstraint.h>
#include <BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/ConstraintSolver/btSliderConstraint.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btDefaultMotionState.h>

#ifdef _MSC_VER
  #pragma warning (disable : 4250) //'class1' : inherits 'class2::member' via dominance
#endif

namespace physics
{

namespace low_level
{

namespace bullet
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ ��������� �������������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Object: virtual public IObject, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Object () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    Object (const Object&); //no impl
    Object& operator = (const Object&); //no impl
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Shape : public IShape, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Shape  (btCollisionShape* shape);
    ~Shape ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Margin    ();
    void  GetMargin (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� bullet ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    btCollisionShape* BulletCollisionShape ();

  private:
    btCollisionShape* collision_shape;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ���� �������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class TriangleMeshShape : public Shape
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TriangleMeshShape (btCollisionShape* shape, btStridingMeshInterface* in_mesh) : Shape (shape), mesh (in_mesh) {}
    ~TriangleMeshShape () { delete mesh; }

  private:
    btStridingMeshInterface* mesh;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Material : public IMaterial, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Material ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������/������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float LinearDamping     ();
    float AngularDamping    ();
    void  SetLinearDamping  (float value);
    void  SetAngularDamping (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float              Friction               ();
    const math::vec3f& AnisotropicFriction    ();
    void               SetFriction            (float value);
    void               SetAnisotropicFriction (const math::vec3f& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Restitution    ();
    void  SetRestitution (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function <void ()> UpdateHandler;

    xtl::connection RegisterUpdateHandler (const UpdateHandler& handler);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class RigidBody : public IRigidBody, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RigidBody  (bullet::Shape* shape, float mass);
    ~RigidBody ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bullet::Shape* Shape ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IMaterial* Material    ();
    void       SetMaterial (IMaterial* material);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Flags    ();
    void   SetFlags (size_t flags);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����/��������/��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddForce   (const math::vec3f& force,   const math::vec3f& relative_position);
    void AddImpulse (const math::vec3f& impulse, const math::vec3f& relative_position);
    void AddTorque  (const math::vec3f& torque);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������/������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f& LinearVelocity     ();
    const math::vec3f& AngularVelocity    ();
    void               SetLinearVelocity  (const math::vec3f& velocity);
    void               SetAngularVelocity (const math::vec3f& velocity);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Mass    ();
    void  SetMass (float mass);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f&  MassSpaceInertiaTensor    ();
    void                SetMassSpaceInertiaTensor (const math::vec3f& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    float SleepLinearVelocity     ();
    float SleepAngularVelocity    ();
    void  SetSleepLinearVelocity  (float value);
    void  SetSleepAngularVelocity (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ��������� �������� �������� � ����� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float CcdMotionThreshold    ();
    void  SetCcdMotionThreshold (float);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ������� � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t CollisionGroup    ();
    void   SetCollisionGroup (size_t group_number);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Transform& WorldTransform    ();
    void             SetWorldTransform (const Transform& transform);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� bullet ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    btRigidBody* BulletRigidBody ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function <void (RigidBody*)> BeforeDestroyHandler;

    xtl::connection RegisterDestroyHandler (const BeforeDestroyHandler& handler);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
class Joint : public IJoint, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Joint (RigidBody* body1, RigidBody* body2, btTypedConstraint* joint);
    ~Joint ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      ObjectsCount ();
    IRigidBody* GetObject    (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� bullet ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    btTypedConstraint* BulletJoint ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function <void (Joint*)> BeforeDestroyHandler;

    xtl::connection RegisterDestroyHandler (const BeforeDestroyHandler& handler);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Scene : public IScene, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Scene ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f& Gravity    ();
    void               SetGravity (const math::vec3f& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float SimulationStep    ();
    void  SetSimulationStep (float step);

    void  PerformSimulation (float dt);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��� � ����� (����, ���������� ������� ������, �������� ������������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    RigidBody* CreateRigidBody (IShape* shape, float mass);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Joint* CreateSphericalJoint (IRigidBody* body1, IRigidBody* body2, const SphericalJointDesc& desc);
    Joint* CreateConeTwistJoint (IRigidBody* body1, IRigidBody* body2, const ConeTwistJointDesc& desc);
    Joint* CreateHingeJoint     (IRigidBody* body1, IRigidBody* body2, const HingeJointDesc& desc);
    Joint* CreatePrismaticJoint (IRigidBody* body1, IRigidBody* body2, const PrismaticJointDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetCollisionFilter (size_t group1, size_t group2, bool collides, const BroadphaseCollisionFilter& filter);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterCollisionCallback (CollisionEventType event_type, const CollisionCallback& callback_handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ColissionNotify (const CollisionEvent& event);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver : public IDriver, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Driver ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Scene* CreateScene ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
    Shape* CreateBoxShape          (const math::vec3f& half_dimensions);
    Shape* CreateSphereShape       (float radius);
    Shape* CreateCapsuleShape      (float radius, float height);
    Shape* CreatePlaneShape        (const math::vec3f& normal, float d);
    Shape* CreateConvexShape       (size_t vertices_count, math::vec3f* vertices);
    Shape* CreateTriangleMeshShape (size_t vertices_count, math::vec3f* vertices, size_t triangles_count, size_t* triangles);
    Shape* CreateCompoundShape     (size_t shapes_count, IShape** shapes, Transform* local_transforms);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

//�������� � btRigidBody
struct RigidBodyInfo
{
  RigidBody* body;
  Scene*     scene;
};

//�������������� ��������
void vector_from_bullet_vector (const btVector3& bullet_vector, math::vec3f& target_vector);
void bullet_vector_from_vector (const math::vec3f& vector, btVector3& target_vector);

//�������������� ������������
void quaternion_from_bullet_quaternion (const btQuaternion& bullet_quaternion, math::quatf& target_quaternion);
void bullet_quaternion_from_quaternion (const math::quatf& quaternion, btQuaternion& target_quaternion);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class DstT, class SrcT>
DstT* cast_object (SrcT* ptr, const char* source, const char* argument_name);

#include "detail/object.inl"

}

}

}

#endif
