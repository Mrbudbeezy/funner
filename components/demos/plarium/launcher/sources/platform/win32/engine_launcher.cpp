#include <windows.h>

#undef CreateWindow

#include <launcher/application.h>

using namespace engine;
using namespace plarium::launcher;

namespace
{

const char* ENGINE_LIBRARY_NAME = "funner.dll";

}

//����� �����
int main (int argc, const char* argv [], const char* env [])
{
  HMODULE library = LoadLibrary (ENGINE_LIBRARY_NAME);
  
  if (!library)
  {
    printf ("Can't load engine library\n");
    return 1;
  }
  
  FunnerInitProc FunnerInit = (FunnerInitProc)GetProcAddress (library, "FunnerInit");  
  
  if (!FunnerInit)
  {
    printf ("Bad library (entries not found)\n");
    return 1;
  }

  IEngine* funner = FunnerInit ();

  if (!funner)
  {
    printf ("Funner startup failed!");
    return 1;
  }
  
  if (!funner->ParseCommandLine (argc, argv, env))
  {
    return 1;
  }

  {
    Application application;

    application.Run (funner);
  }

  delete funner;

  //FreeLibrary (library); //���������� �� �����������, ��-�� ������������� ������������ atexit, ��������� � funner.dll
}
