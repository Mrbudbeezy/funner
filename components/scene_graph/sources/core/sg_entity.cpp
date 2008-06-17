#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;
using namespace xtl;

const float INFINITE_BOUND_VALUE = 1e8f; //�������� ������������� ��� �������������� �������

/*
    �������� ���������� Entity
*/

struct Entity::Impl: public SceneObject
{
  vec3f  color;                    //���� �������
  aaboxf local_bound_box;          //�������������� ��������������� � ��������� ������� ���������
  aaboxf world_bound_box;          //�������������� ��������������� � ������� ������� ���������
  bool   need_local_bounds_update; //��������� �������������� ������ ������� ���������
  bool   need_world_bounds_update; //������� �������������� ������ ������� ���������
  bool   infinite_bounds;          //�������� �� �������������� ������ ���� ������������
  
  Impl (scene_graph::Entity& entity) : SceneObject (entity) {}
};

/*
    ����������� / ����������
*/

Entity::Entity ()
  : impl (new Impl (*this))
{
  impl->need_local_bounds_update = false;
  impl->need_world_bounds_update = false;
  impl->infinite_bounds          = true;
  impl->local_bound_box          = impl->world_bound_box = aaboxf (vec3f (-INFINITE_BOUND_VALUE), vec3f (INFINITE_BOUND_VALUE));
}
  
Entity::~Entity ()
{
  delete impl;
}

/*
    ���� ����
*/

void Entity::SetColor (const vec3f& color)
{
  impl->color = color;
  
  UpdateNotify ();
}

void Entity::SetColor (float red, float green, float blue)
{
  SetColor (vec3f (red, green, blue));
}

const vec3f& Entity::Color () const
{
  return impl->color;
}

/*
    ������ � ��������������� ��������
*/

//���������� �� ���������� ��������� �������������� �������
void Entity::UpdateBoundsNotify ()
{
  impl->need_local_bounds_update = true;
}

//���������� �� ���������� ������� �������������� �������
void Entity::UpdateWorldBoundsNotify ()
{
  impl->need_world_bounds_update = true;
}

//�������� ������� �������������� �������
void Entity::UpdateWorldBounds () const
{
  if (impl->infinite_bounds) impl->world_bound_box = impl->local_bound_box;
  else                       impl->world_bound_box = impl->local_bound_box * WorldTM ();

  impl->need_world_bounds_update = false;
}

//��������� ��������������� ������
void Entity::SetBoundBox (const bound_volumes::aaboxf& box)
{
  impl->infinite_bounds = false;
  impl->local_bound_box = box;
  
  UpdateWorldBoundsNotify ();
  UpdateNotify ();
}

//�������������� ����� ���� � ��������� ������� ���������
const aaboxf& Entity::BoundBox () const
{
  if (impl->need_local_bounds_update)
  {
    const_cast <Entity&> (*this).UpdateBoundsCore ();
    impl->need_local_bounds_update = false;
  }

  return impl->local_bound_box;
}

//�������������� ����� ���� � ������� ������� ���������
const aaboxf& Entity::WorldBoundBox () const
{
  if (impl->need_world_bounds_update)
    UpdateWorldBounds ();

  return impl->world_bound_box;
}

//��������� ����������� �������������� �������
void Entity::SetInfiniteBounds ()
{
  if (impl->infinite_bounds) //���������� ��������� ��������� ����������� �������������� �������
    return;

  impl->infinite_bounds = true;
  impl->local_bound_box = aaboxf (vec3f (-INFINITE_BOUND_VALUE), vec3f (INFINITE_BOUND_VALUE));
  
  UpdateWorldBoundsNotify ();
  UpdateNotify ();
}

//���������� ���������� ��������������� ������
void Entity::UpdateBoundsCore ()
{
}

//�������� �� �������������� ������ ���� ������������
bool Entity::IsInfiniteBounds () const
{
  if (impl->need_local_bounds_update)
  {
    const_cast <Entity&> (*this).UpdateBoundsCore ();
    impl->need_local_bounds_update = false;
  }

  return impl->infinite_bounds;
}

//�������������� ����� �������� � ��������� ������� ��������� ����
aaboxf Entity::ChildrenBoundBox () const
{
  return WorldChildrenBoundBox () * invert (WorldTM ());
}

//������ �������������� ����� ���� � ��������� � ��������� ������� ��������� ����
aaboxf Entity::FullBoundBox () const
{
  return WorldFullBoundBox () * invert (WorldTM ());
}

//�������������� ����� �������� � ������� ������� ���������
aaboxf Entity::WorldChildrenBoundBox () const
{
  struct EntityVisitor: public visitor<void, Entity>
  {
    EntityVisitor () : initialized (false) {}
    
    void visit (Entity& entity)
    {
      if (initialized)
      {
        box += entity.WorldBoundBox ();
        
        return;
      }

      initialized = true;

      box = entity.WorldBoundBox ();
    }

    aaboxf box;
    bool   initialized;
  };

  EntityVisitor visitor;

  VisitEach (visitor);
  
  return visitor.box;
}

//������ �������������� ����� ���� � ��������� � ������� ������� ���������
aaboxf Entity::WorldFullBoundBox () const
{
  return WorldChildrenBoundBox () += WorldBoundBox ();
}

/*
    ��������� ���������� �� ��������� �������� ��������� ����
*/

void Entity::AfterUpdateWorldTransformEvent ()
{
  UpdateWorldBoundsNotify ();
}

/*
    ��������� ���������� �� �������������/������������ ������� � �����
*/

void Entity::AfterSceneAttachEvent ()
{
  Scene ()->Attach (*impl);
}

void Entity::BeforeSceneDetachEvent ()
{
  impl->BindToSpace (0);
}

/*
    �����, ���������� ��� ��������� �������
*/

void Entity::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Node::AcceptCore (visitor);
}
