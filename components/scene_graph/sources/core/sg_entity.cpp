#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;
using namespace xtl;

namespace
{

/*
    ���������
*/

const float  INFINITE_BOUND_VALUE       = 1e8f; //�������� ������������� ��� �������������� �������
const size_t INTERSECTIONS_RESERVE_SIZE = 16;   //������������� ����� �����������

}

/*
    �������� ���������� Entity
*/

struct Entity::Impl: public SceneObject, public xtl::instance_counter<Entity>
{
  vec3f  wire_color;               //���� ������������ ������������� �������
  aaboxf local_bound_box;          //�������������� ��������������� � ��������� ������� ���������
  aaboxf world_bound_box;          //�������������� ��������������� � ������� ������� ���������
  bool   need_local_bounds_update; //��������� �������������� ������ ������� ���������
  bool   need_world_bounds_update; //������� �������������� ������ ������� ���������
  bool   infinite_bounds;          //�������� �� �������������� ������ ���� ������������
  bool   visible;                  //�������� �� ������ �������
  
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
  impl->visible                  = true;
}
  
Entity::~Entity ()
{
  delete impl;
}

/*
    ���� ������������ ������������� �������
*/

void Entity::SetWireColor (const vec3f& color)
{
  impl->wire_color = color;
  
  UpdateNotify ();
}

void Entity::SetWireColor (float red, float green, float blue)
{
  SetWireColor (vec3f (red, green, blue));
}

const vec3f& Entity::WireColor () const
{
  return impl->wire_color;
}

/*
    ��������� �������
*/

void Entity::SetVisible (bool state)
{
  impl->visible = state;
  
  UpdateNotify ();
}

bool Entity::IsVisible () const
{
  return impl->visible;
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
  return WorldChildrenBoundBox () * inverse (WorldTM ());
}

//������ �������������� ����� ���� � ��������� � ��������� ������� ��������� ����
aaboxf Entity::FullBoundBox () const
{
  return WorldFullBoundBox () * inverse (WorldTM ());
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
    ��������� �����������
*/

namespace
{

//������� ��� ���������� ������� � ������
struct ArrayInserter: public INodeTraverser
{
  ArrayInserter (const Entity& in_self, NodeArray& in_array) : self (in_self), array (in_array) {}

  void operator () (Node& entity)
  {
    if (&entity == &self) //������ �� ���������������
      return;

    array.Add (entity);
  }

  const Entity& self;
  NodeArray&    array;
};

}

void Entity::GetIntersections (NodeArray& intersections) const
{
  intersections.Clear ();

  const scene_graph::Scene* scene = Scene ();
  
  if (!scene)
    return;

  ArrayInserter inserter (*this, intersections);

  scene->Traverse (WorldBoundBox (), inserter);
}

NodeArray Entity::GetIntersections () const
{
  NodeArray intersections;

  intersections.Reserve (INTERSECTIONS_RESERVE_SIZE);

  GetIntersections (intersections);

  return intersections;
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

/*
    ���������� �������
*/

void Entity::BindProperties (common::PropertyBindingMap& bindings)
{
  Node::BindProperties (bindings);

  bindings.AddProperty ("InfiniteBounds", xtl::bind (&Entity::IsInfiniteBounds, this));
  bindings.AddProperty ("WireColor", xtl::bind (&Entity::WireColor, this), xtl::bind (xtl::implicit_cast<void (Entity::*)(const math::vec3f&)> (&Entity::SetWireColor), this, _1));
  bindings.AddProperty ("Visible", xtl::bind (&Entity::IsVisible, this), xtl::bind (&Entity::SetVisible, this, _1));  
}
