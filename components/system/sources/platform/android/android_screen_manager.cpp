#include "shared.h"

using namespace syslib;

/*
    ��������� ����������: ������ � �������� ���������� �� ������� �� ���������
*/

namespace
{

const int   MAIN_SCREEN      = 1;
const char* MAIN_SCREEN_NAME = "MainScreen";

//�������� �������
class ScreenManagerImpl
{
  public:
    //�����������
    ScreenManagerImpl ()
    {
      memset (&mode, 0, sizeof (mode));
    }

    //��������� ������ ������
    void SetScreenMode (jint width, jint height, jint refresh_rate, jint xdpi, jint ydpi)
    {
      mode.width        = width;
      mode.height       = height;
      mode.refresh_rate = refresh_rate;
      mode.xdpi         = xdpi;
      mode.ydpi         = ydpi;
    }

    //��������� ������ ������
    void GetScreenMode (ScreenModeDesc& mode_desc)
    {
      mode_desc = mode;
    }

  private:
    ScreenModeDesc mode;
};

typedef common::Singleton <ScreenManagerImpl> ScreenManagerSingleton;

//��������� ������ ������
void set_screen_mode (JNIEnv& env, jobject activity, jint width, jint height, jint refresh_rate, jint xdpi, jint ydpi)
{
  ScreenManagerSingleton::Instance ()->SetScreenMode (width, height, refresh_rate, xdpi, ydpi);
}

}

/*
    ������������ �������
*/

size_t AndroidScreenManager::GetScreensCount ()
{
  return 1;
}

/*
    �������� / �������� ������
*/

screen_t AndroidScreenManager::CreateScreen  (size_t screen_index)
{
  if (screen_index >= GetScreensCount ())
    throw xtl::make_range_exception ("syslib::AndroidScreenManager::CreateScreen", "screen_index", screen_index, 0u, GetScreensCount ());

  return (screen_t)MAIN_SCREEN;
}

void AndroidScreenManager::DestroyScreen (screen_t)
{
  //do nothing
}

/*
    ��� ������
*/

const char* AndroidScreenManager::GetScreenName (screen_t screen)
{
  if (!screen)
    throw xtl::make_null_argument_exception ("syslib::AndroidScreenManager::GetScreenName", "screen");

  return MAIN_SCREEN_NAME;
}

/*
    ��������� ������ �����-������� ������
*/

size_t AndroidScreenManager::GetScreenModesCount (screen_t screen)
{
  if (!screen)
    throw xtl::make_null_argument_exception ("syslib::AndroidScreenManager::GetScreenModesCount", "screen");

  return 1;
}

void AndroidScreenManager::GetScreenMode (screen_t screen, size_t mode_index, ScreenModeDesc& mode_desc)
{
  static const char* METHOD_NAME = "syslib::AndroidScreenManager::GetScreenMode";

  if (!screen)
    throw xtl::make_null_argument_exception (METHOD_NAME, "screen");

  if (mode_index >= GetScreenModesCount (screen))
    throw xtl::make_range_exception (METHOD_NAME, "mode_index", mode_index, 0u, GetScreenModesCount (screen));

  ScreenManagerSingleton::Instance ()->GetScreenMode (mode_desc);
}

/*
    ��������� �������� �����-������ ������
*/

void AndroidScreenManager::SetScreenCurrentMode (screen_t screen, const ScreenModeDesc& mode_desc)
{
  static const char* METHOD_NAME = "syslib::AndroidScreenManager::SetScreenCurrentMode";

  if (!screen)
    throw xtl::make_null_argument_exception (METHOD_NAME, "screen");

  ScreenModeDesc default_mode_desc;

  GetScreenMode (screen, 0, default_mode_desc);

  if (default_mode_desc.width == mode_desc.width && default_mode_desc.height == mode_desc.height && default_mode_desc.refresh_rate == mode_desc.refresh_rate)
    return;

  throw xtl::make_argument_exception (METHOD_NAME, "mode_desc", "Incompatible screen mode");
}

void AndroidScreenManager::GetScreenCurrentMode (screen_t screen, ScreenModeDesc& mode_desc)
{
  GetScreenMode (screen, 0, mode_desc);
}

void AndroidScreenManager::GetScreenDefaultMode (screen_t screen, ScreenModeDesc& mode_desc)
{
  GetScreenMode (screen, 0, mode_desc);
}

/*
    ���������� �����-���������� ������
*/

void AndroidScreenManager::SetScreenGammaRamp (screen_t, const Color3f table [256])
{
  throw xtl::format_not_supported_exception ("syslib::AndroidScreenManager::SetScreenGammaRamp", "Gamma ramp management not supported");
}

void AndroidScreenManager::GetScreenGammaRamp (screen_t, Color3f table [256])
{
  for (size_t i = 0; i < 256; i++)
  {
    float value = i / 255.f;

    table[i].red   = value;
    table[i].green = value;
    table[i].blue  = value;
  }
}

/*
    ����� ������ ���������� ����
*/

screen_t AndroidScreenManager::FindContainingScreen (const void*)
{
  return (screen_t)MAIN_SCREEN;
}

/*
    ��������� ���������-���������� ����������� ������
*/

const void* AndroidScreenManager::GetNativeScreenHandle (screen_t)
{
  throw xtl::format_not_supported_exception ("syslib::AndroidScreenManager::GetNativeScreenHandle", "Screen handle not supported");
}

/*
    ��������� ���������-��������� ������� ������
*/

void AndroidScreenManager::GetScreenProperties (screen_t screen, common::PropertyMap& properties)
{
  if (!screen)
    throw xtl::make_null_argument_exception ("syslib::AndroidScreenManager::GetScreenProperties", "screen");
}

namespace syslib
{

namespace android
{

/// ����������� ������� ��������� ������ screen manager
void register_screen_callbacks (JNIEnv* env, jclass activity_class)
{
  try
  {
    if (!env)
      throw xtl::make_null_argument_exception ("", "env");

    if (!activity_class)
      throw xtl::make_null_argument_exception ("", "activity_class");

    static const JNINativeMethod methods [] = {
      {"setScreenMode", "(IIIII)V", (void*)&set_screen_mode},
    };

    static const size_t methods_count = sizeof (methods) / sizeof (*methods);

    jint status = env->RegisterNatives (activity_class, methods, methods_count);

    if (status)
      throw xtl::format_operation_exception ("", "Can't register natives (status=%d)", status);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::android::register_screen_callbacks");
    throw;
  }
}

}

}
