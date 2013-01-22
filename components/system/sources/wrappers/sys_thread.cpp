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

namespace syslib
{

///�������� ��������� �����
class ThreadManager
{
  public:
    ///���������� / �������� / ����� ���� �� ��������������
    void AddThread (size_t thread_id, Thread::Impl* thread)
    {
      ThreadsMap::iterator iter = threads_map.find (thread_id);

      if (iter != threads_map.end ())
        throw xtl::make_argument_exception ("syslib::ThreadManager::AddThread", "thread_id", thread_id, "Thread for this id already registered");

      threads_map.insert_pair (thread_id, thread);
    }

    void RemoveThread (size_t thread_id)
    {
      threads_map.erase (thread_id);
    }

    xtl::com_ptr<Thread::Impl> GetThread (size_t thread_id)
    {
      ThreadsMap::iterator iter = threads_map.find (thread_id);

      if (iter == threads_map.end ())
        throw xtl::make_argument_exception ("syslib::ThreadManager::GetThread", "thread_id", thread_id, "No such thread found");

      return iter->second;
    }

  private:
    typedef stl::hash_map<size_t, Thread::Impl*> ThreadsMap;

  private:
    ThreadsMap threads_map;
};

typedef common::Singleton<ThreadManager> ThreadManagerSingleton;

}

/*
    �������� ���������� ����
*/

struct Thread::Impl: public IThreadCallback
{
  Function               thread_function;  //������� ����
  thread_t               handle;           //������������� ����
  size_t                 id;               //��������� ������������� ����
  stl::string            name;             //��� ����
  int                    exit_code;        //��� ������ ����
  bool                   cancel_requested; //��� �� ������� ������ �� ������ ����
  xtl::reference_counter ref_count;        //������� ������

///�����������
  Impl (const char* in_name, const Function& in_thread_function)
    : thread_function (in_thread_function)
    , name (in_name)
    , exit_code ()
    , cancel_requested (false)    
  {
    xtl::com_ptr<Impl> self_lock (this);

    try
    {
      AddRef ();
      
      handle = Platform::CreateThread (this);
      id     = Platform::GetThreadId (handle);

      ThreadManagerSingleton::Instance ()->AddThread (id, this);
    }
    catch (...)
    {
      Release ();
      throw;
    }
  }

///����������
  ~Impl ()
  {
    try
    {
      Platform::DestroyThread (handle);
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
      xtl::com_ptr<Impl> self_lock (this);

      Release (); //����������� ���������� ������ � ������������

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
  void AddRef () { ref_count.increment (); }

  void Release ()
  {
    if (!ref_count.decrement ())
      return;

    ThreadManagerSingleton::Instance instance;

    ref_count.increment ();

    if (!ref_count.decrement ())
      return;

    instance->RemoveThread (id);

    delete this;    
  }
};

/*
    ������������ / ���������� / ������������
*/

Thread::Thread (const Function& thread_function)
{
  try
  {
    static volatile int thread_auto_counter = 0;

    xtl::atomic_increment (thread_auto_counter);

    impl = new Impl (format ("Thread%u", thread_auto_counter).c_str (), thread_function);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Thread::Thread(const Function&)");
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
    exception.touch ("syslib::Thread::Thread(const char*, const Function&)");
    throw;
  }
}

Thread::Thread (Impl* in_impl)
  : impl (in_impl)
{
  impl->AddRef ();
}

Thread::Thread (const Thread& thread)
  : impl (thread.impl)
{
  impl->AddRef ();
}

Thread::~Thread ()
{
  impl->Release ();
}

/*
    ������������� ����
*/

Thread::threadid_t Thread::Id () const
{
  return impl->id;
}

/*
    ��� ����
*/

const char* Thread::Name () const
{
  return impl->handle ? impl->name.c_str () : "<nil>";
}

/*
   ��������� ���������� ����
*/

void Thread::SetPriority (ThreadPriority thread_priority)
{
  Platform::SetThreadPriority (impl->handle, thread_priority);
}

/*
    ������ ����
*/

void Thread::Cancel ()
{
  impl->cancel_requested = true;
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

Thread Thread::GetCurrent ()
{
  try
  {
    return Thread (&*ThreadManagerSingleton::Instance ()->GetThread ((Platform::GetCurrentThreadId ())));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Thread::GetCurrent");
    throw;
  }
}

Thread::threadid_t Thread::GetCurrentThreadId ()
{
  try
  {
    return Platform::GetCurrentThreadId ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Thread::GetCurrentThreadId");
    throw;
  }
}

/*
   ��������� ����� ������ ����
*/

void Thread::TestCancel ()
{
  try
  {
    Thread thread = GetCurrent ();

    if (!thread.impl->cancel_requested)
      return;

    thread.impl->exit_code = THREAD_CANCELED_EXIT_CODE;

    throw xtl::format_exception <cancel_thread_exception> ("", "Thread has been cancelled");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Thread::TestCancel");
    throw;
  }
}
