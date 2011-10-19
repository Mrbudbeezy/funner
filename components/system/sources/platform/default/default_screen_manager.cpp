#include "shared.h"

using namespace syslib;

/*
    ��������� ����������: ������ � �������� ���������� �� ������� �� ���������
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No screen support for this platform");
}

}

/*
    ������������ �������
*/

size_t DefaultScreenManager::GetScreensCount ()
{
  return 0;
}

/*
    ��� ������
*/

const char* DefaultScreenManager::GetScreenName (screen_t)
{
  raise ("syslib::DefaultScreenManager::GetScreenName");
  
  return "";
}

/*
    ��������� ������ �����-������� ������
*/

size_t DefaultScreenManager::GetScreenModesCount (screen_t)
{
  raise ("syslib::DefaultScreenManager::GetScreenModesCount");

  return 0;
}

void DefaultScreenManager::GetScreenMode (screen_t, size_t mode_index, ScreenModeDesc& mode_desc)
{
  raise ("syslib::DefaultScreenManager::GetScreenMode");
}

/*
    ��������� �������� �����-������ ������
*/

void DefaultScreenManager::SetScreenCurrentMode (screen_t, const ScreenModeDesc&)
{
  raise ("syslib::DefaultScreenManager::SetScreenCurrentMode");
}

void DefaultScreenManager::GetScreenCurrentMode (screen_t, ScreenModeDesc&)
{
  raise ("syslib::DefaultScreenManager::GetScreenCurrentMode");
}

/*
    ���������� �����-���������� ������
*/

void DefaultScreenManager::SetScreenGammaRamp (screen_t, const Color3f table [256])
{
  raise ("syslib::DefaultScreenManager::SetScreenGammaRamp");
}

void DefaultScreenManager::GetScreenGammaRamp (screen_t, Color3f table [256])
{
  raise ("syslib::DefaultScreenManager::GetScreenGammaRamp");
}
