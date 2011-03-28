#include "shared.h"

using namespace physics;

/*
   �������������� ����
*/

struct Shape::Impl : public xtl::reference_counter
{
  ShapePtr  shape;
  ShapeList subshapes;

  Impl (ShapePtr in_shape)
    : shape (in_shape)
    {}

  Impl (ShapePtr in_shape, const ShapeList& in_subshapes)
    : shape (in_shape), subshapes (in_subshapes)
    {}
};

/*
   ����������� / ���������� / �����������
*/

Shape::Shape (Impl* source_impl)
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
  impl->shape->SetMargin (value);
}

float Shape::Margin () const
{
  return impl->shape->Margin ();
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

/*
   ��������
*/

Shape ShapeImplProvider::CreateShape (ShapePtr shape)
{
  return Shape (new Shape::Impl (shape));
}

Shape ShapeImplProvider::CreateShape (ShapePtr shape, const ShapeList& shape_list)
{
  return Shape (new Shape::Impl (shape, shape_list));
}

/*
   ��������� ��������������� ����
*/

physics::low_level::IShape* ShapeImplProvider::LowLevelShape (const Shape& shape)
{
  return shape.impl->shape.get ();
}
