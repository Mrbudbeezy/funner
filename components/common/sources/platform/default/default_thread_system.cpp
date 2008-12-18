#include "shared.h"

using namespace common;

/*
    �������� / �������� ����
*/

DefaultThreadSystem::thread_t DefaultThreadSystem::CreateThread (IThreadCallback*)
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::CreateThread", "Threads are not supported");
}

void DefaultThreadSystem::DeleteThread (thread_t)
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::DeleteThread", "Threads are not supported");
}

/*
    ���������� ���������� ������ ����
*/

void DefaultThreadSystem::CancelThread (thread_t)
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::CancelThread", "Threads are not supported");
}

/*
    �������� ���������� ����
*/

void DefaultThreadSystem::JoinThread (thread_t)
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::JointThread", "Threads are not supported");
}

/*
    ��������� ������� ����
*/

ICustomThreadSystem::thread_t DefaultThreadSystem::GetCurrentThread ()
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::GetCurrentThread", "Threads are not supported");
}

/*
    ������ � ������������ �������� ����
*/

void DefaultThreadSystem::EnterCriticalSection ()
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::EnterCriticalSection", "Threads are not supported");  
}

void DefaultThreadSystem::ExitCriticalSection ()
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::ExitCriticalSection", "Threads are not supported");  
}
