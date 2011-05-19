#include "shared.h"

using namespace syslib;

//�������� ��������
semaphore_t IPhoneThreadManager::CreateSemaphore (size_t initial_value)
{
  throw xtl::make_not_implemented_exception ("syslib::IPhoneThreadManager::CreateSemaphore");
}

//����������� ��������
void IPhoneThreadManager::DestroySemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::IPhoneThreadManager::DestroySemaphore");
}

//�������� ��������� ������
void IPhoneThreadManager::WaitSemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::IPhoneThreadManager::WaitSemaphore (semaphore_t)");
}

//�������� ��������� ������ � ���������
bool IPhoneThreadManager::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  throw xtl::make_not_implemented_exception ("syslib::IPhoneThreadManager::WaitSemaphore (semaphore_t, size_t)");
}

//������� �������� ��������� ������
bool IPhoneThreadManager::TryWaitSemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::IPhoneThreadManager::TryWaitSemaphore");
}

//������� ��������� ������
void IPhoneThreadManager::PostSemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::IPhoneThreadManager::PostSemaphore");
}
