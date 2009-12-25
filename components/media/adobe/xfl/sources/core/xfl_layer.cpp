#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<Frame, ICollection<Frame> > FrameCollection;

/*
    �������� ���������� ���� ��������
*/

struct Layer::Impl : public xtl::reference_counter
{
  stl::string     name;   //��� ����
  FrameCollection frames; //�����
};

/*
    ������������ / ���������� / ������������
*/

Layer::Layer ()
  : impl (new Impl)
  {}
  
Layer::Layer (const Layer& source)
  : impl (source.impl)
{
  addref (impl);
}

Layer::~Layer ()
{
  release (impl);
}

Layer& Layer::operator = (const Layer& source)
{
  Layer (source).Swap (*this);

  return *this;
}

/*
   ���
*/

const char* Layer::Name () const
{
  return impl->name.c_str ();
}

void Layer::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::Layer::SetName", "name");

  impl->name = name;
}

/*
   ����� �������� ����
*/

Layer::FrameList& Layer::Frames ()
{
  return const_cast<FrameList&> (const_cast<const Layer&> (*this).Frames ());
}

const Layer::FrameList& Layer::Frames () const
{
  return impl->frames;
}

/*
   �����
*/

void Layer::Swap (Layer& layer)
{
  stl::swap (impl, layer.impl);
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

void swap (Layer& layer1, Layer& layer2)
{
  layer1.Swap (layer2);
}

}

}

}
