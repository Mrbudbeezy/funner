#include "shared.h"

using namespace syslib;

namespace
{

/// ������� ����������
class DefaultApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///�����������
    DefaultApplicationDelegate ()
    {
      idle_enabled = false;
      is_exited    = false;
      listener     = 0;
    }

///������ ����� ��������� ���������
    void Run ()
    {            
      if (listener)
        listener->OnInitialized ();
      
      while (!is_exited)
      {
        if (idle_enabled && listener)
          listener->OnIdle ();
      }
    }

///����� �� ����������
    void Exit (int code)
    {
      is_exited = true;

      if (listener)
        listener->OnExit (code);
    }

///��������� ������������� ������ ������� idle
    void SetIdleState (bool state)
    {
      idle_enabled = state;
    }

///��������� ��������� ������� ����������
    void SetListener (IApplicationListener* in_listener)
    {
      listener = in_listener;
    }
    
///������� ������
    void AddRef ()
    {
      addref (this);
    }
    
    void Release ()
    {
      release (this);
    }    

  private:
    bool                  idle_enabled;
    bool                  is_exited;
    IApplicationListener* listener;
};

}

/*
    �������� �������� ����������
*/

IApplicationDelegate* Platform::CreateDefaultApplicationDelegate ()
{
  return new DefaultApplicationDelegate;
}
