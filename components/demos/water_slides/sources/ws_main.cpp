#include "shared.h"

void my_log (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

int main ()
{
  try
  {
      //�����᪠ �� ᮡ��� ��⮪���஢����
    
    common::LogFilter filter ("*", &my_log);
    
      //���樠������ �ਫ������

    MyApplication::Instance ();  
    
    MyApplication::Instance ().SetView (create_test_game_view ());

      //����� �ਫ������
      
    syslib::Application::Run ();

      //�����襭�� �ਫ������

    return syslib::Application::GetExitCode ();
  }
  catch (std::exception& e)
  {
    printf ("Exception caught: %s\n", e.what ());
    return 1;
  }
}
