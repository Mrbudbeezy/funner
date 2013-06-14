#include "../shared.h"

using namespace syslib;
using namespace common;

namespace
{

const wchar_t* WINDOW_CLASS_NAME = L"Funner.Default window class";
const char* LOG_NAME          = "system.windows";

/*
    ��������� ���������� ����������
*/

HINSTANCE GetApplicationInstance ()
{
  return GetModuleHandle (0);
}

/*
    ����������� ����� � ����� temp
*/

class SysTempFile
{
  public:
    SysTempFile (const char* source_file_name)
    {
      if (!source_file_name)
        throw xtl::make_null_argument_exception ("syslib::TempFile::TempFile", "source_file_name");
        
        //��������� ����� ������ �����
        
      stl::wstring dir_name;
      
      dir_name.fast_resize (MAX_PATH);
      
      DWORD dir_len = GetTempPathW (dir_name.size (), &dir_name [0]);
      
      if (!dir_len || dir_len > MAX_PATH)
        raise_error ("::GetTempPath");
        
      file_name.fast_resize (MAX_PATH);
        
      if (!GetTempFileNameW (dir_name.c_str (), L"TEMP", 0, &file_name [0]))
        raise_error ("::GetTempFileName");
        
        //����������� �����������
      
      #ifndef WINCE
        #undef CopyFile
      #endif
        
      FileSystem::CopyFile (source_file_name, tostring (file_name).c_str ());
    }
    
    ~SysTempFile ()
    {
      try
      {
        FileSystem::Remove (tostring(file_name).c_str ());
      }
      catch (...)
      {
      }
    }
    
    stl::string Path () const { return tostring(file_name); }
  
  private:
    stl::wstring file_name; //��� �����
};

/*
    �������� ���������� ����
*/

struct WindowImpl
{
  void*                user_data;             //��������� �� ���������������� ������
  WindowMessageHandler message_handler;       //������� ��������� ��������� ����
  bool                 is_cursor_visible;     //����� �� ������
  bool                 is_cursor_in_window;   //��������� �� ������ � ����
  HCURSOR              default_cursor;        //������ �� ���������
  HCURSOR              preferred_cursor;      //�������������� ������ ��� ������� ����
  HBRUSH               background_brush;      //����� ��� ������� ������� ����
  Color                background_color;      //���� ������� ����
  bool                 background_state;      //������� �� ������ ���
  bool                 is_multitouch_enabled; //������� �� multitouch
  void*                locked_session;        //��������������� ������

  WindowImpl (WindowMessageHandler handler, void* in_user_data)
    : user_data (in_user_data)
    , message_handler (handler)
    , is_cursor_visible (true)
    , is_cursor_in_window (false)
#ifdef WINCE
    , default_cursor (LoadCursorW (0, IDC_ARROW))
#else
    , default_cursor (LoadCursorA (0, IDC_ARROW))
#endif
    , preferred_cursor (0)
    , background_brush (CreateSolidBrush (RGB (0, 0, 0)))
    , background_state (false)
    , is_multitouch_enabled (false)
    , locked_session (0)
  {
    if (!background_brush)
      raise_error ("::CreateSolidBrush");
  }

  void TrackCursor (HWND wnd)
  {
#ifdef WINCE
    return;
#else
    if (is_cursor_in_window)
      return;
    
    TRACKMOUSEEVENT track_mouse_event;
    
    memset (&track_mouse_event, 0, sizeof (track_mouse_event));
    
    track_mouse_event.cbSize     = sizeof (TRACKMOUSEEVENT);
    track_mouse_event.dwFlags    = TME_LEAVE;
    track_mouse_event.hwndTrack  = wnd;
    
    if (!TrackMouseEvent (&track_mouse_event))
      raise_error ("::TrackMouseEvent");

    is_cursor_in_window = true;
#endif
  }

