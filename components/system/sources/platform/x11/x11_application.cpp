#include "shared.h"

using namespace syslib;
using namespace syslib::x11;

/*
    ������ � �������� ���������
*/

namespace
{

class X11ApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///�����������
    X11ApplicationDelegate ()
      : message_queue (*MessageQueueSingleton::Instance ())
    {
      idle_enabled = false;
      is_exited    = false;
      listener     = 0;
            
      message_queue.RegisterHandler (this);
    }
    
///����������
    ~X11ApplicationDelegate ()
    {
      try
      {
        message_queue.UnregisterHandler (this);
      }
      catch (...)
      {
      }
    }

///������ ����� ��������� ���������
    void Run ()
    {
      try
      {
        if (listener)
          listener->OnInitialized ();
        
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
        e.touch ("syslib::X11ApplicationDelegate::Run");
        throw;
      }
    }

///����� �� ����������
    void Exit (int code)
    {
      message_queue.PushMessage (this, MessageQueue::MessagePtr (new ExitMessage (*this, code), false));
    }
    
///��������� ������� ������ �� ����������
    void OnExit (int code)
    {
      is_exited = true;
      
      if (listener)
        listener->OnExit (code);
    }

///��������� ������������� ������ ������� idle
    void SetIdleState (bool state)
    {
      idle_enabled = state;
      
      if (state)
      {
        message_queue.PushEmptyMessage ();
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
///���������� ������� ������ �� ����������
    struct ExitMessage: public MessageQueue::Message
    {
      ExitMessage (X11ApplicationDelegate& in_delegate, int in_code) : delegate (in_delegate), code (in_code) {}
      
      void Dispatch ()
      {
        delegate.OnExit (code);
      }
      
      X11ApplicationDelegate& delegate;
      int                     code;
    };
    
///�������� ������� ������� �� �������
    bool IsMessageQueueEmpty ()
    {
      if (is_exited)
        return true;
        
      return message_queue.IsEmpty ();
    }
    
///�������� �������
    void WaitMessage ()
    {
      try
      {
        message_queue.WaitMessage ();
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::X11ApplicationDelegate::WaitMessage");
        throw;
      }
    }
    
///��������� ���������� �������
    void DoNextEvent ()
    {
      try
      {
        message_queue.DispatchFirstMessage ();
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::X11ApplicationDelegate::DoNextEvent");
        throw;
      }
    }    

  private:
    bool                  idle_enabled;
    bool                  is_exited;
    IApplicationListener* listener;
    MessageQueue&         message_queue;
};

}

/*
    �������� �������� ����������
*/

IApplicationDelegate* Platform::CreateDefaultApplicationDelegate ()
{
  return new X11ApplicationDelegate;
}
