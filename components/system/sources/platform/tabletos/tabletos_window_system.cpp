#include "shared.h"

using namespace syslib;

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
    ����
*/

struct syslib::window_handle
{
  screen_context_t screen_cxt; // 
  screen_window_t* pwin;       //
  
///�����������
  window_handle ()
  {
    //Create a screen context that will be used to create an EGL surface to to receive libscreen events.
    if (0 < screen_create_context (&screen_cxt, 0))
      throw xtl::format_operation_exception ("", "Can't create context. An error occurred (%s).", strerror (errno));
          
    //Creates a window that can be used to make graphical content visible on a display
    if (0 < screen_create_window (pwin, screen_cxt))
      throw xtl::format_operation_exception ("", "Can't create window. An error occurred (%s).", strerror (errno));

  }
  
///����������
  ~window_handle ()
  {
    //Destroys a window and free associated resources
    screen_destroy_window(*pwin);

    //
    screen_destroy_context(screen_cxt);
  }
};

/*
    ��������/��������/����������� ����
*/

window_t TabletOsWindowManager::CreateWindow (WindowStyle style, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data)
{
  try
  {
    switch (style)
    {
      case WindowStyle_Overlapped:
        break;
      case WindowStyle_PopUp:
        throw xtl::format_not_supported_exception ("", "WindowStyle_Popup not supported");
      default:
        throw xtl::make_argument_exception ("", "style", style);
    }
    
    if (parent_handle)
      throw xtl::format_not_supported_exception ("", "Child windows are not supported");
      
      //�������� � ������������� ����

    stl::auto_ptr<window_handle> handle (new window_handle);
    
    return handle.release ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsWindowManager::CreateWindow");
    throw;
  }
}

void TabletOsWindowManager::CloseWindow (window_t)
{
  raise ("syslib::TabletOsWindowManager::CloseWindow");
}

void TabletOsWindowManager::DestroyWindow (window_t)
{
  raise ("syslib::TabletOsWindowManager::DestroyWindow");
}

/*
    ��������� ���������-���������� ����������� ����
*/

const void* TabletOsWindowManager::GetNativeWindowHandle (window_t)
{
  raise ("syslib::TabletOsWindowManager::GetNativeWindowHandle");
  
  return 0;
}

const void* TabletOsWindowManager::GetNativeDisplayHandle (window_t)
{
  raise ("syslib::TabletOsWindowManager::GetNativeDisplayHandle");
  
  return 0;
}

/*
    ��������� ����
*/

void TabletOsWindowManager::SetWindowTitle (window_t, const wchar_t*)
{
  raise ("syslib::TabletOsWindowManager::SetWindowTitle");
}

void TabletOsWindowManager::GetWindowTitle (window_t, size_t, wchar_t*)
{
  raise ("syslib::TabletOsWindowManager::GetWindowTitle");
}

/*
    ������� ���� / ���������� �������
*/

void TabletOsWindowManager::SetWindowRect (window_t, const Rect&)
{
  raise ("syslib::TabletOsWindowManager::SetWindowRect");
}

void TabletOsWindowManager::SetClientRect (window_t, const Rect&)
{
  raise ("syslib::TabletOsWindowManager::SetClientRect");
}

void TabletOsWindowManager::GetWindowRect (window_t, Rect&)
{
  raise ("syslib::TabletOsWindowManager::GetWindowRect");
}

void TabletOsWindowManager::GetClientRect (window_t, Rect&)
{
  raise ("syslib::TabletOsWindowManager::GetClientRect");
}

/*
    ��������� ������ ����
*/

void TabletOsWindowManager::SetWindowFlag (window_t, WindowFlag, bool)
{
  raise ("syslib::TabletOsWindowManager::SetWindowFlag");
}

bool TabletOsWindowManager::GetWindowFlag (window_t, WindowFlag)
{
  raise ("syslib::TabletOsWindowManager::GetWindowFlag");

  return false;
}

