#include <cfloat>

#include <common/log.h>

#include <tools/ui/main_window.h>

#include "shared.h"

extern "C"
{

extern double _HUGE = DBL_MAX;

}

namespace
{

//����� ��������� ����������
struct Test
{
  Test () : log ("modeler.test"), application_server (new MyApplicationServer, false)
  {
      //����������� ������� ����������

    tools::ui::WindowSystemManager::RegisterApplicationServer ("MyApplicationServer", application_server.get ());

      //�������� �������� ����

    tools::ui::MainWindow ("MyApplicationServer", "WindowsForms").Swap (main_window);

      //�������� �� ������� ����������������

    main_window.SetLogHandler (xtl::bind (&Test::LogPrint, this, _1));

      //�������� �����

    application_server->OpenProjectPath ("projects\\project1\\");
  }

  void LogPrint (const char* message)
  {
    log.Print (message);
  }

  typedef xtl::com_ptr<MyApplicationServer> MyApplicationServerPtr;

  common::Log            log;
  MyApplicationServerPtr application_server;
  tools::ui::MainWindow  main_window;
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
