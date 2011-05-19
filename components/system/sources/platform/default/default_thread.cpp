#include "shared.h"

using namespace syslib;

/*
    ��������� ����������: ������ � ��������������� ���������� ��� ��������� �� ���������
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No thread support for this platform");
}

}

/*
    �������� / �������� ����
*/

thread_t DefaultThreadManager::CreateThread (IThreadCallback*)
{
  raise ("syslib::DefaultThreadManager::CreateThread");
  return 0;
}

void DefaultThreadManager::DestroyThread (thread_t)
{
  raise ("syslib::DefaultThreadManager::DestroyThread");
}

/*
    �������� ���������� ����
*/

void DefaultThreadManager::JoinThread (thread_t)
{
  raise ("syslib::DefaultThreadManager::JoinThread");
}

/*
   ��������� �������������� ����
*/

size_t DefaultThreadManager::GetThreadId (thread_t thread)
{
  raise ("syslib::DefaultThreadManager::GetThreadId");
  return 0;
}

size_t DefaultThreadManager::GetCurrentThreadId ()
{
  return 0;
}
