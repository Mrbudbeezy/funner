#include "shared.h"

using namespace syslib;
using namespace syslib::bada;

namespace
{

/*
    ���������
*/

//const char*    LOG_NAME           = "system.bada.application"; //����� ����������������
const wchar_t* APPLICATION_NAME   = DEFAULT_APP_NAME;          //��� ����������
const wchar_t* APPLICATION_ID     = DEFAULT_APP_ID;            //������������� ����������
const wchar_t* APPLICATION_SECRET = DEFAULT_APP_SECRET;        //��������� ��� ����������

/*
    �������� ���������� bada-����������
*/

struct ApplicationDelegate: public IApplicationDelegate
{
  IApplicationListener* listener;
  bool                  idle_state;
  
///�����������
  ApplicationDelegate ()
  {
    listener   = 0;
    idle_state = false;
  }

///������ ����� ��������� ���������
  void Run ()
  {
    Osp::Base::Collection::ArrayList* args = new Osp::Base::Collection::ArrayList;

    args->Construct ();    
    
    int          argc = get_osp_main_argc ();
    const char** argv = get_osp_main_argv ();
    
    for (int i= 0; i<argc; i++)
      args->Add (*(new Osp::Base::String (argv [i])));

    result res = Osp::App::Application::Execute (DefaultApplication::CreateInstance, args);
    
    if (IsFailed (res))
    {
      args->RemoveAll (true);

      delete args;

      raise ("syslib::bada::ApplicationImpl::Run", res);
    }

    args->RemoveAll(true);

    delete args;    
  }

///����� �� ����������
  void Exit (int code)
  {
  }
  
///��������� ������������� ������ ������� idle
  void SetIdleState (bool state)
  {
  }

///��������� ��������� ������� ����������
  void SetListener (IApplicationListener* in_listener)
  {
    listener = in_listener;
  }
  
///������� ������
  void AddRef ()
  {
  }
  
  void Release ()
  {
  }  
  
///���������� � ������ ����������  
  bool OnAppInitializing (Osp::App::Application& app, Osp::App::AppRegistry& app_registry)
  {
    if (listener)
      listener->OnInitialized ();

//    app.GetAppFrame ()->GetFrame ()->AddControl (*form);

    return true;    
  }
  
///���������� � ���������� ����������
  bool OnAppTerminating (Osp::App::Application& app, Osp::App::AppRegistry& app_registry, bool forced_termination)
  {
    return true;
  }
  
///���������� � ����������� ������ ���������� �� �������� ����
  void OnForeground (Osp::App::Application& app)
  {
  }
  
///���������� � ����������� ������ ���������� �� ������ ����
  void OnBackground (Osp::App::Application& app)
  {
  }
  
///���������� � �������� ������
  void OnLowMemory (Osp::App::Application& app)
  {
  }
  
///���������� � ������ ������ �������
  void OnBatteryLevelChanged (Osp::App::Application& app, Osp::System::BatteryLevel battery_level)
  {
  }
};

//������� ����������
DefaultApplication* current_application = 0;

}

typedef common::Singleton<ApplicationDelegate> ApplicationSingleton;

/*
    Bada-����������
*/

///�����������
DefaultApplication::DefaultApplication ()
{
  if (current_application)
    throw xtl::format_operation_exception ("syslib::bada::DefaultApplication::DefaultApplication", "Application has already created");
    
  current_application = this;
}
    
///����������
DefaultApplication::~DefaultApplication ()
{
  current_application = 0;
}
  
///�������� ���������� ����������
Osp::App::Application* DefaultApplication::CreateInstance ()
{
  if (current_application)
    return 0;

  return new DefaultApplication;
}

///��� ����������    
Osp::Base::String DefaultApplication::GetAppName(void) const
{
  static Osp::Base::String app_name (APPLICATION_NAME);

  return app_name;
}

///���������� � ������ ����������
bool DefaultApplication::OnAppInitializing (Osp::App::AppRegistry& app_registry)
{
  return ApplicationSingleton::Instance ()->OnAppInitializing (*this, app_registry);
}

///���������� � ���������� ����������
bool DefaultApplication::OnAppTerminating (Osp::App::AppRegistry& app_registry, bool forced_termination)
{
  return ApplicationSingleton::Instance ()->OnAppTerminating (*this, app_registry, forced_termination);
}

///���������� � ����������� ������ ���������� �� �������� ����
void DefaultApplication::OnForeground ()
{
  ApplicationSingleton::Instance ()->OnForeground (*this);
}

///���������� � ����������� ������ ���������� �� ������ ����
void DefaultApplication::OnBackground ()
{
  ApplicationSingleton::Instance ()->OnBackground (*this);
}

///���������� � �������� ������
void DefaultApplication::OnLowMemory ()
{
  ApplicationSingleton::Instance ()->OnLowMemory (*this);
}

///���������� � ������ ������ �������
void DefaultApplication::OnBatteryLevelChanged (Osp::System::BatteryLevel battery_level)
{
  ApplicationSingleton::Instance ()->OnBatteryLevelChanged (*this, battery_level);
}

///������������� ����������
Osp::App::AppId DefaultApplication::GetAppId () const
{
  static Osp::App::AppId app_id (APPLICATION_ID);

  return app_id;      
}

///��������� ��� ����������
Osp::App::AppSecret DefaultApplication::GetAppSecret () const
{
  static Osp::App::AppSecret app_secret (APPLICATION_SECRET);

  return app_secret;
}

/*
    �������� ������� ����������
*/

IApplicationDelegate* Platform::CreateDefaultApplicationDelegate ()
{
  return &*ApplicationSingleton::Instance ();
}

/*
    ��������� ���������� �������� ����������
*/

namespace syslib
{

namespace bada
{

Osp::App::Application& get_application ()
{
  if (!current_application)
    throw xtl::format_operation_exception ("syslib::bada::get_application", "Application has not initialized");
    
  return *current_application;
}

}

}
