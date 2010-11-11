#include "../shared.h"

using namespace syslib;
using namespace syslib::bada;

struct Platform::semaphore_handle
{
  Osp::Base::Runtime::Semaphore semaphore;
};

//�������� ��������
Platform::semaphore_t Platform::CreateSemaphore (size_t initial_value)
{
  try
  {
    stl::auto_ptr<semaphore_handle> handle (new semaphore_handle);

    result r = handle->semaphore.Create (INT_MAX);

    if (IsFailed (r))
      raise ("Osp::Base::Runtime::Semaphore::Create", r);

    for (size_t i = 0; i < initial_value; i++)
      WaitSemaphore (handle.get ());

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaPlatform::CreateSemaphore");
    throw;
  }
}

//����������� ��������
void Platform::DestroySemaphore (semaphore_t handle)
{
  if (!handle)
    return;

  delete handle;
}

//�������� ��������� ������
void Platform::WaitSemaphore (semaphore_t handle)
{
  WaitSemaphore (handle, INFINITE);
}

//�������� ��������� ������ � ���������
bool Platform::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    result r = handle->semaphore.Acquire (wait_in_milliseconds);

    if (IsFailed (r))
      raise ("Osp::Base::Runtime::Semaphore::Acquire", r);
      
    return true;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaPlatform::WaitSemaphore (semaphore_t, size_t)");
    throw;
  }
}

//������� �������� ��������� ������
bool Platform::TryWaitSemaphore (semaphore_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    result r = handle->semaphore.Acquire (0);   //!!!!!!!!need testing

    switch (r)
    {
      case E_SUCCESS: return true;
      case E_TIMEOUT: return false;
      default:
        raise ("Osp::Base::Runtime::Semaphore::Acquire", r);
        return false;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaPlatform::TryLockSemaphore");
    throw;
  }
}

//������� ��������� ������
void Platform::PostSemaphore (semaphore_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    result r = handle->semaphore.Release ();

    if (IsFailed (r))
      raise ("Osp::Base::Runtime::Semaphore::Release", r);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaPlatform::PostSemaphore");
    throw;
  }
}
