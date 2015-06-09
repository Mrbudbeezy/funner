#include "shared.h"

//TODO: �������� ������� �������� �������� pthread_win32_process_detach_np () !!!!!

namespace syslib
{

#ifdef _WIN32

//������� ������������� ����������
void thread_init ()
{
  static bool is_process_initialized = false;
  
  if (!is_process_initialized)
  {
    pthread_win32_process_attach_np ();
    
    is_process_initialized = true;
  }

//!!!!!!!!!!!! �� �������� !!!!!!!!!!!!!
/*  static __declspec(thread) bool is_thread_initialized = false;

  if (is_thread_initialized)
    return;
*/
  pthread_win32_thread_attach_np ();

//  is_thread_initialized = true;
}

//������� ��������������� ����������
void thread_done (void*)
{
  pthread_win32_thread_detach_np ();
}

#elif defined (ANDROID)

void thread_init ()
{
}

void thread_done (void*)
{
  syslib::android::get_vm ()->DetachCurrentThread ();
}

#endif

}
