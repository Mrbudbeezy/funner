#include "shared.h"

using namespace syslib;

//�������� ������������ ��������
Platform::mutex_t Platform::CreateMutex ()
{
  raise ("syslib::DefaultPlatform::CreateMutex");
  return 0;
}

//�������� ������������ ��������
void Platform::DestroyMutex (mutex_t handle)
{
  raise ("syslib::DefaultPlatform::DestroyMutex");
}

//������ ������������ ��������
void Platform::LockMutex (mutex_t handle)
{
  raise ("syslib::DefaultPlatform::LockMutex");
}

//������ ������������ �������� � ��������� ������������� ������� �������
bool Platform::LockMutex (mutex_t handle, size_t wait_in_milliseconds)
{
  raise ("syslib::DefaultPlatform::LockMutex");
  return false;
}

//������� ������� ������������ ��������
bool Platform::TryLockMutex (mutex_t handle)
{
  raise ("syslib::DefaultPlatform::TryLockMutex");
  return false;
}

//������������ ������������ ��������
void Platform::UnlockMutex (mutex_t handle)
{
  raise ("syslib::DefaultPlatform::UnlockMutex");
}
