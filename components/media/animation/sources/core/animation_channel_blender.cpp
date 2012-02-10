#include "shared.h"

using namespace media::animation;

namespace
{

/*
    ���������
*/

const size_t RESERVED_CHANNELS_COUNT = 4; //������������� ����� �������

}

/*
    �������� ���������� ������� �������
*/

struct ChannelBlenderBase::Impl: public xtl::reference_counter
{
  typedef stl::vector<ChannelDesc>                   DescArray;
  typedef xtl::intrusive_ptr<detail::IEvaluatorBase> EvaluatorPtr;
  
  struct ChannelImpl: public xtl::reference_counter
  {
    EvaluatorPtr         evaluator;
    xtl::auto_connection state_tracker;
  };
  
  typedef xtl::intrusive_ptr<ChannelImpl> ChannelImplPtr;
  typedef stl::vector<ChannelImplPtr>     ChannelArray;  

  const std::type_info* value_type; //��� ��������
  DescArray             descs;      //������ ������������ �������
  ChannelArray          channels;   //������
  
///�����������
  Impl (const std::type_info& in_value_type)
    : value_type (&in_value_type)
  {
    descs.reserve (RESERVED_CHANNELS_COUNT);
    channels.reserve (RESERVED_CHANNELS_COUNT);
  }
  
///�������� �������
  void RemoveChannelsByState (IAnimationState* state)
  {
    for (DescArray::iterator iter=descs.begin (); iter!=descs.end ();)
      if (iter->state == state)
      {
        descs.erase (iter);
        channels.erase (channels.begin () + (iter - descs.begin ()));
      }
      else ++iter;
  }
  
  void RemoveChannelsByChannel (const Channel& channel)
  {
    detail::IEvaluatorBase* evaluator = channel.TrackCore ();
    
    if (!evaluator)
      return;
    
    for (DescArray::iterator iter=descs.begin (); iter!=descs.end ();)
      if (iter->evaluator == evaluator)
      {
        descs.erase (iter);
        channels.erase (channels.begin () + (iter - descs.begin ()));
      }
      else ++iter;
  }  
};

/*
    ������������ / ���������� / ������������
*/

ChannelBlenderBase::ChannelBlenderBase (const std::type_info& value_type)
  : impl (new Impl (value_type))
{
}

ChannelBlenderBase::ChannelBlenderBase (const ChannelBlenderBase& blender)
  : impl (blender.impl)
{
  addref (impl);
}

ChannelBlenderBase::~ChannelBlenderBase ()
{
  release (impl);
}

ChannelBlenderBase& ChannelBlenderBase::operator = (const ChannelBlenderBase& blender)
{
  ChannelBlenderBase (blender).Swap (*this);
  
  return *this;
}

/*
    ��� ��������
*/

const std::type_info& ChannelBlenderBase::ValueType () const
{
  return *impl->value_type;
}

/*
    ���������� �������
*/

size_t ChannelBlenderBase::ChannelsCount () const
{
  return impl->descs.size ();
}

/*
    ��������� ������� � ������������ ���������
*/

const ChannelBlenderBase::ChannelDesc* ChannelBlenderBase::Channels () const
{
  if (impl->descs.empty ())
    return 0;

  return &impl->descs [0];
}

/*
    ���������� � �������� �������
*/

void ChannelBlenderBase::AddChannel (const media::animation::AnimationState& state, const media::animation::Channel& channel)
{
  static const char* METHOD_NAME = "media::animation::ChannelBlenderBase";

    //�������� ������������ ����������

  ChannelDesc desc;
  
  desc.state     = &state.AnimationStateCore ();
  desc.evaluator = channel.TrackCore ();
  
  if (!desc.evaluator)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't add empty animation channel to blender");
  
  if (&desc.evaluator->ValueType () != impl->value_type)
    throw xtl::format_operation_exception (METHOD_NAME, "Attempt to add animation channel with value type '%s' to blender with value type '%s'", desc.evaluator->ValueType ().name (),
      impl->value_type->name ());
      
    //���������� � �����
    
  Impl::ChannelImplPtr channel_impl (new Impl::ChannelImpl, false);
  
  channel_impl->evaluator     = desc.evaluator;
  channel_impl->state_tracker = state.GetTrackable ().connect_tracker (xtl::bind (&Impl::RemoveChannelsByState, impl, &state.AnimationStateCore ()));
    
  impl->descs.reserve (impl->descs.size () + 1);
  impl->channels.reserve (impl->channels.size () + 1);
  
  impl->descs.push_back (desc);
  impl->channels.push_back (channel_impl);
}

void ChannelBlenderBase::RemoveChannels (const media::animation::AnimationState& state)
{
  impl->RemoveChannelsByState (&state.AnimationStateCore ());
}

void ChannelBlenderBase::RemoveChannels (const media::animation::Channel& channel)
{
  impl->RemoveChannelsByChannel (channel);
}

void ChannelBlenderBase::RemoveAllChannels ()
{
  impl->descs.clear ();
  impl->channels.clear ();
}

/*
    �����
*/

void ChannelBlenderBase::Swap (ChannelBlenderBase& blender)
{
  stl::swap (impl, blender.impl);
}

namespace media
{

namespace animation
{

void swap (ChannelBlenderBase& b1, ChannelBlenderBase& b2)
{
  b1.Swap (b2);
}

}

}
