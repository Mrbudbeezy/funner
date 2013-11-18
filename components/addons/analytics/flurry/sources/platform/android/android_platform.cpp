#include "shared.h"

using namespace analytics::flurry;
using namespace analytics::flurry::android;

/*
   ��������� Android
*/

/*
   �������������� �� ��������� �� ������ ���������
*/

bool AndroidPlatform::IsSupported ()
{
  return FlurrySingleton::Instance ()->IsSupported ();
}

/*
   ��������� ����� ������ ������������ ����������
*/

void AndroidPlatform::GetReleaseVersion (stl::string& version)
{
  FlurrySingleton::Instance ()->GetReleaseVersion (version);
}

/*
   ������ ���������
*/

void AndroidPlatform::StartSession (const char* api_key)
{
  FlurrySingleton::Instance ()->StartSession (api_key);
}

/*
   ��������� �������������� ���������� ���������
*/

void AndroidPlatform::SetUserId (const char* user_id)
{
  FlurrySingleton::Instance ()->SetUserId (user_id);
}

void AndroidPlatform::SetAge (size_t age)
{
  FlurrySingleton::Instance ()->SetAge (age);
}

void AndroidPlatform::SetGender (Gender gender)
{
  FlurrySingleton::Instance ()->SetGender (gender);
}

void AndroidPlatform::SetUseHttps (bool use_https)
{
  FlurrySingleton::Instance ()->SetUseHttps (use_https);
}

/*
   ������� �������
*/

void AndroidPlatform::LogEvent (const char* event, const common::PropertyMap& parameters, bool timed)
{
  FlurrySingleton::Instance ()->LogEvent (event, parameters, timed);
}

void AndroidPlatform::EndTimedEvent (const char* event, const common::PropertyMap& parameters)
{
  FlurrySingleton::Instance ()->EndTimedEvent (event, parameters);
}

void AndroidPlatform::LogPageView ()
{
  FlurrySingleton::Instance ()->LogPageView ();
}

/*
   ���������� �����������������
*/

void AndroidPlatform::SetDebugLogEnabled (bool state)
{
  FlurrySingleton::Instance ()->SetDebugLogEnabled (state);
}

void AndroidPlatform::SetLogLevel (LogLevel level)
{
  FlurrySingleton::Instance ()->SetLogLevel (level);
}
