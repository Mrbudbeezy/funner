#include <stl/hash_map>

#include <xtl/common_exceptions.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <platform/platform.h>

#define _WIN32_WINNT 0x0600 //��� ������������� WM_MOUSEWHEEL
#include <windows.h>

namespace syslib
{

//�������� ������ ������������� WinAPI � ��������� ���������� � ������ �� �������
void check_errors (const char* source);
void raise_error  (const char* source);

}
