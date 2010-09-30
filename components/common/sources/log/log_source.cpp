#include "shared.h"

using namespace common;

/*
===================================================================================================
    LogImpl
===================================================================================================
*/

/*
    ����������� / ����������
*/

LogImpl::LogImpl (const char* log_name)
  : log_system (LogSystemImpl::Instance ()),
    name (log_name ? log_name : "")
{
  if (!log_name)
    throw xtl::make_null_argument_exception ("common::LogImpl::LogImpl", "log_name");
    
  common::Lock lock (*log_system);

  log_system->Register (this);
}

LogImpl::~LogImpl ()
{
  try
  {
    Lock ();
    
    for (FilterList::iterator iter=filters.begin (), end=filters.end (); iter!=end;)
    {
      FilterList::iterator next = iter;

      ++next;

      (*iter)->RemoveSource (this);

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
    ��� ���������
*/

const char* LogImpl::Name ()
{
  common::Lock lock (*this);

  return name.c_str ();
}

/*
    ���������� / �������� ��������
*/

void LogImpl::AddFilter (LogFilterImpl* filter)
{  
  common::Lock lock (*this);

  filters.push_back (filter);

  try
  {
    filter->AddSource (this);
  }
  catch (...)
  {
    filters.pop_back ();
    throw;
  }
}

void LogImpl::RemoveFilter (LogFilterImpl* filter)
{
  common::Lock lock (*this);

  filter->RemoveSource (this);
  filters.remove (filter);
}

/*
    ����������������
*/

void LogImpl::Print (const char* message)
{
  common::Lock lock (*this);

  xtl::intrusive_ptr<LogImpl> self_lock (this);

  for (FilterList::iterator iter=filters.begin (), end=filters.end (); iter!=end; ++iter)
  {
    xtl::intrusive_ptr<LogFilterImpl> lock = *iter;

    lock->Print (name.c_str (), message);
  }
}

/*
===================================================================================================
    LogImpl
===================================================================================================
*/

/*
    �����������/����������
*/

Log::Log (const char* name)
  : impl (new LogImpl (name))
{
}

Log::Log (const Log& log)
  : impl (log.impl)
{
  addref (impl);
}

Log::~Log ()
{
  release (impl);
}

/*
    ������������
*/

Log& Log::operator = (const Log& log)
{
  Log (log).Swap (*this);
  
  return *this;
}

/*
    ��������� �����
*/

const char* Log::Name () const
{
  return impl->Name ();
}

/*
    ������ ���������
*/

void Log::Print (const char* message) const
{
  if (!message)
    return;

  impl->Print (message);
}

void Log::VPrintf (const char* message, va_list list) const
{
  if (!message)
    return;

  try
  {
    Print (common::vformat (message, list).c_str ());
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

void Log::Printf (const char* message, ...) const
{
  if (!message)
    return;
    
  va_list list;
  
  va_start (list, message);  
  VPrintf  (message, list);
}

/*
    �����
*/

void Log::Swap (Log& log)
{
  stl::swap (impl, log.impl);
}

namespace common
{

void swap (Log& log1, Log& log2)
{
  log1.Swap (log2);
}

}
