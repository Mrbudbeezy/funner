#include "shared.h"

using namespace syslib;

/*
    �������� ���������� ������������ ��������
*/

struct Condition::Impl
{
  Platform::condition_t condition;

///�����������
  Impl ()
  {
    condition = Platform::CreateCondition ();
  }
  
///����������
  ~Impl ()
  {
    try
    {
      Platform::DestroyCondition (condition);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
};

/*
    ������������ / ���������� / ������������
*/

Condition::Condition ()
{
  try
  {
    impl = new Impl;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Condition::Condition");
    throw;
  }
}

Condition::~Condition ()
{
}

/*
    ������� ���������� �� ��������� �������
*/

void Condition::NotifyOne ()
{
  try
  {
    Platform::NotifyCondition (impl->condition, false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Condition::NotifyOne");
    throw;
  }
}

void Condition::NotifyAll ()
{
  try
  {
    Platform::NotifyCondition (impl->condition, true);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Condition::NotifyAll");
    throw;
  }
}

/*
    �������� ��������� �������
*/

void Condition::Wait (Mutex& lock)
{
  try
  {
    Platform::WaitCondition (impl->condition, reinterpret_cast<Platform::mutex_t> (lock.Handle ()));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Condition::Wait(Mutex&)");
    throw;
  }
}

void Condition::Wait (Mutex& lock, size_t wait_in_milliseconds)
{
  try
  {
    if (!Platform::WaitCondition (impl->condition, reinterpret_cast<Platform::mutex_t> (lock.Handle ()), wait_in_milliseconds))
      throw xtl::format_operation_exception ("", "Condition wait timeout");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Condition::Wait(Mutex&,size_t)");
    throw;
  }
}

bool Condition::TryWait (Mutex& lock)
{
  try
  {
    return Platform::WaitCondition (impl->condition, reinterpret_cast<Platform::mutex_t> (lock.Handle ()), 0);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Condition::TryWait(Mutex&)");
    throw;
  }
}

bool Condition::TryWait (Mutex& lock, size_t wait_in_milliseconds)
{
  try
  {
    return Platform::WaitCondition (impl->condition, reinterpret_cast<Platform::mutex_t> (lock.Handle ()), wait_in_milliseconds);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Condition::TryWait(Mutex&,size_t)");
    throw;
  }
}
