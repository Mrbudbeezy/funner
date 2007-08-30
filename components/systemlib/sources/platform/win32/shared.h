#include <platform/platform.h>
#include <common/exception.h>
#include <common/singleton.h>
#include <stl/hash_map>

#define _WIN32_WINNT 0x0501 //��� ������������� WM_MOUSEWHEEL
#include <windows.h>

namespace syslib
{

//����������: ������ ������������� WinAPI
struct WinAPIExceptionTag;

typedef common::DerivedException<common::Exception, WinAPIExceptionTag> WinAPIException;

//�������� ������ ������������� WinAPI � ��������� ���������� � ������ �� �������
void CheckErrors (const char* source);

}
