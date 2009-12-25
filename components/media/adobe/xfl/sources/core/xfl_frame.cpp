#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<FrameElement, INamedCollection<FrameElement> > FrameElementCollection;

/*
    �������� ���������� ����� ��������
*/

struct Frame::Impl : public xtl::reference_counter
{
  size_t                 first_frame; //����� ����� (� ���������� �������), � �������� ���������� ����������� ������� �����
  size_t                 duration;    //������������ ����������� ������� ����� � ���������� ������ (� ���������� �������)
  FrameElementCollection elements;    //����� �������� ����
  AnimationCore          animation;   //��������
  
  Impl ()
    : first_frame (0)
    , duration (0)
  { }
};

/*
    ������������ / ���������� / ������������
*/

Frame::Frame ()
  : impl (new Impl)
  {}
  
Frame::Frame (const Frame& source)
  : impl (source.impl)
{
  addref (impl);
}

Frame::~Frame ()
{
  release (impl);
}

Frame& Frame::operator = (const Frame& source)
{
  Frame (source).Swap (*this);

  return *this;
}

/*
   ��������
*/

size_t Frame::FirstFrame () const
{
  return impl->first_frame;
}

size_t Frame::Duration () const
{
  return impl->duration;
}

void Frame::SetFirstFrame (size_t first_frame)
{
  impl->first_frame = first_frame;
}

void Frame::SetDuration (size_t duration)
{
  impl->duration = duration;
}

/*
   ����� �������� ����
*/

Frame::FrameElementList& Frame::Elements ()
{
  return const_cast<FrameElementList&> (const_cast<const Frame&> (*this).Elements ());
}

const Frame::FrameElementList& Frame::Elements () const
{
  return impl->elements;
}

/*
   �������� �����
*/

const AnimationCore& Frame::Animation () const
{
  return impl->animation;
}

void Frame::SetAnimation (const AnimationCore& animation)
{
  impl->animation = animation;
}

/*
   �����
*/

void Frame::Swap (Frame& frame)
{
  stl::swap (impl, frame.impl);
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

void swap (Frame& frame1, Frame& frame2)
{
  frame1.Swap (frame2);
}

}

}

}