  void Notify (window_t window, WindowEvent event, const WindowEventContext& context)
  {
    try
    {
      message_handler (window, event, context, user_data);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }
};

/*
    ��������� ��������� �������
*/

void GetEventContext (HWND wnd, WindowEventContext& context, RECT& client_rect)
{
  memset (&context, 0, sizeof (context));

  context.handle = wnd;

  GetClientRect (wnd, &client_rect);
 
  POINT cursor_position;

  GetCursorPos (&cursor_position);
  ScreenToClient (wnd, &cursor_position);

  if (cursor_position.x < client_rect.left)   cursor_position.x = client_rect.left;
  if (cursor_position.y < client_rect.top)    cursor_position.y = client_rect.top;
  if (cursor_position.x > client_rect.right)  cursor_position.x = client_rect.right;
  if (cursor_position.y > client_rect.bottom) cursor_position.y = client_rect.bottom;

  context.cursor_position.x = cursor_position.x;
  context.cursor_position.y = cursor_position.y;

  context.keyboard_alt_pressed        = (GetKeyState (VK_MENU) & 2) != 0;
  context.keyboard_control_pressed    = (GetKeyState (VK_CONTROL) & 2) != 0;
  context.keyboard_shift_pressed      = (GetKeyState (VK_SHIFT) & 2) != 0;
  context.mouse_left_button_pressed   = (GetKeyState (VK_LBUTTON) & 2) != 0;
  context.mouse_right_button_pressed  = (GetKeyState (VK_RBUTTON) & 2) != 0;
  context.mouse_middle_button_pressed = (GetKeyState (VK_MBUTTON) & 2) != 0;
}

/*
    �������������� ����������� ����� ������ � syslib::Key
*/

Key VirtualKey2SystemKey (size_t vkey)
{
  static size_t map [] =
  {
    0, 0, 0, 0, 0, 0, 0, 0,  //0x00-0x07
    Key_BackSpace, Key_Tab, 0, 0, 0, Key_Enter, 0, 0,  //0x08-0x0F
    Key_Shift, Key_Control, Key_Alt, Key_Pause, Key_CapsLock, 0, 0, 0, //0x10-0x17
    0, 0, 0, Key_Escape, 0, 0, 0, 0, //0x18-0x1F
    Key_Space, Key_PageUp, Key_PageDown, Key_End, Key_Home, Key_Left, Key_Up, Key_Right, //0x20-0x27
    Key_Down, 0, 0, 0, 0, Key_Insert, Key_Delete, 0, //0x28-0x2F
    Key_0, Key_1, Key_2, Key_3, Key_4, Key_5, Key_6, Key_7, //0x30-0x37
    Key_8, Key_9, 0, 0, 0, 0, 0, 0, //0x38-0x3F
    0,     Key_A, Key_B, Key_C, Key_D, Key_E, Key_F, Key_G, //0x40-0x47
    Key_H, Key_I, Key_J, Key_K, Key_L, Key_M, Key_N, Key_O, //0x48-0x4F
    Key_P, Key_Q, Key_R, Key_S, Key_T, Key_U, Key_V, Key_W, //0x50-0x57
    Key_X, Key_Y, Key_Z, 0, 0, 0, 0, 0, //0x58-0x5F
    Key_NumPad0, Key_NumPad1, Key_NumPad2, Key_NumPad3, Key_NumPad4, Key_NumPad5, Key_NumPad6, Key_NumPad7, //0x60-0x67
    Key_NumPad8, Key_NumPad9, Key_NumPadMultiply, Key_NumPadPlus, 0, Key_NumPadMinus, Key_NumPadDot, Key_NumPadDivide, //0x68-0x6F
    Key_F1, Key_F2, Key_F3, Key_F4, Key_F5, Key_F6, Key_F7, Key_F8, //0x70-0x77
    Key_F9, Key_F10, Key_F11, Key_F12, 0, 0, 0, 0, //0x78-0x7F
    0, 0, 0, 0, 0, 0, 0, 0, //0x80-0x87
    0, 0, 0, 0, 0, 0, 0, 0, //0x88-0x8F
    Key_NumLock, Key_ScrollLock, 0, 0, 0, 0, 0, 0, //0x90-0x97
    0, 0, 0, 0, 0, 0, 0, 0, //0x98-0x9F
    0, 0, 0, 0, 0, 0, 0, 0, //0xA0-0xA7
    0, 0, 0, 0, 0, 0, 0, 0, //0xA8-0xAF
    0, 0, 0, 0, 0, 0, 0, 0, //0xB0-0xB7
    0, 0, Key_Semicolon, Key_Plus, Key_Comma, Key_Minus, Key_Dot, Key_Slash, //0xB8-0xBF
    Key_Tilda, 0, 0, 0, 0, 0, 0, 0, //0xC0-0xC7
    0, 0, 0, 0, 0, 0, 0, 0, //0xC8-0xCF
    0, 0, 0, 0, 0, 0, 0, 0, //0xD0-0xD7
    0, 0, 0, Key_LeftBracket, Key_BackSlash, Key_RightBracket, Key_Apostrophe, 0, //0xD8-0xDF
    0, 0, 0, 0, 0, 0, 0, 0, //0xE0-0xE7
    0, 0, 0, 0, 0, 0, 0, 0, //0xE8-0xEF
    0, 0, 0, 0, 0, 0, 0, 0, //0xF0-0xF7
    0, 0, 0, 0, 0, 0, 0, 0, //0xF8-0xFF
  };

  if (vkey >= sizeof (map) / sizeof (*map))
    return Key_Unknown;

  return (Key)map [vkey];
}

/*
    ��������� ����-����
*/

ScanCode GetScanCode (size_t lParam)
{
  return (ScanCode)((lParam >> 16) & 0x1ff);
}

/*
    ������� ��������� ��������� ����
*/

LRESULT CALLBACK WindowMessageHandler (HWND wnd, UINT message, WPARAM wparam, LPARAM lparam)
{

    //��������� ��������� �� ���������������� ������

  WindowImpl* impl          = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));
  window_t    window_handle = (window_t)wnd;

