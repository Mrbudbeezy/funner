#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    �������� ���������� Sprite
*/

struct Sprite::Impl
{
  stl::string material; //��� ���������
  size_t      frame;    //����� �����
};

/*
    ����������� / ����������
*/

Sprite::Sprite ()
  : impl (new Impl)
{
}

Sprite::~Sprite ()
{
}

/*
    �������� �������
*/

Sprite::Pointer Sprite::Create ()
{
  return Pointer (new Sprite, false);
}

/*
    ��������
*/

void Sprite::SetMaterial (const char* in_material)
{
  if (!in_material)
    throw xtl::make_null_argument_exception ("scene_graph::Sprite::SetMaterial", "material");

  impl->material = in_material;
}

const char* Sprite::Material () const
{
  return impl->material.c_str ();
}

/*
   ��������� ������ �����
*/

void Sprite::SetFrame (size_t frame)
{
  impl->frame = frame;
}

size_t Sprite::Frame () const
{
  return impl->frame;
}

/*
    �����, ���������� ��� ��������� �������
*/

void Sprite::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
