#include "shared.h"

using namespace syslib;
using namespace syslib::bada;

namespace
{

/*
    ���������
*/

const wchar_t* APPLICATION_NAME   = L"TestApplication";                  //��� ����������
const wchar_t* APPLICATION_ID     = L"aospd00017";                       //������������� ����������
const wchar_t* APPLICATION_SECRET = L"AF60E5CB8B43BC9650412311E01BAB6C"; //��������� ��� ����������

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
    Osp::Ui::Controls::Form* form = new Osp::Ui::Controls::Form;
    
//    form->Initialize ();
    
    app.GetAppFrame ()->GetFrame ()->AddControl (*form);

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

}

typedef common::Singleton<ApplicationDelegate> ApplicationSingleton;

/*
    Bada-����������
*/

///�����������
DefaultApplication::DefaultApplication ()
{
}
    
///����������
DefaultApplication::~DefaultApplication ()
{
}
  
///�������� ���������� ����������
Osp::App::Application* DefaultApplication::CreateInstance ()
{
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
