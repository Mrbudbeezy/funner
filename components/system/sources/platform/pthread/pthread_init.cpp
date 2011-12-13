#include "shared.h"

//TODO: �������� ���⪨ ����ᮢ ����� pthread_win32_process_detach_np () !!!!!

namespace syslib
{

#ifdef _WIN32

//�㭪�� ���樠����樨 ������⥪�
void thread_init ()
{
  static bool is_process_initialized = false;
  
  if (!is_process_initialized)
  {
    pthread_win32_process_attach_np ();
    
    is_process_initialized = true;
  }

  static __declspec(thread) bool is_thread_initialized = false;

  if (is_thread_initialized)
    return;

  pthread_win32_thread_attach_np ();

  is_thread_initialized = true;
}

//�㭪�� �����樠����樨 ������⥪�
void thread_done (void*)
{
  pthread_win32_thread_detach_np ();
}

#endif

}
