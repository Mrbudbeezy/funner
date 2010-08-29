#include <winsock2.h>
#include <windows.h>

#include <xtl/common_exceptions.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <platform/platform.h>

namespace network
{

//�������� ������ ������������� WinAPI � ��������� ���������� � ������ �� �������
void check_errors (const char* source);
void raise_error  (const char* source);

}
