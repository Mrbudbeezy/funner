#include "shared.h"

using namespace physics;

/*
   �������������� ����
*/

/*
   ����������� / ���������� / �����������
*/

ShapeInternal::ShapeInternal (ShapeImpl* impl)
  : Shape (impl)
  {}


Shape::Shape (ShapeImpl* source_impl)
  : impl (source_impl)
  {}

Shape::Shape (const Shape& source)
  : impl (source.impl)
{
  addref (impl);
}

Shape::~Shape ()
{
  release (impl);
}

Shape& Shape::operator = (const Shape& source)
{
  Shape (source).Swap (*this);

  return *this;
}

/*
   ������� �����
*/

void Shape::SetMargin (float value)
{
  impl->SetMargin (value);
}

float Shape::Margin () const
{
  return impl->Margin ();
}

/*
   �����
*/

void Shape::Swap (Shape& shape)
{
  stl::swap (impl, shape.impl);
}

namespace physics
{

/*
   �����
*/

void swap (Shape& shape1, Shape& shape2)
{
  shape1.Swap (shape2);
}

}
