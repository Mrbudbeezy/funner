#ifndef SYSLIB_LINUX_PLATFORM_HEADER
#define SYSLIB_LINUX_PLATFORM_HEADER

#include <shared/unistd.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������� XLib
///////////////////////////////////////////////////////////////////////////////////////////////////
class XlibWindowManager: public DefaultWindowManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������/��������/����������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef void (*WindowMessageHandler)(window_t, WindowEvent, const WindowEventContext&, void* user_data);

    static window_t CreateWindow  (WindowStyle, WindowMessageHandler, const void* parent_handle, const char* init_string, void* user_data);
    static void     CloseWindow   (window_t);
    static void     DestroyWindow (window_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������-���������� ����������� ���� � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const void* GetNativeWindowHandle  (window_t);
    static const void* GetNativeDisplayHandle (window_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetWindowTitle (window_t, const wchar_t* title);
    static void GetWindowTitle (window_t, size_t buffer_size_in_chars, wchar_t* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� / ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetWindowRect (window_t, const Rect&);
    static void SetClientRect (window_t, const Rect&);
    static void GetWindowRect (window_t, Rect&);
    static void GetClientRect (window_t, Rect&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetWindowFlag (window_t, WindowFlag flag, bool state);
    static bool GetWindowFlag (window_t, WindowFlag flag);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void        SetParentWindowHandle (window_t child, const void* parent_handle);
    static const void* GetParentWindowHandle (window_t child);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InvalidateWindow (window_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void  SetCursorPosition (const Point& position);
    static Point GetCursorPosition ();
    static void  SetCursorPosition (window_t, const Point& client_position); //� ���������� ����������� ����
    static Point GetCursorPosition (window_t);                               //� ���������� ����������� ����

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetCursorVisible (window_t, bool state);
    static bool GetCursorVisible (window_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static cursor_t CreateCursor  (const char* name, int hotspot_x, int hotspot_y); //hotspot_x/hotspot_y = -1 - default value
    static void     DestroyCursor (cursor_t);
    static void     SetCursor     (window_t window, cursor_t cursor);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� multitouch ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetMultitouchEnabled (window_t window, bool state);
    static bool IsMultitouchEnabled  (window_t window);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void  SetBackgroundColor (window_t window, const Color& color);
    static void  SetBackgroundState (window_t window, bool state);
    static Color GetBackgroundColor (window_t window);
    static bool  GetBackgroundState (window_t window);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetKeyName (ScanCode scan_code, size_t buffer_size, char* buffer); //������������ ����� ������ ��� ����� '\0'
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class LinuxPlatform
 : public XlibWindowManager
 , public PThreadManager 
 , public UnistdTimerManager  
 , public UnistdLibraryManager
 , public UnistdApplicationManager
 
{
};

}

#endif
