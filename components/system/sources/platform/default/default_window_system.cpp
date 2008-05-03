#include <platform/platform.h>
#include <common/exception.h>

using namespace syslib;

/*
    ��������� ����������: ������ � ������ ���������� ��� ��������� �� ���������
*/

namespace
{

void Raise (const char* method_name)
{
  common::RaiseNotSupported (method_name, "No window support for default platform");
}

}

/*
    ��������/��������/����������� ����
*/

Platform::window_t Platform::CreateWindow (WindowStyle, WindowMessageHandler, void*)
{
  Raise ("syslib::DefaultPlatform::CreateWindow");
  
  return 0;
}

void Platform::CloseWindow (window_t)
{
  Raise ("syslib::DefaultPlatform::CloseWindow");
}

void Platform::DestroyWindow (window_t)
{
  Raise ("syslib::DefaultPlatform::DestroyWindow");
}
    
/*
    ��������� ����
*/

void Platform::SetWindowTitle (window_t, const char*)
{
  Raise ("syslib::DefaultPlatform::SetWindowTitle");
}

void Platform::SetWindowTitle (window_t, const wchar_t*)
{
  Raise ("syslib::DefaultPlatform::SetWindowTitle");
}

void Platform::GetWindowTitle (window_t, size_t, char*)
{
  Raise ("syslib::DefaultPlatform::GetWindowTitle");
}

void Platform::GetWindowTitle (window_t, size_t, wchar_t*)
{
  Raise ("syslib::DefaultPlatform::GetWindowTitle");
}

/*
    ������� ���� / ���������� �������
*/

void Platform::SetWindowRect (window_t, const Rect&)
{
  Raise ("syslib::DefaultPlatform::SetWindowRect");
}

void Platform::GetWindowRect (window_t, Rect&)
{
  Raise ("syslib::DefaultPlatform::GetWindowRect");  
}

void Platform::GetClientRect (window_t, Rect&)
{
  Raise ("syslib::DefaultPlatform::GetClientRect");
}

/*
    ��������� ������ ����
*/

void Platform::SetWindowFlag (window_t, WindowFlag, bool)
{
  Raise ("syslib::DefaultPlatform::SetWindowFlag");
}

bool Platform::GetWindowFlag (window_t, WindowFlag)
{
  Raise ("syslib::DefaultPlatform::GetWindowFlag");
  
  return false;
}

/*
    ��������� �������
*/

void Platform::SetCursorPosition (const Point&)
{
  Raise ("syslib::DefaultPlatform::SetCursorPosition");
}

Point Platform::GetCursorPosition ()
{
  Raise ("syslib::DefaultPlatform::GetCursorPosition");
  
  return Point ();
}