    //���� ��������� �� ���������������� ������ �� ���������� - ������ ������� ��� ����������

  if (!impl)
  {
    if (message == WM_CREATE)
    {
      CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*> (lparam);

      if (!cs->lpCreateParams)
        return 0;

      impl = reinterpret_cast<WindowImpl*> (cs->lpCreateParams);

        //������������� ��������� �� ���������������� ������

      SetWindowLong (wnd, GWL_USERDATA, (LONG)impl);

        //��������� ���� �� ��������� ��������������� �����������

      WindowEventContext context;

      memset (&context, 0, sizeof (context));

      context.handle = wnd;

      impl->Notify (window_handle, WindowEvent_OnChangeHandle, context);

      return 0;
    }

    return DefWindowProc (wnd, message, wparam, lparam);
  }

    //��������� ��������� �������

  WindowEventContext context;
  RECT               client_rect;

  GetEventContext (wnd, context, client_rect);

    //��������� ���������
    
  switch (message)
  {
    case WM_DESTROY: //����������� ����
      impl->Notify (window_handle, WindowEvent_OnDestroy, context);

      delete impl;

      SetWindowLong (wnd, GWL_USERDATA, 0);

      return 0;
    case WM_CLOSE: //������� �������� ����
      impl->Notify (window_handle, WindowEvent_OnClose, context);
      return 0;
    case WM_ACTIVATE: //���������/����������� ����
      switch (wparam)
      {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
          impl->Notify (window_handle, WindowEvent_OnActivate, context);
          break;
        case WA_INACTIVE:
          impl->Notify (window_handle, WindowEvent_OnDeactivate, context);
          break;
      }

      return 0;
    case WM_SHOWWINDOW: //��������� ���������
      impl->Notify (window_handle, wparam ? WindowEvent_OnShow : WindowEvent_OnHide, context);
      return 0;
    case WM_SETFOCUS: //��������� ������ �����
      impl->Notify (window_handle, WindowEvent_OnSetFocus, context);
      return 0;
    case WM_KILLFOCUS: //������ ������ �����
      impl->Notify (window_handle, WindowEvent_OnLostFocus, context);
      return 0;
    case WM_SETCURSOR: //��������� ��������� �������
      if (LOWORD(lparam) == HTCLIENT) //���� ��������� �������
      {
        if (impl->is_cursor_visible)
        {
          HCURSOR required_cursor = impl->preferred_cursor ? impl->preferred_cursor : impl->default_cursor;

          if (required_cursor != GetCursor ())
            SetCursor (required_cursor);
        }
        else if (!impl->is_cursor_visible)
        {
          if (GetCursor () != 0)
            SetCursor (0);
        }

        return 1;
      }

        //�� ��������� ������� - ���������� �� ���������

      return DefWindowProc (wnd, message, wparam, lparam);
    case WM_PAINT: //���������� �����������
    {
      PAINTSTRUCT ps;

      HDC dc = BeginPaint (wnd, &ps);
      
      if (impl->background_state && impl->background_brush)
      {
        SelectObject (dc, impl->background_brush);
        Rectangle (dc, client_rect.left, client_rect.top, client_rect.right, client_rect.bottom);
      }

      impl->Notify (window_handle, WindowEvent_OnPaint, context);

      EndPaint (wnd, &ps);

      return 0;
    }
    case WM_ERASEBKGND: //����� �� ������� ���
      return 1; //������� ��� �� �����
    case WM_SIZE: //��������� �������� ����
      impl->Notify (window_handle, WindowEvent_OnSize, context);
      return 0;
    case WM_MOVE: //��������� ��������� ����
      impl->Notify (window_handle, WindowEvent_OnMove, context);
      return 0;
    case WM_MOUSEMOVE: //��������� ��������� ������� ��� �������� ����
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->TrackCursor (wnd);
      impl->Notify (window_handle, WindowEvent_OnMouseMove, context);

      return 0;
#ifndef WINCE
    case WM_MOUSELEAVE:
      impl->is_cursor_in_window = false;
      impl->Notify (window_handle, WindowEvent_OnMouseLeave, context);
      return 0;
#endif
    case WM_MOUSEWHEEL: //���������� ��������� ������������� ������ ����
      context.mouse_vertical_wheel_delta = float (GET_WHEEL_DELTA_WPARAM (wparam)) / float (WHEEL_DELTA);

      impl->Notify (window_handle, WindowEvent_OnMouseVerticalWheel, context);
      return 0;
#ifdef WM_MOUSEHWHEEL
    case WM_MOUSEHWHEEL: //���������� ��������� ��������������� ������ ����
      context.mouse_horisontal_wheel_delta = float (GET_WHEEL_DELTA_WPARAM (wparam)) / float (WHEEL_DELTA);
      impl->Notify (window_handle, WindowEvent_OnMouseHorisontalWheel, context);
      return 0;
#endif
    case WM_LBUTTONDOWN: //������ ����� ������ ����
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnLeftButtonDown, context);
      return 0;
    case WM_LBUTTONUP: //�������� ����� ������ ����
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnLeftButtonUp, context);
      return 0;
    case WM_LBUTTONDBLCLK: //������� ������ ����� ������� ����
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnLeftButtonDoubleClick, context);
      return 0;
    case WM_RBUTTONDOWN: //������ ������ ������ ����
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnRightButtonDown, context);
      return 0;
    case WM_RBUTTONUP: //�������� ������ ������ ����
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnRightButtonUp, context);
      return 0;
    case WM_RBUTTONDBLCLK: //������� ������ ������ ������� ����
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnRightButtonDoubleClick, context);
      return 0;
    case WM_XBUTTONDOWN: //������ X ������ ����
    {
      WindowEvent event;

      switch (HIWORD (wparam))
      {
        case 1:  event = WindowEvent_OnXButton1Down; break;
        case 2:  event = WindowEvent_OnXButton2Down; break;
        default: return 1;
      }

      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, event, context);

      return 1;
    }
    case WM_XBUTTONUP: //�������� X ������ ����
    {
      WindowEvent event;

      switch (HIWORD (wparam))
      {
        case 1:  event = WindowEvent_OnXButton1Up; break;
        case 2:  event = WindowEvent_OnXButton2Up; break;
        default: return 1;
      }

      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, event, context);

      return 1;
    }
    case WM_XBUTTONDBLCLK: //������� ������ X ������� ����
    {
      WindowEvent event;

      switch (HIWORD (wparam))
      {
        case 1:  event = WindowEvent_OnXButton1DoubleClick; break;
        case 2:  event = WindowEvent_OnXButton2DoubleClick; break;
        default: return 1;
      }

      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, event, context);

      return 1;
    }
    case WM_MBUTTONDOWN: //������ ������� ������ ����
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnMiddleButtonDown, context);
      return 0;
    case WM_MBUTTONUP: //�������� ������� ������ ����
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnMiddleButtonUp, context);
      return 0;
    case WM_MBUTTONDBLCLK: //������� ������ ������� ������� ����
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnMiddleButtonDoubleClick, context);
      return 0;
    case WM_KEYDOWN: //������ ������� ����������
      context.key           = VirtualKey2SystemKey (wparam);
      context.key_scan_code = GetScanCode (lparam);

      impl->Notify (window_handle, WindowEvent_OnKeyDown, context);
      return 0;
    case WM_KEYUP: //�������� ������� ����������
      context.key           = VirtualKey2SystemKey (wparam);
      context.key_scan_code = GetScanCode (lparam);

      impl->Notify (window_handle, WindowEvent_OnKeyUp, context);
      return 0;
    case WM_CHAR: //� ������ ���� �������� ������
      wcscpy ( &context.char_code, to_wstring_from_utf8((char*)&wparam, 1).c_str () );
