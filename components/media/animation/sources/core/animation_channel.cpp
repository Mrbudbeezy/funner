#include "shared.h"

using namespace media;
using namespace media::animation;

/*
   ����� ��������
*/

typedef xtl::intrusive_ptr<animation::detail::IEvaluatorBase> EvaluatorBasePtr;

struct Channel::Impl : public xtl::reference_counter
{
  stl::string           parameter_name;
  EvaluatorBasePtr      track;
  const std::type_info* track_type;
  const std::type_info* value_type;

  Impl ()
    : track_type (&typeid (void)), value_type (&typeid (void))
  {
  }  
};

/*
   ������������ / ���������� / ������������
*/

Channel::Channel ()
  : impl (new Impl)
{
}

Channel::Channel (Impl* in_impl)
  : impl (in_impl)
{
}

Channel::Channel (const Channel& source)
  : impl (source.impl)
{
  addref (impl);
}

Channel::~Channel ()
{
  release (impl);
}

Channel& Channel::operator = (const Channel& source)
{
  Channel (source).Swap (*this);
  
  return *this;
}
    
/*
   �������� �����
*/

Channel Channel::Clone () const
{
  return Channel (new Impl (*impl));
}
    
/*
   �������������
*/

size_t Channel::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    ����������� / ������������ �����
*/

float Channel::MinTime () const
{
  if (!impl->track)
    return 0.0f;    

  return impl->track->MinTime ();
}

float Channel::MaxTime () const
{
  if (!impl->track)
    return 0.0f;

  return impl->track->MaxTime ();
}

/*
    ����������� / ������������ ������������ ����� (-INF/INF � ������ ��������� ���������)
*/

float Channel::MinUnwrappedTime () const
{
  if (!impl->track)
    return 0.0f;

  return impl->track->MinUnwrappedTime ();
}

float Channel::MaxUnwrappedTime () const
{
  if (!impl->track)
    return 0.0f;

  return impl->track->MaxUnwrappedTime ();
}

/*
    ��� ������������ ���������
*/

const char* Channel::ParameterName () const
{
  return impl->parameter_name.c_str ();
}

void Channel::SetParameterName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::animation::Channel::SetParameterName", "name");
  
  impl->parameter_name = name;
}

/*
    ��� ����� / ��� ��������
*/

const std::type_info& Channel::TrackType () const
{
  return *impl->track_type;
}

const std::type_info& Channel::ValueType () const
{
  return *impl->value_type;
}

/*
    ��������� �����
*/

void Channel::SetTrackCore (animation::detail::IEvaluatorBase* in_track)
{
  if (!in_track)
    throw xtl::make_null_argument_exception ("media::animation::Channel::SetTrackCore", "track");

  EvaluatorBasePtr track (in_track, false);

  const std::type_info &track_type = track->TrackType (),
                       &value_type = track->ValueType ();

  impl->track      = track;
  impl->track_type = &track_type;
  impl->value_type = &value_type;
}

animation::detail::IEvaluatorBase* Channel::TrackCore () const
{
  return impl->track.get ();
}

bool Channel::HasTrack () const
{
  return impl->track;
}

/*
    ������ ���������� �������������� ����
*/

void Channel::RaiseTypeError (const char* source, const std::type_info& type) const
{
  throw xtl::format_operation_exception (source, "Type mismatch, requested type '%s', but channel type is '%s'", type.name (), ValueType ().name ());
}

/*
    �����
*/

void Channel::Swap (Channel& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace animation
{

void swap (Channel& channel1, Channel& channel2)
{
  channel1.Swap (channel2);
}

}

}
