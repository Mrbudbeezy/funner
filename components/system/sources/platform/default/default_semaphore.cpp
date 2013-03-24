#include "shared.h"

namespace syslib
{

/*
    ��������� ����������: ������ � ��������������� ���������� ��� ��������� �� ���������
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No semaphore support for this platform");
}

}

//�������� ��������
semaphore_t DefaultThreadManager::CreateSemaphore (size_t initial_value)
{
  raise ("syslib::DefaultThreadManager::CreateSemaphore");
  return 0;
}

//����������� ��������
void DefaultThreadManager::DestroySemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultThreadManager::DestroySemaphore");
}

//�������� ��������� ������
void DefaultThreadManager::WaitSemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultThreadManager::WaitSemaphore");
}

//�������� ��������� ������ � ���������
bool DefaultThreadManager::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  raise ("syslib::DefaultThreadManager::WaitSemaphore");
  return false;
}

//������� ������� ��������� ������
bool DefaultThreadManager::TryWaitSemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultThreadManager::TryWaitSemaphore");
  return false;
}

//������� ��������� ������
void DefaultThreadManager::PostSemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultThreadManager::PostSemaphore");
}

}
