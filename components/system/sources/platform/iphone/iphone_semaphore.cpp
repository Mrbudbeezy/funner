#include "shared.h"

using namespace syslib;

//�������� ��������
Platform::semaphore_t Platform::CreateSemaphore (size_t initial_value)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::CreateSemaphore");
}

//����������� ��������
void Platform::DestroySemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::DestroySemaphore");
}

//�������� ��������� ������
void Platform::WaitSemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::WaitSemaphore (semaphore_t)");
}

//�������� ��������� ������ � ���������
bool Platform::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::WaitSemaphore (semaphore_t, size_t)");
}

//������� �������� ��������� ������
bool Platform::TryWaitSemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::TryWaitSemaphore");
}

//������� ��������� ������
void Platform::PostSemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::PostSemaphore");
}
