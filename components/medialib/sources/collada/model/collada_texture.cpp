#include <media/collada/material.h>
#include <stl/string>
#include <common/exception.h>

using namespace media::collada;
using namespace common;

/*
    ���������� ��������
*/

struct Texture::Impl
{
  float       amount;     //��� ��������
  stl::string image_name; //��� ��������
  stl::string texcoord;   //��� ������ ���������� ���������
  math::mat4f transform;  //������� �������������� ���������� ���������
};

/*
    ����������� / ����������
*/

Texture::Texture  ()
  : impl (new Impl)
{
  impl->amount = 1.0f;
}

Texture::~Texture ()
{
  delete impl;
}

/*
    ��� �������� ��������� � ���������
*/

const char* Texture::Image () const
{
  return impl->image_name.c_str ();
}

void Texture::SetImage (const char* image_name)
{
  if (!image_name)
    RaiseNullArgument ("media::collada::Texture::SetImage", "image_name");
    
  impl->image_name = image_name;
}

/*
    ��� ������ ���������� ���������
*/

const char* Texture::TexcoordChannel () const
{
  return impl->texcoord.c_str ();
}

void Texture::SetTexcoordChannel (const char* name)
{
  if (!name)
    RaiseNullArgument ("media::collada::Texture::SetTexcoordChannel", "name");
    
  impl->texcoord = name;
}

/*
    ��� ��������
*/

float Texture::Amount () const
{
  return impl->amount;
}

void Texture::SetAmount (float amount) const
{
  impl->amount = amount;
}

/*
    ������� �������������� ���������� ���������
*/

const math::mat4f& Texture::Transform () const
{
  return impl->transform;
}

void Texture::SetTransform (const math::mat4f& tm)
{
  impl->transform = tm;
}
