#include "shared.h"

using namespace syslib::android;

extern "C"
{

int main (...);

}

namespace
{

/*
    ���������
*/

const char* THREAD_NAME             = "system.android.launcher";                     //��� ���� ����������
const char* ENGINE_UTILS_CLASS_NAME = "com/untgames/funner/application/EngineUtils"; //��� ������ java ������

/// �������� ������� ����������
ApplicationContext application_context;

/// ��������� ������� ����������
struct ApplicationStartArgs
{
  common::StringArray args;
  
  ApplicationStartArgs (const char* program_name, const char* in_args)
  {
      //������ ���������� �������
      
    args.Add (program_name);
    args.Add (common::split (in_args));    
  }
};

/// ���� ����������
class ApplicationThread: private ApplicationStartArgs
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
      : ApplicationStartArgs (program_name, in_args)
      , thread (THREAD_NAME, syslib::Thread::Function (xtl::bind (&ApplicationThread::ThreadRoutine, this)))
    {
    }
    
/// ������� ����
    int ThreadRoutine ()
    {
      try
      {
          //��������� ���������
        
        JNIEnv* env = 0;
        
        jint status = get_context ().vm->AttachCurrentThread (&env, 0);
        
        if (status)
          throw xtl::format_operation_exception ("", "JavaVM::AttachCurrentThread failed (status=%d)", status);          

          //�������� ���������� ���������
        
        int exit_code = 0;
        
        try
        {
          exit_code = main (args.Size (), args.Data ());          

//          get_vm ()->DetachCurrentThread ();
        }
        catch (...)
        {
//          get_vm ()->DetachCurrentThread ();          
          throw;
        }
        
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
      
      Exit (0);
      
      return 0;
    }
    
/// ������� ��������� JavaVM � ������������� ��������� ����������
    void Exit (int code)
    {
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
void start_application (JavaVM* vm, jobject activity, const char* program_name, const char* args)
{
  static const char* METHOD_NAME = "syslib::android::start_application";
  
    //�������� ������������ ����������

  if (!vm)
    throw xtl::make_null_argument_exception (METHOD_NAME, "vm");
    
  if (!activity)
    throw xtl::make_null_argument_exception (METHOD_NAME, "activity");    
    
  if (!program_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "program_name");
    
  if (!args)
    throw xtl::make_null_argument_exception (METHOD_NAME, "args");
    
    //���������� ��������� ������� ����������

  application_context.vm       = vm;
  application_context.activity = activity;
  
  application_context.utils_class = get_env ().FindClass (ENGINE_UTILS_CLASS_NAME);
  
  if (!application_context.utils_class)
    throw xtl::format_operation_exception ("", "EngineUtils class '%s' not found", ENGINE_UTILS_CLASS_NAME);    
  
    //������ ���� ����������
    
  ApplicationThread::Start (program_name, args);
}

/// ��������� ��������� ������� ����������
const ApplicationContext& get_context ()
{
  return application_context;
}

/// ��������� ����������� ������
JavaVM* get_vm ()
{
  return application_context.vm;
}

/// ��������� ��������� ������� ����
JNIEnv& get_env ()
{
  JNIEnv* env = 0;

  jint status = application_context.vm->GetEnv ((void**)&env, JNI_VERSION_1_4);
  
  if (status || !env)
    throw xtl::format_operation_exception ("syslib::android::get_env", "JavaVM::GetEnv failed (status=%d)", status);
    
  return *env;
}

}

}
