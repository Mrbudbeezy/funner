#include <time.h>
#include <platform/platform.h>
#include <common/exception.h>

using namespace syslib;

/*
    ������������ ���������� ���������� �� miliseconds ����������
*/

void Platform::Sleep (size_t miliseconds)
{
  clock_t end = clock () + CLOCKS_PER_SEC / 1000 * miliseconds;
  
  while (clock () < end);
}

/*
    �������� / ����������� �������
*/

Platform::timer_t Platform::CreateTimer (size_t, TimerHandler, void*)
{
  common::raise_not_implemented ("syslib::DefaultPlatform::CreateTimer");
}

void Platform::KillTimer (timer_t)
{
  common::raise_not_implemented ("syslib::DefaultPlatform::KillTimer");
}
