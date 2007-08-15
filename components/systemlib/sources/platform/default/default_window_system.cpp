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

DefaultPlatform::window_t DefaultPlatform::CreateWindow (WindowStyle, WindowMessageHandler, void*)
{
  Raise ("syslib::DefaultPlatform::CreateWindow");
  
  return 0;
}

void DefaultPlatform::CloseWindow (window_t)
{
  Raise ("syslib::DefaultPlatform::CloseWindow");
}

void DefaultPlatform::DestroyWindow (window_t)
{
  Raise ("syslib::DefaultPlatform::DestroyWindow");
}
    
/*
    ��������� ����
*/

void DefaultPlatform::SetWindowTitle (window_t, const char*)
{
  Raise ("syslib::DefaultPlatform::SetWindowTitle");
}

void DefaultPlatform::GetWindowTitle (window_t, size_t, char*)
{
  Raise ("syslib::DefaultPlatform::GetWindowTitle");
}

/*
    ������� ���� / ���������� �������
*/

void DefaultPlatform::SetWindowRect (window_t, const Rect&)
{
  Raise ("syslib::DefaultPlatform::SetWindowRect");
}

void DefaultPlatform::GetWindowRect (window_t, Rect&)
{
  Raise ("syslib::DefaultPlatform::GetWindowRect");  
}

void DefaultPlatform::GetClientRect (window_t, Rect&)
{
  Raise ("syslib::DefaultPlatform::GetClientRect");
}

/*
    ��������� ������ ����
*/

void DefaultPlatform::SetWindowFlag (window_t, WindowFlag, bool)
{
  Raise ("syslib::DefaultPlatform::SetWindowFlag");
}

bool DefaultPlatform::GetWindowFlag (window_t, WindowFlag)
{
  Raise ("syslib::DefaultPlatform::GetWindowFlag");
  
  return false;
}

/*
    ��������� �������
*/

void DefaultPlatform::SetCursorPosition (const Point&)
{
  Raise ("syslib::DefaultPlatform::SetCursorPosition");
}

Point DefaultPlatform::GetCursorPosition ()
{
  Raise ("syslib::DefaultPlatform::GetCursorPosition");
  
  return Point ();
}
