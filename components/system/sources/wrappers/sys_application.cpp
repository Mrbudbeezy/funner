#include "shared.h"

using namespace syslib;
using namespace xtl;
using namespace common;

namespace
{

/*
    ������� ��� ��������� �������� ��������� � ���������� ������� ���������
*/

class MessageLoopScope
{
  public:
    MessageLoopScope  (size_t& in_counter) : counter (in_counter) { counter++; }
    ~MessageLoopScope () { counter--; }
    
  private:
    size_t& counter;
};

/*
    ����������� ������� � ��������� OR
*/

struct or_signal_accumulator
{
  typedef bool result_type;
  
  template <class InIter> bool operator () (InIter first, InIter last) const
  {
    for (; first!=last; ++first)
      if (*first)
        return true;

    return false;
  }
};

}

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
    void DoEvents ();

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
    connection RegisterEventHandler   (ApplicationEvent event, const Application::EventHandler& handler);
    connection RegisterSuspendHandler (const Application::SuspendHandler& handler);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Notify (ApplicationEvent);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Suspend ();
  
  private:
    typedef xtl::signal<void ()>                        ApplicationSignal;
    typedef xtl::signal<bool (), or_signal_accumulator> SuspendSignal;

    ApplicationSignal  signals [ApplicationEvent_Num]; //������� ����������
    SuspendSignal      suspend_handler;                //������ �������� ������� �������������� ���������
    int                exit_code;                      //��� ���������� ����������
    bool               is_exit_detected;               //������� ������ ���������� ����������
    size_t             message_loop_count;             //���������� ��������� � ���������� ������� ���������
};

typedef Singleton<ApplicationImpl> ApplicationSingleton;

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
    ���������
*/

void ApplicationImpl::Suspend ()
{
  try
  {
    if (!suspend_handler ())
      return;

    if (!is_exit_detected)
      Platform::WaitMessage ();
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ��������� ��������� � ������� ���������
*/

void ApplicationImpl::DoEvents ()
{
  MessageLoopScope scope (message_loop_count);

  while (!Platform::IsMessageQueueEmpty () && !is_exit_detected)
    Platform::DoNextEvent ();

  if (is_exit_detected)
    return;

  try
  {
    signals [ApplicationEvent_OnDoEvents] ();
  }
  catch (...)
  {
    //���������� ���� ����������
  }
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
      if (!is_exit_detected && Platform::IsMessageQueueEmpty ())
        Suspend ();
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
    throw xtl::make_argument_exception ("syslib::Application::RegisterEventHandler", "event", event);

  return signals [event].connect (handler);
}

connection ApplicationImpl::RegisterSuspendHandler (const Application::SuspendHandler& handler)
{
  return suspend_handler.connect (handler);
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

connection Application::RegisterSuspendHandler (const SuspendHandler& handler)
{
  return ApplicationSingleton::Instance ().RegisterSuspendHandler (handler);
}

void Application::Sleep (size_t milliseconds)
{
  Platform::Sleep (milliseconds);
}
