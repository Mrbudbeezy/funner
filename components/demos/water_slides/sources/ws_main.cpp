#include "shared.h"

int main ()
{
    //���樠������ �ਫ������

  MyApplication::Instance ();  
  
  MyApplication::Instance ().SetView (create_test_game_view ());

    //����� �ਫ������
    
  syslib::Application::Run ();
  
    //�����襭�� �ਫ������

  return syslib::Application::GetExitCode ();
}
