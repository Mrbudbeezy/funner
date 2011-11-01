#include "shared.h"

using namespace syslib;
using namespace syslib::tabletos;

namespace
{

class TabletOsApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///�����������
    TabletOsApplicationDelegate ()
    {
      idle_enabled = false;
      is_exited    = false;
      listener     = 0;
    }

///������ ����� ��������� ���������
    void Run ()
    {
      platform_initialize ();      
      
      if (navigator_request_events (0) != BPS_SUCCESS)
        raise_error ("::navigator_request_events");
      
      if (listener)
        listener->OnInitialized ();
      
      while (!is_exited)
      {
        const int timeout_ms = idle_enabled && listener ? 0 : 100;
        
        bps_event_t *event = 0;

        if (bps_get_event (&event, timeout_ms) != 0)
          raise_error ("::bps_get_event");
          
        if (event)
        {
          /*
          void* handle = ...;
          
          IWindowImpl* window = WindowRegistry::FindWindow (handle);
          
          if (!window)
            continue;
            
            ....
            
          window->onTouch (x, y);
          */
          
          //TODO: handle event
        }
        
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

IApplicationDelegate* TabletOsApplicationManager::CreateDefaultApplicationDelegate ()
{
  return new TabletOsApplicationDelegate;
}

/*
    �������� URL �� ������� ��������
*/

void TabletOsApplicationManager::OpenUrl (const char* url)
{
  throw xtl::make_not_implemented_exception ("syslib::TabletOsApplicationManager::OpenUrl");
}
