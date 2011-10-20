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
    �������� / �������� ������
*/

screen_t DefaultScreenManager::CreateScreen  (size_t screen_index)
{
  raise ("syslib::DefaultScreenManager::CreateScreen");
  
  return 0;
}

void DefaultScreenManager::DestroyScreen (screen_t)
{
  raise ("syslib::DefaultScreenManager::DestroyScreen");
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

void DefaultScreenManager::RestoreScreenDefaultMode (screen_t)
{
  raise ("syslib::DefaultScreenManager::RestoreScreenDefaultMode");
}

void DefaultScreenManager::GetScreenCurrentMode (screen_t, ScreenModeDesc&)
{
  raise ("syslib::DefaultScreenManager::GetScreenCurrentMode");
}

void DefaultScreenManager::GetScreenDefaultMode (screen_t, ScreenModeDesc&)
{
  raise ("syslib::DefaultScreenManager::GetScreenDefaultMode");
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

/*
    ����� ������ ���������� ����
*/

screen_t DefaultScreenManager::FindContainingScreen (const void*)
{
  raise ("syslib::DefaultScreenManager::FindContainingScreen");
  
  return 0;
}

/*
    ��������� ���������-���������� ����������� ������
*/

const void* DefaultScreenManager::GetNativeScreenHandle (screen_t)
{
  raise ("syslib::DefaultScreenManager::GetNativeScreenHandle");
  
  return 0;
}

/*
    ��������� ���������-��������� ������� ������
*/

void DefaultScreenManager::GetScreenProperties (screen_t, common::PropertyMap& properties)
{
  raise ("syslib::DefaultScreenManager::GetScreenProperties");
}
