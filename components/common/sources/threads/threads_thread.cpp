#include "shared.h"

using namespace common;

namespace
{

/*
    ���������
*/

const char* LOG_NAME_PREFIX = "common.threads"; //��� ������ ����������������

}

/*
    �������� ���������� ����
*/

struct Thread::Impl: public IThreadCallback, public xtl::reference_counter
{
  Function                      thread_function; //������� ����
  ICustomThreadSystem::thread_t handle;          //������������� ����
  stl::string                   name;            //��� ����
  int                           exit_code;       //��� ������ ����

///������������
  Impl () : handle (0), exit_code (0) {}  
  
  Impl (const char* in_name, const Function& in_thread_function, ThreadState initial_state)
    : thread_function (in_thread_function),
      name (in_name)
  {
      //�������� ������������ ����������

    switch (initial_state)
    {
      case ThreadState_Suspended:
      case ThreadState_Worked:     
        break;
      case ThreadState_Exited:
        throw xtl::make_argument_exception ("", "initial_state", "ThreadState_Exited", "Can't create thread");
      default:
        throw xtl::make_argument_exception ("", "initial_state", initial_state);
    }    

      //�������� ����

    handle = Platform::GetThreadSystem ()->CreateThread (this, initial_state);
  }
  
///����������
  ~Impl ()
  {
    try
    {
      Platform::GetThreadSystem ()->DeleteThread (handle);
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

Thread::Thread (const Function& thread_function, ThreadState initial_state)
{
  try
  {
    static size_t thread_auto_counter = 0;

    ++thread_auto_counter;

    impl = new Impl (format ("Thread%u", thread_auto_counter).c_str (), thread_function, initial_state);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Thread::Thread(const Function&, ThreadState)");
    throw;
  }
}

Thread::Thread (const char* name, const Function& thread_function, ThreadState initial_state)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    impl = new Impl (name, thread_function, initial_state);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Thread::Thread(const char*, const Function&, ThreadState)");
    throw;
  }
}

Thread::Thread (const Thread& thread)
  : impl (thread.impl)
{
  addref (impl);
}

Thread::~Thread ()
{
  release (impl);
}

Thread& Thread::operator = (const Thread& thread)
{
  Thread (thread).Swap (*this);

  return *this;
}

/*
    ��� ����
*/

const char* Thread::Name () const
{
  return impl->name.c_str ();
}

/*
    ���������� ���������� ����������
*/

ThreadState Thread::State () const
{
  if (!impl->handle)
    return ThreadState_Exited;
    
  try
  {    
    return Platform::GetThreadSystem ()->GetThreadState (impl->handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Thread::State");
    throw;
  }
}

void Thread::SetState (ThreadState state)
{
  try
  {
    switch (state)
    {
      case ThreadState_Suspended:
      case ThreadState_Worked:     
      case ThreadState_Exited:
        break;
      default:
        throw xtl::make_argument_exception ("", "state", state);
    }

    if (!impl->handle)
    {
      if (state == ThreadState_Exited)
        return;

      throw xtl::format_operation_exception ("", "Can't set state on dummy thread");
    }    

    Platform::GetThreadSystem ()->SetThreadState (impl->handle, state);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Thread::SetState");
    throw;
  }
}

/*
    �������� ���������� ����
*/

int Thread::Join ()
{
  if (!impl->handle)
    return 0;    

  Platform::GetThreadSystem ()->JoinThread (impl->handle);

  return impl->exit_code;
}

/*
    �����
*/

void Thread::Swap (Thread& thread)
{
  stl::swap (impl, thread.impl);
}

namespace common
{

void swap (Thread& thread1, Thread& thread2)
{
  thread1.Swap (thread2);
}

}
