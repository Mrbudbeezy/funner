#ifndef PHYSICS_MANAGER_SHARED_HEADER
#define PHYSICS_MANAGER_SHARED_HEADER

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/reference_counter.h>

#include <stl/vector>

#include <media/physics/basic_shapes.h>
#include <media/physics/compound.h>
#include <media/physics/physics_library.h>

#include <physics/low_level/driver.h>
#include <physics/low_level/joints.h>
#include <physics/low_level/material.h>
#include <physics/low_level/rigid_body.h>
#include <physics/low_level/scene.h>
#include <physics/low_level/shape.h>

#include <physics/manager.h>

namespace physics
{

typedef xtl::com_ptr<physics::low_level::IJoint>     JointPtr;
typedef xtl::com_ptr<physics::low_level::IRigidBody> RigidBodyPtr;
typedef xtl::com_ptr<physics::low_level::IScene>     ScenePtr;
typedef xtl::com_ptr<physics::low_level::IShape>     ShapePtr;
typedef stl::vector<RigidBody>                       RigidBodyArray;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MaterialImplProvider
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Material CreateMaterial (physics::low_level::IDriver* driver);
    static Material CreateMaterial (physics::low_level::IDriver* driver, const media::physics::Material& material);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static physics::low_level::IMaterial* LowLevelMaterial (const Material& material);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������� ������ ���������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
class JointImplProvider
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Joint CreateJoint (JointPtr joint, const RigidBodyArray& bodies);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������� ������ ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class SceneImplProvider
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Scene CreateScene (physics::low_level::IDriver* driver, ScenePtr scene, const media::physics::PhysicsLibrary::RigidBodyCollection& body_collection);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������� ������ ��������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShapeImplProvider
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Shape CreateShape (ShapePtr shape);
    static Shape CreateShape (ShapePtr shape, const ShapeList& shape_list);
    static Shape CreateShape (physics::low_level::IDriver* driver, const media::physics::Shape& shape);
    static Shape CreateShape (physics::low_level::IDriver* driver, const ShapeList& shape_list);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static physics::low_level::IShape* LowLevelShape (const Shape& shape);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Shape CreateShapeCore     (physics::low_level::IDriver* driver, const media::physics::Shape& shape);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������� ������ �������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class RigidBodyImplProvider
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static RigidBody CreateRigidBody (RigidBodyPtr body, const Shape& shape, const Material& material);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static physics::low_level::IRigidBody* LowLevelBody (const RigidBody& body);
};

}

#endif
