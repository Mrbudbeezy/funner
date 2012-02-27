#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    �������� ���������� Sprite
*/

struct Sprite::Impl
{
  SpriteDesc sprite_desc; //�������� �������
  
  Impl ()
  {
    sprite_desc.frame = 0;
    sprite_desc.size  = vec2f (1.0f);
    sprite_desc.color = vec4f (1.0f, 1.0f, 1.0f, 1.0f);
  }
};

/*
    ����������� / ����������
*/

Sprite::Sprite ()
  : impl (new Impl)
{
  SetBoundBox (bound_volumes::axis_aligned_box <float> (-0.5f, -0.5f, 0, 0.5f, 0.5f, 0));
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
   ��������� ������ �����
*/

void Sprite::SetFrame (size_t frame)
{
  impl->sprite_desc.frame = frame;
  
  UpdateSpriteDescsNotify ();
}

size_t Sprite::Frame () const
{
  return impl->sprite_desc.frame;
}

/*
    ���� �������
*/

namespace
{

float clamp (float x)
{
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;
  
  return x;
}

vec4f clamp (const vec4f& color)
{
  return vec4f (clamp (color.x), clamp (color.y), clamp (color.z), clamp (color.w));
}

}

void Sprite::SetColor (const vec4f& color)
{
  impl->sprite_desc.color = clamp (color);

  UpdateSpriteDescsNotify ();
}

void Sprite::SetColor (float red, float green, float blue, float alpha)
{
  SetColor (vec4f (red, green, blue, alpha));
}

void Sprite::SetColor (float red, float green, float blue)
{
  SetColor (vec4f (red, green, blue, impl->sprite_desc.color.w));
}

void Sprite::SetAlpha (float alpha)
{
  impl->sprite_desc.color.w = clamp (alpha);

  UpdateSpriteDescsNotify ();
}

const vec4f& Sprite::Color () const
{
  return impl->sprite_desc.color;
}

float Sprite::Alpha () const
{
  return impl->sprite_desc.color.w;
}

/*
    �����, ���������� ��� ��������� �������
*/

void Sprite::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    SpriteModel::AcceptCore (visitor);
}

/*
    ���������� ��������� ���������� �������� � ������� ��������
*/

size_t Sprite::SpriteDescsCountCore ()
{
  return 1;
}

const SpriteModel::SpriteDesc* Sprite::SpriteDescsCore ()
{
  return &impl->sprite_desc;
}

/*
    ���������� �������
*/

void Sprite::BindProperties (common::PropertyBindingMap& bindings)
{
  SpriteModel::BindProperties (bindings);

  bindings.AddProperty ("Color", xtl::bind (&Sprite::Color, this), xtl::bind (xtl::implicit_cast<void (Sprite::*)(const math::vec4f&)> (&Sprite::SetColor), this, _1));
  bindings.AddProperty ("Alpha", xtl::bind (&Sprite::Alpha, this), xtl::bind (&Sprite::SetAlpha, this, _1));
  bindings.AddProperty ("Frame", xtl::bind (&Sprite::Frame, this), xtl::bind (&Sprite::SetFrame, this, _1));
}
