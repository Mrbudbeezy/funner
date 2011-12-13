#ifndef SYSLIB_THREAD_HEADER
#define SYSLIB_THREAD_HEADER

#include <stl/auto_ptr.h>
#include <xtl/exception.h>
#include <xtl/functional_fwd>

namespace syslib
{

//forward declarations
struct CurrentThreadHolder;
class  Mutex;

struct cancel_thread_exception: virtual public xtl::exception {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const int THREAD_CANCELED_EXIT_CODE = -1; //��� ���������� ���������� ����

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ThreadPriority
{
  ThreadPriority_Low,
  ThreadPriority_Normal,
  ThreadPriority_High
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Thread
{
  friend struct CurrentThreadHolder;
  public:
    typedef size_t                threadid_t;
    typedef xtl::function<int ()> Function;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Thread  (const Function& thread_function);
    Thread  (const char* name, const Function& thread_function);
    ~Thread ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    threadid_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
//    const char* Name () const; //������� ����������� ������� � ��������� ������, ����� ���� ����� ���������, ������� badptr

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetPriority (ThreadPriority thread_priority);

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
    static Thread&    GetCurrent         ();
    static threadid_t GetCurrentThreadId ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void TestCancel ();

  private:
    struct Impl;

    Thread  (const Thread&); //no impl
    Thread& operator = (const Thread&); //no impl

  private:
    Impl* impl;
};

}

#endif
