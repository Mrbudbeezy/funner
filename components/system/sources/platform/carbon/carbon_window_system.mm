#import <AppKit/NSCursor.h>
#import <AppKit/NSImage.h>
#import <Foundation/NSData.h>

#if ! defined (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) || (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1040)
  #error "__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ not defined or less then 1040"
#endif

#if (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1050)
  #define MACOSX_10_5_SUPPORTED
#endif

#include "shared.h"

using namespace syslib;

namespace
{

const char* LOG_NAME = "syslib.Platform";

const OSType WINDOW_PROPERTY_CREATOR     = 'untg';  //��� ����������
const OSType FULLSCREEN_PROPERTY_TAG     = 'fscr';  //��� �������� ���������������
const OSType CURSOR_VISIBLE_PROPERTY_TAG = 'hcrs';  //��� ��������� ������� (���� ������ - ������ �����)
const OSType WINDOW_IMPL_PROPERTY        = 'impl';  //��������� �� ���������� ������� ����

const size_t CHAR_CODE_BUFFER_SIZE = 4;  //������ ������� ��� ��������������� ����� �������

/*
    �������� ���������� ����
*/

struct WindowImpl
{
  void*                user_data;                               //��������� �� ���������������� ������
  WindowMessageHandler message_handler;                         //������� ��������� ��������� ����
  EventHandlerRef      carbon_window_event_handler;             //���������� ��������� ����
  EventHandlerUPP      carbon_window_event_handler_proc;        //���������� ��������� ����
  EventHandlerRef      carbon_application_event_handler;        //���������� ��������� ����������
  EventHandlerUPP      carbon_application_event_handler_proc;   //���������� ��������� ����������
  WindowRef            carbon_window;                           //����
  UniChar              char_code_buffer[CHAR_CODE_BUFFER_SIZE]; //������ ��� ��������� ����� ���������� �������
  bool                 is_cursor_in_window;                     //��������� �� ������ � ����
  NSCursor             *cursor;                                 //������ ������������ ��� �����
  bool                 background_state;                        //������� �� ������ ���
  Color                background_color;                        //���� ������� ����
  WindowGroupRef       window_group;
  bool                 is_maximized;                            //���� ��������� � ������������� ������
  bool                 is_multitouch_enabled;                   //������� �� multitouch

  WindowImpl (WindowMessageHandler handler, void* in_user_data)
    : user_data (in_user_data)
    , message_handler (handler)
    , carbon_window_event_handler (0)
    , carbon_window_event_handler_proc (0)
    , carbon_application_event_handler (0)
    , carbon_application_event_handler_proc (0)
    , carbon_window (0)
    , is_cursor_in_window (false)
    , cursor ([[NSCursor arrowCursor] retain])
    , background_state (false)
    , window_group (0)
    , is_maximized (false)
    , is_multitouch_enabled (false)
    {}

