#include "shared.h"

int main ()
{
    //���樠������ �ਫ������

  MyApplication::Instance ();  

    //����� �ਫ������
    
  syslib::Application::Run ();
  
    //�����襭�� �ਫ������

  return syslib::Application::GetExitCode ();
}
