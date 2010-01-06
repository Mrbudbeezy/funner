#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;
using namespace common;

namespace
{

/*
    ���������
*/

const char*  DRIVER_NAME    = "Bullet";                   //��� ��������
const char*  COMPONENT_NAME = "physics.low_level.bullet"; //��� ����������

}

/*
    �������� ���������� �������� ���������� ������� �� ������ ���������� Bullet
*/

struct Driver::Impl
{
};

/*
    �����������
*/

Driver::Driver ()
  : impl (new Impl)
  {}

/*
    �������� ��������
*/

const char* Driver::GetDescription ()
{
  return "physics::low_level::bullet::Driver";
}

/*
   �������� �����
*/

Scene* Driver::CreateScene ()
{
  return new Scene ();
}

/*
   �������� �������������� ���
*/

Shape* Driver::CreateBoxShape (const math::vec3f& half_dimensions)
{
  btVector3 half_extents (half_dimensions.x, half_dimensions.y, half_dimensions.z);

  return new Shape (new btBoxShape (half_extents));
}

Shape* Driver::CreateSphereShape (float radius)
{
  return new Shape (new btSphereShape (radius));
}

Shape* Driver::CreateCapsuleShape (float radius, float height)
{
  return new Shape (new btCapsuleShape (radius, height));
}

Shape* Driver::CreatePlaneShape (const math::vec3f& normal, float d)
{
  btVector3 plane_normal (normal.x, normal.y, normal.z);

  return new Shape (new btStaticPlaneShape (plane_normal, d));
}

Shape* Driver::CreateConvexShape (size_t vertices_count, math::vec3f* vertices)
{
  return new Shape (0);
}

Shape* Driver::CreateTriangleMeshShape (size_t vertices_count, math::vec3f* vertices, size_t triangles_count, int* triangles)
{
  return new Shape (0);
}

Shape* Driver::CreateCompoundShape (size_t shapes_count, IShape* shapes, Transform* local_transforms)
{
  return new Shape (0);
}

/*
    ��������� ��������
*/

namespace
{

class BulletDriverComponent
{
  public:
    BulletDriverComponent ()
    {
      DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

}

extern "C"
{

common::ComponentRegistrator<BulletDriverComponent> BulletDriver (COMPONENT_NAME);

}