//      mbtowc (&context.char_code, (char*)&wparam, 1);
      impl->Notify (window_handle, WindowEvent_OnChar, context);
      return 0;
#ifndef WINCE
    case WM_WTSSESSION_CHANGE: //��������� ������ ����������              
      switch (wparam)
      {
        case WTS_SESSION_LOCK:
          if (!impl->locked_session)
          {        
            impl->locked_session = (void*)lparam;

            impl->Notify (window_handle, WindowEvent_OnScreenLock, context);
          }

          return 0;
        case WTS_SESSION_UNLOCK:
          if ((void*)lparam == impl->locked_session)
          {        
            impl->locked_session = 0;

            impl->Notify (window_handle, WindowEvent_OnScreenUnlock, context);
          }

          return 0;
        default:
          break;
      }

      break;      
#endif

#ifndef WINCE
    case WM_SYSCOMMAND:
      if (wparam == SC_SCREENSAVE && !Application::GetScreenSaverState ())
        return 0;

      break;
#endif
  }

    //��������� ��������� �� ���������    

  return DefWindowProc (wnd, message, wparam, lparam);
}

/*
    ����������� �������� ������
*/

void RegisterWindowClass ()
{
  WNDCLASSW wc;

  memset (&wc, 0, sizeof (wc));

#ifdef WINCE
  wc.style         =            CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
#else
  wc.style         = CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
#endif
  wc.lpfnWndProc   = &WindowMessageHandler;
  wc.hInstance     = GetApplicationInstance ();

#ifndef WINCE
  wc.hIcon         = LoadIcon (GetApplicationInstance (), IDI_APPLICATION);
#endif

  wc.hCursor       = LoadCursor (GetApplicationInstance (), IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszClassName = WINDOW_CLASS_NAME;


  if (!RegisterClassW (&wc))
    raise_error ("::RegisterClass");
}

UINT get_window_style (WindowStyle style, bool has_parent)
{
    //����������� ����� ����

  UINT win_style;

  switch (style)
  {
    case WindowStyle_Overlapped:
      win_style = WS_OVERLAPPEDWINDOW;
//      win_style = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

      if (has_parent)
        win_style |= WS_CHILD | WS_CLIPSIBLINGS;

      break;
    case WindowStyle_PopUp:
      win_style = has_parent ? WS_CHILD  | WS_CLIPSIBLINGS : WS_POPUP;
      break;
    default:
      throw xtl::make_argument_exception ("", "style", style);
  }
  
  return win_style;
}

}