  ~WindowImpl ()
  {
    if (carbon_window_event_handler)
      RemoveEventHandler (carbon_window_event_handler);

    if (carbon_window_event_handler_proc)
      DisposeEventHandlerUPP (carbon_window_event_handler_proc);

    if (carbon_application_event_handler)
      RemoveEventHandler (carbon_application_event_handler);

    if (carbon_application_event_handler_proc)
      DisposeEventHandlerUPP (carbon_application_event_handler_proc);

    if (carbon_window)
      DisposeWindow (carbon_window);

    if (window_group)
      ReleaseWindowGroup (window_group);

    if (is_maximized)
      SetSystemUIMode (kUIModeNormal, 0);

    [cursor release];
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

bool check_fullscreen (WindowRef window)
{
  bool is_fullscreen;

  try
  {
    check_window_manager_error (GetWindowProperty (window, WINDOW_PROPERTY_CREATOR, FULLSCREEN_PROPERTY_TAG,
                                sizeof (is_fullscreen), 0, &is_fullscreen), "::GetWindowProperty", "Can't get window property");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::check_fullscreen");
    throw;
  }

  return is_fullscreen;
}

/*
    ��������� / ��������� ������� ����
*/

void get_rect (WindowRef wnd, WindowRegionCode region, syslib::Rect& rect, const char* source)
{
  ::Rect window_rect;

  check_window_manager_error (GetWindowBounds (wnd, region, &window_rect), source,
                              "Can't get window rect, ::GetWindowBounds error");

  if (check_fullscreen (wnd))  //� ������������� ������ ���� ����� ��������� ����������, ���� � ������������ ���������
  {
    rect.top    = 0;
    rect.left   = 0;
    rect.bottom = window_rect.bottom - window_rect.top;
    rect.right  = window_rect.right - window_rect.left;
  }
  else
  {
    rect.top    = window_rect.top;
    rect.left   = window_rect.left;
    rect.bottom = window_rect.bottom;
    rect.right  = window_rect.right;
  }
}

void set_rect (WindowRef wnd, WindowRegionCode region, const syslib::Rect& rect, const char* source)
{
  ::Rect new_rect = { rect.top, rect.left, rect.bottom, rect.right};

  check_window_manager_error (SetWindowBounds (wnd, region, &new_rect), source,
                              "Can't set window rect, error at ::SetWindowBounds");
}

/*
   ���������� ������, ���� ������ ��������� � ���������� ������� ����.
*/

bool is_cursor_in_client_region (WindowRef wnd)
{
  syslib::Rect client_rect;
  ::Point      mouse_coord;

  GetGlobalMouse (&mouse_coord);

  get_rect (wnd, kWindowContentRgn, client_rect, "::window_message_handler");

  if ((mouse_coord.h >= (int)client_rect.left) && (mouse_coord.v >= (int)client_rect.top) &&
      (mouse_coord.h <= (int)client_rect.right) && (mouse_coord.v <= (int)client_rect.bottom))
    return true;

  return false;
}

/*
    ����� ������ ����
*/

enum MouseKey
{
  MouseKey_Primary   = 1,
  MouseKey_Secondary = 2,
  MouseKey_Tertiary  = 4
};

/*
    ��������� ��������� �������
*/

void GetEventContext (WindowRef wnd, WindowEventContext& context)
{
  memset (&context, 0, sizeof (context));

  context.handle = wnd;

  context.cursor_position = CarbonWindowManager::GetCursorPosition ((window_t)wnd);

  UInt32 key_modifiers = GetCurrentEventKeyModifiers ();

  context.keyboard_alt_pressed        = (key_modifiers & optionKey) != 0;
  context.keyboard_control_pressed    = (key_modifiers & controlKey) != 0;
  context.keyboard_shift_pressed      = (key_modifiers & shiftKey) != 0;

  UInt32 mouse_keys = GetCurrentButtonState ();
  context.mouse_left_button_pressed   = (mouse_keys & MouseKey_Primary) != 0;
  context.mouse_right_button_pressed  = (mouse_keys & MouseKey_Secondary) != 0;
  context.mouse_middle_button_pressed = (mouse_keys & MouseKey_Tertiary) != 0;
}

/*
    �������������� ����������� ����� ������ � syslib::Key
*/

Key VirtualKey2SystemKey (size_t vkey)
{
  static size_t map [] =
  {
    Key_A, Key_S, Key_D, Key_F, Key_H, Key_G, Key_Z, Key_X,  //0x00-0x07
    Key_C, Key_V, 0, Key_B, Key_Q, Key_W, Key_E, Key_R,  //0x08-0x0F
    Key_Y, Key_T, Key_1, Key_2, Key_3, Key_4, Key_6, Key_5, //0x10-0x17
    Key_Plus, Key_9, Key_7, Key_Minus, Key_8, Key_0, Key_RightBracket, Key_O, //0x18-0x1F
    Key_U, Key_LeftBracket, Key_I, Key_P, Key_Enter, Key_L, Key_J, Key_Apostrophe, //0x20-0x27
    Key_K, Key_Semicolon, Key_BackSlash, Key_Comma, Key_Slash, Key_N, Key_M, Key_Dot, //0x28-0x2F
    Key_Tab, Key_Space, Key_Tilda, Key_BackSpace, 0, Key_Escape, 0, 0, //0x30-0x37
    0, 0, 0, 0, 0, 0, 0, 0, //0x38-0x3F
    0, Key_NumPadDot, 0, Key_NumPadMultiply, 0, Key_NumPadPlus, 0, Key_NumLock, //0x40-0x47
    0, 0, 0, Key_NumPadDivide, Key_Enter, 0, Key_NumPadMinus, 0, //0x48-0x4F
    0, 0, Key_NumPad0, Key_NumPad1, Key_NumPad2, Key_NumPad3, Key_NumPad4, Key_NumPad5, //0x50-0x57
    Key_NumPad6, Key_NumPad7, 0, Key_NumPad8, Key_NumPad9, 0, 0, 0, //0x58-0x5F
    Key_F5, Key_F6, Key_F7, Key_F3, Key_F8, Key_F9, 0, Key_F11, //0x60-0x67
    0, 0, 0, 0, 0, Key_F10, 0, Key_F12, //0x68-0x6F
    0, 0, Key_Insert, Key_Home, Key_PageUp, Key_Delete, Key_F4, Key_End, //0x70-0x77
    Key_F2, Key_PageDown, Key_F1, Key_Left, Key_Right, Key_Down, Key_Up, 0, //0x78-0x7F
    0, 0, 0, 0, 0, 0, 0, 0, //0x80-0x87
    0, 0, 0, 0, 0, 0, 0, 0, //0x88-0x8F
    0, 0, 0, 0, 0, 0, 0, 0, //0x90-0x97
    0, 0, 0, 0, 0, 0, 0, 0, //0x98-0x9F
    0, 0, 0, 0, 0, 0, 0, 0, //0xA0-0xA7
    0, 0, 0, 0, 0, 0, 0, 0, //0xA8-0xAF
    0, 0, 0, 0, 0, 0, 0, 0, //0xB0-0xB7
    0, 0, 0, 0, 0, 0, 0, 0, //0xB8-0xBF
    0, 0, 0, 0, 0, 0, 0, 0, //0xC0-0xC7
    0, 0, 0, 0, 0, 0, 0, 0, //0xC8-0xCF
    0, 0, 0, 0, 0, 0, 0, 0, //0xD0-0xD7
    0, 0, 0, 0, 0, 0, 0, 0, //0xD8-0xDF
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
    ��������� ������� ����
*/

OSStatus window_message_handler (EventHandlerCallRef event_handler_call_ref, EventRef event, void* impl)
{
  bool               event_processed = true;
  UInt32             event_kind      = GetEventKind (event);
  WindowImpl*        window_impl     = (WindowImpl*)impl;
  WindowRef          wnd             = window_impl->carbon_window;
  window_t window_handle   = (window_t)wnd;

  try
  {
    //��������� ��������� �������

    WindowEventContext context;

    GetEventContext (wnd, context);

    switch (GetEventClass (event))
    {
      case kEventClassTextInput:
      {
        if (event_kind != kEventTextInputUnicodeForKeyEvent)
        {
          event_processed = false;
          break;
        }

        UInt32 char_code_size;

        check_event_manager_error (GetEventParameter(event, kEventParamTextInputSendText, typeUnicodeText, 0, 0, &char_code_size, 0),
                                   "::GetEventParameter", "Can't get char code");

        char_code_size /= sizeof (UniChar);

        if (char_code_size > CHAR_CODE_BUFFER_SIZE)
          char_code_size = CHAR_CODE_BUFFER_SIZE;

        check_event_manager_error (GetEventParameter(event, kEventParamTextInputSendText, typeUnicodeText, 0,
                                   sizeof(UniChar) * char_code_size, 0, window_impl->char_code_buffer),
                                   "::GetEventParameter", "Can't get char code");

        CFStringRef char_code = CFStringCreateWithCharacters (0, window_impl->char_code_buffer, char_code_size);

        if (!char_code)
          throw xtl::format_operation_exception ("::CFStringCreateWithCharacters", "Can't create char code string");

        CFRange char_code_range = { 0, 1 }; //CFStringGetLength (char_code)};

        CFStringGetBytes (char_code, char_code_range, kCFStringEncodingUTF32, '?', false, (UInt8*)&context.char_code,
                          sizeof (wchar_t), 0);

        CFRelease (char_code);

        window_impl->Notify (window_handle, WindowEvent_OnChar, context);

        break;
      }
      case kEventClassWindow:
        switch (event_kind)
        {
          case kEventWindowClose: //������� �������� ����
            window_impl->Notify (window_handle, WindowEvent_OnClose, context);
            break;
          case kEventWindowClosed: //���� �������
            window_impl->Notify (window_handle, WindowEvent_OnDestroy, context);

            delete window_impl;
            break;
          case kEventWindowDrawContent: //����������� ����
            window_impl->Notify (window_handle, WindowEvent_OnPaint, context);
            break;
          case kEventWindowActivated: //���� ����� ��������
            if (is_cursor_in_client_region (wnd))
            {
              CarbonWindowManager::SetCursorVisible (window_handle, CarbonWindowManager::GetCursorVisible (window_handle));

              [window_impl->cursor set];

              window_impl->is_cursor_in_window = true;
            }

            window_impl->Notify (window_handle, WindowEvent_OnActivate, context);
            break;
          case kEventWindowDeactivated: //���� ����� ����������
            window_impl->Notify (window_handle, WindowEvent_OnDeactivate, context);
            break;
          case kEventWindowShown: //���� ����� �������
            window_impl->is_cursor_in_window = is_cursor_in_client_region (wnd);
            window_impl->Notify (window_handle, WindowEvent_OnShow, context);
            break;
          case kEventWindowHidden: //���� ����� ���������
            window_impl->is_cursor_in_window = false;
            window_impl->Notify (window_handle, WindowEvent_OnHide, context);
            break;
          case kEventWindowBoundsChanged: //���� �������� ������� ��� ���������
            CarbonWindowManager::InvalidateWindow (window_handle);
            window_impl->Notify (window_handle, WindowEvent_OnSize, context);
            window_impl->Notify (window_handle, WindowEvent_OnMove, context);
            break;
#ifdef MACOSX_10_5_SUPPORTED
          case kEventWindowFocusRestored:
#endif
          case kEventWindowFocusAcquired: //���� �������� �����
            window_impl->Notify (window_handle, WindowEvent_OnSetFocus, context);
            break;
          case kEventWindowFocusRelinquish:
#ifdef MACOSX_10_5_SUPPORTED
          case kEventWindowFocusLost:       //���� �������� �����
#endif
            window_impl->Notify (window_handle, WindowEvent_OnLostFocus, context);
            break;
          default:
            event_processed = false;
            break;
        }
        break;
      case kEventClassKeyboard:
      {
        UInt32 key;

        check_event_manager_error (GetEventParameter(event, kEventParamKeyCode, typeUInt32, 0, sizeof(UInt32), 0, &key),
                                   "::GetEventParameter", "Can't get key code");

        context.key           = VirtualKey2SystemKey (key);
        context.key_scan_code = (syslib::ScanCode)key;

        switch (event_kind)
        {
          case kEventRawKeyDown: //������� ������� ����������
          {
            UInt32 key_modifiers = GetCurrentEventKeyModifiers ();

            if (key_modifiers & cmdKey)
            {
              switch (context.key)
              {
                case Key_Q:
                  syslib::Application::Exit (0);
                  break;
                case Key_W:
                  CarbonWindowManager::CloseWindow (window_handle);
                  break;
                case Key_M:
                  if (IsWindowCollapsable (wnd))
                    check_window_manager_error (CollapseWindow (wnd, true), "::CollapseWindow", "Can't collapse window");

                  break;
                default:
                  window_impl->Notify (window_handle, WindowEvent_OnKeyDown, context);
                  break;
              }
            }
            else
              window_impl->Notify (window_handle, WindowEvent_OnKeyDown, context);

            break;
          }
          case kEventRawKeyRepeat: //��������� ������� ����������
            window_impl->Notify (window_handle, WindowEvent_OnKeyDown, context);
            break;
          case kEventRawKeyUp: //������� ������� ����������
            window_impl->Notify (window_handle, WindowEvent_OnKeyUp, context);
            break;
          default:
            event_processed = false;
            break;
        }
        break;
      }
      case kEventClassMouse:
      {
        bool mouse_in_client_rect = is_cursor_in_client_region (wnd);

        context.cursor_position = CarbonWindowManager::GetCursorPosition (window_handle);

        switch (event_kind)
        {
          case kEventMouseDown: //������� ������� ����
          {
            if (!mouse_in_client_rect)
              break;

            EventMouseButton pressed_button;

            check_event_manager_error (GetEventParameter (event, kEventParamMouseButton, typeMouseButton, 0, sizeof (EventMouseButton), 0, &pressed_button),
                                       "::GetEventParameter", "Can't get mouse button");

            UInt32 click_count;

            check_event_manager_error (GetEventParameter (event, kEventParamClickCount, typeUInt32, 0, sizeof (UInt32), 0, &click_count),
                                       "::GetEventParameter", "Can't get click count");

            switch (pressed_button)
            {
              case kEventMouseButtonPrimary:
                if (click_count % 2)
                  window_impl->Notify (window_handle, WindowEvent_OnLeftButtonDown, context);
                else
                  window_impl->Notify (window_handle, WindowEvent_OnLeftButtonDoubleClick, context);
                break;
              case kEventMouseButtonSecondary:
                if (click_count % 2)
                  window_impl->Notify (window_handle, WindowEvent_OnRightButtonDown, context);
                else
                  window_impl->Notify (window_handle, WindowEvent_OnRightButtonDoubleClick, context);
                break;
              case kEventMouseButtonTertiary:
                if (click_count % 2)
                  window_impl->Notify (window_handle, WindowEvent_OnMiddleButtonDown, context);
                else
                  window_impl->Notify (window_handle, WindowEvent_OnMiddleButtonDoubleClick, context);
                break;
            }

            break;
          }
          case kEventMouseUp: //������� ������� ����
          {
            if (!mouse_in_client_rect)
              break;

            EventMouseButton released_button;

            check_event_manager_error (GetEventParameter (event, kEventParamMouseButton, typeMouseButton, 0, sizeof (EventMouseButton), 0, &released_button),
                                       "::GetEventParameter", "Can't get mouse button");

            switch (released_button)
            {
              case kEventMouseButtonPrimary:
                window_impl->Notify (window_handle, WindowEvent_OnLeftButtonUp, context);
                break;
              case kEventMouseButtonSecondary:
                window_impl->Notify (window_handle, WindowEvent_OnRightButtonUp, context);
                break;
              case kEventMouseButtonTertiary:
                window_impl->Notify (window_handle, WindowEvent_OnMiddleButtonUp, context);
                break;
            }

            break;
          }
          case kEventMouseDragged:
          case kEventMouseMoved: //����������� ����
            if (!mouse_in_client_rect)
              break;

            if (!CarbonWindowManager::GetCursorVisible (window_handle) && CGCursorIsVisible ()) //���� ������ ������� - ������ ��� ��� ����� � ����
              check_quartz_error (CGDisplayHideCursor (kCGDirectMainDisplay), "::CGDisplayHideCursor", "Can't hide cursor");
            else
              [window_impl->cursor set];

            window_impl->is_cursor_in_window = true;

            window_impl->Notify (window_handle, WindowEvent_OnMouseMove, context);

            break;
          case kEventMouseWheelMoved: //��������� ��������� ������ ����
          {
            EventMouseWheelAxis moved_axis;

            check_event_manager_error (GetEventParameter (event, kEventParamMouseWheelAxis, typeMouseWheelAxis, 0,
                                                          sizeof (EventMouseWheelAxis), 0, &moved_axis),
                                       "::GetEventParameter", "Can't get mouse wheel axis");

            SInt32 wheel_delta;

            check_event_manager_error (GetEventParameter (event, kEventParamMouseWheelDelta, typeSInt32, 0,
                                                          sizeof (SInt32), 0, &wheel_delta),
                                       "::GetEventParameter", "Can't get mouse wheel delta");

            switch (moved_axis)
            {
              case kEventMouseWheelAxisX:
                context.mouse_horisontal_wheel_delta = wheel_delta;
                window_impl->Notify (window_handle, WindowEvent_OnMouseHorisontalWheel, context);
                break;
              case kEventMouseWheelAxisY:
                context.mouse_vertical_wheel_delta = wheel_delta;
                window_impl->Notify (window_handle, WindowEvent_OnMouseVerticalWheel, context);
                break;
            }

            break;
          }
          default:
            event_processed = false;
            break;
        }
        break;
      }
      default:
        event_processed = false;
        break;
    }

    if (event_processed)
    {
      OSStatus operation_result = CallNextEventHandler (event_handler_call_ref, event);

      if (operation_result != eventNotHandledErr)
        check_event_manager_error (operation_result, "::CallNextEventHandler", "Can't call next event handler");
      return noErr;
    }
    else
      return eventNotHandledErr;
  }
  catch (std::exception& exception)
  {
    printf ("Exception at processing event in ::window_message_handler : '%s'\n", exception.what ());
  }
  catch (...)
  {
    printf ("Exception at processing event in ::window_message_handler : unknown exception\n");
  }

  return eventNotHandledErr;
}

/*
    ��������� ������� ����������
*/

OSStatus application_message_handler (EventHandlerCallRef event_handler_call_ref, EventRef event, void* impl)
{
  bool        is_fullscreen = false;
  WindowImpl* window_impl   = (WindowImpl*)impl;
  WindowRef   wnd           = window_impl->carbon_window;

  try
  {
    is_fullscreen = check_fullscreen (wnd);
  }
  catch (std::exception& exception)
  {
    printf ("Exception at processing event in ::application_message_handler : '%s'\n", exception.what ());
  }
  catch (...)
  {
    printf ("Exception at processing event in ::application_message_handler : unknown exception\n");
  }

  if (window_impl->is_cursor_in_window && GetEventClass (event) == kEventClassMouse)  //��������� ������ ����� �� ������� ����
  {
    switch (GetEventKind (event))
    {
      case kEventMouseMoved:
      case kEventMouseDragged:
      {
        //��������� ��������� �������

        WindowEventContext context;

        GetEventContext (wnd, context);

        if (!is_cursor_in_client_region (wnd))
        {
          window_t window_handle = (window_t)wnd;

          window_impl->is_cursor_in_window = false;
          window_impl->Notify (window_handle, WindowEvent_OnMouseLeave, context);

          if (!CarbonWindowManager::GetCursorVisible (window_handle)) //���� ������ ������� - ���������� ��� ��� ������ �� ����
            while (!CGCursorIsVisible ())
              CGDisplayShowCursor (kCGDirectMainDisplay);

          [[NSCursor arrowCursor] set];
        }

        break;
      }
      default:
        break;
    }
  }

  if (is_fullscreen)
    return window_message_handler (event_handler_call_ref, event, impl);

  return eventNotHandledErr;
}

}


/*
    ��������/��������/����������� ����
*/

window_t CarbonWindowManager::CreateWindow (WindowStyle style, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data)
{
  static const char* METHOD_NAME = "syslib::CarbonWindowManager::CreateWindow";

  WindowClass window_class;
  UInt32      window_attributes;

  switch (style)
  {
    case WindowStyle_Overlapped:
      window_class = kDocumentWindowClass;
      window_attributes = kWindowStandardDocumentAttributes | kWindowStandardHandlerAttribute | kWindowLiveResizeAttribute;
      break;
    case WindowStyle_PopUp:
      window_class = kSheetWindowClass;
      window_attributes = kWindowStandardHandlerAttribute | kWindowLiveResizeAttribute;
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "style", style);
      return 0;
  }

  try
  {
    WindowImpl* window_impl = new WindowImpl (handler, user_data);

    try
    {
        //�������� ����

      WindowRef new_window;

      ::Rect window_rect = {0, 0, 400, 600};

      check_window_manager_error (CreateNewWindow (window_class, window_attributes, &window_rect, &new_window), "::CreateNewWindow",
                                  "Can't create window");

      window_impl->carbon_window = new_window;

      bool initial_cursor_visibility = true;

      check_window_manager_error (SetWindowProperty (new_window, WINDOW_PROPERTY_CREATOR, CURSOR_VISIBLE_PROPERTY_TAG,
                                  sizeof (initial_cursor_visibility), &initial_cursor_visibility), "::SetWindowProperty", "Can't set window property");

      RepositionWindow (new_window, 0, kWindowCenterOnMainScreen);

        //��������� ����������� ������� ����

      EventHandlerRef window_event_handler;

      EventHandlerUPP window_event_handler_proc = NewEventHandlerUPP (&window_message_handler);

      EventTypeSpec window_handled_event_types [] = {
        { kEventClassWindow,    kEventWindowClose },
        { kEventClassWindow,    kEventWindowClosed },
        { kEventClassWindow,    kEventWindowDrawContent },
        { kEventClassWindow,    kEventWindowActivated },
        { kEventClassWindow,    kEventWindowDeactivated },
        { kEventClassWindow,    kEventWindowShown },
        { kEventClassWindow,    kEventWindowHidden },
        { kEventClassWindow,    kEventWindowBoundsChanged },
        { kEventClassWindow,    kEventWindowFocusAcquired },
        { kEventClassWindow,    kEventWindowFocusRelinquish },
#ifdef MACOSX_10_5_SUPPORTED
        { kEventClassWindow,    kEventWindowFocusLost },
        { kEventClassWindow,    kEventWindowFocusRestored },
#endif
        { kEventClassKeyboard,  kEventRawKeyDown },
        { kEventClassKeyboard,  kEventRawKeyRepeat },
        { kEventClassKeyboard,  kEventRawKeyUp },
        { kEventClassMouse,     kEventMouseDown },
        { kEventClassMouse,     kEventMouseUp },
        { kEventClassMouse,     kEventMouseMoved },
        { kEventClassMouse,     kEventMouseDragged },
        { kEventClassMouse,     kEventMouseWheelMoved },
        { kEventClassTextInput, kEventTextInputUnicodeForKeyEvent },
      };

      check_event_manager_error (InstallEventHandler (GetWindowEventTarget (new_window), window_event_handler_proc,
                                 sizeof (window_handled_event_types) / sizeof (window_handled_event_types[0]), window_handled_event_types,
                                 window_impl, &window_event_handler), "::InstallEventHandler", "Can't install event handler");

      window_impl->carbon_window_event_handler = window_event_handler;
      window_impl->carbon_window_event_handler_proc = window_event_handler_proc;

        //��������� ����������� ������� ���������� (��� ��������� ������� ���� � ������������� ������)

      EventHandlerRef application_event_handler;

      EventHandlerUPP application_event_handler_proc = NewEventHandlerUPP (&application_message_handler);

      EventTypeSpec application_handled_event_types [] = {
        { kEventClassMouse,     kEventMouseDown },
        { kEventClassMouse,     kEventMouseUp },
        { kEventClassMouse,     kEventMouseMoved },
        { kEventClassMouse,     kEventMouseDragged },
        { kEventClassMouse,     kEventMouseWheelMoved },
      };

      check_event_manager_error (InstallEventHandler (GetApplicationEventTarget (), application_event_handler_proc,
                                 sizeof (application_handled_event_types) / sizeof (application_handled_event_types[0]),
                                 application_handled_event_types, window_impl, &application_event_handler),
                                 "::InstallEventHandler", "Can't install event handler");

      window_impl->carbon_application_event_handler = application_event_handler;
      window_impl->carbon_application_event_handler_proc = application_event_handler_proc;

      bool is_fullscreen = false;

      check_window_manager_error (SetWindowProperty (new_window, WINDOW_PROPERTY_CREATOR, FULLSCREEN_PROPERTY_TAG,
                                  sizeof (is_fullscreen), &is_fullscreen), "::SetWindowProperty", "Can't set window property");

      bool is_cursor_visible = true;

      check_window_manager_error (SetWindowProperty (new_window, WINDOW_PROPERTY_CREATOR, CURSOR_VISIBLE_PROPERTY_TAG,
                                  sizeof (is_cursor_visible), &is_cursor_visible), "::SetWindowProperty", "Can't set window property");

      check_window_manager_error (SetWindowProperty (new_window, WINDOW_PROPERTY_CREATOR, WINDOW_IMPL_PROPERTY,
                                  sizeof (window_impl), &window_impl), "::SetWindowProperty", "Can't set window property");

      SetMouseCoalescingEnabled (false, 0);

      transform_process_type ();

      check_window_manager_error (CreateWindowGroup (kWindowGroupAttrMoveTogether | kWindowGroupAttrLayerTogether | kWindowGroupAttrSharedActivation | kWindowGroupAttrHideOnCollapse,
                                  &window_impl->window_group), "::CreateWindowGroup", "Can't create window group");

      check_window_manager_error (SetWindowGroup (new_window, window_impl->window_group), "::SetWindowGroup", "Can't set window group");

      SetParentWindowHandle ((window_t)new_window, parent_handle);

      return (window_t)new_window;
    }
    catch (...)
    {
      delete window_impl;

      throw;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);

    throw;
  }

  return 0;
}

void CarbonWindowManager::CloseWindow (window_t handle)
{
  EventRef close_window_event = 0;

  try
  {
    check_event_manager_error (CreateEvent (0, kEventClassWindow, kEventWindowClose, 0, kEventAttributeNone, &close_window_event),
                               "::CreateEvent", "Can't create window close event");

    check_event_manager_error (SendEventToEventTarget (close_window_event, GetWindowEventTarget ((WindowRef)handle)),
                               "::SendEventToEventTarget", "Can't send close window event");
  }
  catch (xtl::exception& exception)
  {
    if (close_window_event)
      ReleaseEvent (close_window_event);

    exception.touch ("syslib::CarbonWindowManager::CloseWindow");
    throw;
  }

  if (close_window_event)
    ReleaseEvent (close_window_event);
}

void CarbonWindowManager::DestroyWindow (window_t handle)
{
  EventRef closed_window_event = 0;

  try
  {
    check_event_manager_error (CreateEvent (0, kEventClassWindow, kEventWindowClosed, 0, kEventAttributeNone, &closed_window_event),
                               "::CreateEvent", "Can't create window closed event");

    check_event_manager_error (SendEventToEventTarget (closed_window_event, GetWindowEventTarget ((WindowRef)handle)),
                               "::SendEventToEventTarget", "Can't send closed window event");
  }
  catch (xtl::exception& exception)
  {
    if (closed_window_event)
      ReleaseEvent (closed_window_event);

    exception.touch ("syslib::CarbonWindowManager::DestroyWindow");
    throw;
  }

  if (closed_window_event)
    ReleaseEvent (closed_window_event);
}

/*
    ��������� ���������-���������� ����������� ����
*/

const void* CarbonWindowManager::GetNativeWindowHandle (window_t handle)
{
  return reinterpret_cast<const void*> (handle);
}

const void* CarbonWindowManager::GetNativeDisplayHandle (window_t)
{
  return 0;
}

/*
    ��������� ����
*/

void CarbonWindowManager::SetWindowTitle (window_t handle, const wchar_t* title)
{
  CFStringRef new_title = 0;

  try
  {
    if (!title)
      throw xtl::make_null_argument_exception ("", "title");

    CFStringEncoding title_encoding;

    int dummy = 1;

    if (reinterpret_cast<char*> (&dummy)[0])
    {
      //Little endian
      if (sizeof (wchar_t) == 4) title_encoding = kCFStringEncodingUTF32LE;
      else                       title_encoding = kCFStringEncodingUTF16LE;
    }
    else
    {
      //Big endian
      if (sizeof (wchar_t) == 4) title_encoding = kCFStringEncodingUTF32BE;
      else                       title_encoding = kCFStringEncodingUTF16BE;
    }

    new_title = CFStringCreateWithBytes (0, (UInt8*)title, xtl::xstrlen (title) * sizeof (wchar_t),
                                         title_encoding, false);

    if (!new_title)
      throw xtl::format_operation_exception ("", "Can't create string with new title, CFStringCreateWithCharacters error");

    check_window_manager_error (SetWindowTitleWithCFString ((WindowRef)handle, new_title), "::SetWindowTitleWithCFString",
                                "Can't set window title");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::SetWindowTitle");
    CFRelease (new_title);
    throw;
  }

  CFRelease (new_title);
}

void CarbonWindowManager::GetWindowTitle (window_t handle, size_t buffer_size_in_chars, wchar_t* buffer)
{
  CFStringRef window_title;

  try
  {
    check_window_manager_error (CopyWindowTitleAsCFString ((WindowRef)handle, &window_title), "::CopyWindowTitleAsCFString",
                                "Can't get window title");

    CFRange title_range = { 0, CFStringGetLength (window_title) };

    CFIndex getted_chars = CFStringGetBytes (window_title, title_range, kCFStringEncodingUTF32, '?', false, (UInt8*)buffer,
                                             (buffer_size_in_chars - 1) * sizeof (wchar_t), 0);

    buffer [getted_chars] = 0;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::GetWindowTitle");
    CFRelease (window_title);
    throw;
  }

  CFRelease (window_title);
}

/*
    ������� ���� / ���������� �������
*/

void CarbonWindowManager::SetWindowRect (window_t handle, const Rect& rect)
{
  set_rect ((WindowRef)handle, kWindowStructureRgn, rect, "syslib::CarbonWindowManager::SetWindowRect");
}

void CarbonWindowManager::SetClientRect (window_t handle, const Rect& rect)
{
  set_rect ((WindowRef)handle, kWindowContentRgn, rect, "syslib::CarbonWindowManager::SetWindowRect");
}

void CarbonWindowManager::GetWindowRect (window_t handle, Rect& rect)
{
  get_rect ((WindowRef)handle, kWindowStructureRgn, rect, "syslib::CarbonWindowManager::GetWindowRect");
}

void CarbonWindowManager::GetClientRect (window_t handle, Rect& rect)
{
  get_rect ((WindowRef)handle, kWindowContentRgn, rect, "syslib::CarbonWindowManager::GetClientRect");

  rect.bottom -= rect.top;
  rect.right  -= rect.left;
  rect.top     = 0;
  rect.left    = 0;
}

/*
    ��������� ������ ����
*/

void CarbonWindowManager::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  WindowRef wnd = (WindowRef)handle;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible: //��������� ����
        state ? ShowWindow (wnd) : HideWindow (wnd);
        break;
      case WindowFlag_Active: //���������� ����
        check_window_manager_error (ActivateWindow (wnd, state), "::ActivateWindow", "Can't activate/deactivate window");
        break;
      case WindowFlag_Focus: //����� �����
        check_window_manager_error (SetUserFocusWindow (state ? wnd : kUserFocusAuto), "::SetUserFocusWindow", "Can't set focus window");
        break;
      case WindowFlag_Maximized:
      {
        WindowClass window_class;

        check_window_manager_error (GetWindowClass (wnd, &window_class), "::GetWindowClass", "Can't get window class");

        ShowWindow (wnd);

        if (window_class == kSheetWindowClass)
        {
          WindowImpl *impl;

          check_window_manager_error (GetWindowProperty (wnd, WINDOW_PROPERTY_CREATOR, WINDOW_IMPL_PROPERTY,
                                      sizeof (impl), 0, &impl), "::GetWindowProperty", "Can't get window property");

          if (state)
          {
            SetSystemUIMode (kUIModeAllHidden, 0);

            impl->is_maximized = true;

            CGRect display_bounds = CGDisplayBounds (CGMainDisplayID ());
            Rect   window_bounds;

            window_bounds.left   = display_bounds.origin.x;
            window_bounds.top    = display_bounds.origin.y;
            window_bounds.right  = display_bounds.origin.x + display_bounds.size.width;
            window_bounds.bottom = display_bounds.origin.y + display_bounds.size.height;

            SetClientRect (handle, window_bounds);
          }
          else
          {
            if (impl->is_maximized)
            {
              SetSystemUIMode (kUIModeNormal, 0);
              impl->is_maximized = false;
            }
          }
        }
        else
          ZoomWindow (wnd, state ? inZoomOut : inZoomIn, true);

        break;
      }
      case WindowFlag_Minimized:
        check_window_manager_error (CollapseWindow (wnd, state), "::CollapseWindow", "Can't minimize window");
        break;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
        break;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::SetWindowFlag");
    throw;
  }
}

bool CarbonWindowManager::GetWindowFlag (window_t handle, WindowFlag flag)
{
  WindowRef wnd = (WindowRef)handle;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
        return IsWindowVisible (wnd);
      case WindowFlag_Active:
        return IsWindowActive (wnd);
      case WindowFlag_Focus:
        return GetUserFocusWindow () == wnd;
      case WindowFlag_Maximized:
        throw xtl::format_operation_exception ("", "Can't get window flag %d value", flag);
      case WindowFlag_Minimized:
        return IsWindowCollapsed (wnd);
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
        break;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::GetWindowFlag");
    throw;
  }
}

