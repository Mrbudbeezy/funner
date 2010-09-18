#include "shared.h"

using namespace common;

/*
===================================================================================================
    LogFilterImpl
===================================================================================================
*/

/*
    ����������� / ����������
*/

LogFilterImpl::LogFilterImpl (const char* in_mask, const LogHandler& handler)
  : log_system (LogSystemImpl::Instance ()),
    mask (in_mask ? in_mask : ""),
    log_handler (handler)
{
  if (!in_mask)
    throw xtl::make_null_argument_exception ("common::LogFilterImpl::LogFilterImpl", "mask");

  common::Lock lock (*log_system);

  log_system->Register (this);
}

LogFilterImpl::~LogFilterImpl ()
{
  try
  {
    Lock ();
    
    for (LogList::iterator iter=sources.begin (), end=sources.end (); iter!=end;)
    {
      LogList::iterator next = iter;

      ++next;

      (*iter)->RemoveFilter (this);

      iter = next;
    }
    
    {
      common::Lock lock (*log_system);

      log_system->Unregister (this);
    }
  }
  catch (...)
  {
  }
}

/*
    ����� ����� ���������� ����������������
*/

const char* LogFilterImpl::Mask ()
{
  common::Lock lock (*this);

  return mask.c_str ();
}

/*
    ����������������
*/

void LogFilterImpl::Print (const char* log_name, const char* log_message)
{
  try
  {
    common::Lock lock (*this);
    
    log_handler (log_name, log_message);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ���������� / �������� ����������
*/

void LogFilterImpl::AddSource (LogImpl* source)
{
  common::Lock lock (*this);

  sources.push_back (source);
}

void LogFilterImpl::RemoveSource (LogImpl* source)
{
  common::Lock lock (*this);

  sources.remove (source);
}

/*
===================================================================================================
    LogFilterImpl
===================================================================================================
*/

/*
    �����������/����������
*/

LogFilter::LogFilter (const char* log_name_mask, const LogHandler& handler)
  : impl (new LogFilterImpl (log_name_mask, handler))
{
}

LogFilter::LogFilter (const LogFilter& filter)
  : impl (filter.impl)
{
  addref (impl);
}

LogFilter::~LogFilter ()
{
  release (impl);
}

/*
    ������������
*/

LogFilter& LogFilter::operator = (const LogFilter& filter)
{
  LogFilter (filter).Swap (*this);

  return *this;
}

/*
    ��������� �����
*/

const char* LogFilter::Mask () const
{
  return impl->Mask ();
}

/*
    �����
*/

void LogFilter::Swap (LogFilter& filter)
{
  stl::swap (impl, filter.impl);
}

namespace common
{

void swap (LogFilter& filter1, LogFilter& filter2)
{
  filter1.Swap (filter2);
}

}
