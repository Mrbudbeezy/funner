#include "shared.h"

extern "C"
{

int main (...);

}

namespace
{

struct A
{
  A () { printf ("A::A\n"); fflush (stdout); }
  ~A () { printf ("A::~A\n"); fflush (stdout); }
};

A a;

/*
    ���������
*/

const char* THREAD_NAME = "system.android.launcher"; //��� ���� ����������

/// ������ ����������� ������
JavaVM* java_vm = 0;

/// ���� ����������
class ApplicationThread
{
  public:
/// ����� ������� ���� ����������
    static void Start (const char* program_name, const char* args)
    {
      static stl::auto_ptr<ApplicationThread> instance;
      
      if (instance)
        throw xtl::format_operation_exception ("syslib::android::ApplicationThread::Start", "Application thread has already started");
        
      instance = stl::auto_ptr<ApplicationThread> (new ApplicationThread (program_name, args));
    }

  private:
/// �����������
    ApplicationThread (const char* program_name, const char* in_args)
      : thread (THREAD_NAME, syslib::Thread::Function (xtl::bind (&ApplicationThread::ThreadRoutine, this)))
    {
        //������ ���������� �������
        
      args.Add (program_name);
      args.Add (common::split (in_args));   
    }
    
/// ������� ����
    int ThreadRoutine ()
    {
      try
      {
        int exit_code = main (args.Size (), args.Data ());
        
        Exit (exit_code);
      }
      catch (std::exception& e)
      {        
        printf ("%s\n  at syslib::android::ApplicationThread::ThreadRoutine", e.what ());
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unhandled exception\n  at syslib::android::ApplicationThread::ThreadRoutine");
        fflush (stdout);        
      }
      
      return 0;
    }
    
/// ������� ��������� JavaVM � ������������� ��������� ����������
    void Exit (int code)
    {
//      if (!java_vm)
//        throw xtl::format_operation_exception ("syslib::android::ApplicationThread::Exit", "Null JavaVM");        
      exit (code);
    }
  
  private:
    syslib::Thread       thread;
    common::StringArray  args;
};

}

namespace syslib
{

namespace android
{

/// ����� ����� � ����������
void start_application (JavaVM* vm, const char* program_name, const char* args)
{
  static const char* METHOD_NAME = "syslib::android::start_application";
  
    //�������� ������������ ����������

  if (!vm)
    throw xtl::make_null_argument_exception (METHOD_NAME, "vm");
    
  if (!program_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "program_name");
    
  if (!args)
    throw xtl::make_null_argument_exception (METHOD_NAME, "args");
    
    //���������� ������� ����������� ������    

  java_vm = vm;
  
    //������ ���� ����������
    
  ApplicationThread::Start (program_name, args);
}

/// ��������� ������� ����������� ������
JavaVM* get_vm ()
{
  return java_vm;
}

}

}
