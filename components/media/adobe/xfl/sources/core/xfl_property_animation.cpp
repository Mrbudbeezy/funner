#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<PropertyAnimationKeyframe, ICollection<PropertyAnimationKeyframe> > KeyframeCollection;

/*
    �������� ���������� ������������ ��������
*/

struct PropertyAnimation::Impl : public xtl::reference_counter
{
  stl::string        name;      //��� ��������
  bool               enabled;   //������
  KeyframeCollection keyframes; //�������� �����
  
  Impl ()
    : enabled (true)
  { }
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
  PropertyAnimation (source).Swap (*this);

  return *this;
}

/*
   ��� ��������
*/

const char* PropertyAnimation::Name () const
{
  return impl->name.c_str ();
}

void PropertyAnimation::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::PropertyAnimation::SetName", "name");

  impl->name = name;
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
