#ifndef COMMONLIB_PLATFORM_WIN32_SHARED_HEADER
#define COMMONLIB_PLATFORM_WIN32_SHARED_HEADER

#ifndef _WIN32_WCE
  #define _WIN32_WINNT 0x0600
  #define _WIN32_IE 0x0400
#endif

#include <windows.h>
#include <shlobj.h>

#include <xtl/common_exceptions.h>

#include <common/component.h>
#include <common/file.h>
#include <common/strlib.h>

#include <platform/platform.h>

#if defined(WINCE)
   #include "common\utf_converter.h"
#endif

namespace common
{

//�������� ������ ������������� WinAPI � ��������� ���������� � ������ �� �������
void check_errors (const char* source);
void raise_error  (const char* source);

}

#endif