/*
    ��������/��������/����������� ����
*/

#undef CreateWindow

window_t WindowsWindowManager::CreateWindow (WindowStyle style, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data, const wchar_t* class_name)
{  
  try
  {
      //����������� ����� ����

    UINT win_style = get_window_style (style, parent_handle != 0);
    
    static bool is_window_class_registered = false;

      //����������� �������� ������

    if (!is_window_class_registered)
    {
        //���������� ���������� ������� ����� ������ UnregisterClass ��� ������ �� ����������, �� ������ �����
        //�������������� ������������� ������� �� ������������ � ������

      RegisterWindowClass ();

      is_window_class_registered = true;
    }

      //�������� ����

    WindowImpl* volatile window_impl = new WindowImpl (handler, user_data);

    try
    {
      if (!class_name)
        class_name = WINDOW_CLASS_NAME;
      
      HWND wnd = CreateWindowExW (0, class_name, L"", win_style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                (HWND)parent_handle, 0, GetApplicationInstance (), window_impl);

      if (!wnd)
        raise_error ("::CreateWindow");                

      if (!UpdateWindow (wnd))
        raise_error ("::UpdateWindow");
        
      try
      {
        if (!WTSRegisterSessionNotification (wnd, NOTIFY_FOR_THIS_SESSION))
          raise_error ("::WTSRegisterSessionNotification");  
      }
      catch (std::exception& e)
      {
        common::Log (LOG_NAME).Printf ("%s", e.what ());
      }

      return (window_t)wnd;
    }
    catch (...)
    {
      delete window_impl;
      throw;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::CreateWindow");

    throw;
  }
}

void WindowsWindowManager::CloseWindow (window_t handle)
{
  try
  {
    HWND wnd = (HWND)handle;

    if (!PostMessage (wnd, WM_CLOSE, 0, 0))
      raise_error ("::PostMessage(window, WM_CLOSE,0, 0)");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::CloseWindow");
    throw;
  }
}

void WindowsWindowManager::DestroyWindow (window_t handle)
{
  try
  {
    HWND wnd = (HWND)handle;

    if (!::DestroyWindow (wnd))
      raise_error ("::DestroyWindow");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::DestroyWindow");
    throw;
  }
}

/*
    ������� ��������� ����� ���� (����� ���� ���������������)
*/

bool WindowsWindowManager::ChangeWindowStyle (window_t handle, WindowStyle style)
{
  try
  {
    HWND wnd       = (HWND)handle;    
    LONG win_style = get_window_style (style, GetParentWindowHandle (handle) != 0);
    
    if (!SetWindowLong (wnd, GWL_STYLE, win_style))
      raise_error ("::SetWindowLongPtr");
      
    HWND prev_window = 0;
#ifndef WINCE
    prev_window = ::GetNextWindow (wnd, GW_HWNDPREV);
    
    if (!prev_window)
      prev_window = HWND_TOP;
#endif
      
    RECT window_rect;

    if (!::GetWindowRect (wnd, &window_rect))
      raise_error ("::GetWindowRect");    
      
    if (!SetWindowPos (wnd, prev_window, window_rect.left, window_rect.top, window_rect.right - window_rect.left, 
      window_rect.bottom - window_rect.top, SWP_FRAMECHANGED|SWP_SHOWWINDOW))
      raise_error ("::SetWindowPos");

    return true;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::ChangeWindowStyle");
    throw;
  }
}

/*
    ��������� ���������-���������� ����������� ����
*/

const void* WindowsWindowManager::GetNativeWindowHandle (window_t handle)
{
  return reinterpret_cast<const void*> (handle);
}

const void* WindowsWindowManager::GetNativeDisplayHandle (window_t)
{
  return 0;
}

/*
    ��������� ����
*/

void WindowsWindowManager::SetWindowTitle (window_t handle, const wchar_t* title)
{
  try
  {
    if (!title)
      throw xtl::make_null_argument_exception ("", "title");

    HWND wnd = (HWND)handle;

#undef SetWindowTextW
    if (!SetWindowTextW (wnd, (LPCWSTR)common::tostring (title).c_str ()))    
      raise_error ("::SetWindowTextW");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::SetWindowTitle(const wchar_t*)");
    throw;
  }
}

void WindowsWindowManager::GetWindowTitle (window_t handle, size_t buffer_size, wchar_t* buffer)
{

  try
  {
    HWND wnd = (HWND)handle;    
    
    stl::string tmp_buffer;
    
    tmp_buffer.fast_resize (buffer_size);

    if (!GetWindowTextW (wnd, (LPWSTR)&tmp_buffer [0], tmp_buffer.size ())) //very strange behaviour: GetWindowTextA have to be called
      raise_error ("::GetWindowTextW");      

    memcpy (buffer, common::towstring (tmp_buffer.c_str ()).c_str (), tmp_buffer.size () * sizeof (wchar_t));

    buffer [buffer_size-1] = 0;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::GetWindowTitle");
    throw;
  }
}

/*
    ������� ���� / ���������� �������
*/

void WindowsWindowManager::SetWindowRect (window_t handle, const Rect& rect)
{
  try
  {
    HWND wnd = (HWND)handle;

    RECT window_rect;

    if (!::GetWindowRect (wnd, &window_rect))
      raise_error ("::GetWindowRect");

    UINT flags = SWP_NOACTIVATE;

    if (size_t (window_rect.right - window_rect.left) == size_t (rect.right - rect.left) &&
        size_t (window_rect.bottom - window_rect.top) == size_t (rect.bottom - rect.top))
    {
        //�������� ������� ���� �� �����

      flags |= SWP_NOSIZE;
    }

    if ((size_t)window_rect.left == rect.left && (size_t)window_rect.top == rect.top)
    {
        //�������� ��������� ���� �� �����

      flags |= SWP_NOMOVE;
    }

    if (!SetWindowPos (wnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, flags))
      raise_error ("::SetWindowPos");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::SetWindowRect");
    throw;
  }
}

void WindowsWindowManager::SetClientRect (window_t handle, const Rect& rect)
{
  try
  {
    HWND wnd = (HWND)handle;

    RECT window_rect;

    window_rect.left   = rect.left;
    window_rect.right  = rect.right;
    window_rect.top    = rect.top;
    window_rect.bottom = rect.bottom;

    if (!AdjustWindowRectEx (&window_rect, GetWindowLong (wnd, GWL_STYLE), 0, GetWindowLong (wnd, GWL_EXSTYLE)))
      raise_error ("::AdjustWindowRectEx");

    Rect new_window_rect;

    new_window_rect.left   = window_rect.left;
    new_window_rect.right  = window_rect.right;
    new_window_rect.top    = window_rect.top;
    new_window_rect.bottom = window_rect.bottom;

    SetWindowRect (handle, new_window_rect);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::SetClientRect");
    throw;
  }
}

void WindowsWindowManager::GetWindowRect (window_t handle, Rect& rect)
{
  try
  {
    HWND wnd = (HWND)handle;

    RECT window_rect;

    if (!::GetWindowRect (wnd, &window_rect))
      raise_error ("::GetWindowRect");

    rect.left   = window_rect.left;
    rect.right  = window_rect.right;
    rect.top    = window_rect.top;
    rect.bottom = window_rect.bottom;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::GetWindowRect");
    throw;
  }
}

void WindowsWindowManager::GetClientRect (window_t handle, Rect& rect)
{
  try
  {
    HWND wnd = (HWND)handle;

    RECT window_rect;

    if (!::GetClientRect (wnd, &window_rect))
      raise_error ("::GetClientRect");

    rect.left   = window_rect.left;
    rect.right  = window_rect.right;
    rect.top    = window_rect.top;
    rect.bottom = window_rect.bottom;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::GetClientRect");
    throw;
  }
}

/*
    ��������� ������ ����
*/

void WindowsWindowManager::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  HWND wnd = (HWND)handle;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible: //��������� ����
        ShowWindow (wnd, state ? SW_SHOW : SW_HIDE);
        SetLastError (0);  //���������� ������ Win32 error 126. �� ������ ��������� ������. ����������� ��� ������� fraps.
//        check_errors ("::ShowWindow");
        break;
      case WindowFlag_Active: //���������� ����
        if (state)
        {
          if (!SetActiveWindow (wnd))
            raise_error ("::SetActiveWindow");
        }
        else
        {
#ifdef WINCE
          ShowWindow (wnd, SW_HIDE);
          ShowWindow (wnd, SW_HIDE);
#else
          ShowWindow (wnd, SW_HIDE);
          ShowWindow (wnd, SW_SHOWNOACTIVATE);
#endif
          check_errors ("::ShowWindow");
        }

        break;
      case WindowFlag_Focus: //����� �����
        if (!SetFocus (state ? wnd : HWND_TOP))
          raise_error ("::SetFocus");
        break;
      case WindowFlag_Maximized:
        if (state)
        {
          ShowWindow (wnd, SW_MAXIMIZE);
        }
        else
        {
          ShowWindow (wnd, SW_SHOW);
        }

        break;
      case WindowFlag_Minimized:
        if (state)
        {
          ShowWindow (wnd, SW_MINIMIZE);
        }
        else
        {
          ShowWindow (wnd, SW_SHOW);
        }
        break;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::SetWindowFlag");
    throw;
  }
}

