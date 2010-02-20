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

/*
    ����������� ������� do-events
*/

struct do_events_accumulator
{
  typedef void result_type;

  template <class InIter> void operator () (InIter first, InIter last) const
  {
    if (first == last)
      return;

    *--last;
  }
};

}

/*
    �������� ���������� ����������
*/

class ApplicationImpl: private IRunLoopContext
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
    typedef xtl::signal<void ()>                        ApplicationSignal;
    typedef xtl::signal<bool (), or_signal_accumulator> SuspendSignal;
    typedef xtl::signal<void (), do_events_accumulator> DoEventsSignal;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Notify (ApplicationSignal&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Suspend ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��� ������������ �������� ��������� �������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DoCustomRunLoop (); //����� ���� �� ������, ���� ��������� ���� ���������� ��������� ������� ������ (��������, iPhone)
    void OnExit          (int code);
    void OnIdle          ();
    void OnEnterRunLoop  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessActions (ActionThread);

  private:
    ApplicationSignal  idle_signal;                    //������ ��������� ������� ApplicationEvent_OnIdle
    ApplicationSignal  exit_signal;                    //������ ��������� ������� ApplicationEvent_OnExit
    ApplicationSignal  enter_run_loop_signal;          //������ ��������� ������� ApplicationEvent_OnStartup
    DoEventsSignal     do_events_signal;               //������ ��������� ������� ApplicationEvent_DoEvents
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

  Notify (exit_signal);
}

/*
    ���������
*/

void ApplicationImpl::Suspend ()
{
  try
  {
    if (suspend_handler && !suspend_handler ())
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

  if (do_events_signal.empty ())
  {
    if (!is_exit_detected)
    {
      while (!Platform::IsMessageQueueEmpty () && !is_exit_detected)
        Platform::DoNextEvent ();
    }
  }
  else
  {
    do_events_signal (); //���������� �� �����������
  }
}

void ApplicationImpl::Run ()
{
  Platform::RunLoop (this);
}

/*
    �������� ��� ������������ �������� ��������� �������� ����� ����������
*/

void ApplicationImpl::DoCustomRunLoop ()
{
  MessageLoopScope scope (message_loop_count);

  while (!is_exit_detected)
  {
    Platform::UpdateMessageQueue ();

    DoEvents ();

     //���� ��� ������������ OnIdle - ���������������� ����������

    if (idle_signal.empty () && !ActionQueue::ActionsCount (ActionThread_Main) && !ActionQueue::ActionsCount (ActionThread_Current))
    {
      if (!is_exit_detected && Platform::IsMessageQueueEmpty ())
        Suspend ();
    }
    else
    {
      OnIdle ();
    }
  }
}

void ApplicationImpl::OnExit (int code)
{
  Exit (code);
}

void ApplicationImpl::ProcessActions (ActionThread thread)
{
  size_t count = ActionQueue::ActionsCount (thread);
  
  for (size_t i=0; i<count; i++)
  {
    try
    {
      Action action = ActionQueue::PopAction (thread);
      
      if (action.IsEmpty ())
        continue;
        
      action.Perform ();
    }
    catch (...)
    {
      //���������� ����������
    }
  }  
}

void ApplicationImpl::OnIdle ()
{
  ProcessActions (ActionThread_Main);
  ProcessActions (ActionThread_Current);

  Notify (idle_signal);  
}

void ApplicationImpl::OnEnterRunLoop ()
{
  Notify (enter_run_loop_signal);
}

/*
    �������� �� ������� ����������
*/

connection ApplicationImpl::RegisterEventHandler (ApplicationEvent event, const Application::EventHandler& handler)
{
  switch (event)
  {
    case ApplicationEvent_OnExit:         return exit_signal.connect (handler);
    case ApplicationEvent_OnIdle:         return idle_signal.connect (handler);
    case ApplicationEvent_OnEnterRunLoop: return enter_run_loop_signal.connect (handler);
    case ApplicationEvent_OnDoEvents:     return do_events_signal.connect (handler);
    default:
      throw xtl::make_argument_exception ("syslib::Application::RegisterEventHandler", "event", event);
  }
}

connection ApplicationImpl::RegisterSuspendHandler (const Application::SuspendHandler& handler)
{
  return suspend_handler.connect (handler);
}

/*
    ���������� � �������� ����������
*/

void ApplicationImpl::Notify (ApplicationSignal& signal)
{
  try
  {
    signal ();
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    ������ ��� ��������
*/

void Application::DoEvents ()
{
  ApplicationSingleton::Instance ()->DoEvents ();
}

void Application::Run ()
{
  ApplicationSingleton::Instance ()->Run ();
}

bool Application::IsMessageLoop ()
{
  return ApplicationSingleton::Instance ()->IsMessageLoop ();
}

void Application::Exit (int code)
{
  ApplicationSingleton::Instance ()->Exit (code);
}

int Application::GetExitCode ()
{
  return ApplicationSingleton::Instance ()->GetExitCode ();
}

connection Application::RegisterEventHandler (ApplicationEvent event, const EventHandler& handler)
{
  return ApplicationSingleton::Instance ()->RegisterEventHandler (event, handler);
}

connection Application::RegisterSuspendHandler (const SuspendHandler& handler)
{
  return ApplicationSingleton::Instance ()->RegisterSuspendHandler (handler);
}

void Application::Sleep (size_t milliseconds)
{
  Platform::Sleep (milliseconds);
}
