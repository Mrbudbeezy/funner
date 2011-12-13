#ifndef SYSLIB_DEFAULT_SCREEN_MANAGER_HEADER
#define SYSLIB_DEFAULT_SCREEN_MANAGER_HEADER

#include <syslib/screen.h>

namespace syslib
{

struct screen_handle;

typedef screen_handle* screen_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DefaultScreenManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetScreensCount ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static screen_t CreateScreen  (size_t screen_index);
    static void     DestroyScreen (screen_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const char* GetScreenName (screen_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �����-������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t GetScreenModesCount (screen_t);
    static void   GetScreenMode       (screen_t, size_t mode_index, ScreenModeDesc& mode_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� �����-������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenCurrentMode (screen_t, const ScreenModeDesc&);
    static void GetScreenCurrentMode (screen_t, ScreenModeDesc&);
    static void GetScreenDefaultMode (screen_t, ScreenModeDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����-���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenGammaRamp (screen_t, const Color3f table [256]);
    static void GetScreenGammaRamp (screen_t, Color3f table [256]);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ ���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static screen_t FindContainingScreen (const void* native_window_handle);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������-���������� ����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const void* GetNativeScreenHandle (screen_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������-��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void GetScreenProperties (screen_t, common::PropertyMap& properties);
};

}

#endif
