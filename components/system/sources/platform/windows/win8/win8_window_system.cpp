#include "shared.h"

using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;

using namespace syslib::win8;

#ifdef CreateWindow
#undef CreateWindow
#endif

namespace syslib
{

namespace
{

/// ���������� ����
struct WindowImpl
{
  Platform::Agile<CoreWindow> window;          //����
  WindowMessageHandler        message_handler; //���������� ������� ����
  void*                       user_data;       //���������������� ������ �����������

/// �����������
  WindowImpl (WindowMessageHandler in_message_handler, void* in_user_data)
    : message_handler (in_message_handler)
    , user_data (in_user_data)
  {
  }

/// ����������
  ~WindowImpl ()
  {
  }
};

}

/*
    ��������/��������/����������� ����
*/

window_t WindowsWindowManager::CreateWindow (WindowStyle, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data, const wchar_t*)
{
  try
  {
    if (parent_handle)
      throw xtl::format_not_supported_exception ("", "Parent windows is not supported in Windows8");

    std::auto_ptr<WindowImpl> impl (new WindowImpl (handler, user_data));

    impl->window = get_context ()->MainWindow ();

    return (window_t)impl.release ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::CreateWindow");
    throw;
  }
}

void WindowsWindowManager::CloseWindow (window_t handle)
{
  try
  {
    WindowImpl* impl = (WindowImpl*)handle;

    if (!impl)
      throw xtl::make_null_argument_exception ("", "window");  

    impl->window->Close ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::CloseWindow");
    throw;
  }
}

void WindowsWindowManager::DestroyWindow (window_t handle)
{
  WindowImpl* impl = (WindowImpl*)handle;

  if (!impl)
    return;

  delete impl;
}

/*
    ������� ��������� ����� ���� (����� ���� ���������������)
*/

bool WindowsWindowManager::ChangeWindowStyle (window_t window, WindowStyle style)
{
  return false;
}

/*
    ��������� ���������-���������� ����������� ����
*/

const void* WindowsWindowManager::GetNativeWindowHandle (window_t handle)
{
  try
  {
    WindowImpl* impl = (WindowImpl*)handle;

    if (!impl)
      throw xtl::make_null_argument_exception ("", "window");  
    
    return (void*)impl->window.Get ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::GetNativeWindowHandle");
    throw;
  }
}

const void* WindowsWindowManager::GetNativeDisplayHandle (window_t)
{
  return 0;
}

/*
    ��������� ����
*/

void WindowsWindowManager::SetWindowTitle (window_t, const wchar_t*)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetWindowTitle");
}

void WindowsWindowManager::GetWindowTitle (window_t, size_t, wchar_t*)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetWindowTitle");
}

/*
    ������� ���� / ���������� �������
*/

void WindowsWindowManager::SetWindowRect (window_t, const Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetWindowRect");
}

void WindowsWindowManager::SetClientRect (window_t, const Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetClientRect");
}

void WindowsWindowManager::GetWindowRect (window_t, Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetWindowRect");
}

void WindowsWindowManager::GetClientRect (window_t, Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetClientRect");
}

/*
    ��������� ������ ����
*/

void WindowsWindowManager::SetWindowFlag (window_t, WindowFlag, bool)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetWindowFlag");
}

bool WindowsWindowManager::GetWindowFlag (window_t, WindowFlag)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetWindowFlag");

  return false;
}

/*
    ��������� ������������� ����
*/

void WindowsWindowManager::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetParentWindow");
}

const void* WindowsWindowManager::GetParentWindowHandle (window_t child)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetParentWindow");

  return 0;
}

/*
   ��������� multitouch ������ ��� ����
*/

void WindowsWindowManager::SetMultitouchEnabled (window_t window, bool enabled)
{
  if (enabled)
    throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetMultitouchEnabled");      
}

bool WindowsWindowManager::IsMultitouchEnabled (window_t window)
{
  return false;
}

/*
    ���������� ����
*/

void WindowsWindowManager::InvalidateWindow (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::InvalidateWindow");
}

/*
    ��������� �������
*/

void WindowsWindowManager::SetCursorPosition (const Point&)
{
  //ignore
//  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetCursorPosition");
}

Point WindowsWindowManager::GetCursorPosition ()
{
//  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetCursorPosition");

  return Point ();
}

void WindowsWindowManager::SetCursorPosition (window_t, const Point& client_position)
{
  //ignore
//  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetCursorPosition (window_t, const Point&)");
}

Point WindowsWindowManager::GetCursorPosition (window_t)
{
//  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetCursorPosition (window_t)");

  return Point ();
}

/*
    ��������� �������
*/

void WindowsWindowManager::SetCursorVisible (window_t, bool state)
{
  if (state)
    throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetCursorVisible");  
}

bool WindowsWindowManager::GetCursorVisible (window_t)
{
  return false;
}

/*
    ����������� �������
*/

cursor_t WindowsWindowManager::CreateCursor (const char*, int, int)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::CreateCursor");
  
  return 0;
}

void WindowsWindowManager::DestroyCursor (cursor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::DestroyCursor");
}

void WindowsWindowManager::SetCursor (window_t, cursor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetCursor");
}

/*
    ���� ����
*/

void WindowsWindowManager::SetBackgroundColor (window_t window, const Color& color)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetBackgroundColor");
}

void WindowsWindowManager::SetBackgroundState (window_t window, bool state)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetBackgroundState");
}

Color WindowsWindowManager::GetBackgroundColor (window_t window)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetBackgroundColor");

  return Color (0, 0, 0);
}

bool WindowsWindowManager::GetBackgroundState (window_t window)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetBackgroundState");
  
  return false;
}

/*
    ��������/����������� web-view
*/

web_view_t WindowsWindowManager::CreateWebView (IWebViewListener*)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::CreateWebView");
}

void WindowsWindowManager::DestroyWebView (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::DestroyWebView");
}

/*
    �������������� ����
*/

window_t WindowsWindowManager::GetWindow (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetWindow");
}

/*
    �������� ������
*/

void WindowsWindowManager::LoadRequest (web_view_t, const char*)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::LoadRequest");
}

void WindowsWindowManager::LoadData (web_view_t, const char*, size_t, const char*, const char*, const char*)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::LoadData");
}

/*
    ������������ �������� / ��������� �������� / �������� ������� ��������
*/

void WindowsWindowManager::Reload (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::Reload");
}

void WindowsWindowManager::StopLoading (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::StopLoading");
}

bool WindowsWindowManager::IsLoading (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::IsLoading");
  return false;  
}

/*
    �������� ���������
*/

bool WindowsWindowManager::CanGoBack (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::CanGoBack");
  return false;
}

bool WindowsWindowManager::CanGoForward (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::CanGoForward");
  return false;  
}

void WindowsWindowManager::GoBack (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GoBack");
}

void WindowsWindowManager::GoForward (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GoForward");
}

/*
    ��������� ����� �������
*/

//������������ ����� ������ ��� ����� '\0'
size_t WindowsWindowManager::GetKeyName (ScanCode scan_code, size_t buffer_size, char* buffer)
{
  static const char* METHOD_NAME = "syslib::WindowsWindowManager::GetKeyName";

  if (scan_code < 0 || scan_code >= ScanCode_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "scan_code", scan_code);

  if (!buffer && buffer_size)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  if (!buffer_size)
    return 0;
    
  strncpy (buffer, "Unknown", buffer_size);

  return strlen (buffer);
}

}
