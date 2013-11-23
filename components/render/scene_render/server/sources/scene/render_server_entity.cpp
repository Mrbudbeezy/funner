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

///�����������
  Impl () : is_infinite (true) {}
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
}

/*
    �������������� ����
*/

void Entity::SetBounds (bool is_infinite, const bound_volumes::aaboxf& box)
{
  impl->is_infinite = true;
  impl->bound_box   = box;
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
    �����
*/

void Entity::VisitCore (ISceneVisitor& visitor)
{
  visitor.Visit (*this);
}
