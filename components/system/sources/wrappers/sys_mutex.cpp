#include "shared.h"

using namespace syslib;

/*
    ���ᠭ�� ॠ����樨 �᪫���饣� ᥬ���
*/

struct Mutex::Impl
{
  Platform::mutex_t handle; //���ਯ�� �᪫���饣� ᥬ���

///���������
  Impl ()
  {
    handle = Platform::CreateMutex ();
  }
  
///��������
  ~Impl ()
  {
    try
    {
      Platform::DestroyMutex (handle);
    }
    catch (...)
    {
      //���������� ��� �᪫�祭��
    }
  }
};

/*
    ���������� / �������� / ��ᢠ������
*/

Mutex::Mutex ()
{
  try
  {
    impl = new Impl;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Mutex::Mutex");
    throw;
  }
}

Mutex::~Mutex ()
{
}

/*
    �����஢���� ����㯠
*/

bool Mutex::TryLock ()
{
  try
  {
    return Platform::TryLockMutex (impl->handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Mutex::TryLock");
    throw;
  }
}

void Mutex::Lock ()
{
  try
  {
    Platform::LockMutex (impl->handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Mutex::Lock()");
    throw;
  }
}

void Mutex::Lock (size_t wait_in_milliseconds)
{
  try
  {
    Platform::LockMutex (impl->handle, wait_in_milliseconds);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Mutex::Lock(size_t)");
    throw;
  }
}

void Mutex::Unlock ()
{
  try
  {
    Platform::UnlockMutex (impl->handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Mutex::Unlock");
    throw;
  }
}
