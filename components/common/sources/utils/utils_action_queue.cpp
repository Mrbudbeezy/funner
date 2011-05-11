#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/lock_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>

#include <common/action_queue.h>
#include <common/component.h>
#include <common/lockable.h>
#include <common/singleton.h>

#include <platform/platform.h>

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

using namespace common;

namespace common
{

/*
    ���������
*/

const char* ACTION_QUEUE_LISTENERS_COMPONENT_MASK = "common.action_queue.*"; //����� ��� ����������� ��������� ������� ������� ��������

/*
    ���������� ��������
*/

struct ActionImpl: public xtl::reference_counter, public Lockable
{
  ActionQueue::ActionHandler  action_handler; //���������� ���������� ��������
  Action::WaitCompleteHandler wait_handler;   //���������� �������� ���������� ��������
  ActionThread                thread_type;    //��� ����
  Timer                       timer;          //������, ��������� � ���������
  ActionQueue::time_t         next_time;      //����� ���������� ���������� ��������
  ActionQueue::time_t         period;         //������ ���������� ��������
  size_t                      thread_id;      //������������� ����, � ������� ������� ��������
  bool                        is_periodic;    //�������� �� �������� �������������
  bool                        is_completed;   //��������� �� ��������
  bool                        is_canceled;    //�������� ��������

  ActionImpl (const ActionQueue::ActionHandler& in_handler, ActionThread in_thread_type, bool in_is_periodic, Timer& in_timer, ActionQueue::time_t delay, ActionQueue::time_t in_period, const Action::WaitCompleteHandler& in_wait_handler)
    : action_handler (in_handler)
    , wait_handler (in_wait_handler)
    , thread_type (in_thread_type)
    , timer (in_timer)
    , next_time (timer.Time () + delay)    
    , period (in_period)
    , thread_id (Platform::GetCurrentThreadId ())
    , is_periodic (in_is_periodic)
    , is_completed (false)
    , is_canceled (false)
  {
  }
  
  Action GetWrapper () { return Action (this); }
};

typedef xtl::intrusive_ptr<ActionImpl> ActionPtr;

}

namespace
{

/*
    �������� � �������� ������� �� ����������
*/

class ActionWithCallback
{
  public:
    ActionWithCallback (const ActionQueue::ActionHandler& in_action, const ActionQueue::CallbackHandler& in_complete_handler)
      : action (in_action)
      , complete_callback (in_complete_handler)
    {
    }
    
    void operator () (Action& state) const
    {
      action (state);
      complete_callback ();
    }

  private:
    ActionQueue::ActionHandler   action;
    ActionQueue::CallbackHandler complete_callback;
};

typedef xtl::lock_ptr<ActionImpl, xtl::intrusive_ptr<ActionImpl> > ActionLock;

/*
    ������� ����
*/

class ThreadActionQueue: public xtl::reference_counter
{
  public:
///�����������
    ThreadActionQueue ()
    {
      actions_count = 0;
    }
  
///�������� �� �������
    bool IsEmpty ()
    {
      return actions.empty ();
    }
    
///���������� �������� � �������
    size_t Size ()
    {
      return actions_count;
    }
  
///���������� ��������
    void PushAction (const ActionPtr& action)
    {
      if (!action)
        return;
        
      actions.push_back (action);
      
      actions_count++;
    }    
    
///��������� ��������
    ActionPtr PopAction ()
    {
      for (ActionList::iterator iter=actions.begin (); iter!=actions.end ();)
      {
        ActionLock action (iter->get ());
        
        if (action->is_canceled || action->is_completed)
        {
          ActionList::iterator next = iter;

          ++next;

          actions.erase (iter);
          
          actions_count--;

          iter = next;

          continue;
        }

        if (action->next_time > action->timer.Time ())
        {
          ++iter;
          continue;
        }                

        if (action->is_periodic)
        {
          actions.splice (actions.end (), actions, iter);          
          
          action->next_time += action->period;

          return action.get ();
        }
          
        actions.erase (iter);
        
        actions_count--;

        return action.get ();
      }

      return ActionPtr ();
    }

  private:
    typedef stl::list<ActionPtr> ActionList;

