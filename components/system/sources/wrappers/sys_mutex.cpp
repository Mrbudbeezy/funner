#include "shared.h"

using namespace syslib;

/*
    Описание реализации исключающего семафора
*/

struct Mutex::Impl
{
  mutex_t handle; //дескриптор исключающего семафора

///Конструктор
  Impl ()
  {
    handle = Platform::CreateMutex ();
  }
  
///Деструктор
  ~Impl ()
  {
    try
    {
      Platform::DestroyMutex (handle);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
};

/*
    Конструкторы / деструктор / присваивание
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
    Блокирование доступа
*/

bool Mutex::TryLock ()
{
  try
  {
    return Platform::TryLockMutex (impl->handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Mutex::TryLock()");
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
    if (!Platform::LockMutex (impl->handle, wait_in_milliseconds))
      throw xtl::format_operation_exception ("", "Mutex lock timeout");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Mutex::Lock(size_t)");
    throw;
  }
}

bool Mutex::TryLock (size_t wait_in_milliseconds)
{
  try
  {
    return Platform::LockMutex (impl->handle, wait_in_milliseconds);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Mutex::TryLock(size_t)");
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

void* Mutex::Handle ()
{
  return impl->handle;
}
