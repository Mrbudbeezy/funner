#include "shared.h"

using namespace syslib;

struct syslib::thread_handle
{  
  uintptr_t thread;
  size_t    id;
};

namespace
{

//������� ����
unsigned CALLBACK thread_run (void* data)
{
  if (!data)
    return 0;

  xtl::com_ptr<IThreadCallback> callback (reinterpret_cast<IThreadCallback*> (data));
  
  try
  {
    callback->Run ();
  }
  catch (...)
  {
    //���������� ���� ����������
  }

  try
  {
    cleanup_tls ();
  }
  catch (...)
  {
    //���������� ���� ����������
  }

  return 0;
}

}

/*
    �������� / �������� ����
*/

thread_t WindowsThreadManager::CreateThread (IThreadCallback* in_callback)
{
  try
  {
      //�������� ������������ ����������, ������ ������� ������������ ����

    if (!in_callback)
      throw xtl::make_null_argument_exception ("", "callback");

    xtl::com_ptr<IThreadCallback> callback (in_callback);

      //�������� ����

    stl::auto_ptr<thread_handle> handle (new thread_handle);

    handle->thread = _beginthreadex (0, 0, &thread_run, callback.get (), 0, &handle->id);

    if (!handle->thread)
      throw xtl::format_operation_exception ("::_beginthreadex", "%s", common::strerror (errno));

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsThreadManager::CreateThread");
    throw;
  }
}

void WindowsThreadManager::DestroyThread (thread_t thread)
{
  if (!thread || !thread->thread)
    return;

  CloseHandle ((HANDLE)thread->thread);

  delete thread;
}

/*
    �������� ���������� ����
*/

void WindowsThreadManager::JoinThread (thread_t thread)
{
  try
  {
    if (!thread || !thread->thread)
      throw xtl::make_null_argument_exception ("", "thread");

    if (WaitForSingleObject ((HANDLE)thread->thread, INFINITE) == WAIT_FAILED)
      raise_error ("::WaitForSingleObject");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsThreadManager::JoinThread");
    throw;
  }
}

/*
   ��������� �������������� ����
*/

size_t WindowsThreadManager::GetThreadId (thread_t thread)
{
  return (size_t)thread->id;
}

size_t WindowsThreadManager::GetCurrentThreadId ()
{
  return (size_t)::GetCurrentThreadId ();
}
