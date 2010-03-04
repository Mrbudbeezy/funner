#include "shared.h"

using namespace syslib;

//�������� ��������
Platform::semaphore_t Platform::CreateSemaphore (size_t initial_value)
{
  raise ("syslib::DefaultPlatform::CreateSemaphore");
  return 0;
}

//����������� ��������
void Platform::DestroySemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultPlatform::DestroySemaphore");
}

//�������� ��������� ������
void Platform::WaitSemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultPlatform::WaitSemaphore");
}

//�������� ��������� ������ � ���������
void Platform::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  raise ("syslib::DefaultPlatform::WaitSemaphore");
}

//������� ������� ��������� ������
bool Platform::TryWaitSemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultPlatform::TryWaitSemaphore");
  return false;
}

//������� ��������� ������
void Platform::PostSemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultPlatform::PostSemaphore");
}