/*
    ��������� ������������� ����
*/

void CarbonWindowManager::SetParentWindowHandle (window_t child, const void* parent)
{
  try
  {
    if (!child)
      throw xtl::make_null_argument_exception ("", "child");

    if (!parent)
    {
      WindowImpl *child_impl;

      check_window_manager_error (GetWindowProperty ((WindowRef)child, WINDOW_PROPERTY_CREATOR, WINDOW_IMPL_PROPERTY,
                                  sizeof (child_impl), 0, &child_impl), "::GetWindowProperty", "Can't get window property");

      check_window_manager_error (SetWindowGroup ((WindowRef)child, child_impl->window_group), "::SetWindowGroup", "Can't set window group");
    }
    else
    {
      WindowImpl *parent_impl;

      check_window_manager_error (GetWindowProperty ((WindowRef)parent, WINDOW_PROPERTY_CREATOR, WINDOW_IMPL_PROPERTY,
          sizeof (parent_impl), 0, &parent_impl), "::GetWindowProperty", "Can't get window property");

      check_window_manager_error (SetWindowGroup ((WindowRef)child, parent_impl->window_group), "::SetWindowGroup", "Can't set window group");
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::SetParentWindow");
    throw;
  }
}

const void* CarbonWindowManager::GetParentWindowHandle (window_t child)
{
  throw xtl::make_not_implemented_exception ("syslib::CarbonWindowManager::GetParentWindow");
}

/*
    ���������� ����
*/

void CarbonWindowManager::InvalidateWindow (window_t handle)
{
  try
  {
    ::Rect update_rect;

    GetWindowPortBounds ((WindowRef)handle, &update_rect);

    check_window_manager_error (InvalWindowRect ((WindowRef)handle, &update_rect), "::InvalidWindowRect", "Can't invalidate window");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::InvalidateWindow");
    throw;
  }
}

/*
    ��������� �������
*/

void CarbonWindowManager::SetCursorPosition (const Point& position)
{
  CGPoint new_cursor_position = { position.x, position.y };

  try
  {
    CGEventRef event = CGEventCreateMouseEvent (0, kCGEventMouseMoved, new_cursor_position, 0);

    if (!event)
      throw xtl::format_operation_exception ("::CGEventCreateMouseEvent", "Can't create mouse event");

    CGEventPost (kCGHIDEventTap, event);

    CFRelease (event);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::SetCursorPosition (const Point&)");
    throw;
  }
}

syslib::Point CarbonWindowManager::GetCursorPosition ()
{
  ::Point mouse_position;

  GetGlobalMouse (&mouse_position);

  return syslib::Point (mouse_position.h, mouse_position.v);
}

void CarbonWindowManager::SetCursorPosition (window_t handle, const Point& client_position)
{
  Rect  client_rect;
  Point new_cursor_position = client_position;

  try
  {
    get_rect ((WindowRef)handle, kWindowContentRgn, client_rect, "");

    size_t client_rect_width  = client_rect.right  - client_rect.left,
           client_rect_height = client_rect.bottom - client_rect.top;

    if (new_cursor_position.x > client_rect_width)
      new_cursor_position.x = client_rect_width;
    if (new_cursor_position.y > client_rect_height)
      new_cursor_position.y = client_rect_height;

    new_cursor_position.x += client_rect.left;
    new_cursor_position.y += client_rect.top;

    SetCursorPosition (new_cursor_position);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::SetCursorPosition (window_t, const Point&)");
    throw;
  }
}

syslib::Point CarbonWindowManager::GetCursorPosition (window_t handle)
{
  Rect client_rect;

  get_rect ((WindowRef)handle, kWindowContentRgn, client_rect, "syslib::CarbonWindowManager::GetCursorPosition (window_t)");

  ::Point mouse_position;

  GetGlobalMouse (&mouse_position);

  if (mouse_position.h < (int)client_rect.left)
    mouse_position.h = client_rect.left;
  if (mouse_position.v < (int)client_rect.top)
    mouse_position.v = client_rect.top;
  if (mouse_position.h > (int)client_rect.right)
    mouse_position.h = client_rect.right;
  if (mouse_position.v > (int)client_rect.bottom)
    mouse_position.v = client_rect.bottom;

  return syslib::Point (mouse_position.h - client_rect.left, mouse_position.v - client_rect.top);
}

/*
    ��������� �������
*/

void CarbonWindowManager::SetCursorVisible (window_t handle, bool state)
{
  try
  {
    if (GetWindowFlag (handle, WindowFlag_Active) && is_cursor_in_client_region ((WindowRef)handle))
    {
      if (state)
      {
        while (!CGCursorIsVisible ())
          check_quartz_error (CGDisplayShowCursor (kCGDirectMainDisplay), "::CGDisplayShowCursor", "Can't show cursor");
      }
      else
      {
        if (CGCursorIsVisible ())
          check_quartz_error (CGDisplayHideCursor (kCGDirectMainDisplay), "::CGDisplayHideCursor", "Can't hide cursor");
      }
    }

    check_window_manager_error (SetWindowProperty ((WindowRef)handle, WINDOW_PROPERTY_CREATOR, CURSOR_VISIBLE_PROPERTY_TAG,
        sizeof (state), &state), "::SetWindowProperty", "Can't set window property");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::SetCursorVisible");
    throw;
  }
}

bool CarbonWindowManager::GetCursorVisible (window_t handle)
{
  bool return_value;

  check_window_manager_error (GetWindowProperty ((WindowRef)handle, WINDOW_PROPERTY_CREATOR, CURSOR_VISIBLE_PROPERTY_TAG,
                              sizeof (return_value), 0, &return_value), "::GetWindowProperty", "Can't get window property");

  return return_value;
}

/*
    ����������� �������
*/

cursor_t CarbonWindowManager::CreateCursor (const char* file_name, int hotspot_x, int hotspot_y)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");

    common::InputFile image_file (file_name);

    size_t bytes_to_read = image_file.Size ();

    NSMutableData* image_data = [[NSMutableData alloc] initWithLength:bytes_to_read];

    if (!image_data)
      throw xtl::format_operation_exception ("[NSMutableData initWithLength:]", "Can't create mutable data with size %u", bytes_to_read);

    char* read_buffer = (char*)[image_data mutableBytes];

    try
    {
      while (bytes_to_read)
      {
        size_t readed_bytes = image_file.Read (read_buffer, bytes_to_read);

        bytes_to_read -= readed_bytes;
        read_buffer   += readed_bytes;
      }
    }
    catch (...)
    {
      [image_data release];
      throw;
    }

    NSImage* cursor_image = [[NSImage alloc] initWithData:image_data];

    [image_data release];

    if (!cursor_image)
      throw xtl::format_operation_exception ("[NSImage initWithData:]", "Can't create image");

    NSPoint cursor_hot_spot;

    cursor_hot_spot.x = hotspot_x == -1 ? 0 : hotspot_x;
    cursor_hot_spot.y = hotspot_y == -1 ? 0 : hotspot_y;

    NSCursor *cursor = [[NSCursor alloc] initWithImage:cursor_image hotSpot:cursor_hot_spot];

    [cursor_image release];

    if (!cursor)
      throw xtl::format_operation_exception ("[NSCursor initWithImage:hotSpot:]", "Can't create cursor");

    return reinterpret_cast<cursor_t> (cursor);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::CreateCursor");
    throw;
  }
}

