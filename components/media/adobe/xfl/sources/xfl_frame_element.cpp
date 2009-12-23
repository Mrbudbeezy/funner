#include "shared.h"

using namespace media::adobe::xfl;

/*
    �������� ���������� �������� ����� ��������
*/

struct FrameElement::Impl : public xtl::reference_counter
{
  stl::string      name;                 //��� ��������
  FrameElementType type;                 //��� ��������
  size_t           first_frame;          //����� ����� (� ���������� �������), � �������� ���������� ����������� ������� �����
  math::vec2f      translation;          //��������
  math::vec2f      transformation_point; //�����
};

/*
    ������������ / ���������� / ������������
*/

FrameElement::FrameElement ()
  : impl (new Impl)
  {}
  
FrameElement::FrameElement (const FrameElement& source)
  : impl (source.impl)
{
  addref (impl);
}

FrameElement::~FrameElement ()
{
  release (impl);
}

FrameElement& FrameElement::operator = (const FrameElement& source)
{
  addref (source.impl);
  release (impl);

  impl = source.impl;

  return *this;
}

/*
   ���
*/

const char* FrameElement::Name () const
{
  return impl->name.c_str ();
}

void FrameElement::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::FrameElement::SetName", "name");

  impl->name = name;
}

/*
   ���
*/

FrameElementType FrameElement::Type () const
{
  return impl->type;
}

void FrameElement::SetType (FrameElementType type)
{
  impl->type = type;
}

/*
   ��������
*/

size_t FrameElement::FirstFrame () const
{
  return impl->first_frame;
}

void FrameElement::SetFirstFrame (size_t first_frame)
{
  impl->first_frame = first_frame;
}

/*
   ���������
*/

const math::vec2f& FrameElement::Translation () const
{
  return impl->translation;
}

const math::vec2f& FrameElement::TransformationPoint () const
{
  return impl->transformation_point;
}

void FrameElement::SetTranslation (const math::vec2f& translation)
{
  impl->translation = translation;
}

void FrameElement::SetTransformationPoint (const math::vec2f& transformation_point)
{
  impl->transformation_point = transformation_point;
}

/*
   �����
*/

void FrameElement::Swap (FrameElement& element)
{
  stl::swap (impl, element.impl);
}

namespace media
{

namespace adobe
{

namespace xfl
{

/*
   �����
*/

void swap (FrameElement& element1, FrameElement& element2)
{
  element1.Swap (element2);
}

}

}

}
