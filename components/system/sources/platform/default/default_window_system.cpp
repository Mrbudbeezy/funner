#include "shared.h"

namespace syslib
{

/*
    ��������� ����������: ������ � ������ ���������� ��� ��������� �� ���������
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No window support for this platform");
}

}

/*
    ��������/��������/����������� ����
*/

window_t DefaultWindowManager::CreateWindow (WindowStyle, WindowMessageHandler, const void*, const char* init_string, void*)
{
  raise ("syslib::DefaultWindowManager::CreateWindow");

  return 0;
}

void DefaultWindowManager::CloseWindow (window_t)
{
  raise ("syslib::DefaultWindowManager::CloseWindow");
}

void DefaultWindowManager::DestroyWindow (window_t)
{
  raise ("syslib::DefaultWindowManager::DestroyWindow");
}

/*
    ������� ��������� ����� ���� (����� ���� ���������������)
*/

bool DefaultWindowManager::ChangeWindowStyle (window_t window, WindowStyle style)
{
  return false;
}

/*
    ��������� ���������-���������� ����������� ����
*/

const void* DefaultWindowManager::GetNativeWindowHandle (window_t)
{
  raise ("syslib::DefaultWindowManager::GetNativeWindowHandle");
  
  return 0;
}

const void* DefaultWindowManager::GetNativeDisplayHandle (window_t)
{
  raise ("syslib::DefaultWindowManager::GetNativeDisplayHandle");
  
  return 0;
}

/*
    ��������� ����
*/

void DefaultWindowManager::SetWindowTitle (window_t, const wchar_t*)
{
  raise ("syslib::DefaultWindowManager::SetWindowTitle");
}

void DefaultWindowManager::GetWindowTitle (window_t, size_t, wchar_t*)
{
  raise ("syslib::DefaultWindowManager::GetWindowTitle");
}

/*
    ������� ���� / ���������� �������
*/

void DefaultWindowManager::SetWindowRect (window_t, const Rect&)
{
  raise ("syslib::DefaultWindowManager::SetWindowRect");
}

void DefaultWindowManager::SetClientRect (window_t, const Rect&)
{
  raise ("syslib::DefaultWindowManager::SetClientRect");
}

void DefaultWindowManager::GetWindowRect (window_t, Rect&)
{
  raise ("syslib::DefaultWindowManager::GetWindowRect");
}

void DefaultWindowManager::GetClientRect (window_t, Rect&)
{
  raise ("syslib::DefaultWindowManager::GetClientRect");
}

/*
    ��������� ������ ����
*/

void DefaultWindowManager::SetWindowFlag (window_t, WindowFlag, bool)
{
  raise ("syslib::DefaultWindowManager::SetWindowFlag");
}

bool DefaultWindowManager::GetWindowFlag (window_t, WindowFlag)
{
  raise ("syslib::DefaultWindowManager::GetWindowFlag");

  return false;
}

/*
    ��������� ������������� ����
*/

void DefaultWindowManager::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  raise ("syslib::DefaultWindowManager::SetParentWindow");
}

const void* DefaultWindowManager::GetParentWindowHandle (window_t child)
{
  raise ("syslib::DefaultWindowManager::GetParentWindow");

  return 0;
}

/*
   ��������� multitouch ������ ��� ����
*/

void DefaultWindowManager::SetMultitouchEnabled (window_t window, bool enabled)
{
  if (enabled)
    raise ("syslib::DefaultWindowManager::SetMultitouchEnabled");      
}

bool DefaultWindowManager::IsMultitouchEnabled (window_t window)
{
  return false;
}

/*
    ���������� ����
*/

void DefaultWindowManager::InvalidateWindow (window_t)
{
  raise ("syslib::DefaultWindowManager::InvalidateWindow");
}

/*
    ��������� �������
*/

void DefaultWindowManager::SetCursorPosition (const Point&)
{
  //ignore
//  raise ("syslib::DefaultWindowManager::SetCursorPosition");
}

Point DefaultWindowManager::GetCursorPosition ()
{
//  raise ("syslib::DefaultWindowManager::GetCursorPosition");

  return Point ();
}

void DefaultWindowManager::SetCursorPosition (window_t, const Point& client_position)
{
  //ignore
//  throw xtl::make_not_implemented_exception ("syslib::DefaultWindowManager::SetCursorPosition (window_t, const Point&)");
}

