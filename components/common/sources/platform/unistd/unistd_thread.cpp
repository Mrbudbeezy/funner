#include <pthread.h>

#include <platform/platform.h>

using namespace common;

/*
    ��������� �������������� ������� ����
*/
   
UnistdPlatform::threadid_t UnistdPlatform::GetCurrentThreadId ()
{
  return (UnistdPlatform::threadid_t)pthread_self ();
}
