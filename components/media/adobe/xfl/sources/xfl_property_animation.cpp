#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<PropertyAnimationKeyframe, ICollection<PropertyAnimationKeyframe> > KeyframeCollection;

/*
    �������� ���������� ������������ ��������
*/

struct PropertyAnimation::Impl : public xtl::reference_counter
{
  stl::string        id;        //������������� ���� ��������
  bool               enabled;   //������
  KeyframeCollection keyframes; //�������� �����
};

/*
    ������������ / ���������� / ������������
*/

PropertyAnimation::PropertyAnimation ()
  : impl (new Impl)
  {}
  
PropertyAnimation::PropertyAnimation (const PropertyAnimation& source)
  : impl (source.impl)
{
  addref (impl);
}

PropertyAnimation::~PropertyAnimation ()
{
  release (impl);
}

PropertyAnimation& PropertyAnimation::operator = (const PropertyAnimation& source)
{
  addref (source.impl);
  release (impl);

  impl = source.impl;

  return *this;
}

/*
   ������������� ���� ��������
*/

const char* PropertyAnimation::Id () const
{
  return impl->id.c_str ();
}

void PropertyAnimation::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::PropertyAnimation::SetId", "id");

  impl->id = id;
}

/*
   �������� �� �������� ��� ����� ��������
*/

bool PropertyAnimation::Enabled () const
{
  return impl->enabled;
}

void PropertyAnimation::SetEnabled (bool enabled)
{
  impl->enabled = enabled;
}

/*
   ��������
*/

PropertyAnimation::KeyframeList& PropertyAnimation::Keyframes ()
{
  return const_cast<KeyframeList&> (const_cast<const PropertyAnimation&> (*this).Keyframes ());
}

const PropertyAnimation::KeyframeList& PropertyAnimation::Keyframes () const
{
  return impl->keyframes;
}

/*
   �����
*/

void PropertyAnimation::Swap (PropertyAnimation& animation)
{
  stl::swap (impl, animation.impl);
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

void swap (PropertyAnimation& animation1, PropertyAnimation& animation2)
{
  animation1.Swap (animation2);
}

}

}

}