  private:
    size_t     actions_count;
    ActionList actions;
};

/*
    ���������� ������� ��������
*/

class ActionQueueImpl
{
  public:
    typedef ActionQueue::time_t        time_t;
    typedef ActionQueue::ActionHandler ActionHandler;
    
///�����������
    ActionQueueImpl ()
      : default_wait_handler (xtl::bind (&ActionQueueImpl::DefaultWaitHandler, _1, _2))
    {
      default_timer.Start ();
    }

///���������� �������� � �������
    Action PushAction (const ActionHandler& action_handler, ActionThread thread, bool is_periodic, time_t delay, time_t period, Timer& timer)
    {
      try
      {
        ActionPtr action (new ActionImpl (action_handler, thread, is_periodic, timer, delay, period, default_wait_handler), false);

        ThreadActionQueue& queue = GetQueue (thread);

        queue.PushAction (action);

        Action result = action->GetWrapper ();

        try
        {
          static common::ComponentLoader loader (ACTION_QUEUE_LISTENERS_COMPONENT_MASK);
          
          signals [ActionQueueEvent_OnPushAction] (thread, result);
        }
        catch (...)
        {
          //���������� ���� ����������
        }

        return result;
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ActionQueue::PushAction");
        throw;
      }
    }

///������ �������
    size_t ActionsCount (ActionThread thread)
    {
      try
      {
        switch (thread)
        {
          case ActionThread_Main:
            return main_thread_queue.Size ();
          case ActionThread_Background:
            return background_queue.Size ();
          case ActionThread_Current:
          {        
            Platform::threadid_t thread_id = Platform::GetCurrentThreadId ();

            ThreadActionQueueMap::iterator iter = thread_queues.find (thread_id);

            if (iter == thread_queues.end ())
              return 0;

            ThreadActionQueuePtr queue = iter->second;

            return queue->Size ();
          }
          default:
            throw xtl::make_argument_exception ("", "thread", thread);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ActionQueue::QueueActionsCount");
        throw;
      }
    }

///���������� �������� �� �������
    Action PopAction (ActionThread thread)
    {
      try
      {
        ThreadActionQueue& queue = GetQueue (thread);

        ActionPtr action = queue.PopAction ();
        
        if (!action)
          return Action ();
          
        Action result = action->GetWrapper ();
          
        try
        {
          signals [ActionQueueEvent_OnPopAction] (thread, result);
        }
        catch (...)
        {
          //���������� ���� ����������
        }        

        return result;
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ActionQueue::PopAction");
        throw;
      }      
    }

///������� ��������������� �������� ��� ������� ����
    void CleanupCurrentThread ()
    {
      try
      {
        Platform::threadid_t thread_id = Platform::GetCurrentThreadId ();

        ThreadActionQueueMap::iterator iter = thread_queues.find (thread_id);

        if (iter == thread_queues.end ())
          return;

        ThreadActionQueuePtr queue = iter->second;

        if (!queue->IsEmpty ())
          throw xtl::format_operation_exception ("", "Can't cleanup current thread queue. Queue is not empty");

        thread_queues.erase (iter);
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ActionQueue::CleanupCurrentThread");
        throw;
      }
    }
    
///����������� ������������ �������
    xtl::connection RegisterEventHandler (ActionQueueEvent event, const ActionQueue::EventHandler& handler)
    {
      switch (event)
      {
        case ActionQueueEvent_OnPushAction:
        case ActionQueueEvent_OnPopAction:
          break;
        default:
          throw xtl::make_argument_exception ("common::ActionQueue::RegisterEventHandler", "event", event);
      }

      return signals [event].connect (handler);
    }

///��������� ������� �� ���������
    Timer& DefaultTimer () { return default_timer; }    

  private:
///��������� ������� ����
    ThreadActionQueue& GetQueue (ActionThread thread)
    {
      switch (thread)
      {
        case ActionThread_Main:
          return main_thread_queue;
        case ActionThread_Background:
          return background_queue;
        case ActionThread_Current:
        {
          Platform::threadid_t thread_id = Platform::GetCurrentThreadId ();
          
          ThreadActionQueueMap::iterator iter = thread_queues.find (thread_id);
          
          if (iter != thread_queues.end ())
            return *iter->second;
            
          ThreadActionQueuePtr queue (new ThreadActionQueue, false);

          thread_queues.insert_pair (thread_id, queue);

          return *queue;
        }
        default:
          throw xtl::make_argument_exception ("common::ActionQueueImpl::GetQueue", "thread", thread);
      }
    }
    
///������� �������� �� ���������
    static bool DefaultWaitHandler (Action& action, size_t milliseconds)
    {
      size_t       end_time = common::milliseconds () + milliseconds;
      bool         infinite = milliseconds == ~0u;
      ActionThread thread   = action.ThreadType ();
      
      while (!action.IsCompleted () && !action.IsCanceled () && (infinite || common::milliseconds () < end_time))
      {
        Action performed_action = ActionQueue::PopAction (thread);
        
        if (performed_action.IsEmpty () || performed_action.IsCanceled () || performed_action.IsCompleted ())
          continue;

        performed_action.Perform ();
      }

      return action.IsCompleted ();
    }

  private:
    typedef xtl::intrusive_ptr<ThreadActionQueue>                     ThreadActionQueuePtr;
    typedef stl::hash_map<Platform::threadid_t, ThreadActionQueuePtr> ThreadActionQueueMap;
    typedef xtl::signal<void (ActionThread, Action&)>                 Signal;

  private:
    Timer                       default_timer;
    Action::WaitCompleteHandler default_wait_handler;
    ThreadActionQueue           main_thread_queue;
    ThreadActionQueue           background_queue;
    ThreadActionQueueMap        thread_queues;
    Signal                      signals [ActionQueueEvent_Num];
};

typedef common::Singleton<ActionQueueImpl> ActionQueueSingleton;

}

/*
    ��������
*/

Action ActionQueue::PushAction (const ActionHandler& action, ActionThread thread, time_t delay)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread, false, delay, 0.0, queue->DefaultTimer ());
}

