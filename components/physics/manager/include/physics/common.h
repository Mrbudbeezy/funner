#ifndef PHYSICS_MANAGER_COMMON_HEADER
#define PHYSICS_MANAGER_COMMON_HEADER

namespace physics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Transform
{
  math::vec3f position;
  math::quatf orientation;
  
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

}

#endif
