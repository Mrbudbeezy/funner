#ifndef SYSLIB_IPHONE_PLATFORM_HEADER
#define SYSLIB_IPHONE_PLATFORM_HEADER

#include <shared/cocoa.h>
#include <shared/unistd.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оконный менеджер IPhone
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhoneWindowManager: public DefaultWindowManager
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
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка/получение multitouch режима
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetMultitouchEnabled (window_t window, bool state);
    static bool IsMultitouchEnabled  (window_t window);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Цвет фона
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void  SetBackgroundColor (window_t window, const Color& color);
    static void  SetBackgroundState (window_t window, bool state);
    static Color GetBackgroundColor (window_t window);
    static bool  GetBackgroundState (window_t window);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание/уничтожение web-view
///////////////////////////////////////////////////////////////////////////////////////////////////
    static web_view_t CreateWebView  (IWebViewListener*);
    static void       DestroyWebView (web_view_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присоединенное окно
///////////////////////////////////////////////////////////////////////////////////////////////////
    static window_t GetWindow (web_view_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void LoadRequest (web_view_t, const char* uri);
    static void LoadData    (web_view_t, const char* data, size_t data_size, const char* mime_type, const char* encoding, const char* base_url);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перезагрузка страницы / остановка загрузки / проверка наличия загрузки
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Reload      (web_view_t);
    static void StopLoading (web_view_t);
    static bool IsLoading   (web_view_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Средства навигации
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool CanGoBack    (web_view_t);
    static bool CanGoForward (web_view_t);
    static void GoBack       (web_view_t);
    static void GoForward    (web_view_t);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер таймеров IPhone
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhoneTimerManager: public DefaultTimerManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание / уничтожение таймера
///////////////////////////////////////////////////////////////////////////////////////////////////
    static timer_t CreateTimer (size_t period_in_milliseconds, TimerHandler, void* user_data);
    static void    KillTimer   (timer_t handle);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер нитей
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhoneThreadManager: public PThreadManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с семафорами
///////////////////////////////////////////////////////////////////////////////////////////////////
    static semaphore_t CreateSemaphore   (size_t initial_count);
    static void        DestroySemaphore  (semaphore_t);
    static void        WaitSemaphore     (semaphore_t);
    static bool        WaitSemaphore     (semaphore_t, size_t wait_in_milliseconds);
    static bool        TryWaitSemaphore  (semaphore_t);
    static void        PostSemaphore     (semaphore_t);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер приложения IPhone
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhoneApplicationManager: public UnistdApplicationManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Открытие URL во внешнем браузере
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void OpenUrl (const char* url);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с очередью сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IApplicationDelegate* CreateDefaultApplicationDelegate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление энергосбережением
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenSaverState (bool state);
    static bool GetScreenSaverState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление режимом работы в фоне
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void                              SetApplicationBackgroundMode (syslib::ApplicationBackgroundMode mode);
    static syslib::ApplicationBackgroundMode GetApplicationBackgroundMode ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение системных свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void GetSystemProperties (common::PropertyMap&);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер сенсоров iOS
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhoneSensorManager: public DefaultSensorManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество сенсоров
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetSensorsCount ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание / удаление сенсора
///////////////////////////////////////////////////////////////////////////////////////////////////
    static sensor_t CreateSensor  (size_t sensor_index);
    static void     DestroySensor (sensor_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя сенсора
///////////////////////////////////////////////////////////////////////////////////////////////////
    static stl::string GetSensorName (sensor_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Производитель сенсора
///////////////////////////////////////////////////////////////////////////////////////////////////
    static stl::string GetSensorVendor (sensor_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    static stl::string GetSensorType (sensor_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Максимальное значение
///////////////////////////////////////////////////////////////////////////////////////////////////
    static float GetSensorMaxRange (sensor_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Частота обновления
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void  SetSensorUpdateRate (sensor_t, float rate);
    static float GetSensorUpdateRate (sensor_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение платформо-зависимого дескриптора экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const void* GetNativeSensorHandle (sensor_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение платформо-зависимых свойств экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void GetSensorProperties (sensor_t, common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение событий сенсора
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void StartSensorPolling (sensor_t, ISensorEventListener&);
    static void StopSensorPolling  (sensor_t);
    static void PollSensorEvents   (sensor_t);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Экранный менеджер
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhoneScreenManager: public DefaultScreenManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление экранов
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetScreensCount ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание / удаление экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static screen_t CreateScreen  (size_t screen_index);
    static void     DestroyScreen (screen_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const char* GetScreenName (screen_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение списка видео-режимов экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetScreenModesCount (screen_t);
    static void   GetScreenMode       (screen_t, size_t mode_index, ScreenModeDesc& mode_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка текущего видео-режима экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenCurrentMode (screen_t, const ScreenModeDesc&);
    static void GetScreenCurrentMode (screen_t, ScreenModeDesc&);
    static void GetScreenDefaultMode (screen_t, ScreenModeDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление гамма-коррекцией экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenGammaRamp (screen_t, const Color3f table [256]);
    static void GetScreenGammaRamp (screen_t, Color3f table [256]);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск экрана вмещающего окно
///////////////////////////////////////////////////////////////////////////////////////////////////
    static screen_t FindContainingScreen (const void* native_window_handle);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение платформо-зависимого дескриптора экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const void* GetNativeScreenHandle (screen_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение платформо-зависимых свойств экрана
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void GetScreenProperties (screen_t, common::PropertyMap& properties);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер экранной клавиатуры
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhoneScreenKeyboardManager : public DefaultScreenKeyboardManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание и уничтожение экранной клавиатуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    static screen_keyboard_t CreateScreenKeyboard  (window_t window, ScreenKeyboardType type, const char* platform_specific);
    static void              DestroyScreenKeyboard (screen_keyboard_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Показ и скрытие клавиатуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void ShowScreenKeyboard (screen_keyboard_t);
    static void HideScreenKeyboard (screen_keyboard_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка поддержки клавиатуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool IsScreenKeyboardSupported (ScreenKeyboardType type);
    static bool IsScreenKeyboardSupported (const char* keyboard_name);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Платформа
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPhonePlatform
 : public IPhoneWindowManager
 , public IPhoneTimerManager 
 , public IPhoneApplicationManager 
 , public IPhoneThreadManager 
 , public UnistdLibraryManager
 , public IPhoneScreenManager
 , public IPhoneSensorManager
 , public CocoaCookieManager
 , public IPhoneScreenKeyboardManager
{
};

}

#endif
