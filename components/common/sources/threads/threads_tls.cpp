#include "shared.h"

using namespace common;

/*
    �������� ��������� ���������� ��������� ������
*/

struct ThreadLocalStorage::Impl: public IThreadCleanupCallback
{
  ICustomThreadSystem::tls_t key;     //���� ��������� ������ ����
  CleanupHandler             cleanup; //��������� ����������
  
///�����������
  Impl (const CleanupHandler& in_cleanup) : cleanup (in_cleanup)
  {
    key = Platform::GetThreadSystem ()->CreateTls (this);
  }
  
///����������
  ~Impl ()
  {
    Platform::GetThreadSystem ()->DeleteTls (key);
  }

///���������� � ������������� ������� ������
  void Cleanup (void* data)
  {
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

ThreadLocalStorage::ThreadLocalStorage (const CleanupHandler& cleanup)
{
  try
  {
    impl = new Impl (cleanup);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::ThreadLocalStorage::ThreadLocalStorage");
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
    exception.touch ("common::ThreadLocalStorage::ThreadLocalStorage");
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
    Platform::GetThreadSystem ()->SetTls (impl->key, data);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::ThreadLocalStorage::SetValue");
    throw;
  }
}

void* ThreadLocalStorage::GetValue () const
{
  try
  {
    return Platform::GetThreadSystem ()->GetTls (impl->key);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::ThreadLocalStorage::GetValue");
    throw;
  }
}