Point DefaultWindowManager::GetCursorPosition (window_t)
{
//  throw xtl::make_not_implemented_exception ("syslib::DefaultWindowManager::GetCursorPosition (window_t)");

  return Point ();
}

/*
    ��������� �������
*/

void DefaultWindowManager::SetCursorVisible (window_t, bool state)
{
  if (state)
    raise ("syslib::DefaultWindowManager::SetCursorVisible");  
}

bool DefaultWindowManager::GetCursorVisible (window_t)
{
  return false;
}

/*
    ����������� �������
*/

cursor_t DefaultWindowManager::CreateCursor (const char*, int, int)
{
  raise ("syslib::DefaultWindowManager::CreateCursor");
  
  return 0;
}

void DefaultWindowManager::DestroyCursor (cursor_t)
{
  raise ("syslib::DefaultWindowManager::DestroyCursor");
}

void DefaultWindowManager::SetCursor (window_t, cursor_t)
{
  raise ("syslib::DefaultWindowManager::SetCursor");
}

/*
    ���� ����
*/

void DefaultWindowManager::SetBackgroundColor (window_t window, const Color& color)
{
  raise ("syslib::DefaultWindowManager::SetBackgroundColor");
}

void DefaultWindowManager::SetBackgroundState (window_t window, bool state)
{
  raise ("syslib::DefaultWindowManager::SetBackgroundState");
}

Color DefaultWindowManager::GetBackgroundColor (window_t window)
{
  raise ("syslib::DefaultWindowManager::GetBackgroundColor");

  return Color (0, 0, 0);
}

bool DefaultWindowManager::GetBackgroundState (window_t window)
{
  raise ("syslib::DefaultWindowManager::GetBackgroundState");
  
  return false;
}

/*
    ��������� ����� �������
*/

//������������ ����� ������ ��� ����� '\0'
size_t DefaultWindowManager::GetKeyName (ScanCode scan_code, size_t buffer_size, char* buffer)
{
  static const char* METHOD_NAME = "syslib::DefaultWindowManager::GetKeyName";

  if (scan_code < 0 || scan_code >= ScanCode_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "scan_code", scan_code);

  if (!buffer && buffer_size)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  if (!buffer_size)
    return 0;
    
  strncpy (buffer, "Unknown", buffer_size);

  return strlen (buffer);
}

/*
    ��������/����������� web-view
*/

web_view_t DefaultWindowManager::CreateWebView (IWebViewListener*)
{
  raise ("syslib::DefaultWindowManager::CreateWebView");
  return 0;
}

void DefaultWindowManager::DestroyWebView (web_view_t)
{
  raise ("syslib::DefaultWindowManager::DestroyWebView");
}

/*
    �������������� ����
*/

window_t DefaultWindowManager::GetWindow (web_view_t)
{
  raise ("syslib::DefaultWindowManager::GetWindow");
  return 0;
}

/*
    �������� ������
*/

void DefaultWindowManager::LoadRequest (web_view_t, const char*)
{
  raise ("syslib::DefaultWindowManager::LoadRequest");
}

void DefaultWindowManager::LoadData (web_view_t, const char*, size_t, const char*, const char*, const char*)
{
  raise ("syslib::DefaultWindowManager::LoadData");
}

/*
    ������������ �������� / ��������� �������� / �������� ������� ��������
*/

void DefaultWindowManager::Reload (web_view_t)
{
  raise ("syslib::DefaultWindowManager::Reload");
}

void DefaultWindowManager::StopLoading (web_view_t)
{
  raise ("syslib::DefaultWindowManager::StopLoading");
}

bool DefaultWindowManager::IsLoading (web_view_t)
{
  raise ("syslib::DefaultWindowManager::IsLoading");
  return false;  
}

/*
    �������� ���������
*/

bool DefaultWindowManager::CanGoBack (web_view_t)
{
  raise ("syslib::DefaultWindowManager::CanGoBack");
  return false;
}

bool DefaultWindowManager::CanGoForward (web_view_t)
{
  raise ("syslib::DefaultWindowManager::CanGoForward");
  return false;  
}

void DefaultWindowManager::GoBack (web_view_t)
{
  raise ("syslib::DefaultWindowManager::GoBack");
}

void DefaultWindowManager::GoForward (web_view_t)
{
  raise ("syslib::DefaultWindowManager::GoForward");
}

}
