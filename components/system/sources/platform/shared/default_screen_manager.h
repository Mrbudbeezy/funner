#ifndef SYSLIB_DEFAULT_SCREEN_MANAGER_HEADER
#define SYSLIB_DEFAULT_SCREEN_MANAGER_HEADER

#include <syslib/screen.h>

namespace syslib
{

struct screen_handle;

typedef screen_handle* screen_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Экранный менеджер по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
class DefaultScreenManager
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

}

#endif
