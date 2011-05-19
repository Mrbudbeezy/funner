#ifndef SYSLIB_IPHONE_PLATFORM_HEADER
#define SYSLIB_IPHONE_PLATFORM_HEADER

#include <shared/unistd.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������� IPhone
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhoneWindowManager: public DefaultWindowManager
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
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InvalidateWindow (window_t);
    
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
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� IPhone
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhoneTimerManager: public DefaultTimerManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static timer_t CreateTimer (size_t period_in_milliseconds, TimerHandler, void* user_data);
    static void    KillTimer   (timer_t handle);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� MacOSX
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhoneThreadManager: public PThreadManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static semaphore_t CreateSemaphore   (size_t initial_count);
    static void        DestroySemaphore  (semaphore_t);
    static void        WaitSemaphore     (semaphore_t);
    static bool        WaitSemaphore     (semaphore_t, size_t wait_in_milliseconds);
    static bool        TryWaitSemaphore  (semaphore_t);
    static void        PostSemaphore     (semaphore_t);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� IPhone
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhoneApplicationManager: public DefaultApplicationManager
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
///������ � �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IApplicationDelegate* CreateDefaultApplicationDelegate ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhonePlatform
 : public IPhoneWindowManager
 , public IPhoneTimerManager 
 , public IPhoneApplicationManager 
 , public IPhoneThreadManager 
 , public UnistdLibraryManager

{
};

}

#endif
