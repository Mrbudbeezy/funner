#include "shared.h"

using namespace media;
using namespace media::animation;

namespace
{

/*
    ���������
*/

const float  EPSILON                           = 0.0001f; //����������� ��� �������� � ����
const size_t DEFAULT_EVENTS_TO_FIRE_ARRAY_SIZE = 4;       //������������� ���������� ������� ��� ������ ������ GetEvents

/*
    ���������� � �������
*/

struct EventDesc : public xtl::reference_counter
{
  float       delay;  //�������� ����� ������ ������������� �������
  float       period; //����� ������������ �������
  stl::string event;  //�������

  EventDesc (float in_delay, float in_period, const char* in_event)
    : delay (in_delay)
    , period (in_period)
    , event (in_event)
  {
  }
};

typedef xtl::intrusive_ptr<EventDesc> EventDescPtr;
typedef stl::vector<EventDescPtr>     EventDescArray;

}

/*
    ���������� ����� ������� ��������
*/

struct EventTrack::Impl: public xtl::reference_counter
{
  EventDescArray events;         //������ �������
  EventDescArray events_to_fire; //������ �������, ����������� ��� ������ GetEvents (����� ��� ���������� ���������� �������� ��������� � ������������ ������)

  Impl ()
  {
    events_to_fire.reserve (DEFAULT_EVENTS_TO_FIRE_ARRAY_SIZE);
  }
  
  Impl (const Impl& impl)
    : events (impl.events)
  {
    events_to_fire.reserve (DEFAULT_EVENTS_TO_FIRE_ARRAY_SIZE);    
  }
};

/*
   ������������ / ���������� / ������������
*/

EventTrack::EventTrack ()
  : impl (new Impl)
{
}

EventTrack::EventTrack (Impl* in_impl)
  : impl (in_impl)
{
}

EventTrack::EventTrack (const EventTrack& source)
  : impl (source.impl)
{
  addref (impl);
}

EventTrack::~EventTrack ()
{
  release (impl);
}

EventTrack& EventTrack::operator = (const EventTrack& source)
{
  EventTrack (source).Swap (*this);

  return *this;
}

/*
    �����������
*/

EventTrack EventTrack::Clone () const
{
  return EventTrack (new Impl (*impl));
}

/*
    �������������
*/

size_t EventTrack::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

    
/*
    ���������� �������
*/

size_t EventTrack::Size () const
{
  return impl->events.size ();
}

/*
    �������������� ����� �������
*/

size_t EventTrack::Capacity () const
{
  return impl->events.capacity ();
}

void EventTrack::Reserve (size_t events_count)
{
  impl->events.reserve (events_count);
}

/*
    ���������� / ��������� �������
*/

size_t EventTrack::AddEvent (float delay, float period, const char* event)
{
  static const char* METHOD_NAME = "media::animation::EventTrack::AddEvent";

  if (!event)
    throw xtl::make_null_argument_exception (METHOD_NAME, "event");

  if (delay < 0)
    throw xtl::make_argument_exception (METHOD_NAME, "delay", delay, "Delay must be not negative");

  if (period < 0)
    throw xtl::make_argument_exception (METHOD_NAME, "period", period, "Period must be not negative");

  impl->events.push_back (EventDescPtr (new EventDesc (delay, period, event), false));

  return impl->events.size () - 1;
}

void EventTrack::SetDelay (size_t event_index, float delay)
{
  static const char* METHOD_NAME = "media::animation::EventTrack::SetDelay";

  if (event_index >= impl->events.size ())
    throw xtl::make_range_exception (METHOD_NAME, "event_index", event_index, impl->events.size ());

  if (delay < 0)
    throw xtl::make_argument_exception (METHOD_NAME, "delay", delay, "Delay must be not negative");

  impl->events [event_index]->delay = delay;
}

void EventTrack::SetPeriod (size_t event_index, float period)
{
  static const char* METHOD_NAME = "media::animation::EventTrack::SetPeriod";

  if (event_index >= impl->events.size ())
    throw xtl::make_range_exception (METHOD_NAME, "event_index", event_index, impl->events.size ());

  if (period < 0)
    throw xtl::make_argument_exception (METHOD_NAME, "period", period, "Period must be not negative");

  impl->events [event_index]->period = period;
}

