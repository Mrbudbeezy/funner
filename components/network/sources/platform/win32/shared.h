#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>

#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <platform/platform.h>

namespace network
{

//������������� ������ � �����
void init_networking ();

//�������� ������ ������������� WinAPI � ��������� ���������� � ������ �� �������
void check_errors (const char* source);
void raise_error  (const char* source);

//��������� ������ � ���������� �� ������
stl::string get_error_message (DWORD error_code);

}
