#include <cfloat>

#include <common/log.h>
#include <common/var_registry_container.h>

#include <tools/ui/main_window.h>

#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

extern "C"
{

extern double _HUGE = DBL_MAX;

}

namespace
{

const char* APPLICATION_SERVER_REGISTRY_BRANCH = "ApplicationServer";

//����� ��������� ����������
struct Test
{
  Test () :
    log ("modeler.test"),
    application_server (new MyApplicationServer, false),
    window_title_update_timer (xtl::bind (&Test::UpdateWindowTitle, this), 1000, syslib::TimerState_Paused)
  {
      //����������� ������� ����������

    tools::ui::WindowSystemManager::RegisterApplicationServer ("MyApplicationServer", application_server.get ());

      //�������� �������� ����

    tools::ui::MainWindow ("MyApplicationServer", "WindowsForms").Swap (main_window);
    window_title_update_timer.Run ();

      //�������� �� ������� ����������������

    main_window.SetLogHandler (xtl::bind (&Test::LogPrint, this, _1));

      //�������� �����

    application_server->OpenProjectPath ("projects\\project1\\");
  }

  void LogPrint (const char* message)
  {
    log.Print (message);
  }

  void UpdateWindowTitle ()
  {
    size_t calculating_trajectories_count = application_server->CalculatingTrajectoriesCount ();

    if (calculating_trajectories_count)
    {
      stl::string window_title = common::format ("if (MainFormTitle == nil) then Application.MainForm.Text = \"Modeler. Calculating %u trajectories\" else Application.MainForm.Text = MainFormTitle .. \" Calculating %u trajectories\" end",
                                                 calculating_trajectories_count, calculating_trajectories_count);

      main_window.ExecuteCommand (window_title.c_str ());
    }
    else
      main_window.ExecuteCommand ("if (MainFormTitle == nil) then Application.MainForm.Text = \"Modeler\" else Application.MainForm.Text = MainFormTitle end");
  }

  typedef xtl::com_ptr<MyApplicationServer> MyApplicationServerPtr;

  common::Log            log;
  MyApplicationServerPtr application_server;
  tools::ui::MainWindow  main_window;
  syslib::Timer          window_title_update_timer;            //������ ���������� ��������� ����
};

void print (const char* message)
{
  common::Console::Printf ("%s\n", message);
}

void log_handler (const char* log_name, const char* message)
{
  common::Console::Printf ("%s: %s\n", log_name, message);
}

}

int main ()
{
  try
  {
    common::LogFilter log_filter ("*", &log_handler);

    common::VarRegistryContainer<stl::string> application_server_registry_container;

    application_server_registry_container.Mount (APPLICATION_SERVER_REGISTRY_BRANCH);

    Test test;

    test.main_window.SetLogHandler (&print);
    test.main_window.ExecuteFile ("media/ui.lua");

    syslib::Application::Run ();

    return syslib::Application::GetExitCode ();
  }
  catch (xtl::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