void EventTrack::SetEvent (size_t event_index, float delay, float period, const char* event)
{
  static const char* METHOD_NAME = "media::animation::EventTrack::SetEvent (size_t, float, float, const char*)";

  if (event_index >= impl->events.size ())
    throw xtl::make_range_exception (METHOD_NAME, "event_index", event_index, impl->events.size ());

  if (!event)
    throw xtl::make_null_argument_exception (METHOD_NAME, "event");

  if (delay < 0)
    throw xtl::make_argument_exception (METHOD_NAME, "delay", delay, "Delay must be not negative");

  if (period < 0)
    throw xtl::make_argument_exception (METHOD_NAME, "period", period, "Period must be not negative");

  impl->events [event_index] = EventDescPtr (new EventDesc (delay, period, event), false);
}

void EventTrack::SetEvent (size_t event_index, const char* event)
{
  static const char* METHOD_NAME = "media::animation::EventTrack::SetEvent (size_t, const char*)";

  if (event_index >= impl->events.size ())
    throw xtl::make_range_exception (METHOD_NAME, "event_index", event_index, impl->events.size ());

  if (!event)
    throw xtl::make_null_argument_exception (METHOD_NAME, "event");

  impl->events [event_index]->event = event;
}

/*
    ��������� �������
*/

const char* EventTrack::GetEvent (size_t event_index) const
{
  if (event_index >= impl->events.size ())
    throw xtl::make_range_exception ("media::animation::EventTrack::GetEvent", "event_index", event_index, impl->events.size ());

  return impl->events [event_index]->event.c_str ();
}

float EventTrack::GetDelay (size_t event_index) const
{
  if (event_index >= impl->events.size ())
    throw xtl::make_range_exception ("media::animation::EventTrack::GetTime", "event_index", event_index, impl->events.size ());

  return impl->events [event_index]->delay;
}

float EventTrack::GetPeriod (size_t event_index) const
{
  if (event_index >= impl->events.size ())
    throw xtl::make_range_exception ("media::animation::EventTrack::GetTime", "event_index", event_index, impl->events.size ());

  return impl->events [event_index]->period;
}

/*
    ������������ �������, ��������� � ��������� ���������� �������
*/

void EventTrack::GetEvents (float previous_time, float current_time, const EventHandler& handler) const
{
  if (impl->events.empty () || current_time < previous_time)
    return;

  impl->events_to_fire.clear ();

  for (EventDescArray::iterator iter = impl->events.begin (), end = impl->events.end (); iter != end; ++iter)
  {
    float delay  = (*iter)->delay,
          period = (*iter)->period;

    if (delay > current_time || (delay < previous_time && fabs (period) < EPSILON))
      continue;

    if (delay >= previous_time)
    {
      impl->events_to_fire.push_back (*iter);
      continue;
    }
    
    size_t previous_event_fire_count = size_t ((previous_time - delay) / period),
           current_event_fire_count  = size_t ((current_time - delay) / period);

    if (current_event_fire_count > previous_event_fire_count)
      impl->events_to_fire.push_back (*iter);
  }

  size_t events_to_fire_count = impl->events_to_fire.size ();

  for (float t = 0; t < current_time - previous_time;)
  {
    float min_dt = current_time - t;

    for (size_t i = 0; i < events_to_fire_count; i++)
    {
      float dt = min_dt, delay = impl->events_to_fire [i]->delay - previous_time, period = impl->events_to_fire [i]->period;

      if (delay > t)
      {
          //������� ��� �� ��������� (�������� ������ �������� �������)
        
        dt = delay - t;
      }
      else
      {
          //������� ���� ��������� � ������ ���, ���� ��� ����������� �� ������� t
        
        if (fabs (period) < EPSILON)
        {
            //������� - ���������������
          
          if (fabs (t - delay) < EPSILON)
          {
              //������� ��������� � ������ � ������������ ���
            
            handler (t + previous_time, impl->events_to_fire [i]->event.c_str ());
          }
        }
        else
        {
            //������� �������������

          float clamped_t = fmod (t - delay, period);

          dt = period - clamped_t;

          if (fabs (clamped_t) < EPSILON)
          {
            handler (t + previous_time, impl->events_to_fire [i]->event.c_str ());
          }
        }
      }

      if (dt < min_dt)
        min_dt = dt;
    }

    t += min_dt;
  }
}

/*
   �������� �������� �����
*/

void EventTrack::Remove (size_t event_index)
{
  if (event_index >= impl->events.size ())
    return;

  impl->events.erase (impl->events.begin () + event_index);
}

void EventTrack::Clear ()
{
  impl->events.clear ();
}

/*
   �����
*/

void EventTrack::Swap (EventTrack& track)
{
  stl::swap (impl, track.impl);
}

namespace media
{

namespace animation
{

/*
   �����
*/

void swap (EventTrack& track1, EventTrack& track2)
{
  track1.Swap (track2);
}

}

}
