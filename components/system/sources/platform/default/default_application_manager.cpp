#include "shared.h"

namespace syslib
{

namespace
{

class DefaultApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///�����������
    DefaultApplicationDelegate ()
    {
      idle_enabled = false;
      is_exited    = false;
      listener     = 0;
    }

///������ ����� ��������� ���������
    void Run ()
    {
      if (listener)
        listener->OnInitialize ();
      
      while (!is_exited)
      {
        if (idle_enabled && listener)
          listener->OnIdle ();
      }
    }

///����� �� ����������
    void Exit (int code)
    {
      is_exited = true;

      if (listener)
        listener->OnExit (code);
    }

///��������� ������������� ������ ������� idle
    void SetIdleState (bool state)
    {
      idle_enabled = state;
    }

///��������� ��������� ������� ����������
    void SetListener (IApplicationListener* in_listener)
    {
      listener = in_listener;
    }
    
///������� ������
    void AddRef ()
    {
      addref (this);
    }
    
    void Release ()
    {
      release (this);
    }    

  private:
    bool                  idle_enabled;
    bool                  is_exited;
    IApplicationListener* listener;
};

}

/*
    �������� �������� ����������
*/

IApplicationDelegate* DefaultApplicationManager::CreateDefaultApplicationDelegate ()
{
  return new DefaultApplicationDelegate;
}

/*
    ������������ ���������� ���� �� miliseconds ����������
*/

void DefaultApplicationManager::Sleep (size_t milliseconds)
{
  size_t start = common::milliseconds ();

  while (common::milliseconds () - start < milliseconds);
}

/*
    �������� URL �� ������� ��������
*/

void DefaultApplicationManager::OpenUrl (const char* url)
{
  throw xtl::format_not_supported_exception ("syslib::DefaultApplicationManager::OpenUrl", "No OpenURL support for this platform");
}

/*
    ��������� �������� ���������� �����
*/

stl::string DefaultApplicationManager::GetEnvironmentVariable (const char* name)
{
  const char* result = getenv (name);
  
  if (!result)
    throw xtl::format_operation_exception ("syslib::DefaultApplicationManager::GetEnvironmentVariable", "Environment variable '%s' not found");

  return result;
}

/*
    ���������� �����������������
*/

void DefaultApplicationManager::SetScreenSaverState (bool state)
{
  throw xtl::format_not_supported_exception ("syslib::DefaultApplicationManager::SetScreenSaverState", "No screen saver support for this platform");
}

bool DefaultApplicationManager::GetScreenSaverState ()
{
  return true;
}

/*
    ���������� ������� ������ � ����
*/

void DefaultApplicationManager::SetApplicationBackgroundMode (syslib::ApplicationBackgroundMode)
{
}

ApplicationBackgroundMode DefaultApplicationManager::GetApplicationBackgroundMode ()
{
  return ApplicationBackgroundMode_Active;
}

/*
    ��������� ��������� �������
*/

void DefaultApplicationManager::GetSystemProperties (common::PropertyMap& properties)
{
#ifdef HAS_GEEK_INFO
  try
  {
    for (int i=(int)kSystemMetricPlatform; i!=kSystemMetricCount; i++)
      properties.SetProperty (systemMetricName((SystemMetricType)i).c_str (), systemMetric((SystemMetricType)i).c_str ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::DefaultApplicationManager::GetSystemProperties");
    throw;
  }
#endif
}

/*
    ��������� ��������� ��������� ����
*/

void DefaultApplicationManager::ProcessThreadMessages ()
{
}

}
