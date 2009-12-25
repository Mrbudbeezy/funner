#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<PropertyAnimation, ICollection<PropertyAnimation> > PropertyAnimationCollection;

/*
   �������� ���������� �������� �����
*/

struct AnimationCore::Impl : public xtl::reference_counter
{
  float                       duration;       //������������ � ��������
  bool                        orient_to_path; //�������������� �� ����
  PropertyAnimationCollection properties;     //����������� ��������

  Impl () : duration (0.0f), orient_to_path (false) {}
};

/*
    ������������ / ���������� / ������������
*/

AnimationCore::AnimationCore ()
  : impl (new Impl)
  {}
  
AnimationCore::AnimationCore (const AnimationCore& source)
  : impl (source.impl)
{
  addref (impl);
}

AnimationCore::~AnimationCore ()
{
  release (impl);
}

AnimationCore& AnimationCore::operator = (const AnimationCore& source)
{
  AnimationCore (source).Swap (*this);

  return *this;
}

/*
   ������������ � ��������
*/

float AnimationCore::Duration () const
{
  return impl->duration;
}

void AnimationCore::SetDuration (float duration)
{
  impl->duration = duration;
}

/*
   ������ �� ����������� �������������� �� ����
*/

bool AnimationCore::OrientToPath () const
{
  return impl->orient_to_path;
}

void AnimationCore::SetOrientToPath (bool orient_to_path)
{
  impl->orient_to_path = orient_to_path;
}

/*
   ����������� ��������
*/

AnimationCore::PropertyAnimationList& AnimationCore::Properties ()
{
  return const_cast<PropertyAnimationList&> (const_cast<const AnimationCore&> (*this).Properties ());
}

const AnimationCore::PropertyAnimationList& AnimationCore::Properties () const
{
  return impl->properties;
}

PropertyAnimation* AnimationCore::FindProperty (const char* property_id)
{
  return const_cast<PropertyAnimation*> (const_cast<const AnimationCore&> (*this).FindProperty (property_id));
}

const PropertyAnimation* AnimationCore::FindProperty (const char* property_id) const
{
  if (!property_id)
    return 0;

  for (PropertyAnimationCollection::ConstIterator iter = impl->properties.CreateIterator (); iter; ++iter)
    if (!xtl::xstrcmp (property_id, iter->Id ()))
      return &(*iter);

  return 0;
}

/*
   �����
*/

void AnimationCore::Swap (AnimationCore& animation)
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

void swap (AnimationCore& animation1, AnimationCore& animation2)
{
  animation1.Swap (animation2);
}

}

}

}
