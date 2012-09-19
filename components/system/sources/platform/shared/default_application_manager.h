#ifndef SYSLIB_DEFAULT_APPLICATION_MANAGER_HEADER
#define SYSLIB_DEFAULT_APPLICATION_MANAGER_HEADER

#include <stl/string>

#include <syslib/application_delegate.h>

namespace common
{

//forward declaration
class PropertyMap;

}

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DefaultApplicationManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������� ���� �� miliseconds ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Sleep (size_t miliseconds);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� URL �� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void OpenUrl (const char* url);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static stl::string GetEnvironmentVariable (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IApplicationDelegate* CreateDefaultApplicationDelegate ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenSaverState (bool state);
    static bool GetScreenSaverState ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void GetSystemProperties (common::PropertyMap&);
};

}

#endif
