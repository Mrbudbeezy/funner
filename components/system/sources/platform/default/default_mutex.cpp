#include "shared.h"

using namespace syslib;

/*
    ��������� ����������: ������ � ��������������� ���������� ��� ��������� �� ���������
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No mutex support for this platform");
}

}

//�������� ������������ ��������
mutex_t DefaultThreadManager::CreateMutex ()
{
  raise ("syslib::DefaultThreadManager::CreateMutex");
  return 0;
}

//�������� ������������ ��������
void DefaultThreadManager::DestroyMutex (mutex_t handle)
{
  raise ("syslib::DefaultThreadManager::DestroyMutex");
}

//������ ������������ ��������
void DefaultThreadManager::LockMutex (mutex_t handle)
{
  raise ("syslib::DefaultThreadManager::LockMutex");
}

//������ ������������ �������� � ��������� ������������� ������� ��������
bool DefaultThreadManager::LockMutex (mutex_t handle, size_t wait_in_milliseconds)
{
  raise ("syslib::DefaultThreadManager::LockMutex");
  return false;
}

//������� ������� ������������ ��������
bool DefaultThreadManager::TryLockMutex (mutex_t handle)
{
  raise ("syslib::DefaultThreadManager::TryLockMutex");
  return false;
}

//������������ ������������ ��������
void DefaultThreadManager::UnlockMutex (mutex_t handle)
{
  raise ("syslib::DefaultThreadManager::UnlockMutex");
}
