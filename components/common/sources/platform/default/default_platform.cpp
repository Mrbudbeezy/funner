#include "shared.h"

using namespace common;

/*
    ��������� ���������� �������������� ������
*/

ICustomAllocator* DefaultPlatform::GetSystemAllocator ()
{
  return &*Singleton<MallocAllocator>::Instance ();
}

/*
    ��������� �������� ������
*/

ICustomFileSystem* DefaultPlatform::GetFileSystem ()
{
  return &*Singleton<StdioFileSystem>::Instance ();
}

ICustomFileSystem* DefaultPlatform::GetIOSystem ()
{
  return &*Singleton<StdioIOSystem>::Instance ();
}

/*
    ������ � ������������
*/

void DefaultPlatform::InitLockable (lockable_t& lockable)
{
  lockable = 0;
}

void DefaultPlatform::DestroyLockable (lockable_t& lockable)
{
  lockable = 0;
}

void DefaultPlatform::Lock (lockable_t& lockable)
{
  static const char* METHOD_NAME = "common::DefaultPlatform::Lock";

  if (lockable)
    throw xtl::format_operation_exception (METHOD_NAME, "Recursive lock not supported");

  lockable = (void*)~0;
}

void DefaultPlatform::Unlock (lockable_t& lockable)
{
  lockable = 0;
}
    
/*
    ��������� ���������� �������
*/
    
size_t DefaultPlatform::GetMilliseconds ()
{
  return size_t (clock () * 1000.0 / CLOCKS_PER_SEC);
}

/*
    ��������� �������������� ������� ����
*/
   
DefaultPlatform::threadid_t DefaultPlatform::GetCurrentThreadId ()
{
  return 0;
}