/*
    ��������� ������������� ����
*/

void TabletOsWindowManager::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  raise ("syslib::TabletOsWindowManager::SetParentWindow");
}

const void* TabletOsWindowManager::GetParentWindowHandle (window_t child)
{
  raise ("syslib::TabletOsWindowManager::GetParentWindow");

  return 0;
}

/*
   ��������� multitouch ������ ��� ����
*/

void TabletOsWindowManager::SetMultitouchEnabled (window_t window, bool enabled)
{
  if (enabled)
    raise ("syslib::TabletOsWindowManager::SetMultitouchEnabled");      
}

bool TabletOsWindowManager::IsMultitouchEnabled (window_t window)
{
  return false;
}

/*
    ���������� ����
*/

void TabletOsWindowManager::InvalidateWindow (window_t)
{
  raise ("syslib::TabletOsWindowManager::InvalidateWindow");
}

/*
    ��������� �������
*/

void TabletOsWindowManager::SetCursorPosition (const Point&)
{
  //ignore
//  raise ("syslib::TabletOsWindowManager::SetCursorPosition");
}

Point TabletOsWindowManager::GetCursorPosition ()
{
//  raise ("syslib::TabletOsWindowManager::GetCursorPosition");

  return Point ();
}

void TabletOsWindowManager::SetCursorPosition (window_t, const Point& client_position)
{
  //ignore
//  throw xtl::make_not_implemented_exception ("syslib::TabletOsWindowManager::SetCursorPosition (window_t, const Point&)");
}

Point TabletOsWindowManager::GetCursorPosition (window_t)
{
//  throw xtl::make_not_implemented_exception ("syslib::TabletOsWindowManager::GetCursorPosition (window_t)");

  return Point ();
}

/*
    ��������� �������
*/

void TabletOsWindowManager::SetCursorVisible (window_t, bool state)
{
  if (state)
    raise ("syslib::TabletOsWindowManager::SetCursorVisible");  
}

bool TabletOsWindowManager::GetCursorVisible (window_t)
{
  return false;
}

/*
    ����������� �������
*/

cursor_t TabletOsWindowManager::CreateCursor (const char*, int, int)
{
  raise ("syslib::TabletOsWindowManager::CreateCursor");
  
  return 0;
}

void TabletOsWindowManager::DestroyCursor (cursor_t)
{
  raise ("syslib::TabletOsWindowManager::DestroyCursor");
}

void TabletOsWindowManager::SetCursor (window_t, cursor_t)
{
  raise ("syslib::TabletOsWindowManager::SetCursor");
}

/*
    ���� ����
*/

void TabletOsWindowManager::SetBackgroundColor (window_t window, const Color& color)
{
  raise ("syslib::TabletOsWindowManager::SetBackgroundColor");
}

void TabletOsWindowManager::SetBackgroundState (window_t window, bool state)
{
  raise ("syslib::TabletOsWindowManager::SetBackgroundState");
}

Color TabletOsWindowManager::GetBackgroundColor (window_t window)
{
  raise ("syslib::TabletOsWindowManager::GetBackgroundColor");

  return Color (0, 0, 0);
}

bool TabletOsWindowManager::GetBackgroundState (window_t window)
{
  raise ("syslib::TabletOsWindowManager::GetBackgroundState");
  
  return false;
}

/*
    ��������� ����� �������
*/

//������������ ����� ������ ��� ����� '\0'
size_t TabletOsWindowManager::GetKeyName (ScanCode scan_code, size_t buffer_size, char* buffer)
{
  static const char* METHOD_NAME = "syslib::TabletOsWindowManager::GetKeyName";

  if (scan_code < 0 || scan_code >= ScanCode_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "scan_code", scan_code);

  if (!buffer && buffer_size)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  if (!buffer_size)
    return 0;
    
  strncpy (buffer, "Unknown", buffer_size);

  return strlen (buffer);
}
