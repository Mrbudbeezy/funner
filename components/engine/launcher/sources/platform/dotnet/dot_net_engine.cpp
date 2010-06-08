#include "shared.h"

using namespace engine;
using namespace engine::dot_net;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Forms;

namespace Funner
{

/*
    ���� ������
*/

public ref class Window: public Form
{
  public:
    ///�����������
    Window (IEngine* in_engine, const char* name)
      : engine (in_engine)
      , window (0)
    {
      if (!engine)
        throw gcnew ArgumentException ("Null engine passed to constructor of Funner.Window class");
        
      if (!name)
        throw gcnew ArgumentException ("Null name passed to constructor of Funner.Window class");

      window = engine->CreateWindow (name);
    }

    ///����������
    ~Window ()
    {
      delete window;
    }
  
  private:
    IEngine* engine; //��������� �� ������
    IWindow* window; //��������� �� ���� ������
};

/*
    ����� ������
*/

public ref class Engine
{
  public:
///�����������
    Engine ()
    {
      engine = FunnerInit ();
      
      if (!engine)
        throw gcnew InvalidOperationException ("Funner native engine has bad entries");
    }
    
///����������
    ~Engine ()
    {
      delete engine;

      engine = 0;
    }

///�������� ���������� ��������� ������� ����
    Window^ CreateWindow (const char* name)
    {
      return gcnew Window (engine, name);
    }

  private:
///������ native API ������
    [DllImport ("funner.dll")]
    static IEngine* FunnerInit ();

  private:
    IEngine* engine; //��������� �� ������ ������
};

}