bool WindowsWindowManager::GetWindowFlag (window_t handle, WindowFlag flag)
{
  HWND wnd = (HWND)handle;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
      {
        bool result = (GetWindowLong (wnd, GWL_STYLE) & WS_VISIBLE) != 0;

//        check_errors ("::GetWindowLong");

        return result;
      }
      case WindowFlag_Active:
      {
        HWND active_wnd = GetActiveWindow ();

#ifdef WINCE
       if (active_wnd == 0)
         return false;
#else
        check_errors ("::GetActiveWindow");
#endif
        return active_wnd == wnd;
      }
      case WindowFlag_Focus:
      {
        HWND focus_wnd = GetFocus ();

        check_errors ("::GetFocus");

        return focus_wnd == wnd;
      }
      case WindowFlag_Minimized:
      case WindowFlag_Maximized:
        throw xtl::format_operation_exception ("", "Can't get window flag %d value", flag);
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::GetWindowFlag");
    throw;
  }

  return false;
}

/*
    ��������� ������������� ����
*/

void WindowsWindowManager::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  try
  {
    if (!SetParent ((HWND)child, (HWND)parent_handle))
      raise_error ("::SetParent");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::SetParentWindow");
    throw;
  }
}

const void* WindowsWindowManager::GetParentWindowHandle (window_t child)
{
  try
  {
    HWND parent = GetParent ((HWND)child);

    check_errors ("::GetParent");

    return parent;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::GetParentWindow");
    throw;
  }
}