Action ActionQueue::PushAction (const ActionHandler& action, ActionThread thread, time_t delay, time_t period)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread, true, delay, period, queue->DefaultTimer ());
}

Action ActionQueue::PushAction (const ActionHandler& action, ActionThread thread, time_t delay, Timer& timer)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread, false, delay, 0.0, timer);
}

Action ActionQueue::PushAction (const ActionHandler& action, ActionThread thread, time_t delay, time_t period, Timer& timer)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread, true, delay, period, timer);
}

Action ActionQueue::PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, ActionThread thread, time_t delay)
{
  return PushAction (ActionWithCallback (action, complete_callback), thread, delay);
}

Action ActionQueue::PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, ActionThread thread, time_t delay, Timer& timer)
{
  return PushAction (ActionWithCallback (action, complete_callback), thread, delay, timer);
}

size_t ActionQueue::ActionsCount (ActionThread thread)
{
  return ActionQueueSingleton::Instance ()->ActionsCount (thread);
}

Action ActionQueue::PopAction (ActionThread thread)
{
  return ActionQueueSingleton::Instance ()->PopAction (thread);
}

xtl::connection ActionQueue::RegisterEventHandler (ActionQueueEvent event, const EventHandler& handler)
{
  return ActionQueueSingleton::Instance ()->RegisterEventHandler (event, handler);
}

void ActionQueue::CleanupCurrentThread ()
{
  ActionQueueSingleton::Instance ()->CleanupCurrentThread ();
}

/*
    ��������
*/

Action::Action ()
  : impl (0)
{
}

Action::Action (ActionImpl* in_impl)
  : impl (in_impl)
{
  if (impl)
    addref (impl);
}

Action::Action (const Action& action)
  : impl (action.impl)
{
  ActionLock locked_impl (impl);

  if (impl)
    addref (impl);
}

Action::~Action ()
{
  ActionLock locked_impl (impl);

  if (impl)
    release (impl);
}

Action& Action::operator = (const Action& action)
{
  Action (action).Swap (*this);

  return *this;
}

size_t Action::CreaterThreadId () const
{
  return impl ? impl->thread_id : 0;
}

ActionThread Action::ThreadType () const
{
  return impl ? impl->thread_type : ActionThread_Current;
}

bool Action::IsEmpty () const
{
  return impl == 0;
}

bool Action::IsCompleted () const
{
  ActionLock locked_impl (impl);

  return impl ? impl->is_completed : false;
}

bool Action::IsCanceled () const
{
  ActionLock locked_impl (impl);

  return impl ? locked_impl->is_canceled : false;
}

bool Action::Wait (size_t milliseconds)
{
  try
  {
    ActionLock locked_impl (impl);
    
    if (!impl)
      throw xtl::format_operation_exception ("", "Can't wait for empty action is empty");

    return impl->wait_handler (*this, milliseconds);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::Action::Wait");
    throw;
  }
}

void Action::Perform ()
{
  try
  {
    xtl::intrusive_ptr<ActionImpl> impl_holder;

    ActionQueue::ActionHandler handler;

    {
      ActionLock locked_impl (impl);

      if (!impl)
        return;

      if (impl->is_completed)
        throw xtl::format_operation_exception ("", "Action already completed");

      if (impl->is_canceled)
        throw xtl::format_operation_exception ("", "Action already canceled");

      handler = impl->action_handler;

      impl_holder = impl;
    }

    handler (*this);

    ActionLock locked_impl (impl);
    
    if (!impl->is_periodic)
      impl->is_completed = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::Action::Perform");
    throw;
  }
}

void Action::Cancel ()
{
  ActionLock locked_impl (impl);
  
  if (!impl)
    return;

  impl->is_canceled = true;
}

void Action::SetWaitHandler (const WaitCompleteHandler& handler)
{
  ActionLock locked_impl (impl);
  
  if (!impl)
    throw xtl::format_operation_exception ("common::Action::SetWaitHandler", "Can't set wait handler for empty action");

  impl->wait_handler = handler;
}

const Action::WaitCompleteHandler& Action::WaitHandler () const
{
  ActionLock locked_impl (impl);

  return impl->wait_handler;
}

void Action::Swap (Action& action)
{
  if (impl < action.impl)
  {
    ActionLock locked_impl1 (impl);
    ActionLock locked_impl2 (action.impl);
    
    stl::swap (impl, action.impl);
  }
  else
  {
    ActionLock locked_impl1 (action.impl);
    ActionLock locked_impl2 (impl);

    stl::swap (impl, action.impl);    
  }
}

namespace common
{

void swap (Action& action1, Action& action2)
{
  action1.Swap (action2);
}

}
