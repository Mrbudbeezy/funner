#include "../shared.h"

using namespace syslib;

namespace
{

/*
    ���������
*/

const size_t DEFAULT_SCREEN_ARRAY_RESERVE = 8; //������������� ���������� �������

/*
    �������� �������
*/

struct ScreenDesc
{
  
};

struct ScreenManagerImpl
{
};

typedef common::Singleton<ScreenManagerImpl> ScreenManagerSingleton;

}

/*
    ������������ �������
*/

size_t WindowsScreenManager::GetScreensCount ()
{
  return 0;
}

/*
    ��� ������
*/

const char* WindowsScreenManager::GetScreenName (screen_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenName");
}

/*
    ��������� ������ �����-������� ������
*/

size_t WindowsScreenManager::GetScreenModesCount (screen_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenModesCount");
}

void WindowsScreenManager::GetScreenMode (screen_t, size_t mode_index, ScreenModeDesc& mode_desc)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenMode");
}

/*
    ��������� �������� �����-������ ������
*/

void WindowsScreenManager::SetScreenCurrentMode (screen_t, const ScreenModeDesc&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::SetScreenCurrentMode");
}

void WindowsScreenManager::RestoreScreenDefaultMode (screen_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::RestoreScreenDefaultMode");
}

void WindowsScreenManager::GetScreenCurrentMode (screen_t, ScreenModeDesc&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenCurrentMode");
}

/*
    ���������� �����-���������� ������
*/

void WindowsScreenManager::SetScreenGammaRamp (screen_t, const Color3f table [256])
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::SetScreenGammaRamp");
}

void WindowsScreenManager::GetScreenGammaRamp (screen_t, Color3f table [256])
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::GetScreenGammaRamp");
}

/*
    ����� ������ ���������� ����
*/

screen_t WindowsScreenManager::FindContainingScreen (const void* native_window_handle)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsScreenManager::FindContainingScreen");
}