void CarbonWindowManager::DestroyCursor (cursor_t cursor)
{
  [(NSCursor*)cursor release];
}

void CarbonWindowManager::SetCursor (window_t handle, cursor_t cursor)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    WindowImpl *impl;

    check_window_manager_error (GetWindowProperty ((WindowRef)handle, WINDOW_PROPERTY_CREATOR, WINDOW_IMPL_PROPERTY,
                                sizeof (impl), 0, &impl), "::GetWindowProperty", "Can't get window property");

    [impl->cursor release];

    impl->cursor = (NSCursor*)cursor ? [(NSCursor*)cursor retain] : [[NSCursor arrowCursor] retain];

    if (is_cursor_in_client_region ((WindowRef)handle))
      [impl->cursor set];
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::SetCursor");
    throw;
  }
}

/*
   ���������/��������� multitouch ������
*/

void CarbonWindowManager::SetMultitouchEnabled (window_t handle, bool state)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("", "handle");

  WindowImpl *impl;

  check_window_manager_error (GetWindowProperty ((WindowRef)handle, WINDOW_PROPERTY_CREATOR, WINDOW_IMPL_PROPERTY,
                              sizeof (impl), 0, &impl), "::GetWindowProperty", "Can't get window property");

  impl->is_multitouch_enabled = state;
}

