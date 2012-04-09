#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/singleton.h>
#include <common/time.h>

#include <platform/platform.h>

using namespace common;

/*
    ���������� ������ � ����������� ������������
*/

namespace
{

size_t start_milliseconds_time = Platform::GetMilliseconds ();

}

namespace common
{

struct TimerImpl: public xtl::reference_counter
{
  typedef Timer::TimeSource TimeSource;
  typedef Timer::time_t     time_t;
  typedef Timer::timeint_t  timeint_t;

  TimeSource source;      //�������� ��������� �������
  bool       auto_update; //����������� �� ����� �������������
  time_t     time;        //������� �����  
  time_t     precision;   //�������� ������ ��������� �������
  timeint_t  start_time;  //��������� �����
  bool       started;     //������� �� ������
  
///�����������
  TimerImpl (const TimeSource& in_source, time_t in_precision, bool in_auto_update)
    : source (in_source)
    , auto_update (in_auto_update)
    , time (0)
    , precision (in_precision)
    , start_time (0)
    , started (false)
  {
  }
  
///����������
  void Update ()
  {
    if (!started)
      return;
      
    time = ComputeTimeInteger () / precision;
  }
  
///������ ��������� �������
  timeint_t ComputeTimeInteger ()
  {
    return started ? source () - start_time : start_time;
  }
};

}

namespace
{

/*
    �������� ������� ������� �� ���������
*/

struct DefaultTimeSource
{
  Timer::TimeSource source;
  
  DefaultTimeSource () : source (&common::milliseconds) { }
  
  static Timer::TimeSource& Instance ()
  {
    return common::Singleton<DefaultTimeSource>::Instance ()->source;
  }
};

/*
    ������ ��� �������� ��� ������������ ���������� ��������
*/

struct ParentTimeSource
{
  xtl::intrusive_ptr<TimerImpl> impl;
  
  ParentTimeSource (TimerImpl* in_impl) : impl (in_impl) {}

  Timer::timeint_t operator () () const
  {
    return impl->ComputeTimeInteger ();
  }
};

}

/*
    ������������ / ���������� / ������������
*/

Timer::Timer (bool is_auto_update)
  : impl (new TimerImpl (DefaultTimeSource::Instance (), 1000.0f, is_auto_update))
{
}

Timer::Timer (const TimeSource& source, time_t precision, bool is_auto_update)
  : impl (new TimerImpl (source, precision, is_auto_update))
{
}

Timer::Timer (const Timer& source, bool is_auto_update)
  : impl (new TimerImpl (ParentTimeSource (source.impl), source.impl->precision, is_auto_update))
{
}

Timer::Timer (const Timer& source)
  : impl (source.impl)
{
  addref (impl);
}

Timer::~Timer ()
{
  release (impl);
}

Timer& Timer::operator = (const Timer& source)
{
  Timer (source).Swap (*this);
  
  return *this;
}

/*
    ��������� �������� �������
*/

void Timer::SetTime (time_t time)
{
  impl->time = time;
}

Timer::time_t Timer::Time () const
{
  if (impl->auto_update)
    impl->Update ();

  return impl->time;
}

/*
    ���������� �������� �������
*/

void Timer::Update ()
{
  impl->Update ();
}

/*
    ���������� ����-������������ ���������
*/

void Timer::SetAutoUpdate (bool state)
{
  impl->auto_update = state;
}

bool Timer::IsAutoUpdate () const
{
  return impl->auto_update;
}

/*
    ���������� ��������
*/

void Timer::Start ()
{
  if (impl->started)
    return;

  impl->start_time = impl->source () - impl->start_time;    
  impl->started    = true;
}

void Timer::Stop ()
{
  if (!impl->started)
    return;

  Update ();

  impl->start_time = impl->source () - impl->start_time;
  impl->started    = false;
}

void Timer::Reset ()
{
  impl->started    = false;
  impl->time       = 0;
  impl->start_time = 0;
}

bool Timer::IsStarted () const
{
  return impl->started;
}

/*
    �����
*/

void Timer::Swap (Timer& timer)
{
  TimerImpl* tmp = impl;
  impl           = timer.impl;
  timer.impl     = tmp;
}

namespace common
{

//����� ���� ��������
void swap (Timer& timer1, Timer& timer2)
{
  timer1.Swap (timer2);
}

//��������� ������� � ������������� �� ������� ������ milliseconds
size_t milliseconds ()
{  
  return Platform::GetMilliseconds () - start_milliseconds_time;
}

}
