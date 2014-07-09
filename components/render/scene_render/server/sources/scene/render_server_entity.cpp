#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    �������� ���������� ��������
*/

struct Entity::Impl
{
  bool                  is_infinite; //���������� �� �������������� �����
  bound_volumes::aaboxf bound_box;   //�������������� ����
  bool                  is_visible;  //�������� �� ������ �������� �������
  Scene*                scene;       //�����

///�����������
  Impl () : is_infinite (true), is_visible (true), scene () {}
};

/*
    ������������ / ���������� / ������������
*/

Entity::Entity ()
  : impl (new Impl)
{
}

Entity::~Entity ()
{
  SetSceneOwner (0);
}

/*
    �����
*/

void Entity::SetSceneOwner (Scene* scene)
{
  try
  {
    if (scene == impl->scene)
      return;

    if (impl->scene)
      impl->scene->DetachNode (this);

    impl->scene = 0;

    if (scene)
    {
      scene->AttachNode (this);

      impl->scene = scene;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Node::SetSceneOwner");
    throw;
  }
}

Scene* Entity::SceneOwner () const
{
  return impl->scene;
}

/*
    �������������� ����
*/

void Entity::SetBounds (bool is_infinite, const bound_volumes::aaboxf& box)
{
  impl->is_infinite = true;
  impl->bound_box   = box;

  OnUpdateBounds ();
}

bool Entity::IsInfiniteBounds () const
{
  return impl->is_infinite;
}

const bound_volumes::aaboxf& Entity::BoundBox () const
{
  return impl->bound_box;
}

/*
    ��������� �������
*/

void Entity::SetVisible (bool state)
{
  impl->is_visible = state;
}

bool Entity::IsVisible () const
{
  return impl->is_visible;
}

/*
    ���������� �� ���������� ��������������� ����
*/

void Entity::OnUpdateBounds ()
{
}

/*
    �����
*/

void Entity::Visit (ISceneVisitor& visitor)
{
  try
  {
    VisitCore (visitor);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Entity::Visit(%s='%s')", typeid (*this).name (), Name ());
    throw;
  }
}

void Entity::VisitCore (ISceneVisitor& visitor)
{
  visitor.Visit (*this);
}
