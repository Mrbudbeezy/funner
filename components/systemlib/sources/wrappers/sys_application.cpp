#include <syslib/application.h>
#include <xtl/signal.h>
#include <common/singleton.h>
#include <common/exception.h>
#include <platform/platform.h>

using namespace syslib;
using namespace xtl;
using namespace common;

/*
    �������� ���������� ����������
*/

class ApplicationImpl
{
  public:
    ApplicationImpl ();    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� � ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DoNextEvent ();
    void DoEvents    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Run ();
    bool IsMessageLoop () const { return message_loop_count > 0; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Exit        (int code);
    int  GetExitCode () const { return exit_code; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    connection RegisterEventHandler (ApplicationEvent event, const Application::EventHandler& handler);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Notify (ApplicationEvent);
  
  private:
    typedef signal<void ()> ApplicationSignal;

    ApplicationSignal signals [ApplicationEvent_Num]; //������� ����������
    int               exit_code;                      //��� ���������� ����������
    bool              is_exit_detected;               //������� ������ ���������� ����������
    size_t            message_loop_count;             //���������� ��������� � ���������� ������� ���������
};

typedef Singleton<ApplicationImpl> ApplicationSingleton;

/*
    ������� ��� ��������� �������� ��������� � ���������� ������� ���������
*/

namespace
{

class MessageLoopScope
{
  public:
    MessageLoopScope  (size_t& in_counter) : counter (in_counter) { counter++; }
    ~MessageLoopScope () { counter--; }
    
  private:
    size_t& counter;
};

}

/*
    �����������
*/

ApplicationImpl::ApplicationImpl ()
{
  exit_code          = 0;
  message_loop_count = false;
  is_exit_detected   = false;
}

/*
    ����������� ��������� ������� ���������
*/

void ApplicationImpl::Exit (int code)
{
  if (is_exit_detected)
    return;

  is_exit_detected = true;
  exit_code        = code;
  
  Notify (ApplicationEvent_OnExit);
}

/*
    ��������� ��������� � ������� ���������
*/

void ApplicationImpl::DoNextEvent ()
{
  if (is_exit_detected)
    return;

  MessageLoopScope scope (message_loop_count);
  
  Platform::DoNextEvent ();
}

void ApplicationImpl::DoEvents ()
{
  MessageLoopScope scope (message_loop_count);

  while (!Platform::IsMessageQueueEmpty () && !is_exit_detected)
    Platform::DoNextEvent ();
}

void ApplicationImpl::Run ()
{
  MessageLoopScope scope (message_loop_count);

  while (!is_exit_detected)
  {
    DoEvents ();

     //���� ��� ������������ OnIdle - ���������������� ����������

    if (signals [ApplicationEvent_OnIdle].empty ())
    {
      if (!is_exit_detected)
        Platform::WaitMessage ();
    }
    else
    {
      Notify (ApplicationEvent_OnIdle);
    }
  }
}

/*
    �������� �� ������� ����������
*/

connection ApplicationImpl::RegisterEventHandler (ApplicationEvent event, const Application::EventHandler& handler)
{
  if (event < 0 || event >= ApplicationEvent_Num)
    RaiseInvalidArgument ("syslib::Application::RegisterEventHandler", "event", event);

  return signals [event].connect (handler);
}

/*
    ���������� � �������� ����������
*/

void ApplicationImpl::Notify (ApplicationEvent event)
{
  signals [event] ();
}

/*
    ������ ��� ��������
*/

void Application::DoNextEvent ()
{
  ApplicationSingleton::Instance ().DoNextEvent ();
}

void Application::DoEvents ()
{
  ApplicationSingleton::Instance ().DoEvents ();
}

void Application::Run ()
{
  ApplicationSingleton::Instance ().Run ();
}

bool Application::IsMessageLoop ()
{
  return ApplicationSingleton::Instance ().IsMessageLoop ();
}

void Application::Exit (int code)
{
  ApplicationSingleton::Instance ().Exit (code);
}

int Application::GetExitCode ()
{
  return ApplicationSingleton::Instance ().GetExitCode ();
}

connection Application::RegisterEventHandler (ApplicationEvent event, const EventHandler& handler)
{
  return ApplicationSingleton::Instance ().RegisterEventHandler (event, handler);
}

void Application::Sleep (size_t milliseconds)
{
  Platform::Sleep (milliseconds);
}
