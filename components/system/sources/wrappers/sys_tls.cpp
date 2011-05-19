#include "shared.h"

using namespace syslib;

/*
    �������� ��������� ���������� ��������� ������
*/

struct ThreadLocalStorage::Impl: public IThreadCleanupCallback
{
  tls_t          key;     //���� ��������� ������ ����
  CleanupHandler cleanup; //��������� ����������
  
///�����������
  Impl (const CleanupHandler& in_cleanup) : cleanup (in_cleanup)
  {
    key = Platform::CreateTls (this);
  }
  
///����������
  ~Impl ()
  {
    try
    {
      Platform::DestroyTls (key);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }

///���������� � ������������� ������� ������
  void Cleanup (void* data)
  {
    if (!cleanup)
      return;
    
    try
    {
      cleanup (data);
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

ThreadLocalStorage::ThreadLocalStorage ()  
{
  try
  {
    impl = new Impl (CleanupHandler ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::ThreadLocalStorage::ThreadLocalStorage()");
    throw;
  }
}

ThreadLocalStorage::ThreadLocalStorage (const CleanupHandler& cleanup)
{
  try
  {
    impl = new Impl (cleanup);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::ThreadLocalStorage::ThreadLocalStorage(const CleanupHandler&)");
    throw;
  }
}

ThreadLocalStorage::ThreadLocalStorage (void (*cleanup)(void*))
{
  try
  {
    impl = new Impl (cleanup);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::ThreadLocalStorage::ThreadLocalStorage(void (*)(void*))");
    throw;
  }
}

ThreadLocalStorage::~ThreadLocalStorage ()
{
}

/*
    ��������� / ������ ������
*/

void ThreadLocalStorage::SetValue (void* data)
{
  try
  {
    Platform::SetTls (impl->key, data);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::ThreadLocalStorage::SetValue");
    throw;
  }
}

void* ThreadLocalStorage::GetValue () const
{
  try
  {
    return Platform::GetTls (impl->key);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::ThreadLocalStorage::GetValue");
    throw;
  }
}
