#include "shared.h"

using namespace common;

/*
    ��������� �������������� ������� ����
*/
   
Win32Platform::threadid_t Win32Platform::GetCurrentThreadId ()
{
  return (threadid_t)::GetCurrentThreadId ();
}
