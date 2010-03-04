#include "../shared.h"

using namespace syslib;

namespace
{

class Win32ApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///�����������
    Win32ApplicationDelegate ()
    {
      idle_enabled    = false;
      is_exited       = false;
      listener        = 0;
      wake_up_message = RegisterWindowMessage ("Win32ApplicationDelegate.WakeUp");
      main_thread_id  = 0;
      
      if (!wake_up_message)
        raise_error ("::RegisterWindowMessage");
    }

///������ ����� ��������� ���������
    void Run ()
    {
      try
      {
        main_thread_id = GetCurrentThreadId ();
        
        while (!is_exited)
        {
          while (!IsMessageQueueEmpty ())
            DoNextEvent ();

           //���� ��� ������������ OnIdle - ���������������� ����������

          if (!idle_enabled)
          {
            if (IsMessageQueueEmpty () && !is_exited)
            {
              WaitMessage ();
            }
          }
          else
          {
            if (listener)
              listener->OnIdle ();
          }
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::Win32ApplicationDelegate::Run");
        throw;
      }
    }

///����� �� ����������
    void Exit (int code)
    {
      if (!PostThreadMessage (main_thread_id, WM_QUIT, code, 0))
        raise_error ("::PostThreadMessage");
    }

///��������� ������������� ������ ������� idle
    void SetIdleState (bool state)
    {
      idle_enabled = state;
      
      if (state)
      {
        if (!::PostMessage (0, wake_up_message, 0, 0))
          raise_error ("::PostMessage");
      }
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
///�������� ������� ������� �� �������
    bool IsMessageQueueEmpty ()
    {
      if (is_exited)
        return true;
      
      MSG msg;

      bool result = PeekMessage (&msg, 0, 0, 0, PM_NOREMOVE) == 0;

        //���������� ��� ������

      SetLastError (0);

      return result;
    }
    
///�������� �������
    void WaitMessage ()
    {
      try
      {
        if (!::WaitMessage ())
          raise_error ("::WaitMessage");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::Win32ApplicationDelegate::WaitMessage");
        throw;
      }
    }
    
///��������� ���������� �������
    void DoNextEvent ()
    {
      try
      {
        MSG msg;

        is_exited = GetMessage (&msg, 0, 0, 0) == 0;
        
        if (is_exited && listener)
          listener->OnExit (msg.wParam);

        TranslateMessage (&msg);
        DispatchMessage  (&msg);
        SetLastError     (0);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::Win32ApplicationDelegate::DoNextEvent");
        throw;
      }
    }

  private:
    bool                  idle_enabled;
    bool                  is_exited;
    IApplicationListener* listener;
    UINT                  wake_up_message;
    DWORD                 main_thread_id;
};

}

/*
    �������� �������� ����������
*/

IApplicationDelegate* Platform::CreateDefaultApplicationDelegate ()
{
  return new Win32ApplicationDelegate;
}
