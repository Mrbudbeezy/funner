#include "shared.h"

using namespace syslib;

namespace
{

/*
    ���������
*/

const wchar_t* APPLICATION_NAME   = "TestApplication";                   //��� ����������
const wchar_t* APPLICATION_ID     = L"aospd00017";                       //������������� ����������
const wchar_t* APPLICATION_SECRET = L"AF60E5CB8B43BC9650412311E01BAB6C"; //��������� ��� ����������

/*
    Bada-����������
*/

class BasicApp: public Osp::App::Application
{
  public:
///�����������
    BasicApp ()
    {
    }
    
///����������
    ~BasicApp ()
    {
    }
  
///�������� ���������� ����������
    static Osp::App::Application* CreateInstance ()
    {
    }

///��� ����������    
    Osp::Base::String GetAppName(void) const
    {
      static String app_name (APPLICATION_NAME);

      return app_name;
    }
    
///���������� � ������ ����������
    bool OnAppInitializing (Osp::App::AppRegistry& app_registry)
    {
    }

///���������� � ���������� ����������
    bool OnAppTerminating (Osp::App::AppRegistry& app_registry, bool forcedTermination)
    {
    }

///���������� � ����������� ������ ���������� �� �������� ����
    void OnForeground ()
    {
    }

///���������� � ����������� ������ ���������� �� ������ ����
    void OnBackground ()
    {
    }

///���������� � �������� ������
    void OnLowMemory ()
    {
    }

///���������� � ������ ������ �������
    void OnBatteryLevelChanged (Osp::System::BatteryLevel batteryLevel)
    {
    }

  protected:
///������������� ����������
    Osp::App::AppId GetAppId () const
    {
      static AppId app_id (APPLICATION_ID);

      return app_id;      
    }

///��������� ��� ����������
    Osp::App::AppSecret GetAppSecret () const
    {
      static AppSecret app_secret (APPLICATION_SECRET);

      return app_secret;
    }
};

}

/*
    ������ � �������� ���������
*/

bool Platform::IsMessageQueueEmpty ()
{
  throw xtl::format_not_supported_exception ("syslib::BadaPlatform::IsMessageQueueEmpty", "Message queue doesn't supported");
}

void Platform::DoNextEvent ()
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::DoNextEvent", "Message queue doesn't supported");
}

void Platform::WaitMessage ()
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::WaitMessage", "Message queue doesn't supported");
}

void Platform::UpdateMessageQueue ()
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::UpdateMessageQueue");
}

/*
    ������ ����������
*/

void Platform::RunLoop (IRunLoopContext* context)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::RunLoop");
/*  if (!context)
    throw xtl::make_null_argument_exception ("syslib::BadaPlatform::RunLoop", "context");

  context->OnEnterRunLoop ();

  context->DoCustomRunLoop ();*/
}