bool CarbonWindowManager::IsMultitouchEnabled (window_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("", "handle");

  WindowImpl *impl;

  check_window_manager_error (GetWindowProperty ((WindowRef)handle, WINDOW_PROPERTY_CREATOR, WINDOW_IMPL_PROPERTY,
                              sizeof (impl), 0, &impl), "::GetWindowProperty", "Can't get window property");

  return impl->is_multitouch_enabled;
}

/*
   ���� ����
*/

void CarbonWindowManager::SetBackgroundColor (window_t window, const Color& color)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    WindowImpl *impl;

    check_window_manager_error (GetWindowProperty ((WindowRef)window, WINDOW_PROPERTY_CREATOR, WINDOW_IMPL_PROPERTY,
                                sizeof (impl), 0, &impl), "::GetWindowProperty", "Can't get window property");

    RGBColor rgb_color;

    rgb_color.red   = color.red / 255.f * 0xffff;
    rgb_color.green = color.green / 255.f * 0xffff;
    rgb_color.blue  = color.blue / 255.f * 0xffff;

    check_window_manager_error (SetWindowContentColor ((WindowRef)window, &rgb_color), "::SetWindowContentColor", "Can't set window content color");

    impl->background_color = color;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::CarbonWindowManager::SetBackgroundColor");
    throw;
  }
}

