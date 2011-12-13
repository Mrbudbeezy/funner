#include "../shared.h"

using namespace syslib;
using namespace syslib::bada;

struct syslib::mutex_handle
{
  Osp::Base::Runtime::Mutex mutex;
};

//�������� ������������ ��������
mutex_t BadaThreadManager::CreateMutex ()
{
  try
  {
    stl::auto_ptr<mutex_handle> handle (new mutex_handle);

    result r = handle->mutex.Create ();

    if (IsFailed (r))
      raise ("Osp::Base::Runtime::Mutex::Create", r);

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaThreadManager::CreateMutex");
    throw;
  }
}

//�������� ������������ ��������
void BadaThreadManager::DestroyMutex (mutex_t handle)
{
  if (!handle)
    return;

  delete handle;
}

//������ ������������ ��������
void BadaThreadManager::LockMutex (mutex_t handle)
{
  LockMutex (handle, INFINITE);
}

//������ ������������ �������� � ��������� ������������� ������� ��������
bool BadaThreadManager::LockMutex (mutex_t handle, size_t wait_in_milliseconds)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    result r = handle->mutex.Acquire (wait_in_milliseconds);

    if (IsFailed (r))
      raise ("Osp::Base::Runtime::Mutex::Acquire", r);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaThreadManager::LockMutex(mutex_t, size_t)");
    throw;
  }
}

//������� ������� ������������ ��������
bool BadaThreadManager::TryLockMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    result r = handle->mutex.Acquire (0);   //!!!!!!!!need testing

    switch (r)
    {
      case E_SUCCESS: return true;
      case E_TIMEOUT: return false;
      default:
        raise ("Osp::Base::Runtime::Mutex::Acquire", r);
        return false;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaThreadManager::TryLockMutex");
    throw;
  }
}

//������������ ������������ ��������
void BadaThreadManager::UnlockMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    result r = handle->mutex.Release ();

    if (IsFailed (r))
      raise ("Osp::Base::Runtime::Mutex::Release", r);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaThreadManager::UnlockMutex");
    throw;
  }
}
