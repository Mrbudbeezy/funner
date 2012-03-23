#include "shared.h"

using namespace syslib::android;

namespace
{

/// ���������
const char* ENGINE_ACTIVITY_CLASS_NAME                    = "com/untgames/funner/application/EngineActivity";
const char* ENGINE_ACTIVITY_START_APPLICATION_METHOD_NAME = "startApplication";

/// ����������� ������
JavaVM* java_vm = 0;

jint JNICALL startApplication (JNIEnv* env, jobject thiz, jstring jprogram_name, jstring jwork_dir, jstring jprogram_args, jstring jenv_vars)
{
  if (!env)
  {  
    printf ("Bad JNIEnv passed to EngineActivity::startApplication\n");
    return 0;
  }

  if (!jprogram_name || !jprogram_args || !jwork_dir)
  {
    printf ("Bad arguments passed to EngineActivity::startApplication\n");
    return 0; 
  }

  jni_string program_name (env, jprogram_name), program_args (env, jprogram_args), work_dir (env, jwork_dir), env_vars (env, jenv_vars);

  printf ("Starting program '%s' (args='%s', workdir='%s', env='%s')\n", program_name.get (), program_args.get (), work_dir.get (), env_vars.get ());

  try
  {   
    if (chdir (work_dir.get ()))
    {
      printf ("Can't set current working dir to '%s'. %s\n", work_dir.get (), strerror (errno));

      return 0;
    }

    startStdioRedirection ();

    start_application (java_vm, thiz, program_name.get (), program_args.get (), env_vars.get ());

    return 1;
  }
  catch (std::exception& e)
  {
    log_error ("%s\n  at syslib::android::startApplication\n", e.what ());
    
    return 0;
  }
  catch (...)
  {
    log_error ("unknown exception appears\n  at syslib::android::startApplication\n");
    
    return 0;
  }
}

}

extern "C"
{

__attribute__ ((visibility("default"))) extern JNIEXPORT jint JNICALL JNI_OnLoad (JavaVM* vm, void* reserved)
{
  if (!vm)
  {
    printf ("JavaVM is null\n");
    fflush (stdout);

    return -1; 
  }

  java_vm = vm;

  JNIEnv* env;

  if (vm->GetEnv ((void**)&env, JNI_VERSION_1_4) != JNI_OK)
  {
    printf ("Can't get Java environment\n");
    fflush (stdout);    
    
    return -1;
  }

  jclass skeletonActivityClass = env->FindClass (ENGINE_ACTIVITY_CLASS_NAME); 
  
  if (!skeletonActivityClass)
  {
    printf ("Can't find EngineActivity class\n");
    fflush (stdout);
    
    return -1;
  }
  
  JNINativeMethod method;
  
  memset (&method, 0, sizeof (JNINativeMethod));
  
  method.name      = ENGINE_ACTIVITY_START_APPLICATION_METHOD_NAME;
  method.signature = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I";
  method.fnPtr     = (void*)&startApplication;

  jint status = env->RegisterNatives (skeletonActivityClass, &method, 1);

  if (status)
  {
    printf ("Can't register natives (status=%d)\n", status);
    fflush (stdout);
    
    return -1;
  }

  try
  {
    register_window_callbacks (env);
  }
  catch (std::exception& e)
  {
    printf ("Can't register window callbacks: %s\n", e.what ());
    fflush (stdout);

    return -1;    
  }
  catch (...)
  {
    printf ("Can't register window callbacks: unknown exception\n");
    fflush (stdout);

    return -1;    
  }  

  try
  {
    register_log_entries (env, skeletonActivityClass);
  }
  catch (std::exception& e)
  {
    printf ("Can't register log entries: %s\n", e.what ());
    fflush (stdout);

    return -1;    
  }
  catch (...)
  {
    printf ("Can't register log entries: unknown exception\n");
    fflush (stdout);

    return -1;    
  }    

  return JNI_VERSION_1_4;
}

}