void CarbonWindowManager::SetBackgroundState (window_t window, bool state)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    WindowImpl *impl;

    check_window_manager_error (GetWindowProperty ((WindowRef)window, WINDOW_PROPERTY_CREATOR, WINDOW_IMPL_PROPERTY,
                                sizeof (impl), 0, &impl), "::GetWindowProperty", "Can't get window property");

    if (impl->background_state == state)
      return;

    impl->background_state = state;

    Color default_color;

    memset (&default_color, 0, sizeof (default_color));

    SetBackgroundColor (window, state ? impl->background_color : default_color);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::SetBackgroundState");
    throw;
  }
}

Color CarbonWindowManager::GetBackgroundColor (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    RGBColor rgb_color;

    check_window_manager_error (GetWindowContentColor ((WindowRef)window, &rgb_color), "::GetWindowContentColor", "Can't set window content color");

    Color return_value;

    return_value.red   = rgb_color.red;
    return_value.green = rgb_color.green;
    return_value.blue  = rgb_color.blue;

    return return_value;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::CarbonWindowManager::SetBackgroundColor");
    throw;
  }
}

bool CarbonWindowManager::GetBackgroundState (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    WindowImpl *impl;

    check_window_manager_error (GetWindowProperty ((WindowRef)window, WINDOW_PROPERTY_CREATOR, WINDOW_IMPL_PROPERTY,
                                sizeof (impl), 0, &impl), "::GetWindowProperty", "Can't get window property");

    return impl->background_state;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::GetBackgroundState");
    throw;
  }
}
