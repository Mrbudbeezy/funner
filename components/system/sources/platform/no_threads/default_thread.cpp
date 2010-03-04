#include "shared.h"

using namespace syslib;

/*
    �������� / �������� ����
*/

Platform::thread_t Platform::CreateThread (IThreadCallback*)
{
  raise ("syslib::DefaultPlatform::CreateThread");
  return 0;
}

void Platform::DestroyThread (thread_t)
{
  raise ("syslib::DefaultPlatform::DestroyThread");
}

/*
    �������� ���������� ����
*/

void Platform::JoinThread (thread_t)
{
  raise ("syslib::DefaultPlatform::JoinThread");
}

/*
   ��������� �������������� ����
*/

size_t Platform::GetThreadId (thread_t thread)
{
  raise ("syslib::DefaultPlatform::GetThreadId");
  return 0;
}

size_t Platform::GetCurrentThreadId ()
{
  return 0;
}
