#include "shared.h"

using namespace syslib;

/*
    �������� ���������� ��������
*/

struct Semaphore::Impl
{
  Platform::semaphore_t handle; //���������� ��������
  
///�����������
  Impl (size_t initial_value) : handle (Platform::CreateSemaphore (initial_value)) {}
  
///����������
  ~Impl ()
  {
    try
    {
      Platform::DestroySemaphore (handle);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
};

/*
    ����������� / ����������
*/

Semaphore::Semaphore (size_t initial_value)
{
  try
  {
    impl = new Impl (initial_value);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Semaphore::Semaphore");
    throw;
  }
}

Semaphore::~Semaphore ()
{
}

/*
    �������� ��������
*/

size_t Semaphore::Value () const
{
  try
  {
    return Platform::GetSemaphoreValue (impl->handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Semaphore::Value");
    throw;
  }
}

/*
    �������� ����� ������ / ������� ����� ������
*/

void Semaphore::Wait ()
{
  try
  {
    Platform::WaitSemaphore (impl->handle);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Semaphore::Wait()");
    throw;
  }
}

void Semaphore::Wait (size_t wait_in_milliseconds)
{
  try
  {
    Platform::WaitSemaphore (impl->handle, wait_in_milliseconds);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Semaphore::Wait(size_t)");
    throw;
  }
}

bool Semaphore::TryWait ()
{
  try
  {
    return Platform::TryWaitSemaphore (impl->handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Semaphore::TryWait");
    throw;
  }
}

void Semaphore::Post ()
{
  try
  {
    Platform::PostSemaphore (impl->handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Semaphore::Post");
    throw;
  }
}
