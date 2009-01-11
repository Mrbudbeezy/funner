#include "shared.h"

using namespace syslib;
using namespace common;

namespace
{

/*
    ���������
*/

const char* LOG_NAME_PREFIX = "system.threads"; //��� ������ ����������������

}

/*
    �������� ���������� ����
*/

struct Thread::Impl: public IThreadCallback, public xtl::reference_counter
{
  Function           thread_function; //������� ����
  Platform::thread_t handle;          //������������� ����
  stl::string        name;            //��� ����
  int                exit_code;       //��� ������ ����

///������������
  Impl () : handle (0), exit_code (0) {}

  Impl (const char* in_name, const Function& in_thread_function)
    : thread_function (in_thread_function),
      name (in_name)
  {
    handle = Platform::CreateThread (this);
  }

///����������
  ~Impl ()
  {
    try
    {
      Platform::DeleteThread (handle);
    }
    catch (...)
    {
      //���������� ���� ���������� 
    }
  }
  
///������ �� ����������
  void Run ()
  {
    try
    {
      try
      {
        exit_code = thread_function ();
      }
      catch (std::exception& exception)
      {
        Log (common::format ("%s.%s", LOG_NAME_PREFIX, name.c_str ()).c_str ()).Printf ("%s", exception.what ());
      }
      catch (...)
      {
        Log (common::format ("%s.%s", LOG_NAME_PREFIX, name.c_str ()).c_str ()).Printf ("unknown exception");
      }
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }

///������� ������
  void AddRef () { addref (this); }

  void Release () { release (this); }
};

/*
    ������������ / ���������� / ������������
*/

Thread::Thread ()
  : impl (new Impl)
{
}

Thread::Thread (const Function& thread_function)
{
  try
  {
    static size_t thread_auto_counter = 0;

    ++thread_auto_counter;

    impl = new Impl (format ("Thread%u", thread_auto_counter).c_str (), thread_function);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Thread::Thread(const Function&, ThreadState)");
    throw;
  }
}

Thread::Thread (const char* name, const Function& thread_function)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    impl = new Impl (name, thread_function);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Thread::Thread(const char*, const Function&, ThreadState)");
    throw;
  }
}

Thread::~Thread ()
{
  release (impl);
}

/*
    ��� ����
*/

/*const char* Thread::Name () const
{
  return impl->handle ? impl->name.c_str () : Platform::GetCurrentThreadName ();
}*/

/*
    ������ ����
*/

void Thread::Cancel ()
{
  try
  {  
    Platform::CancelThread (impl->handle);
    
    impl->exit_code = THREAD_CANCELED_EXIT_CODE;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Thread::Cancel");
    throw;
  }
}

/*
    �������� ���������� ����
*/

int Thread::Join ()
{
  Platform::JoinThread (impl->handle);

  return impl->exit_code;
}

/*
    ��������� ������� ����
*/

namespace syslib
{

struct CurrentThreadHolder
{
  Thread thread;
};

typedef Singleton<CurrentThreadHolder> CurrentThreadSingleton;

}

Thread& Thread::GetCurrent ()
{
  try
  {
    return CurrentThreadSingleton::Instance ().thread;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Thread::GetCurrent");
    throw;
  }
}
