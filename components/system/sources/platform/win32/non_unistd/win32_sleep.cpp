#include "../shared.h"

using namespace syslib;

/*
    ������������ ���������� ���������� �� miliseconds ����������
*/

void Platform::Sleep (size_t miliseconds)
{
  SetLastError (0);
  ::Sleep (miliseconds);
}
