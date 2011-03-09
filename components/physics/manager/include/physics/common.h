#ifndef PHYSICS_MANAGER_COMMON_HEADER
#define PHYSICS_MANAGER_COMMON_HEADER

#include <math/quat.h>
#include <math/vector.h>

namespace physics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Transform
{
  math::vec3f position;
  math::quatf orientation;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
  Transform ();
  Transform (const math::vec3f& position, const math::quatf& orientation = math::quatf ());
  Transform (const math::quatf& orientation);
};

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

class RigidBody;

typedef xtl::function<void (CollisionEventType event, RigidBody& first_body, RigidBody& second_body, const math::vec3f& collision_point)> CollisionCallback;

#include <physics/detail/common.inl>

}

#endif
