#include "shared.h"

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
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::CreateTimer");
}

void Platform::KillTimer (timer_t)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::KillTimer");
}
