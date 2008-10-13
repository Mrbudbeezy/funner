#include "shared.h"

using namespace syslib;

/*
    ��������� ����������: ������ � ������ ���������� ��� ��������� �� ���������
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No window support for default platform");
}

}

/*
    ��������/��������/����������� ����
*/

Platform::window_t Platform::CreateWindow (WindowStyle, WindowMessageHandler, window_t, void*)
{
  raise ("syslib::DefaultPlatform::CreateWindow");

  return 0;
}

void Platform::CloseWindow (window_t)
{
  raise ("syslib::DefaultPlatform::CloseWindow");
}

void Platform::DestroyWindow (window_t)
{
  raise ("syslib::DefaultPlatform::DestroyWindow");
}

/*
    ��������� ����
*/

void Platform::SetWindowTitle (window_t, const wchar_t*)
{
  raise ("syslib::DefaultPlatform::SetWindowTitle");
}

void Platform::GetWindowTitle (window_t, size_t, wchar_t*)
{
  raise ("syslib::DefaultPlatform::GetWindowTitle");
}

/*
    ������� ���� / ���������� �������
*/

void Platform::SetWindowRect (window_t, const Rect&)
{
  raise ("syslib::DefaultPlatform::SetWindowRect");
}

void Platform::GetWindowRect (window_t, Rect&)
{
  raise ("syslib::DefaultPlatform::GetWindowRect");
}

void Platform::GetClientRect (window_t, Rect&)
{
  raise ("syslib::DefaultPlatform::GetClientRect");
}

/*
    ��������� ������ ����
*/

void Platform::SetWindowFlag (window_t, WindowFlag, bool)
{
  raise ("syslib::DefaultPlatform::SetWindowFlag");
}

bool Platform::GetWindowFlag (window_t, WindowFlag)
{
  raise ("syslib::DefaultPlatform::GetWindowFlag");

  return false;
}

/*
    ��������� ������������� ����
*/

void Platform::SetParentWindow (window_t child, window_t parent)
{
  raise ("syslib::DefaultPlatform::SetParentWindow");
}

Platform::window_t Platform::GetParentWindow (window_t child)
{
  raise ("syslib::DefaultPlatform::GetParentWindow");

  return 0;
}

/*
    ���������� ����
*/

void Platform::InvalidateWindow (window_t)
{
  raise ("syslib::DefaultPlatform::InvalidateWindow");
}

/*
    ��������� �������
*/

void Platform::SetCursorPosition (const Point&)
{
  raise ("syslib::DefaultPlatform::SetCursorPosition");
}

Point Platform::GetCursorPosition ()
{
  raise ("syslib::DefaultPlatform::GetCursorPosition");

  return Point ();
}

void Platform::SetCursorPosition (window_t, const Point& client_position)
{
  throw xtl::make_not_implemented_exception ("syslib::Platform::SetCursorPosition (window_t, const Point&)");
}

Point Platform::GetCursorPosition (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::Platform::GetCursorPosition (window_t)");
}

/*
    ��������� �������
*/

void Platform::SetCursorVisible (window_t, bool state)
{
  raise ("syslib::DefaultPlatform::SetCursorVisible");
}

bool Platform::GetCursorVisible (window_t)
{
  raise ("syslib::DefaultPlatform::GetCursorVisible");

  return false;
}
