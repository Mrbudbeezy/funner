#include "shared.h"

using namespace scene_graph;

/*
    �������� ���������� ���������� ������
*/

struct SpriteModel::Impl
{
  stl::string material; //��� ���������
};

/*
    ����������� / ����������
*/

SpriteModel::SpriteModel ()
  : impl (new Impl)
{
}

SpriteModel::~SpriteModel ()
{
}

/*
    ��������
*/

void SpriteModel::SetMaterial (const char* in_material)
{
  if (!in_material)
    throw xtl::make_null_argument_exception ("scene_graph::SpriteModel::SetMaterial", "material");

  impl->material = in_material;
  
  UpdateNotify ();
}

const char* SpriteModel::Material () const
{
  return impl->material.c_str ();
}

/*
    ���������� �������� / ��������� ������� ��������
*/

size_t SpriteModel::SpritesCount () const
{
  return const_cast<SpriteModel&> (*this).SpritesCountCore ();
}

const SpriteModel::SpriteDesc* SpriteModel::Sprites () const
{
  return const_cast<SpriteModel&> (*this).SpritesCore ();
}

/*
    ������������ ���������������
*/

void SpriteModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
