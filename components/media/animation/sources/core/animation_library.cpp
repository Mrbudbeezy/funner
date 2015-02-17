#include "shared.h"

using namespace media;
using namespace media::animation;

/*
   ���������� ���������� ��������
*/

typedef ResourceLibrary<Animation> AnimationLibraryHolder;

struct AnimationLibrary::Impl : public xtl::reference_counter
{
  AnimationLibraryHolder animations;
};

/*
   ������������ / ���������� / ������������
*/

AnimationLibrary::AnimationLibrary ()
  : impl (new Impl)
  {}

AnimationLibrary::AnimationLibrary (const AnimationLibrary& source)
  : impl (source.impl)
{
  addref (impl);
}

AnimationLibrary::AnimationLibrary (const char* file_name)
  : impl (new Impl)
{
  try
  {
    Load (file_name);
  }
  catch (xtl::exception& e)
  {
    release (impl);
    e.touch ("media::animation::AnimationLibrary::AnimationLibrary (const char*)");
    throw;
  }
  catch (...)
  {
    release (impl);
    throw;
  }
}

AnimationLibrary::~AnimationLibrary ()
{
  release (impl);
}

AnimationLibrary& AnimationLibrary::operator = (const AnimationLibrary& source)
{
  AnimationLibrary (source).Swap (*this);

  return *this;
}

/*
   ���������� ��������� � ���������� / �������� �� �������
*/

unsigned int AnimationLibrary::Size () const
{
  return (unsigned int)impl->animations.Size ();
}

bool AnimationLibrary::IsEmpty () const
{
  return impl->animations.IsEmpty ();
}

/*
   �������
*/

AnimationLibrary::Iterator AnimationLibrary::CreateIterator ()
{
  return impl->animations.CreateIterator ();
}

AnimationLibrary::ConstIterator AnimationLibrary::CreateIterator () const
{
  return impl->animations.CreateIterator ();
}
    
/*
   ��������� �������������� �������� ����������
*/

const char* AnimationLibrary::ItemId (const ConstIterator& iter) const
{
  return impl->animations.ItemId (iter);
}

/*
   �����
*/

Animation* AnimationLibrary::Find (const char* id)
{
  return impl->animations.Find (id);
}

const Animation* AnimationLibrary::Find (const char* id) const
{
  return impl->animations.Find (id);
}
    
/*
   ���������� / �������� ���������
*/

void AnimationLibrary::Attach (const char* id, Animation& animation)
{
  if (impl->animations.Size () >= (unsigned int)-1)
    throw xtl::format_operation_exception ("media::animation::AnimationLibrary::Attach", "Can't attach animation, animations count limit exceeded");

  impl->animations.Insert (id, animation);
}

void AnimationLibrary::Detach (const char* id)
{
  impl->animations.Remove (id);
}

void AnimationLibrary::DetachAll ()
{
  impl->animations.Clear ();
}
    
/*
   �������� / ����������
*/

void AnimationLibrary::Load (const char* file_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("media::animation::AnimationLibrary::Load", "file_name");

  try
  {
    static common::ComponentLoader loader ("media.animation.loaders.*");

    AnimationLibraryManager::GetLoader (file_name, common::SerializerFindMode_ByName)(file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::animation::AnimationLibrary::Load");
    throw;
  }
}

void AnimationLibrary::Save (const char* file_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("media::animation::AnimationLibrary::Save", "file_name");

  try
  {
    static common::ComponentLoader loader ("media.animation.savers.*");

    AnimationLibraryManager::GetSaver (file_name, common::SerializerFindMode_ByName)(file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::animation::AnimationLibrary::Save");
    throw;
  }
}

/*
   �����
*/

void AnimationLibrary::Swap (AnimationLibrary& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace animation
{

/*
   �����
*/

void swap (AnimationLibrary& library1, AnimationLibrary& library2)
{
  library1.Swap (library2);
}

}

}
