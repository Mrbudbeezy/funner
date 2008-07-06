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
  float       alpha;    //������������
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
  
  UpdateNotify ();
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
  
  UpdateNotify ();
}

size_t Sprite::Frame () const
{
  return impl->frame;
}

/*
    ���������� �������������
*/

void Sprite::SetAlpha (float alpha)
{
  if (alpha < 0.0f || alpha > 1.0f)
    throw xtl::make_range_exception ("scene_graph::Sprite::SetAlpha", "alpha", alpha, 0.0f, 1.0f);
    
  impl->alpha = alpha;
  
  UpdateNotify ();
}

float Sprite::Alpha () const
{
  return impl->alpha;
}

/*
    �����, ���������� ��� ��������� �������
*/

void Sprite::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
