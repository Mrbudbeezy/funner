#include <shared/message_queue.h>

using namespace syslib;

/*
    ������ � �������� ���������
*/

namespace
{

class ApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///�����������
    ApplicationDelegate ()
      : message_queue (*MessageQueueSingleton::Instance ())
    {
      idle_enabled = false;
      is_exited    = false;
      listener     = 0;
            
      message_queue.RegisterHandler (this);
    }
    
///����������
    ~ApplicationDelegate ()
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
          listener->OnInitialize ();
        
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
        e.touch ("syslib::ApplicationDelegate::Run");
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
      ExitMessage (ApplicationDelegate& in_delegate, int in_code) : delegate (in_delegate), code (in_code) {}
      
      void Dispatch ()
      {
        delegate.OnExit (code);
      }
      
      ApplicationDelegate& delegate;
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
        exception.touch ("syslib::ApplicationDelegate::WaitMessage");
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
        exception.touch ("syslib::ApplicationDelegate::DoNextEvent");
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

namespace syslib
{

IApplicationDelegate* create_message_queue_application_delegate ()
{
  return new ApplicationDelegate;
}

}
