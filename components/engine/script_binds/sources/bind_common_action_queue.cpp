#include "shared.h"

#include <xtl/connection.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>

#include <common/time.h>

#include <syslib/application.h>

using namespace script;

namespace
{

const char* LOG_NAME                    = "script.binds.ActionQueue";
const char* COMMON_ACTION_QUEUE_LIBRARY = "Common.ActionQueue";

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ActionQueue: public xtl::reference_counter, public xtl::dynamic_cast_root
{
  public:
    typedef xtl::intrusive_ptr<ActionQueue> Pointer;
    typedef xtl::function<void (float)>     EventHandler;  //dt

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ()
    {
      return Pointer (new ActionQueue, false);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterEventHandler (float first_fire, float period, const EventHandler& handler)
    {
      RegisterIdleHandler ();

      return signal.connect (EventHandlerWrapper (first_fire, period, handler));
    }

//    xtl::connection RegisterEventHandler (float delay, const EventHandler& handler);

    xtl::connection RegisterEventHandler (const EventHandler& handler)
    {
      RegisterIdleHandler ();

      return signal.connect (EventHandlerWrapper (0, 0, handler));
    }

  private:
    void RegisterIdleHandler ()
    {
      if (idle_connection.connected ())
        return;

      idle_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&ActionQueue::IdleHandler, this));
    }

    void IdleHandler ()
    {
      if (signal.empty ())
      {
        idle_connection.disconnect ();
        return;
      }

      signal ();
    }

  private:
    struct EventHandlerWrapper
    {
      public:
        typedef ActionQueue::EventHandler EventHandler;

        EventHandlerWrapper (float in_first_fire, float in_period, const EventHandler& in_handler)
          : handler (in_handler)
        {
          first_fire       = size_t (in_first_fire * 1000.f);
          period           = size_t (in_period * 1000.f);
          accumulated_time = 0;
          previous_time    = common::milliseconds ();
          fired            = false;
        }

        void operator () ()
        {
          size_t current_time = common::milliseconds ();

          accumulated_time += current_time - previous_time;

          previous_time = current_time;

          if (!fired)
          {
            if (accumulated_time < first_fire)
              return;

            CallHandler ();

            fired = true;

            return;
          }

          if (accumulated_time < period)
            return;

          CallHandler ();
        }

        void CallHandler ()
        {
          try
          {
            handler (accumulated_time / 1000.f);
          }
          catch (xtl::exception& exception)
          {
            try
            {
              common::Log (LOG_NAME).Printf ("Exception in action queue handler: '%s'", exception.what ());
            }
            catch (...)
            {
            }
          }
          catch (...)
          {
            try
            {
              common::Log (LOG_NAME).Print ("Unknown exception in action queue handler");
            }
            catch (...)
            {
            }
          }

          accumulated_time = 0;
        }

      private:
        size_t       previous_time;    //����� ����������� ������ �����������
        size_t       accumulated_time; //����� � ����������� ������������ �����������
        size_t       first_fire;
        size_t       period;
        bool         fired;            //true ���� ���������� ���������
        EventHandler handler;
    };

  private:
    typedef xtl::signal<void ()> ActionQueueSignal;

  private:
    ActionQueueSignal    signal;          //������ ��������� ��������
    xtl::auto_connection idle_connection; //���������� idle
};

}

namespace engine
{

void bind_common_action_queue (script::Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (COMMON_ACTION_QUEUE_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&ActionQueue::Create));

    //����������� ��������

  lib.Register ("CreateEventHandler",   make_callback_invoker<ActionQueue::EventHandler::signature_type> ());
  lib.Register ("RegisterEventHandler", make_invoker (make_invoker (xtl::implicit_cast<xtl::connection (ActionQueue::*) (float, float, const ActionQueue::EventHandler&)> (&ActionQueue::RegisterEventHandler)),
                                                      make_invoker (xtl::implicit_cast<xtl::connection (ActionQueue::*) (const ActionQueue::EventHandler&)> (&ActionQueue::RegisterEventHandler))));

    //����������� ����� ������

  environment.RegisterType<ActionQueue> (COMMON_ACTION_QUEUE_LIBRARY);
}

}
