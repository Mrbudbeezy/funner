#include <unistd.h>

#include <platform/platform.h>

using namespace syslib;

/*
    ������������ ���������� ���������� �� miliseconds ����������
*/

void Platform::Sleep (size_t miliseconds)
{
  usleep (miliseconds * 1000);
}

