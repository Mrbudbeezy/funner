#include "shared.h"

using namespace syslib;
using namespace common;

namespace
{

/*
    ���������
*/

const char* LOG_NAME_PREFIX = "system.threads"; //��� ������ ����������������

///�������� ��������� �����
class ThreadManager
{
  public:
    ///���������� / �������� / ����� ���� �� ��������������
    void AddThread (size_t thread_id, Thread* thread)
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

    Thread* GetThread (size_t thread_id)
    {
      ThreadsMap::iterator iter = threads_map.find (thread_id);

      if (iter == threads_map.end ())
        throw xtl::make_argument_exception ("syslib::ThreadManager::GetThread", "thread_id", thread_id, "No such thread found");

      return iter->second;
    }

  private:
    typedef stl::hash_map<size_t, Thread*> ThreadsMap;

  private:
    ThreadsMap threads_map;
};

typedef common::Singleton <ThreadManager> ThreadManagerSingleton;

}

/*
    �������� ���������� ����
*/

struct Thread::Impl: public IThreadCallback, public xtl::reference_counter, public Lockable
{
  typedef xtl::lock_ptr<Impl, xtl::intrusive_ptr<Impl> > LockPtr;

  Function           thread_function;  //������� ����
  Platform::thread_t handle;           //������������� ����
  size_t             id;               //��������� ������������� ����
  stl::string        name;             //��� ����
  int                exit_code;        //��� ������ ����
  bool               cancel_requested; //��� �� ������� ������ �� ������ ����

///�����������
  Impl (const char* in_name, const Function& in_thread_function)
    : thread_function (in_thread_function)
    , name (in_name)
    , cancel_requested (false)    
  {
    try
    {
      addref (this);
      
      handle = Platform::CreateThread (this);
    }
    catch (...)
    {
      release (this);
      throw;
    }

    id = Platform::GetThreadId (handle);
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
      release (this); //����������� ���������� ������ � ������������
      
      try
      {
        id        = Platform::GetCurrentThreadId ();
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

Thread::Thread (const Function& thread_function)
{
  try
  {
    static size_t thread_auto_counter = 0;

    ++thread_auto_counter;

    impl = new Impl (format ("Thread%u", thread_auto_counter).c_str (), thread_function);

    ThreadManagerSingleton::Instance manager = ThreadManagerSingleton::Instance ();

    manager->AddThread (Platform::GetThreadId (impl->handle), this);
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

    ThreadManagerSingleton::Instance manager = ThreadManagerSingleton::Instance ();

    manager->AddThread (Platform::GetThreadId (impl->handle), this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Thread::Thread(const char*, const Function&, ThreadState)");
    throw;
  }
}

Thread::~Thread ()
{
  common::Lock lock (*impl);

  ThreadManagerSingleton::Instance manager = ThreadManagerSingleton::Instance ();

  manager->RemoveThread (Platform::GetThreadId (impl->handle));

  release (impl);
}

/*
    ������������� ����
*/

Thread::threadid_t Thread::Id () const
{
  return Impl::LockPtr (impl)->id;
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
  Impl::LockPtr (impl)->cancel_requested = true;
}

/*
    �������� ���������� ����
*/

int Thread::Join ()
{
  Impl::LockPtr thread_impl (impl);

  Platform::JoinThread (thread_impl->handle);

  return thread_impl->exit_code;
}

/*
    ��������� ������� ����
*/

Thread& Thread::GetCurrent ()
{
  try
  {
    ThreadManagerSingleton::Instance manager = ThreadManagerSingleton::Instance ();

    return *(manager->GetThread (Platform::GetCurrentThreadId ()));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Thread::GetCurrent");
    throw;
  }
}

/*
   ��������� ����� ������ ����
*/

void Thread::TestCancel ()
{
  Impl::LockPtr current_thread_impl;

  try
  {
    current_thread_impl = GetCurrent ().impl;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Thread::TestCancel");
    throw;
  }

  if (!current_thread_impl->cancel_requested)
    return;

  current_thread_impl->exit_code = THREAD_CANCELED_EXIT_CODE;

  throw xtl::format_exception <cancel_thread_exception> ("syslib::Thread::TestCancel", "Thread cancelled");
}
