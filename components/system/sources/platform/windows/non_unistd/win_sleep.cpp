#include "../shared.h"

using namespace syslib;

/*
    ������������ ���������� ���������� �� miliseconds ����������
*/

void WindowsApplicationManager::Sleep (size_t miliseconds)
{
  SetLastError (0);
  ::Sleep (miliseconds);
}
