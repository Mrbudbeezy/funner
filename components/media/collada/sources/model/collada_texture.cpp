#include "shared.h"

using namespace media::collada;
using namespace common;

/*
    Реализация текстуры
*/

struct Texture::Impl: public xtl::reference_counter
{
  float       amount;     //вес текстуры
  stl::string image_name; //имя картинки
  stl::string texcoord;   //имя канала текстурных координат
  math::mat4f transform;  //матрица преобразований текстурных координат
};

/*
    Конструкторы / деструктор / присваивание
*/

Texture::Texture ()
  : impl (new Impl, false)
{
  impl->amount = 1.0f;
}

Texture::Texture (Impl* in_impl)
  : impl (in_impl, false)
{
}

Texture::Texture (const Texture& texture)
  : impl (texture.impl)
  {}

Texture::~Texture ()
{
}

Texture& Texture::operator = (const Texture& texture)
{
  impl = texture.impl;

  return *this;
}

/*
    Создание копии
*/

Texture Texture::Clone () const
{
  return Texture (new Impl (*impl));
}

/*
    Имя картинки связанной с текстурой
*/

const char* Texture::Image () const
{
  return impl->image_name.c_str ();
}

void Texture::SetImage (const char* image_name)
{
  if (!image_name)
    throw xtl::make_null_argument_exception ("media::collada::Texture::SetImage", "image_name");
    
  impl->image_name = image_name;
}

/*
    Имя канала текстурных координат
*/

const char* Texture::TexcoordChannel () const
{
  return impl->texcoord.c_str ();
}

void Texture::SetTexcoordChannel (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::collada::Texture::SetTexcoordChannel", "name");
    
  impl->texcoord = name;
}

/*
    Вес текстуры
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
    Матрица преобразования текстурных координат
*/

const math::mat4f& Texture::Transform () const
{
  return impl->transform;
}

void Texture::SetTransform (const math::mat4f& tm)
{
  impl->transform = tm;
}