/*
    ���������� ����
*/

void WindowsWindowManager::InvalidateWindow (window_t handle)
{
  HWND wnd = (HWND)handle;

  try
  {
    if (!InvalidateRect (wnd, 0, FALSE))
      raise_error ("::InvalidateRect");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::InvalidateWindow");
    throw;
  }
}

/*
    ��������� �������
*/

void WindowsWindowManager::SetCursorPosition (const Point& position)
{
  try
  {
    if (!SetCursorPos (position.x, position.y))
      raise_error  ("::SetCursorPos");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::SetCursorPosition(const Point&)");
    throw;
  }
}

Point WindowsWindowManager::GetCursorPosition ()
{
  try
  {
    POINT position;

    if (!GetCursorPos (&position))
      raise_error ("::GetCursorPos");

    return Point (position.x, position.y);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::GetCursorPosition()");
    throw;
  }
}

//� ���������� ����������� ����
void WindowsWindowManager::SetCursorPosition (window_t handle, const Point& client_position)
{
  try
  {
    HWND wnd = (HWND)handle;

    POINT screen_position = { client_position.x, client_position.y };

    if (!ClientToScreen (wnd, &screen_position))
      raise_error ("::ClientToScreen");

    if (!SetCursorPos (screen_position.x, screen_position.y))
      raise_error ("::SetCursorPos");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::SetCursorPosition(window_t, const Point&)");
    throw;
  }
}

