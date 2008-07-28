#include "shared.h"

ICustomWindowSystem* create_window_system (const char* profile, IApplicationServer* server)
{
  return new MyWindowSystem (server);  
}

int main ()
{
  printf ("Results of custom_ws_test:\n");
  
  try
  {
      //����������� ������� ����������
      
    WindowSystemManager::RegisterApplicationServer ("myapp", ApplicationServerPtr (new MyApplicationServer, false).get ());

      //����������� ������� �������
      
    WindowSystemManager::RegisterWindowSystem ("mywndsys", &create_window_system);
    
      //�������� ����
      
    MainWindow wnd ("myapp", "mywndsys");
    
    printf ("Profile:            '%s'\n", wnd.Profile ());
    printf ("Application server: '%s'\n", wnd.ApplicationServer ());
    
    wnd.Restart ();
    wnd.ExecuteCommand ("test_command");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
