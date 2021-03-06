#ifndef SYSLIB_BADA_PLATFORM_HEADER
#define SYSLIB_BADA_PLATFORM_HEADER

#include <shared/unistd.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оконный менеджер Bada
///////////////////////////////////////////////////////////////////////////////////////////////////
class BadaWindowManager: public DefaultWindowManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание/закрытие/уничтожение окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef void (*WindowMessageHandler)(window_t, WindowEvent, const WindowEventContext&, void* user_data);

    static window_t CreateWindow  (WindowStyle, WindowMessageHandler, const void* parent_handle, const char* init_string, void* user_data);
    static void     CloseWindow   (window_t);
    static void     DestroyWindow (window_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение платформо-зависимого дескриптора окна и дисплея
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const void* GetNativeWindowHandle  (window_t);
    static const void* GetNativeDisplayHandle (window_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Заголовок окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetWindowTitle (window_t, const wchar_t* title);
    static void GetWindowTitle (window_t, size_t buffer_size_in_chars, wchar_t* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Область окна / клиентская область
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetWindowRect (window_t, const Rect&);
    static void SetClientRect (window_t, const Rect&);
    static void GetWindowRect (window_t, Rect&);
    static void GetClientRect (window_t, Rect&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка флагов окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetWindowFlag (window_t, WindowFlag flag, bool state);
    static bool GetWindowFlag (window_t, WindowFlag flag);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void InvalidateWindow (window_t);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер таймеров Bada
///////////////////////////////////////////////////////////////////////////////////////////////////
class BadaTimerManager: public DefaultTimerManager
{
  public:
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер нитей Bada
///////////////////////////////////////////////////////////////////////////////////////////////////
class BadaThreadManager: public DefaultThreadManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание / удаление нити
///////////////////////////////////////////////////////////////////////////////////////////////////
    static thread_t CreateThread  (IThreadCallback*);
    static void     DestroyThread (thread_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ожидание завершения нити
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void JoinThread (thread_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение идентификатора нити
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetThreadId        (thread_t thread);
    static size_t GetCurrentThreadId ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с исключающими семафорами
///////////////////////////////////////////////////////////////////////////////////////////////////
    static mutex_t CreateMutex  ();
    static void    DestroyMutex (mutex_t);
    static void    LockMutex    (mutex_t);
    static bool    LockMutex    (mutex_t, size_t wait_in_milliseconds);
    static bool    TryLockMutex (mutex_t);
    static void    UnlockMutex  (mutex_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с семафорами
///////////////////////////////////////////////////////////////////////////////////////////////////
    static semaphore_t CreateSemaphore   (size_t initial_count);
    static void        DestroySemaphore  (semaphore_t);
    static void        WaitSemaphore     (semaphore_t);
    static bool        WaitSemaphore     (semaphore_t, size_t wait_in_milliseconds);
    static bool        TryWaitSemaphore  (semaphore_t);
    static void        PostSemaphore     (semaphore_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с условиями
///////////////////////////////////////////////////////////////////////////////////////////////////
    static condition_t CreateCondition    ();
    static void        DestroyCondition   (condition_t);
    static void        WaitCondition      (condition_t, mutex_t);
    static bool        WaitCondition      (condition_t, mutex_t, size_t wait_in_milliseconds);
    static void        NotifyCondition    (condition_t, bool broadcast);  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер приложения Bada
///////////////////////////////////////////////////////////////////////////////////////////////////
class BadaApplicationManager: public DefaultApplicationManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Приостановка выполнения нити на miliseconds милисекунд
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Sleep (size_t miliseconds);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Открытие URL во внешнем браузере
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void OpenUrl (const char* url);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с очередью сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IApplicationDelegate* CreateDefaultApplicationDelegate ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Платформа
///////////////////////////////////////////////////////////////////////////////////////////////////
class BadaPlatform
 : public BadaWindowManager
 , public DefaultTimerManager 
 , public BadaApplicationManager 
 , public BadaThreadManager 
 , public UnistdLibraryManager
 , public DefaultScreenManager
 , public DefaultSensorManager 
 , public DefaultCookieManager
 , public DefaultScreenKeyboardManager
{
};

}

#endif