//� ���������� ����������� ����
Point WindowsWindowManager::GetCursorPosition (window_t handle)
{
  try
  {
    HWND wnd = (HWND)handle;

    POINT position;

    if (!GetCursorPos (&position))
      raise_error ("::GetCursorPos");

    if (!ScreenToClient (wnd, &position))
      raise_error ("::ScreenToClient");

    return Point (position.x, position.y);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::GetCursorPosition(window_t)");
    throw;
  }
}

/*
    ��������� �������
*/

void WindowsWindowManager::SetCursorVisible (window_t handle, bool state)
{
  try
  {
    HWND        wnd  = (HWND)handle;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));

    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");

    impl->is_cursor_visible = state;

      //������� WM_SETCURSOR

    POINT position;

    if (!GetCursorPos (&position))
      raise_error ("::GetCursorPos");

    if (!SetCursorPos (position.x, position.y))
      raise_error ("::SetCursorPos");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::SetCursorVisible");
    throw;
  }
}

bool WindowsWindowManager::GetCursorVisible (window_t handle)
{
  try
  {
    HWND        wnd  = (HWND)handle;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));

    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");

    return impl->is_cursor_visible;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::GetCursorVisible");
    throw;
  }
}

/*
    ����������� �������
*/
#ifndef WINCE

cursor_t WindowsWindowManager::CreateCursor (const char* file_name, int hotspot_x, int hotspot_y)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
    
    if (hotspot_x != -1)
      throw xtl::format_not_supported_exception ("", "Custom hotspot_x=%d not supported", hotspot_x);
      
    if (hotspot_y != -1)
      throw xtl::format_not_supported_exception ("", "Custom hotspot_y=%d not supported", hotspot_y);
      
    SysTempFile cursor_file (file_name);
      
    HANDLE cursor = LoadImageA (0, cursor_file.Path ().c_str (), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
    
    if (!cursor)
      raise_error ("::LoadImageA");

    return reinterpret_cast<cursor_t> (cursor);      
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::CreateCursor");
    throw;
  }
}

void WindowsWindowManager::DestroyCursor (cursor_t cursor)
{
  try
  {
    if (GetCursor () == reinterpret_cast<HCURSOR> (cursor))
      ::SetCursor (LoadCursor (0, IDC_ARROW));
//      throw xtl::format_operation_exception ("", "Can't destroy active cursor");

    if (!::DestroyCursor (reinterpret_cast<HCURSOR> (cursor)))
      raise_error ("::DestroyCursor");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::DestroyCursor");
    throw;
  }
}

void WindowsWindowManager::SetCursor (window_t window, cursor_t cursor)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    HWND        wnd  = (HWND)window;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));

    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");    
    
    impl->preferred_cursor = reinterpret_cast<HCURSOR> (cursor);

    if (impl->is_cursor_visible)
    {
      HCURSOR required_cursor = impl->preferred_cursor ? impl->preferred_cursor : impl->default_cursor;
      
      if (required_cursor != ::GetCursor ())
        ::SetCursor (required_cursor);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::SetCursor");
    throw;
  }
}

#endif

/*
   ���������/��������� multitouch ������
*/

void WindowsWindowManager::SetMultitouchEnabled (window_t window, bool state)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    HWND        wnd  = (HWND)window;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));

    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");

    impl->is_multitouch_enabled = state;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::SetMultitouchEnabled");
    throw;
  }
}

bool WindowsWindowManager::IsMultitouchEnabled (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    HWND        wnd  = (HWND)window;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));

    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");

    return impl->is_multitouch_enabled;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::IsMultitouchEnabled");
    throw;
  }
}

/*
    ���� ����
*/

void WindowsWindowManager::SetBackgroundColor (window_t window, const Color& color)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    HWND        wnd  = (HWND)window;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));
    
    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");
      
    if (impl->background_brush)
    {
      DeleteObject (impl->background_brush);
      impl->background_brush = 0;
    }
    
    impl->background_brush = CreateSolidBrush (RGB (color.red, color.green, color.blue));
    
    if (!impl->background_brush)
      raise_error ("::CreateSolidBrush");
      
    impl->background_color = color; 
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::SetBackgroundColor");
    throw;
  }
}

void WindowsWindowManager::SetBackgroundState (window_t window, bool state)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    HWND        wnd  = (HWND)window;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));
    
    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");
      
    impl->background_state = state;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::SetBackgroundState");
    throw;
  }
}

Color WindowsWindowManager::GetBackgroundColor (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    HWND        wnd  = (HWND)window;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));
    
    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");
      
    return impl->background_color;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::GetBackgroundColor");
    throw;
  }
}

bool WindowsWindowManager::GetBackgroundState (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    HWND        wnd  = (HWND)window;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));
    
    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");
      
    return impl->background_state;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::GetBackgroundState");
    throw;
  }
}
