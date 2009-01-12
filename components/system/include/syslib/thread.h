#ifndef SYSLIB_THREAD_HEADER
#define SYSLIB_THREAD_HEADER

#include <stl/auto_ptr.h>
#include <xtl/functional_fwd>

namespace syslib
{

//forward declarations
struct CurrentThreadHolder;
class  Mutex;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const int THREAD_CANCELED_EXIT_CODE = -1; //�� ���������� ���������� ����

///////////////////////////////////////////////////////////////////////////////////////////////////
///����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Thread
{
  friend struct CurrentThreadHolder;
  public:
    typedef xtl::function<int ()> Function;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Thread  (const Function& thread_function);
    Thread  (const char* name, const Function& thread_function);
    ~Thread ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
//    const char* Name () const; //������� ����������� ������� � ��������� ������, ����� ���� ����� ���������, ������� badptr

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Cancel ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    int Join (); //return exit code

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Thread& GetCurrent ();
    
  private:
    struct Impl;

    Thread ();
    Thread  (const Thread&); //no impl
    Thread& operator = (const Thread&); //no impl

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ThreadLocalStorage
{
  public:
    typedef xtl::function<void (void* data)> CleanupHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ThreadLocalStorage  ();
    ThreadLocalStorage  (const CleanupHandler& cleanup);
    ThreadLocalStorage  (void (*cleanup)(void*));
    ~ThreadLocalStorage ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetValue (void* data);
    void* GetValue () const;

  private:
    ThreadLocalStorage (const ThreadLocalStorage&); //no impl
    ThreadLocalStorage& operator = (const ThreadLocalStorage&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ��������� �� ��������� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class ThreadLocalPointer
{
  public:
    typedef xtl::function<void (T* data)> CleanupHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ThreadLocalPointer ();
    ThreadLocalPointer (const CleanupHandler& cleanup);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    T* Get         () const;
    T* operator -> () const;
    T& operator *  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset (T* new_object); //????

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� ��� �������� ������ (������ cleanup)
///////////////////////////////////////////////////////////////////////////////////////////////////
    T* Release ();

  private:
    ThreadLocalPointer (const ThreadLocalPointer&); //no impl
    ThreadLocalPointer& operator = (const ThreadLocalPointer&); //no impl
    
  private:
    ThreadLocalStorage storage;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ ���� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ThreadCondition
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ThreadCondition  ();
    ~ThreadCondition ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� �� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void NotifyOne ();
    void NotifyAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
                          void Wait (Mutex& lock);
    template <class Pred> void Wait (Mutex& lock, Pred pred);

  private:
    ThreadCondition  (const ThreadCondition&); //no impl
    ThreadCondition& operator = (const ThreadCondition&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������� �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class ThreadLockScope
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ThreadLockScope  (T& object);
    ~ThreadLockScope ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    T* Get         () const;
    T* operator -> () const;

  private:
    ThreadLockScope (const ThreadLockScope&); //no impl
    ThreadLockScope& operator = (const ThreadLockScope&); //no impl

  private:
    T* object;
};

#include <syslib/detail/thread.inl>

}

#endif
