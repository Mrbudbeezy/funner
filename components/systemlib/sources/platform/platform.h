#ifndef SYSLIB_PLATFORM_HEADER
#define SYSLIB_PLATFORM_HEADER

#include <syslib/window.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowFlag
{
  WindowFlag_Visible, //��������� ����
  WindowFlag_Active,  //���������� ����
  WindowFlag_Focus    //����� ����� ����
};

#ifdef _WIN32
  #include <platform/win32/platform.h>
  
  typedef Win32Platform Platform;
#else
  #include <platform/default/platform.h>
  
  typedef DefaultPlatform Platform;
#endif

}

#endif
